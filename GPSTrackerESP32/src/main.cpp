#include <Arduino.h>

#include "GPSReader.h"
#include "Constants/Constants.h"



//=========================================  MULTITASKING  =========================================
TaskHandle_t gpsDataReaderHandler = NULL;
TaskHandle_t gpsDataProcessorHandler = NULL;
TaskHandle_t gsmKeepAliveHandler = NULL;
TaskHandle_t updateScreenHandler = NULL;

QueueHandle_t gpsDataProcessorQueue;

void gpsDataReader(void *parameter) {
  GPSReader gpsReader(PERIOD_GPS_READ);
  for(;;) {
    gpsReader.init();
  }
}

void gpsDataProcessor(void *parameter) {
  for(;;) {
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



