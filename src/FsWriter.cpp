#include <Arduino.h>
#include <array>
#include "LittleFS.h"
#include <SensorData.hpp>
#include <FsWriter.hpp>

FsWriter::FsWriter() {
    if (!LittleFS.begin()) {
        Serial.println("LittleFS begin failed!");
        while (1);
    }

    // clearing out old flight data
    if (LittleFS.exists("/flight.csv") && !LittleFS.remove("/flight.csv")) {
        Serial.println("Unable to remove old flight.csv");
    }

    File f = LittleFS.open("/flight.csv", "a");
    if (!f) { Serial.println("file open fail"); return; }
    f.println("time(ms), accelX(m/s^2), accelY(m/s^2), accelZ(m/s^2), gyroX(rad/sec), gyroY(rad/sec), gyroZ(rad/sec), temp (Celsius), pressure (Pa), altitude (m)");
    f.close();
}

FsWriter::~FsWriter() 
{

}

void FsWriter::flush() {
    File f = LittleFS.open("/flight.csv", "a");
    if (!f) { Serial.println("file open fail"); return; }

    // flushing whatever is in the buffer to the file
    for (int i = 0; i < currBuffLoc; i++) {
        f.printf("%ld,", sensorDataBuff[i].t_ms);
        f.printf("%f,", sensorDataBuff[i].accelX);
        f.printf("%f,", sensorDataBuff[i].accelY);
        f.printf("%f,", sensorDataBuff[i].accelZ);
        f.printf("%f,", sensorDataBuff[i].gyroX);
        f.printf("%f,", sensorDataBuff[i].gyroY);
        f.printf("%f,", sensorDataBuff[i].gyroZ);
        f.printf("%f,", sensorDataBuff[i].temp);
        f.printf("%f,", sensorDataBuff[i].pressure);
        f.printf("%f\n", sensorDataBuff[i].altitude);
    }
    f.close();
}

void FsWriter::store(SensorData &record) 
{
    // Check if the buffer is full before writing
    if(currBuffLoc == sensorDataBuff.size())
    {
        this->flush();
        currBuffLoc = 0;
    }
    
    sensorDataBuff[currBuffLoc] = record;
    currBuffLoc++;
}

void FsWriter::streamFSData() {
  while (!Serial) delay(10);

  if (!LittleFS.begin()) {
    Serial.println("Mount failed!");
    return;
  }

  File f = LittleFS.open("/flight.csv", "r");
  if (!f) {
    Serial.println("File not found!");
    return;
  }

  Serial.println(F("BEGIN_FILE"));
  while (f.available()) {
    uint8_t b = f.read();
    Serial.write(b);      // raw binary stream
  }
  f.close();
  Serial.println(F("END_FILE"));
}