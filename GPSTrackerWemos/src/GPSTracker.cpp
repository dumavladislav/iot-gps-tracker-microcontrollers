#include "Constants/Credentials.h"
#include "GPSTracker.h"
#include <JsonMessageBuilder.h>

GPSTracker::GPSTracker(int gpsScanPeriod) {
    this->gpsScanPeriod = gpsScanPeriod;
    authorizationBlock.deviceId = DEVICE_ID;
    authorizationBlock.chipId = ESP.getChipId();
    //gpsSoftwareSerial = new SoftwareSerial(GPS_RX_PIN, GPS_TX_PIN);
    //gpsSoftwareSerial->begin(9600);
    //gpsClient = new GPSClient(gpsSoftwareSerial);
    Serial.begin(9600);
    gpsClient = new GPSClient(&Serial);
}

void GPSTracker::init() {
    oled.init();
    // oled.renderString("GSM Initialized...");
    
    oled.renderString("GSM Connect..");
    /*while(gsmConnect.connect(GSM_APN, GSM_GPRS_USER, GSM_GPRS_PASS) < 1) {
        delay(5000);
        oled.renderString("Failed to Connect");
    };*/
    // Serial.println("Jopa");
    gsmConnect.init();
    // Serial.println("Jopa2");
    gsmConnect.connect(GSM_APN, GSM_GPRS_USER, GSM_GPRS_PASS);
    // Serial.println("Jopa3");
    authorizationBlock.macAddress = gsmConnect.getIMEI();
    mqttClient = new MQTTClient((char*)authorizationBlock.deviceId.c_str(), MQTT_SERVER, MQTT_PORT, (Client*) gsmConnect.getClient());
    // Serial.println("Jopa4");
    // delay(1000);
    mqttClient->keepAlive(MQTT_USER, MQTT_PSSWD);
    // Serial.println("Jopa5");
    // sdClient.init();
    //keepAlive();
    oled.renderString("GPS Init...");
    gpsClient->init(/*GPS_RX_PIN, GPS_TX_PIN*/);
    status = 1;
}

void GPSTracker::keepAlive() {
     
    /*while(gsmConnect.connect(GSM_APN, GSM_GPRS_USER, GSM_GPRS_PASS) < 1) {
        delay(5000);
        oled.renderString("Failed to Connect");
    };*/
    while(!gsmConnect.keepAlive(GSM_APN, GSM_GPRS_USER, GSM_GPRS_PASS)) {
        // mqttClient->authorizationRequest();
        delay(10000);
    }
    mqttClient->keepAlive(MQTT_USER, MQTT_PSSWD);
}

uint8_t GPSTracker::getStatus() {
    return status;
}


void GPSTracker::readGpsData() {
    GpsData gpsData = gpsClient->readGpsData();
    // gsmConnect.forceListen();
    // mqttClient->keepAlive(MQTT_USER, MQTT_PSSWD);
    keepAlive();
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
            if(mqttClient->sendMessage(GPS_TPC, getGpsDataJson(gpsData))) coordSentCounter++;
            prevGpsData = gpsData;
            coordCounter++;
            lastGpsScanTime = millis();
        }
    }
}

String GPSTracker::getGpsDataJson(GpsData gpsData) {
    
    JsonMessageBuilder jsonMessageBuilder(authorizationBlock);

    jsonMessageBuilder.addElement("millis", String(millis()));
    jsonMessageBuilder.addElement("lat", String(gpsData.lat, 6));
    jsonMessageBuilder.addElement("lng", String(gpsData.lng, 6));

    return jsonMessageBuilder.toString();
}

unsigned long GPSTracker::getSentCounter() {
    return coordSentCounter;
}


String GPSTracker::getStatusStr() {
    String status = "MQTT: ";
    if (mqttClient->isConnected()) status += "ON";
    else 
        status += "OFF";
    status += " | Sent: " + String(getSentCounter());
    return status;
}

String GPSTracker::getGpsStatusStr() {
    String status = "Sats: ";
    status += gpsClient->getNumberOfSatellites();
    status += " | " + String(lastGpsScanTime/1000) + " : " + String(millis()/1000);
    return status;
}

String GPSTracker::getGpsDataStr() {
    String status = String(prevGpsData.lat,6) + " : " + String(prevGpsData.lng,6);
    return status;
}

void GPSTracker::showStatus() {
    oled.clear();
    if (getStatus()) {
        oled.addString(getStatusStr());
        oled.addString(getGpsStatusStr());
        oled.addString(getGpsDataStr());
        // u8g2.drawStr(0,10, gpsTracker.getStatusStr().c_str());	// write something to the internal memory
        // u8g2.drawStr(0,20, gpsTracker.getGpsStatusStr().c_str());	// write something to the internal memory
        // u8g2.drawStr(0,30, gpsTracker.getGpsDataStr().c_str());	// write something to the internal memory
    }
    else oled.addString("Initialization....");
    oled.show();
    //u8g2.sendBuffer();					// transfer internal memory to the display
}