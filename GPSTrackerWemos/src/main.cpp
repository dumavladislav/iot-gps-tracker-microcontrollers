#include <Arduino.h>

//#include <U8g2lib.h>

#include <Wire.h>

#include "GPSTracker.h"
#include "Constants/Constants.h"



GPSTracker gpsTracker(GPS_LOG_PERIOD_SEC);

/*void showStatus() {
  u8g2.clearBuffer();					// clear the internal memory
  u8g2.setFont(u8g2_font_ncenB08_tr);	// choose a suitable font
  if (gpsTracker.getStatus()) {
    u8g2.drawStr(0,10, gpsTracker.getStatusStr().c_str());	// write something to the internal memory
    u8g2.drawStr(0,20, gpsTracker.getGpsStatusStr().c_str());	// write something to the internal memory
    u8g2.drawStr(0,30, gpsTracker.getGpsDataStr().c_str());	// write something to the internal memory
  }
  else u8g2.drawStr(0,20, currentInitStatus.c_str());
  u8g2.sendBuffer();					// transfer internal memory to the display
}*/

void setup()
{
  Serial.begin(9600);
  while(!Serial) ;
  // u8g2.begin();
  // showStatus();
  gpsTracker.init();
}

void loop()
{
  // showStatus();
  gpsTracker.readGpsData();
  gpsTracker.showStatus();
}



