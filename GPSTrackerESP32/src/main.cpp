#include <Arduino.h>

#include <GPSTrackerStatus.h>
#include <GPSReader.h>
#include <GPSProcessor.h>
#include <NetworkConnection.h>
#include "Constants/Constants.h"
#include <GPSScreen.h>
#include <RTC3231.h>
#include <SDClient.h>
#include <LogFile.h>

// typedef char GpsDataString[1000];
struct GpsDataString { char gpsJson[1000]; };

GPSTrackerStatus volatile gpsTrackerStatus;
NetworkConnection /*volatile*/ networkConnection;
RTC3231 rtc;

GPSScreen screen;
SDClient sdClient;

//=========================================  MULTITASKING  =========================================
TaskHandle_t gpsDataReaderHandler = NULL;
TaskHandle_t gpsDataProcessorHandler = NULL;
TaskHandle_t gsmKeepAliveHandler = NULL;
TaskHandle_t updateScreenHandler = NULL;
TaskHandle_t gpsFilesProcessorHandler = NULL;

QueueHandle_t gsmQueue;
QueueHandle_t gpsDataProcessorQueue;

SemaphoreHandle_t lasrReadGPSDataSemaphore = xSemaphoreCreateBinary();
GpsData lastReadGPSData;

void gpsDataReader(void *parameter) {
  LogFile log(&SD, "gpsDataReader", SD_LOGS_DIR, 1024, 20);
  log.init(&rtc);
  GPSReader gpsReader(PERIOD_GPS_READ);

  log.logString("Initializing GPS Data Reader...");
  gpsReader.init();
  log.logString("GPS Data Reader initialized!");

  GpsData gpsData;
  for(;;) {
    gpsData = gpsReader.readGpsData();
    gpsData.rtcTimestamp = rtc.getTimeStamp();
    // Serial.print("GPS Data Timestamp: ");
    // Serial.println(gpsData.rtcTimestamp);
    lastReadGPSData = gpsData;
    if (gpsData.gpsDataValid) {
      if (uxQueueSpacesAvailable(gpsDataProcessorQueue) > 0) {
        Serial.println("Writing to gpsDataProcessorQueue...");
        xQueueGenericSendFromISR(gpsDataProcessorQueue, &gpsData, pdFALSE, queueSEND_TO_BACK);
      }
      else {
        log.logString("GPS Data Processor Queue is full!");
      }
    }
    Serial.print("GPS Data Lat: ");
    Serial.println(gpsData.lat);
    vTaskDelay(PERIOD_GPS_READ*1000);
  }
}

void gpsDataProcessor(void *parameter) {
  GPSProcessor gpsProcessor(PERIOD_GPS_PROCCESS);
  GpsData gpsData;
  LogFile log(&SD, "gpsDataProcessor", SD_LOGS_DIR, 1024, 20);
  log.init(&rtc);

  bool wasMqttConnected = false;

  LogFile gpsDataFile(&SD, "gpsData", "", 1024, GPS_BATCH_SIZE, SD_TO_MQTT_DATA);
  gpsDataFile.init(&rtc);

  log.logString("Initializing GPS Data Processor...");
  gpsProcessor.init();
  log.logString("GPS Data Processor initialized!");

  for(;;) {
    if (xQueueIsQueueEmptyFromISR(gpsDataProcessorQueue) != pdTRUE) {
      xQueueGenericReceive(gpsDataProcessorQueue, &gpsData, ( TickType_t ) 10 , false);
      Serial.print("gpsDataProcessor: Received data: ");
      Serial.print(gpsData.lat);
      Serial.print(gpsData.lng);
      Serial.print(". Still in queue: ");
      Serial.println(uxQueueMessagesWaiting(gpsDataProcessorQueue));
      if (networkConnection.getMqttStatus() == 1) {
        if (!wasMqttConnected) {
          wasMqttConnected = true;
          log.logString("MQTT Connection RESTORED!");
          gpsDataFile.rotateFile();
        }
      }
      else {
        if (wasMqttConnected) {
          wasMqttConnected = false;
          log.logString("MQTT Connection LOST!");          
        }
      }

      if (gpsData.gpsDataValid) {
        Serial.print("GPS Data VALID!!!!");
        GpsDataString gpsDataString;
        networkConnection.getGpsDataJson(gpsData).toCharArray(gpsDataString.gpsJson, 1000);
        if (uxQueueSpacesAvailable(gsmQueue) > 0 && networkConnection.getMqttStatus() == 1) {
          Serial.println("Wrinting to gsmQueue: ");
          Serial.println(gpsDataString.gpsJson);
          xQueueGenericSendFromISR(gsmQueue, &gpsDataString.gpsJson, pdFALSE, queueSEND_TO_BACK);
        }
        else {
          if (networkConnection.getMqttStatus() == 1) {
            // Serial.println("NO SPACE IN GSM QUEUE! WRITING TO SD!");
            log.logString("NO SPACE IN GSM QUEUE! WRITING TO SD!");
          }
          else {
            // Serial.println("NO MQTT CONNECTION! WRITING TO SD!");
            log.logString("NO MQTT CONNECTION! WRITING TO SD!");
          }
          gpsDataFile.writeString(gpsDataString.gpsJson);
        }
      }
    }
    vTaskDelay(PERIOD_GPS_READ*1000);
  }
}

void gsmKeepAlive(void *parameter) {

  GPSProcessor gpsProcessor(PERIOD_GPS_PROCCESS);
  GpsData gpsData;
  GpsDataString gpsDataString;
  
  LogFile log(&SD, "gsmKeepAlive", SD_LOGS_DIR, 1024, 20);
  log.init(&rtc);

  bool prevMqttAlive = false;

  for(;;) {
    
    networkConnection.keepAlive();

    // Serial.print("Messages in GSM queue: ");
    // Serial.println(uxQueueMessagesWaiting(gsmQueue));

    if (networkConnection.getMqttStatus() == 1) {
      Serial.println("MQTT CONNECTION = YES");
      if (!prevMqttAlive) {
        log.logString("Connected to MQTT!");
        prevMqttAlive = true;
      }
      // gpsDataString.gpsJson = String("");
      if (xQueueIsQueueEmptyFromISR(gsmQueue) != pdTRUE) {
        Serial.println("GSM QUEUE NOT EMPTY!!!");
        xQueueGenericReceive(gsmQueue, &gpsDataString, ( TickType_t ) 10 , false);
        Serial.print("gpsDataJson: ");
        Serial.println(gpsDataString.gpsJson);
        // if (!gpsDataJson.equals("")) {
          // Serial.println("gpsDataJson NOT EMPTY. Sending.....");
          networkConnection.getMqttClient()->sendMessage(GPS_TPC, gpsDataString.gpsJson);
        // }
      }
    }
    else {
      if(prevMqttAlive) {
        log.logString("LOST CONNECTION to MQTT!");
        prevMqttAlive = false;
      }
    }
    vTaskDelay(1000);
  }



}

void updateScreen(void *parameter) {
  screen.init();
  String str;

  for(;;) {
    screen.clear();
    str = "";
    // Serial.print("GSM Status: ");
    // Serial.println(networkConnection.getGsmStatus());
    switch(networkConnection.getGsmStatus()) {
      case 1:
        str += "GSM  ";
        break;
      case 2:
        str += "GPRS ";
        break;
      default:
        str += "     ";
        break;
    }
    str += " | ";
    if (networkConnection.getMqttStatus() == 1) str += "MQTT";
    screen.addString(str);
    str = "";
    str += String(lastReadGPSData.lat, 6);
    str += "; ";
    str += String(lastReadGPSData.lng, 6);
    screen.addString(str);
    screen.addString(rtc.getDateTime());
    Serial.println(rtc.getDateTime());
    screen.render();
    vTaskDelay(1000);
  }
}


void gpsFilesProcessor(void *parameter) {
  LogFile log(&SD, "gpsFilesProcessor", SD_LOGS_DIR, 1024, 20);
  log.init(&rtc);
  log.logString("GPS Files Processor Started!");

  bool wasMqttConnected = false;
  int waitPeriod = PERIOD_FILES_PROCESSOR;

  for(;;) {
    // sdClient.listDir("/", 1);
    if (uxQueueSpacesAvailable(gsmQueue) > 0 && networkConnection.getMqttStatus() == 1) {
      if (!wasMqttConnected) { 
        log.logString("MQTT CONNECTION RESTORED!");
        waitPeriod = PERIOD_FILES_PROCESSOR;
        wasMqttConnected = true;
      }

      //////////////////      READING GPS DATA FILES

      String nextFileName = sdClient.getNextFileFromDir(SD_TO_MQTT_DATA);
      // Serial.print("Next File Name received: ");
      // Serial.println(nextFileName);
      if (nextFileName /*&& !nextFile.isDirectory() && nextFile.name() != ""*/) {
        log.logString(String("Processing File: ") + nextFileName);
        File nextFile = SD.open(nextFileName, FILE_READ);
        GpsDataString gpsDataString;
        while (nextFile.available()) {
          nextFile.readStringUntil('\n').toCharArray(gpsDataString.gpsJson, 1000);
          Serial.print("GPS DATA FROM FILE: ");
          Serial.println(gpsDataString.gpsJson); //Printing for debugging purpose         
          if (uxQueueSpacesAvailable(gsmQueue) > 0 && networkConnection.getMqttStatus() == 1) {
            xQueueGenericSendFromISR(gsmQueue, &gpsDataString.gpsJson, pdFALSE, queueSEND_TO_BACK);
          }
        }
        nextFile.close();
        sdClient.removeFile(nextFileName);
      }

      //////////////////      READING GPS DATA FILES      \\\\\\\\\\\\\\\\\\\\\\\\\

    }
    else {
      if (wasMqttConnected) log.logString("MQTT CONNECTION LOST!");
      else {

        if (waitPeriod < MAX_FILES_PROCESSOR_WAIT_PERIOD) {
          waitPeriod += 1000;
          log.logString("STILL NO CONNECTION! Increasing wait period: " + waitPeriod);
        }
      }
      wasMqttConnected = false;
    }
    vTaskDelay(waitPeriod);
  }
}
//=========================================  MULTITASKING  =========================================

void setup()
{
  Serial.begin(115200);
  while(!Serial) ;
  
  // Serial.print("THE TIME IS: ");
  // Serial.println(__TIME__);
  // Serial.println(CURRENT_TIME);
  rtc.init(__DATE__, __TIME__, CURRENT_TIME);


  gpsDataProcessorQueue = xQueueCreate( 10, sizeof( GpsData ) );
  gsmQueue = xQueueCreate( 10, sizeof(GpsDataString) );

  Serial.print("Initializing SD card...");

  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD initialization failed!");
    while (1);
  }
  Serial.println("SD initialization done.");

  sdClient.init(&SD, &rtc);

  networkConnection.init();

  //=========================================  PIN Tasks to Cores  =========================================

  xTaskCreatePinnedToCore(
      gpsDataReader, "gpsDataReader", 4000 //const uint32_t usStackDepth
      ,
      NULL //void *constpvParameters
      ,
      1 //UBaseType_t uxPriority
      ,
      &gpsDataReaderHandler //TaskHandle_t *constpvCreatedTask
      ,
      1 //const BaseType_t xCoreID
  );

  xTaskCreatePinnedToCore(
      gpsDataProcessor, "gpsDataProcessor", 4000 //const uint32_t usStackDepth
      ,
      NULL //void *constpvParameters
      ,
      1 //UBaseType_t uxPriority
      ,
      &gpsDataProcessorHandler //TaskHandle_t *constpvCreatedTask
      ,
      1 //const BaseType_t xCoreID
  );

  xTaskCreatePinnedToCore(
      gsmKeepAlive, "gsmKeepAlive", 4000 //const uint32_t usStackDepth
      ,
      NULL //void *constpvParameters
      ,
      1 //UBaseType_t uxPriority
      ,
      &gsmKeepAliveHandler //TaskHandle_t *constpvCreatedTask
      ,
      1 //const BaseType_t xCoreID
  );

  xTaskCreatePinnedToCore(
      updateScreen, "updateScreen", 4000 //const uint32_t usStackDepth
      ,
      NULL //void *constpvParameters
      ,
      1 //UBaseType_t uxPriority
      ,
      &updateScreenHandler //TaskHandle_t *constpvCreatedTask
      ,
      1 //const BaseType_t xCoreID
  );


/*  xTaskCreatePinnedToCore(
      gpsFilesProcessor, "gpsFilesProcessor", 4000 //const uint32_t usStackDepth
      ,
      NULL //void *constpvParameters
      ,
      1 //UBaseType_t uxPriority
      ,
      &gpsFilesProcessorHandler //TaskHandle_t *constpvCreatedTask
      ,
      1 //const BaseType_t xCoreID
  );*/

  //=========================================  PIN Tasks to Cores  =========================================
  
}

void loop()
{
  // gpsTracker.keepAlive();
  // gpsTracker.showStatus();
  vTaskDelete(NULL);
}



