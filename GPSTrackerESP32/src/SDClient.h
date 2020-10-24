#pragma once
#include <SPI.h>
#include <SD.h>
#include <RTC3231.h>
#include <Constants/Constants.h>


class SDClient {

public:

    SDClient();
    void init(FS* fileStorage, RTC3231 *rtc);
    // String generateFileHeader();
    void listDir(const char * dirname, uint8_t levels);
    const char* getNextFileFromDir(const char * dirname);
    // void createFile(String filename);
    // void moveFile(String filename, String filenameTarget);
    void removeFile(String fullFilePath);
    void writeLogString(String logString, String filename);

private:
    fs::FS *fileStorage;
    int filesCounter = 0;
    int recordsCounter = 0;
    int recordsInFileCounter = 0;

    const int maxRecordsInFile = 10000;

    File root;
    File fileCounterFile;
    // File dirRoot;
    // File file;

    RTC3231 *rtc;

    const String deviceId = "GPS1";
};
