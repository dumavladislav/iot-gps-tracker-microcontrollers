#include <Arduino.h>

#include "GPSTracker.h"
#include "Constants/Constants.h"

GPSTracker gpsTracker(GPS_LOG_PERIOD_SEC);

void setup()
{
  Serial.begin(9600);
  gpsTracker.init();
}

void loop()
{
  // This sketch displays information every time a new sentence is correctly encoded.

  gpsTracker.readGpsData();
}

