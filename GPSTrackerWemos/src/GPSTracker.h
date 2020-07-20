
// #include <DumskySDClient.h>
#include <DumskyGPSClient.h>
#include <GSMConnect.h>
#include <MQTTClient.h>
#include <DumskyOLED.h>
#include "Constants/Constants.h"

class GPSTracker {

public:

    GPSTracker(int gpsScanPeriod);
    void init();
    void keepAlive();
    void gsmKeepAlive();
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

    uint8_t status = 0;
    AuthorizationBlock authorizationBlock;

    SoftwareSerial* gpsSoftwareSerial;

    GPSClient* gpsClient;
    // Dumsky::SDClient sdClient;

    unsigned long lastGpsScanTime = 0;
    int gpsScanPeriod;

    GpsData prevGpsData;
//    GpsData lastSuccessfulGpsData;
    unsigned long coordCounter = 0;
    unsigned long coordSentCounter = 0;

    GSMConnect gsmConnect{GSM_RX_PIN, GSM_TX_PIN};
    MQTTClient* mqttClient;
};


