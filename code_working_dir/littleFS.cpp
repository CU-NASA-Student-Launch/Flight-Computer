// Chat GPT generated code

#include "LittleFS.h"

struct Sample {
  uint32_t t_ms;   // 4 bytes
  int16_t ax, ay, az; // 6 bytes
  int16_t gx, gy, gz; // 6 bytes
  // total = 16 bytes
};

const int BUFFER_SLOTS = 8;
Sample buf[BUFFER_SLOTS];
int buf_count = 0;

void setup() {
  Serial.begin(115200);
  if (!LittleFS.begin()) {
    Serial.println("LittleFS begin failed!");
    while (1);
  }
}

void flush_buffer() {
  File f = LittleFS.open("/flight.bin", "a");
  if (!f) { Serial.println("open fail"); return; }
  f.write((const uint8_t*)buf, buf_count * sizeof(Sample));
  f.close();
  buf_count = 0;
}

void loop() {
  // poll DMP / MPU6050 and fill a Sample s
  Sample s;
  s.t_ms = millis();
  // fill s.ax..s.gz from your MPU driver (example values)
  // s.ax = ...; s.ay = ...; ...

  buf[buf_count++] = s;
  if (buf_count >= BUFFER_SLOTS) flush_buffer();

  delay(250); // sample every 0.25s
}
