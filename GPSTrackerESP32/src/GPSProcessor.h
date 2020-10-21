
#include <SDClient.h>
#include <DumskyGPSClient.h>
#include "Constants/Constants.h"


class GPSProcessor {

public:

    GPSProcessor(int gpsProcessPeriod);
    void init();
    void processGpsData(GpsData gpsData);
    String getGpsDataJson(GpsData gpsData);
    unsigned long getSentCounter();
    uint8_t getStatus();

private:
    ////////// STATUSES
    SDClient sdClient;

    // unsigned long lastGpsScanTime = 0;
    int gpsProcessPeriod;
};


