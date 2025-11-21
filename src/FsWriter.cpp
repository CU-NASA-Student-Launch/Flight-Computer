#include <Arduino.h>
#include "LittleFS.h"
#include "Record.hpp"
#include "FsWriter.hpp"

FsWriter::FsWriter() {
    if (!LittleFS.begin()) {
        Serial.println("LittleFS begin failed!");
        while (1);
    }

    BUFFER_SIZE = 8;
    buffer = new Record[BUFFER_SIZE];
    remainingCapacity = BUFFER_SIZE;

    File f = LittleFS.open("/flight.csv", "a");
    if (!f) { Serial.println("file open fail"); return; }
    f.println("time(ms), accelX(m/s^2), accelY(m/s^2), accelZ(m/s^2), gyroX(rad/sec), gyroY(rad/sec), gyroZ(rad/sec), temp (Celsius), pressure (Pa), altitude (m)");
    f.close();
}

FsWriter::~FsWriter() {
    delete[] buffer;
}

void FsWriter::flush() {
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

void FsWriter::store(Record &record) {
    if (remainingCapacity != 0) {
        record.t_ms = millis();
        buffer[BUFFER_SIZE - remainingCapacity] = record;
        remainingCapacity--;
    }
}

bool FsWriter::full() {
    return (remainingCapacity == 0);
}

void FsWriter::streamFSData() {
    int GET[] = {71, 69, 84};

    Serial.println("Ready");

    while(1) {
        while (!Serial) delay(10);

        String word = Serial.readStringUntil('\n');
        
        if (word == "GET") {
            Serial.println("ACK: GET");
        
            // stream file

            if (!LittleFS.begin()) {
                Serial.println("Mount failed!");
                return;
            }

            if (LittleFS.exists("/flight.csv")) {
                File f = LittleFS.open("/flight.csv", "r");
                if (!f) {
                    Serial.println("FILE_NOT_FOUND");
                    return;
                }

                Serial.println(F("BEGIN_FILE"));
                while (f.available()) {
                    uint8_t b = f.read();
                    Serial.write(b);      // raw binary stream
                }
                f.close();
                Serial.println(F("END_FILE"));

                // ideally this should wait for confirmation from client
                bool removal = LittleFS.remove("/flight.csv");
                
                if (!removal) {
                    Serial.println(F("Unable to remove old flight.csv"));
                }
                else {
                    Serial.println(F("Removed flight.csv from flash"));
                }
            }
            else {
                Serial.println(F("FILE_NOT_FOUND"));
            }
        
        }
    }
}