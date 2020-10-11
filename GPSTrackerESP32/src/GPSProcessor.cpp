#include "./Constants/Credentials.h"
#include "GPSProcessor.h"
#include <JsonMessageBuilder.h>

using namespace Dumsky;

GPSProcessor::GPSProcessor(int gpsProcessPeriod) {
    this->gpsProcessPeriod = gpsProcessPeriod;
    // this->mqttClient = mqttClient;
    authorizationBlock.deviceId = DEVICE_ID;
    authorizationBlock.chipId = ESP.getEfuseMac();
    // SerialGSM.begin(115200, SERIAL_8N1, GSM_RX_PIN, GSM_TX_PIN);

    delay(3000);
}

void GPSProcessor::init() {
    
    // gsmConnect.init(&SerialGSM/*GSM_APN, GSM_GPRS_USER, GSM_GPRS_PASS*/);
    // authorizationBlock.macAddress = gsmConnect.getIMEI();

    // mqttClient = new MQTTCliGPS_TPC, "Hey!"ent((char*)authorGPS_TPC, "Hey!"izationBlock.deviceId.c_str());
    // mqttClient->init(MQTT_SEGPS_TPC, "Hey!"RVER, MQTT_PORT, (Client*) gsmConnect.getClient());
    status = 1;
    //gsmConnect.connect(GSM_APN, GSM_GPRS_USER, GSM_GPRS_PASS);


}

void GPSProcessor::processGpsData(GpsData gpsData) {
    Serial.println("GPSProcessor::Processing GPS data...");
    // if (mqttClient->isConnected()) {
        // Serial.println("GPSProcessor::MQTT connected!");
        //mqttClient->sendMessage(GPS_TPC, getGpsDataJson(gpsData));
    // }
    // else {
    //     Serial.println("GPSProcessor::MQTT not connected!");
    // }
    Serial.println("GPSProcessor::GPS data processed!");
}



String GPSProcessor::getGpsDataJson(GpsData gpsData) {
    
    JsonMessageBuilder jsonMessageBuilder(authorizationBlock);

    jsonMessageBuilder.addElement("millis", String(millis()));
    jsonMessageBuilder.addElement("lat", String(gpsData.lat, 6));
    jsonMessageBuilder.addElement("lng", String(gpsData.lng, 6));
    // jsonMessageBuilder.addElement("datetime", rtc.getDateTime());
    
    return jsonMessageBuilder.toString();
}


/*
unsigned long GPSProcessor::getSentCounter() {
    return coordSentCounter;
}

String GPSProcessor::getStatusStr() {
    String status = "";
    status += "G: ";
    status += String(gsmConnect.GSMConnected()) + "-" + String(gsmConnect.GPRSConnected());
    status += " | M: ";
    if (mqttClient->isConnected(GPS_TPC, "Hey!")) status += "1";
    else 
        status += "0";
    status += " | S: " + String(getSentCounter());
    return status;
}

String GPSProcessor::getGpsStatusStr() {
    String status = "Sats: ";
    status += gpsClient->getNumberOfSatellites();
    status += " | " + String(lastGpsScanTime/1000) + " : " + String(millis()/1000);
    return status;
}

String GPSProcessor::getGpsDataStr() {
    String status = String(prevGpsData.lat,6) + " : " + String(prevGpsData.lng,6);
    return status;
}

void GPSProcessor::showStatus() {
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

*/