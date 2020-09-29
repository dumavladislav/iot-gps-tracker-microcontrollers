
#include <DumskySDClient.h>
#include <DumskyGPSClient.h>
#include <GSMConnect.h>
#include <MQTTClient.h>
#include <DumskyOLED.h>
#include "Constants/Constants.h"
#include <Wire.h>
#include <RealTimeClock.h>


class GPSProcessor {

public:

    GPSProcessor(int gpsProcessPeriod);
    void init();
    void processGpsData(GpsData gpsData);
    String getGpsDataJson(GpsData gpsData);
    unsigned long getSentCounter();
    uint8_t getStatus();
    // String getStatusStr();
    // String getGpsStatusStr();
    // String getGpsDataStr();
    // void showStatus();
    

private:

    AuthorizationBlock authorizationBlock;

    ////////// STATUSES
    uint8_t status = 0;
    
    ////////// STATUSES

    HardwareSerial SerialGSM{1};
    GSMConnect gsmConnect{GSM_RX_PIN, GSM_TX_PIN};
    MQTTClient* mqttClient;
    Dumsky::SDClient sdClient;

    // unsigned long lastGpsScanTime = 0;
    int gpsProcessPeriod;

    GpsData prevGpsData;
//    GpsData lastSuccessfulGpsData;
    unsigned long coordCounter = 0;
    unsigned long coordSentCounter = 0;

};


