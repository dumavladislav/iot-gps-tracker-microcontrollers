#include <Arduino.h>

#include <GPSTrackerStatus.h>
#include <GPSReader.h>
#include <GPSProcessor.h>
#include <NetworkConnection.h>
#include "Constants/Constants.h"
#include <GPSScreen.h>

GPSTrackerStatus volatile gpsTrackerStatus;
NetworkConnection /*volatile*/ networkConnection;

GPSScreen screen;

//=========================================  MULTITASKING  =========================================
TaskHandle_t gpsDataReaderHandler = NULL;
TaskHandle_t gpsDataProcessorHandler = NULL;
TaskHandle_t gsmKeepAliveHandler = NULL;
TaskHandle_t updateScreenHandler = NULL;

QueueHandle_t gsmQueue;
QueueHandle_t gpsDataProcessorQueue;

SemaphoreHandle_t lasrReadGPSDataSemaphore = xSemaphoreCreateBinary();
GpsData lastReadGPSData;

void gpsDataReader(void *parameter) {
  GPSReader gpsReader(PERIOD_GPS_READ);
  gpsReader.init();
  for(;;) {
    GpsData gpsData = gpsReader.readGpsData();
    lastReadGPSData = gpsData;
    if (gpsData.gpsDataValid) {
      xQueueGenericSendFromISR(gpsDataProcessorQueue, &gpsData, pdFALSE, queueSEND_TO_BACK);
    }
    vTaskDelay(PERIOD_GPS_READ*1000);
  }
}

void gpsDataProcessor(void *parameter) {
  GPSProcessor gpsProcessor(PERIOD_GPS_PROCCESS);
  GpsData gpsData;
  gpsProcessor.init();
  for(;;) {
    if (xQueueIsQueueEmptyFromISR(gpsDataProcessorQueue) != pdTRUE) {
      xQueueGenericReceive(gpsDataProcessorQueue, &gpsData, ( TickType_t ) 10 , false);
      Serial.print("gpsDataProcessor: Received data: ");
      Serial.print(gpsData.lat);
      Serial.print(gpsData.lng);
      Serial.print(". Still in queue: ");
      Serial.println(uxQueueMessagesWaiting(gpsDataProcessorQueue));
      //gpsProcessor.processGpsData(gpsData);
      //networkConnection.getMqttClient()->sendMessage(GPS_TPC, gpsProcessor.getGpsDataJson(gpsData));
      if (uxQueueSpacesAvailable(gsmQueue) > 0) {
        xQueueGenericSendFromISR(gsmQueue, &gpsData, pdFALSE, queueSEND_TO_BACK);
      }
      else {
        Serial.println("NO SPACE IN GSM QUEUE! WRITING TO SD!");
      }
    }
    vTaskDelay(PERIOD_GPS_READ*1000);
  }
}

void gsmKeepAlive(void *parameter) {

  GPSProcessor gpsProcessor(PERIOD_GPS_PROCCESS);
  GpsData gpsData;
  
  for(;;) {
    
    networkConnection.keepAlive();

    Serial.print("Messages in GSM queue: ");
    Serial.println(uxQueueMessagesWaiting(gsmQueue));

    if (networkConnection.getMqttStatus() == 1) {
      if (!xQueueIsQueueEmptyFromISR(gsmQueue)) {
        xQueueGenericReceive(gsmQueue, &gpsData, ( TickType_t ) 10 , false);
        networkConnection.getMqttClient()->sendMessage(GPS_TPC, gpsProcessor.getGpsDataJson(gpsData));
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
    screen.addString("String 3");
    screen.render();
    vTaskDelay(1000);
  }
}

//=========================================  MULTITASKING  =========================================

void setup()
{
  Serial.begin(115200);
  while(!Serial) ;
  // gpsTracker.init();

  gpsDataProcessorQueue = xQueueCreate( 10, sizeof( GpsData ) );
  gsmQueue = xQueueCreate( 10, sizeof( GpsData ) );

  networkConnection.init();

  //=========================================  PIN Tasks to Cores  =========================================

  xTaskCreatePinnedToCore(
      gpsDataReader, "gpsDataReader", 1000 //const uint32_t usStackDepth
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
      gpsDataProcessor, "gpsDataProcessor", 2000 //const uint32_t usStackDepth
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
      gsmKeepAlive, "gsmKeepAlive", 2000 //const uint32_t usStackDepth
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
      updateScreen, "updateScreen", 2000 //const uint32_t usStackDepth
      ,
      NULL //void *constpvParameters
      ,
      1 //UBaseType_t uxPriority
      ,
      &updateScreenHandler //TaskHandle_t *constpvCreatedTask
      ,
      1 //const BaseType_t xCoreID
  );

  //=========================================  PIN Tasks to Cores  =========================================
  
}

void loop()
{
  // gpsTracker.keepAlive();
  // gpsTracker.showStatus();
  vTaskDelete(NULL);
}



