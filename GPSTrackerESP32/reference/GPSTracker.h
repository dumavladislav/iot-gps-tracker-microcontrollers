
#include <DumskySDClient.h>
#include <DumskyGPSClient.h>
#include <GSMConnect.h>
#include <MQTTClient.h>
#include <DumskyOLED.h>
#include "Constants/Constants.h"
#include <Wire.h>
#include <RealTimeClock.h>


class GPSTracker {

public:

    GPSTracker(int gpsScanPeriod);
    void init();
    uint8_t keepAlive();
    void readGpsData();
    String getGpsDataJson(GpsData gpsData);
    unsigned long getSentCounter();
    uint8_t getStatus();
    String getStatusStr();
    String getGpsStatusStr();
    String getGpsDataStr();
    void showStatus();
    

private:

    DumskyOLED oled;
    AuthorizationBlock authorizationBlock;

    ////////// STATUSES
    uint8_t status = 0;
    
    ////////// STATUSES

    HardwareSerial SerialGSM{1};
    HardwareSerial SerialGPS{2};
    GSMConnect gsmConnect{GSM_RX_PIN, GSM_TX_PIN};
    MQTTClient* mqttClient;
    GPSClient* gpsClient;
    Dumsky::SDClient sdClient;

    unsigned long lastGpsScanTime = 0;
    int gpsScanPeriod;

    GpsData prevGpsData;
//    GpsData lastSuccessfulGpsData;
    unsigned long coordCounter = 0;
    unsigned long coordSentCounter = 0;

    RealTimeClock rtc;
};


