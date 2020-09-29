# include <GPSTrackerStatus.h>

GPSTrackerStatus::GPSTrackerStatus() {
    setGPSInitialized(false);
    setGSMInitialized(false);
    setSDInitialized(false);
    setRTCInitialized(false);
    setOLEDInitialized(false);
}

bool GPSTrackerStatus::isGPSInitialized() {
    return this->GPSInitialized;
}

void GPSTrackerStatus::setGPSInitialized(bool initialized) {
    this->GPSInitialized = initialized;
}

bool GPSTrackerStatus::isGSMInitialized() {
    return this->GSMInitialized;
}
void GPSTrackerStatus::setGSMInitialized(bool initialized) {
    this->GSMInitialized = initialized;
}

bool GPSTrackerStatus::isSDInitialized() {
    return this->SDInitialized;
}

void GPSTrackerStatus::setSDInitialized(bool initialized) {
    this->SDInitialized = initialized;
}

bool GPSTrackerStatus::isRTCInitialized() {
    return this->RTCInitialized;
}

void GPSTrackerStatus::setRTCInitialized(bool initialized) {
    this->RTCInitialized = initialized;
}

bool GPSTrackerStatus::isOLEDInitialized() {
    return this->OLEDInitialized;
}

void GPSTrackerStatus::setOLEDInitialized(bool initialized) {
    this->OLEDInitialized = initialized;
}