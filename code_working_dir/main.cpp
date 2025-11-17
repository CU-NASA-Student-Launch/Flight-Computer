#include <Arduino.h>
#include "SampleBuffer.h"

bool recordingEnabled;
void setup() {
    recordingEnabled = true;
    Serial.begin(115200);
    delay(3000);
}

void loop() {
    if (recordingEnabled) {
        SampleBuffer buffer;

        for(int i = 0; i < 50; ++i) {
            buffer.Collect();
            if (buffer.Full()) {
                buffer.Flush();
            }
            delay(100);
        }

        Serial.println("Done");
        buffer.StreamFSData();
        recordingEnabled = false;
    }
}