#include "./Constants/Credentials.h"
#include "GPSReader.h"
#include <JsonMessageBuilder.h>

GPSReader::GPSReader(int gpsScanPeriod) {
    this->gpsScanPeriod = gpsScanPeriod;
    SerialGPS.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
    gpsClient = new GPSClient(&SerialGPS);
}

void GPSReader::init() {
    
    // rtc.init();  
    delay(1000);  
   
    // sdClient.init();
    gpsClient->init(/*GPS_RX_PIN, GPS_TX_PIN*/);
}

GpsData GPSReader::readGpsData() {
    GpsData gpsData = gpsClient->readGpsData();
    if (((millis() - lastGpsScanTime) > (gpsScanPeriod*1000)) && gpsData.lat > 10 && gpsData.lng > 10) {
        if(prevGpsData.lat == 0) prevGpsData = gpsData;
        if (gpsData.lat != 0 
        && gpsData.lng != 0 
        && gpsData.lat >= -90 
        && gpsData.lat <= 90 
        && gpsData.lng >= -180 
        && gpsData.lng <= 180
        && 
        (
            ((gpsData.lat - prevGpsData.lat) <= 0.5 && (gpsData.lat - prevGpsData.lat) >= -0.5)
            && ((gpsData.lng - prevGpsData.lng) <= 0.5 && (gpsData.lng - prevGpsData.lng) >= -0.5)
            || millis() < 10000
            || coordCounter < 5
        ) 
        ) {
            
            // sdClient.writeLogString(gpsData.toString());
            prevGpsData = gpsData;
            coordCounter++;
            lastGpsScanTime = millis();
        }
    }
    return gpsData;
}


GpsData GPSReader::lastGPSData() {
    return prevGpsData;
}

