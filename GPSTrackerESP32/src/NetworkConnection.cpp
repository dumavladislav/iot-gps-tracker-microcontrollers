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
    Serial.println("NetworkConnection::init()");
    gsmConnect.init(&SerialGSM);
    authorizationBlock.macAddress = gsmConnect.getIMEI();

    mqttClient = new MQTTClient((char*)authorizationBlock.deviceId.c_str());
    mqttClient->init(MQTT_SERVER, MQTT_PORT, (Client*) gsmConnect.getClient());
    Serial.println("NetworkConnection::init() DONE");
}

uint8_t NetworkConnection::keepAlive() {
    if (!mqttClient->isConnected())
    {
        this->setMqttStatus(0);
        Serial.println("NOT MQTT CONNECTED");
        gsmConnect.keepAlive(GSM_APN, GSM_GPRS_USER, GSM_GPRS_PASS);
        if (gsmConnect.GPRSConnected()){
            Serial.println("GPRS CONNECTED");
            this->setGsmStatus(2);
            mqttClient->keepAlive(MQTT_USER, MQTT_PSSWD);
        }
        else {
            if (gsmConnect.GSMConnected())  this->setGsmStatus(1);
            else this->setGsmStatus(0);
        }
    }
    else {
        Serial.println("MQTT CONNECTED");
        this->setMqttStatus(1);
    }
}

MQTTClient* NetworkConnection::getMqttClient() {
    return this->mqttClient;
}

uint8_t NetworkConnection::getGsmStatus() {
    // if (gsmConnect.GPRSConnected()) return 2;
    // if (gsmConnect.GSMConnected()) return 1;
    // return 0;
    return this->gsmStatus;
}

uint8_t NetworkConnection::getMqttStatus() {
    //return mqttClient->isConnected();
    return this->mqttStatus;
}

void NetworkConnection::setGsmStatus(uint8_t status) {
    // if (gsmConnect.GPRSConnected()) return 2;
    // if (gsmConnect.GSMConnected()) return 1;
    // return 0;
    this->gsmStatus = status;
}

void NetworkConnection::setMqttStatus(uint8_t status) {
    //return mqttClient->isConnected();
    this->mqttStatus = status;
}