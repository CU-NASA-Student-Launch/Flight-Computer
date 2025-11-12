#include <Arduino.h>
#include "LittleFS.h"

extern uint8_t _FS_start;
extern uint8_t _FS_end;


struct Sample {
  uint32_t t_ms;   // 4 bytes
  uint8_t x; // 1 bytes
  // total = 5 bytes
};

const int BUFFER_SLOTS = 8;
Sample buf[BUFFER_SLOTS];
int buf_count = 0;
int counter = 0;

void setup() {
  delay(5000);
  Serial.begin(115200);

  uint8_t start = 250;
  uint8_t end = 255;

  if (!LittleFS.begin()) {
    Serial.println("LittleFS begin failed!");
    while (1);
  }

  File f = LittleFS.open("/flight.csv", "a");
  if (!f) { Serial.println("open fail"); return; }
  f.println("time(ms), count");
  f.close();
}

void flush_buffer() {
  File f = LittleFS.open("/flight.csv", "a");
  if (!f) { Serial.println("open fail"); return; }
  // f.write((const uint8_t*)buf, buf_count * sizeof(Sample));
  for (int i = 0; i < buf_count; ++i) {
    f.printf("%ld,%d\n", buf[i].t_ms, buf[i].x);
  }
  f.close();
  buf_count = 0;
}

void streamFSData() {
  Serial.begin(115200);
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

void loop() {
  Sample s;
  s.t_ms = millis();
  s.x = counter;
  // Serial.println("counter value: " + String(s.x));
  counter++;

  buf[buf_count++] = s;
  if (buf_count >= BUFFER_SLOTS) flush_buffer();

  delay(250); // sample every 0.25s

  if (counter == 10) {
    // Done logging
    // Time to read
    streamFSData();
    while(1) {
    }
  }
}
