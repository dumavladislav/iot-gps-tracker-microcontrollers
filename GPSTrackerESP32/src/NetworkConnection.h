
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
    void setGsmStatus(uint8_t status);          // 0 - No Connection, 1 - GSM only, 2 - GPRS
    void setMqttStatus(uint8_t status);         // 0 - Not Connected, 1 - Connected
    uint8_t getGsmStatus();                     // 0 - No Connection, 1 - GSM only, 2 - GPRS
    uint8_t getMqttStatus();                    // 0 - Not Connected, 1 - Connected
  

private:

    AuthorizationBlock authorizationBlock;

    HardwareSerial SerialGSM{1};
    GSMConnect gsmConnect{GSM_RX_PIN, GSM_TX_PIN};
    MQTTClient* mqttClient;

    uint8_t gsmStatus = 0;
    uint8_t mqttStatus = 0; 

    unsigned long coordSentCounter = 0;
};


