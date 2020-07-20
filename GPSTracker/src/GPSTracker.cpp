#include "Constants/Credentials.h"
#include "GPSTracker.h"

GPSTracker::GPSTracker(int gpsScanPeriod) {
    this->gpsScanPeriod = gpsScanPeriod;
}

void GPSTracker::init() {
    
    gsmConnect.init(GSM_APN, GSM_GPRS_USER, GSM_GPRS_PASS);
    // sdClient.init();
    mqttClient = new MQTTClient((char*)"1111", MQTT_SERVER, MQTT_PORT, (Client*) gsmConnect.getClient());
    //mqttClient->connect(MQTT_USER, MQTT_PSSWD);
    mqttClient->authorizationRequest();
    gpsClient.init(GPS_RXQ_PIN, GPS_TXQ_PIN);
}

void GPSTracker::readGpsData() {
    GpsData gpsData = gpsClient.readGpsData();
    if ((millis() - lastGpsScanTime) > (gpsScanPeriod*1000)) {
        if(prevGpsData.lat == 0) prevGpsData = gpsData;
     
        if (/*gpsData.gpsDataValid && */ gpsData.lat != 0 
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
            || coordCounter < 20
        ) 
        ) {
        
            prevGpsData = gpsData;
            coordCounter++;
            lastGpsScanTime = millis();
        }
    }
        
}