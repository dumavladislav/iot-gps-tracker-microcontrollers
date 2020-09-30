#include "./Constants/Credentials.h"
#include "NetworkConnection.h"
#include <JsonMessageBuilder.h>

using namespace Dumsky;

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
        gsmConnect.keepAlive(GSM_APN, GSM_GPRS_USER, GSM_GPRS_PASS);
        if (gsmConnect.GPRSConnected()){
            mqttClient->keepAlive(MQTT_USER, MQTT_PSSWD);
        }
    }
}

MQTTClient* NetworkConnection::getMqttClient() {
    return this->mqttClient;
}