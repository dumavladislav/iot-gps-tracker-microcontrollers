
#include <GSMConnect.h>
#include <MQTTClient.h>
#include "Constants/Constants.h"
//#include <Wire.h>
//#include <RealTimeClock.h>


class NetworkConnection {

public:

    NetworkConnection();
    void init();
    uint8_t keepAlive();
    MQTTClient* getMqttClient();
  

private:

    AuthorizationBlock authorizationBlock;

    HardwareSerial SerialGSM{1};
    GSMConnect gsmConnect{GSM_RX_PIN, GSM_TX_PIN};
    MQTTClient* mqttClient;

    unsigned long coordSentCounter = 0;
};


