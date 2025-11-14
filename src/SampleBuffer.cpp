#include <Arduino.h>
#include "LittleFS.h"
#include "Record.hpp"
#include "SampleBuffer.hpp"

SampleBuffer::SampleBuffer() {
    if (!LittleFS.begin()) {
        Serial.println("LittleFS begin failed!");
        while (1);
    }

    // clearing out old flight data
    if (LittleFS.exists("/flight.csv") && !LittleFS.remove("/flight.csv")) {
        Serial.println("Unable to remove old flight.csv");
    }
    
    BUFFER_SIZE = 8;
    buffer = new Record[BUFFER_SIZE];
    remainingCapacity = BUFFER_SIZE;

    File f = LittleFS.open("/flight.csv", "a");
    if (!f) { Serial.println("file open fail"); return; }
    f.println("time(ms), accelX(m/s^2), accelY(m/s^2), accelZ(m/s^2), gyroX(rad/sec), gyroY(rad/sec), gyroZ(rad/sec), temp (Celsius), pressure (Pa), altitude (m)");
    f.close();
}

SampleBuffer::~SampleBuffer() {
    delete[] buffer;
}

void SampleBuffer::flush() {
    File f = LittleFS.open("/flight.csv", "a");
    if (!f) { Serial.println("file open fail"); return; }

    // flushing whatever is in the buffer to the file
    for (int i = BUFFER_SIZE - remainingCapacity - 1; i >= 0; --i) {
        f.printf("%ld,", buffer[i].t_ms);
        f.printf("%f,", buffer[i].accelX);
        f.printf("%f,", buffer[i].accelY);
        f.printf("%f,", buffer[i].accelZ);
        f.printf("%f,", buffer[i].gyroX);
        f.printf("%f,", buffer[i].gyroY);
        f.printf("%f,", buffer[i].gyroZ);
        f.printf("%f,", buffer[i].temp);
        f.printf("%f,", buffer[i].pressure);
        f.printf("%f\n", buffer[i].altitude);
    }
    f.close();
    remainingCapacity = BUFFER_SIZE;
}

void SampleBuffer::store(Record &record) {
    if (remainingCapacity != 0) {
        record.t_ms = millis();
        buffer[BUFFER_SIZE - remainingCapacity] = record;
        remainingCapacity--;
    }
}

bool SampleBuffer::full() {
    return (remainingCapacity == 0);
}

void SampleBuffer::streamFSData() {
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