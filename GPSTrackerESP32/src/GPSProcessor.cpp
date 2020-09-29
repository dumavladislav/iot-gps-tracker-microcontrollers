#include "./Constants/Credentials.h"
#include "GPSProcessor.h"
#include <JsonMessageBuilder.h>

using namespace Dumsky;

GPSProcessor::GPSProcessor(int gpsProcessPeriod) {
    this->gpsProcessPeriod = gpsProcessPeriod;
    authorizationBlock.deviceId = DEVICE_ID;
    authorizationBlock.chipId = ESP.getEfuseMac();
    SerialGSM.begin(115200, SERIAL_8N1, GSM_RX_PIN, GSM_TX_PIN);

    delay(3000);
}

void GPSProcessor::init() {
    
    gsmConnect.init(&SerialGSM/*GSM_APN, GSM_GPRS_USER, GSM_GPRS_PASS*/);
    authorizationBlock.macAddress = gsmConnect.getIMEI();

    mqttClient = new MQTTClient((char*)authorizationBlock.deviceId.c_str());
    mqttClient->init(MQTT_SERVER, MQTT_PORT, (Client*) gsmConnect.getClient());
    status = 1;
    //gsmConnect.connect(GSM_APN, GSM_GPRS_USER, GSM_GPRS_PASS);


}

/*

uint8_t GPSProcessor::getStatus() {
    return status;
}

String GPSProcessor::getGpsDataJson(GpsData gpsData) {
    
    JsonMessageBuilder jsonMessageBuilder(authorizationBlock);

    jsonMessageBuilder.addElement("millis", String(millis()));
    jsonMessageBuilder.addElement("lat", String(gpsData.lat, 6));
    jsonMessageBuilder.addElement("lng", String(gpsData.lng, 6));
    jsonMessageBuilder.addElement("datetime", rtc.getDateTime());
    
    return jsonMessageBuilder.toString();
}

unsigned long GPSProcessor::getSentCounter() {
    return coordSentCounter;
}

String GPSProcessor::getStatusStr() {
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