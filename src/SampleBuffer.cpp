#include "SampleBuffer.h"

#include <Arduino.h>
#include "LittleFS.h"

SampleBuffer::SampleBuffer() {
    if (!LittleFS.begin()) {
        Serial.println("LittleFS begin failed!");
        while (1);
    }

    if (LittleFS.exists("/flight.csv") && !LittleFS.remove("/flight.csv")) {
        Serial.println("Unable to remove old flight.csv");
    }
    
    BUFFER_SIZE = 8;
    buffer = new Record[BUFFER_SIZE];
    remainingCapacity = BUFFER_SIZE;

    File f = LittleFS.open("/flight.csv", "a");
    if (!f) { Serial.println("file open fail"); return; }
    f.println("time(ms), count");
    f.close();
}

SampleBuffer::~SampleBuffer() {
    delete[] buffer;
}

void SampleBuffer::Flush() {
    File f = LittleFS.open("/flight.csv", "a");
    if (!f) { Serial.println("file open fail"); return; }

    // flushing whatever is in the buffer to the file
    for (int i = BUFFER_SIZE - remainingCapacity - 1; i >= 0; --i) {
        f.printf("%ld,%f\n", buffer[i].t_ms, buffer[i].x);
    }
    f.close();
    remainingCapacity = BUFFER_SIZE;
}

void SampleBuffer::Collect() {
    if (remainingCapacity != 0) {
        Serial.println("Collecting");
        record.t_ms = millis();
        record.x = 1234.1;
        buffer[BUFFER_SIZE - remainingCapacity] = record;
        remainingCapacity--;
    }
}

bool SampleBuffer::Full() {
    return (remainingCapacity == 0);
}

void SampleBuffer::StreamFSData() {
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