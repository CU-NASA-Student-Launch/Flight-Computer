#include <Arduino.h>
#include "SampleBuffer.h"


void setup() {
    Serial.begin(115200);
    delay(3000);
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
}

void loop() {
}