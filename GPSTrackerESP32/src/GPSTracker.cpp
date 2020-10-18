#include "./Constants/Credentials.h"
#include "GPSTracker.h"
#include <JsonMessageBuilder.h>

using namespace Dumsky;

GPSTracker::GPSTracker(int gpsScanPeriod) {
    this->gpsScanPeriod = gpsScanPeriod;
    authorizationBlock.deviceId = DEVICE_ID;
    authorizationBlock.chipId = ESP.getEfuseMac();
    //gpsSoftwareSerial = new SoftwareSerial(GPS_RX_PIN, GPS_TX_PIN);
    //gpsSoftwareSerial->begin(9600);
    //gpsClient = new GPSClient(gpsSoftwareSerial);
    //Serial.begin(9600);
    //Serial2.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
    //HardwareSerial SerialGPS(2);
    // SerialGSM = new SoftwareSerial(GSM_RX_PIN, GSM_TX_PIN);
    SerialGSM.begin(115200, SERIAL_8N1, GSM_RX_PIN, GSM_TX_PIN);
    SerialGPS.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);

    delay(3000);
    gpsClient = new GPSClient(&SerialGPS);
    
}

void GPSTracker::init() {
    
    oled.init();
    showStatus();
    rtc.init();  
    delay(1000);  
    // oled.renderString("GSM Initialized...");
    
    //oled.renderString("GSM Connect..");
    /*while(gsmConnect.connect(GSM_APN, GSM_GPRS_USER, GSM_GPRS_PASS) < 1) {
        delay(5000);
        oled.renderString("Failed to Connect");
    };*/
    // Serial.println("Jopa");
    gsmConnect.init(&SerialGSM/*GSM_APN, GSM_GPRS_USER, GSM_GPRS_PASS*/);
    // gsmConnect.connect(GSM_APN, GSM_GPRS_USER, GSM_GPRS_PASS);
    // Serial.println("Jopa2");
    // //
    // Serial.println("Jopa3");
    authorizationBlock.macAddress = gsmConnect.getIMEI();

    // BEFORE mqttInit
    //gsmConnect.connect(GSM_APN, GSM_GPRS_USER, GSM_GPRS_PASS);
    ////////

    mqttClient = new MQTTClient((char*)authorizationBlock.deviceId.c_str());
    mqttClient->init(MQTT_SERVER, MQTT_PORT, (Client*) gsmConnect.getClient(), MQTT_KEEPALIVE_TIMEOUT);
    // mqttClient->keepAlive(MQTT_USER, MQTT_PSSWD);
    
    // sdClient.init();
    // Serial.println("Jopa4");
    gpsClient->init(/*GPS_RX_PIN, GPS_TX_PIN*/);
    
   
    status = 1;
    //gsmConnect.connect(GSM_APN, GSM_GPRS_USER, GSM_GPRS_PASS);


}

uint8_t GPSTracker::keepAlive() {
    showStatus();
    readGpsData();
    if (!mqttClient->isConnected())
    {
        // Serial.println("Jopa7");
        gsmConnect.keepAlive(GSM_APN, GSM_GPRS_USER, GSM_GPRS_PASS);
        // Serial.println("Jopa8");
        if (gsmConnect.GPRSConnected()){
            // Serial.println("Jopa20");
            // mqttClient->init(MQTT_SERVER, MQTT_PORT, (Client*) gsmConnect.getClient());
            // mqttClient->init((Client*) gsmConnect.getClient());
            mqttClient->keepAlive(MQTT_USER, MQTT_PSSWD);
        }
            
        // Serial.println("Jopa9");
    }
    
    // Serial.println("Jopa10");

}

uint8_t GPSTracker::getStatus() {
    return status;
}


void GPSTracker::readGpsData() {
    GpsData gpsData = gpsClient->readGpsData();
    // gsmConnect.forceListen();
    // mqttClient->keepAlive(MQTT_USER, MQTT_PSSWD);
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
            // sdClient.writeLogString(gpsData.toString());
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
    jsonMessageBuilder.addElement("datetime", rtc.getDateTime());
    
    return jsonMessageBuilder.toString();
}

unsigned long GPSTracker::getSentCounter() {
    return coordSentCounter;
}

String GPSTracker::getStatusStr() {
    String status = "";
    status += "G: ";
    status += String(gsmConnect.GSMConnected()) + "-" + String(gsmConnect.GPRSConnected());
    status += " | M: ";
    if (mqttClient->isConnected()) status += "1";
    else 
        status += "0";
    status += " | S: " + String(getSentCounter());
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
    // Serial.print("STATUS = ");
    // Serial.println(getStatus());
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