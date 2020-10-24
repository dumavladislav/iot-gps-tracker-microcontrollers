#include <Arduino.h>
#include <SDClient.h>

SDClient::SDClient() {

}

void SDClient::init(FS* fileStorage, RTC3231 *rtc) {
  this->fileStorage = fileStorage;
  this->rtc = rtc;

  // Serial.print("Initializing SD card...");

  // if (!fileStorage->begin(csPin)) {
  //   Serial.println("SD initialization failed!");
  //   while (1);
  // }
  // Serial.println("SD initialization done.");

  if(!fileStorage->exists("/to_mqtt")) {
    fileStorage->mkdir("/to_mqtt");
  }

  // if(!fileStorage->exists(SD_LOGS_DIR)) {
  //   fileStorage->mkdir(SD_LOGS_DIR);
  // }

  root = fileStorage->open("/");
  File file;
  while(file = root.openNextFile()) {
    if (!file.isDirectory()) {
      Serial.print("FILE: ");
      Serial.println(file.name());
    }
    else {
      Serial.print("DIR:  ");
      Serial.println(file.name());
    }
  }

  /*
  if (fileStorage->exists("/fcntr.txt")) {
    Serial.println("Reading fileCounter");
    File fileCounterFile = fileStorage->open("/fcntr.txt");

    // if the file is available, write to it:
    if (fileCounterFile) {
      int i = 0;
      byte fileContent[20];
      while (fileCounterFile.available()) {
        fileContent[i] = fileCounterFile.read();
        i++;
        //Serial.write(dataFile.read());
      }
      fileCounterFile.close();
      if(i > 0) filesCounter = atoi((char*)fileContent);
      Serial.println("Files Counter:");
      Serial.println(filesCounter);
    }
    // if the file isn't open, pop up an error:
    else {
      Serial.println("error opening fcntr.txt");
    }
  }
  else { 
    Serial.println("Creating fcntr");
    filesCounter = 0;
    fileCounterFile = fileStorage->open("/fcntr.txt", FILE_WRITE);
    fileCounterFile.println(filesCounter);
    fileCounterFile.close();
    if (fileStorage->exists("/fcntr.txt")) Serial.println("fcntr.txt created");
  }
  */
}

/*
String SDClient::generateFileHeader() {
  String logStringTTT = "";
  logStringTTT += String("recNum");
  logStringTTT += String(",");
  logStringTTT += String("millis");
  logStringTTT += String(",");
  logStringTTT += String("lat ");
  logStringTTT += String(",");
  logStringTTT += String("lng ");
  logStringTTT += String(",");
  logStringTTT += String("date ");
  logStringTTT += String(",");
  logStringTTT += String("time ");
  return logStringTTT;
}
*/

void SDClient::writeLogString(String logString, String filename) {

    recordsCounter++;
    recordsInFileCounter++;

    if(recordsInFileCounter > maxRecordsInFile) {
        filesCounter++;
        fileStorage->remove("/fcntr.txt");
        fileCounterFile = fileStorage->open("/fcntr.txt", FILE_WRITE);
        if (fileCounterFile) {
          fileCounterFile.print(filesCounter);
          fileCounterFile.close();
          Serial.println("Files COUNTER updated!!!");
        }
        // if the file isn't open, pop up an error:
        else {
          Serial.println("error opening fcntr.txt");
        }
        recordsInFileCounter = 1;
      }
      
      File dataFile = fileStorage->open(String("/") + (String(deviceId)+String("_")+String(filesCounter) + String(".txt")).c_str(), FILE_WRITE);

      // if the file is available, write to it:
      if (dataFile) {
        // if(recordsInFileCounter == 1) 
        // { 
        //   Serial.println(generateFileHeader());
        //   dataFile.println(generateFileHeader());
        // }

        dataFile.println(logString);
        dataFile.close();
        // print to the serial port too:
        Serial.println(logString);
      }
      // if the file isn't open, pop up an error:
      else {
        Serial.println("error opening datalog.txt");
      }
}

void SDClient::listDir(const char * dirname, uint8_t levels) {
  
  Serial.printf("Listing directory: %s\n", dirname);
  
  File dirRoot = fileStorage->open(dirname);
  if(!dirRoot){
    Serial.println("Failed to open directory");
    return;
  }
  if(!dirRoot.isDirectory()){
    Serial.println("Not a directory");
    return;
  }
  
  File file = dirRoot.openNextFile();
  while(file){
    if(file.isDirectory()){
      Serial.print(" DIR : ");
      Serial.println(file.name());
      if(levels){
        listDir(file.name(), levels -1);
      }
    } else {
      Serial.print(" FILE: ");
      Serial.print(file.name());
      Serial.print(" SIZE: ");
      Serial.println(file.size());
    }
    file = dirRoot.openNextFile();
  }
}

const char* SDClient::getNextFileFromDir(const char * dirname) {
  
  // Serial.printf("Getting next file from directory: %s\n", dirname);
  
  File dirRoot = fileStorage->open(dirname);
  if(!dirRoot){
    Serial.println("Failed to open directory");
    return NULL;
  }
  if(!dirRoot.isDirectory()){
    Serial.println("Not a directory");
    return NULL;
  }
  
  File file = dirRoot.openNextFile();
  while(file){
    // Serial.print("File Found: ");
    // Serial.println(file.name());
    if(!file.isDirectory()){
      // Serial.println("YEAH! It's a file!!!");
      return file.name();
    }
    file = dirRoot.openNextFile();
  }
  return NULL;
}

void SDClient::removeFile(String fullFilePath) {
  Serial.print("File to be REMOVED: ");
  Serial.println(fullFilePath);
  fileStorage->remove(fullFilePath);
}