
#include <DumskyGPSClient.h>
#include "Constants/Constants.h"
#include <MQTTClient.h>
#include <Wire.h>
#include <RealTimeClock.h>

class GPSReader {

public:

    GPSReader(int gpsScanPeriod);
    void init();
    GpsData readGpsData();
    // String getGpsDataJson(GpsData gpsData);

private:

    HardwareSerial SerialGPS{2};
    GPSClient* gpsClient;

    unsigned long lastGpsScanTime = 0;
    int gpsScanPeriod;

    GpsData prevGpsData;
    unsigned long coordCounter = 0;

    RealTimeClock rtc;
};


