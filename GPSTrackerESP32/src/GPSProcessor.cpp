#include "./Constants/Credentials.h"
#include "GPSProcessor.h"
// #include <JsonMessageBuilder.h>

GPSProcessor::GPSProcessor(int gpsProcessPeriod) {
    this->gpsProcessPeriod = gpsProcessPeriod;


    delay(3000);
}

void GPSProcessor::init() {
    
}

void GPSProcessor::processGpsData(GpsData gpsData) {
    Serial.println("GPSProcessor::Processing GPS data...");

    Serial.println("GPSProcessor::GPS data processed!");
}





