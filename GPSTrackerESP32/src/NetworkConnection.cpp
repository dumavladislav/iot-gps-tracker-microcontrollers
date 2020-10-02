#include "./Constants/Credentials.h"
#include "NetworkConnection.h"
#include <JsonMessageBuilder.h>

NetworkConnection::NetworkConnection() {
    authorizationBlock.deviceId = DEVICE_ID;
    authorizationBlock.chipId = ESP.getEfuseMac();
    SerialGSM.begin(115200, SERIAL_8N1, GSM_RX_PIN, GSM_TX_PIN);

    delay(3000);
}

void NetworkConnection::init() {
    gsmConnect.init(&SerialGSM);
    authorizationBlock.macAddress = gsmConnect.getIMEI();

    mqttClient = new MQTTClient((char*)authorizationBlock.deviceId.c_str());
    mqttClient->init(MQTT_SERVER, MQTT_PORT, (Client*) gsmConnect.getClient());
}

uint8_t NetworkConnection::keepAlive() {
    if (!mqttClient->isConnected())
    {
        Serial.println("NOT MQTT CONNECTED");
        gsmConnect.keepAlive(GSM_APN, GSM_GPRS_USER, GSM_GPRS_PASS);
        if (gsmConnect.GPRSConnected()){
            Serial.println("GPRS CONNECTED");
            mqttClient->keepAlive(MQTT_USER, MQTT_PSSWD);
        }
    }
    else    Serial.println("MQTT CONNECTED");
}

MQTTClient* NetworkConnection::getMqttClient() {
    return this->mqttClient;
}