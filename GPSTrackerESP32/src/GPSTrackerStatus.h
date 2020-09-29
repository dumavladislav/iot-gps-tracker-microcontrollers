#include <Arduino.h>

class GPSTrackerStatus {

public:

    // void setStatus(uint8_t status);
    // uint8_t getStatus();

    GPSTrackerStatus();

    bool isGPSInitialized();
    void setGPSInitialized(bool initialized);
    bool isGSMInitialized();
    void setGSMInitialized(bool initialized);
    bool isSDInitialized();
    void setSDInitialized(bool initialized);
    bool isRTCInitialized();
    void setRTCInitialized(bool initialized);
    bool isOLEDInitialized();
    void setOLEDInitialized(bool initialized);


private:

    // uint8_t status = 0;
    bool GPSInitialized = false;
    bool GSMInitialized = false;
    bool SDInitialized = false;
    bool RTCInitialized = false;
    bool OLEDInitialized = false;

};