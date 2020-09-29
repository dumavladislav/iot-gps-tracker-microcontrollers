#include <Arduino.h>

#include <GPSTrackerStatus.h>
#include <GPSReader.h>
#include <GPSProcessor.h>
#include "Constants/Constants.h"

GPSTrackerStatus volatile gpsTrackerStatus;

//=========================================  MULTITASKING  =========================================
TaskHandle_t gpsDataReaderHandler = NULL;
TaskHandle_t gpsDataProcessorHandler = NULL;
TaskHandle_t gsmKeepAliveHandler = NULL;
TaskHandle_t updateScreenHandler = NULL;

QueueHandle_t gpsDataProcessorQueue;

void gpsDataReader(void *parameter) {
  GPSReader gpsReader(PERIOD_GPS_READ);
  gpsReader.init();
  for(;;) {
    GpsData gpsData = gpsReader.readGpsData();
    xQueueGenericSendFromISR(gpsDataProcessorQueue, &gpsData, pdFALSE, queueSEND_TO_BACK);
    delay(PERIOD_GPS_READ*1000);
  }
}

void gpsDataProcessor(void *parameter) {
  GPSProcessor gpsProcessor(PERIOD_GPS_PROCCESS);
  gpsProcessor.init();
  for(;;) {
    if (!xQueueIsQueueEmptyFromISR(gpsDataProcessorQueue)) {
      GpsData gpsData;
      xQueueGenericReceive(gpsDataProcessorQueue, &gpsData, ( TickType_t ) 10 , false);
      Serial.print("gpsDataProcessor: Received data: ");
      Serial.print(gpsData.lat);
      Serial.print(gpsData.lng);
      Serial.print(". Still in queue: ");
      Serial.println(uxQueueMessagesWaiting(gpsDataProcessorQueue));
    }
    delay(PERIOD_GPS_READ*1000);
  }
}

void gsmKeepAlive(void *parameter) {
  for(;;) {
    // gpsTracker.keepAlive();
  }
}

void updateScreen(void *parameter) {
  for(;;) {
    // gpsTracker.showStatus();
  }
}

//=========================================  MULTITASKING  =========================================

void setup()
{
  Serial.begin(115200);
  while(!Serial) ;
  // gpsTracker.init();

  gpsDataProcessorQueue = xQueueCreate( 10, sizeof( GpsData ) );

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
      gpsDataProcessor, "gpsDataProcessor", 1000 //const uint32_t usStackDepth
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
      gsmKeepAlive, "gsmKeepAlive", 1000 //const uint32_t usStackDepth
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
      updateScreen, "updateScreen", 1000 //const uint32_t usStackDepth
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



