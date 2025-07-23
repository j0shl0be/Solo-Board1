#include "matrix.h"
#include <Arduino.h>

// ------------------------ Hardware pin map ------------------------------
constexpr uint8_t ROW_PINS[3] = { 3, 4, 5 };  // inoputs w/ pull‑downs
constexpr uint8_t COL_PINS[3] = { 0, 1, 2 };  // outputs

static uint8_t keyState[3][3] = { { 0 } };  // 0=released (pull‑downs)
static uint32_t keyDebounceTime[3][3] = { { 0 } };
constexpr uint16_t DEBOUNCE_MS = 10;

void matrix_begin() {
  for (uint8_t c = 0; c < 3; ++c) {
    pinMode(COL_PINS[c], OUTPUT);
    for (uint8_t r = 0; r < 3; ++r) {
      pinMode(ROW_PINS[r], INPUT);
      keyState[c][r] = 0;
    }
  }
}

void matrix_scan(KeyEventCallback cb) {
  for (uint8_t c = 0; c < 3; ++c) {
    // Drive current row high, others low
    for (uint8_t ci = 0; ci < 3; ++ci) {
      digitalWrite(COL_PINS[ci], ci == c ? HIGH : LOW);
    }
    delayMicroseconds(3);

    for (uint8_t r = 0; r < 3; ++r) {
      uint8_t reading = digitalRead(ROW_PINS[r]);
      if (reading != keyState[c][r] && (millis() - keyDebounceTime[c][r]) > DEBOUNCE_MS) {
        keyDebounceTime[c][r] = millis();
        keyState[c][r] = reading;
        Serial.print("READ KEY in column");
        Serial.println(c);
        Serial.println(r);
        Serial.println(reading);
        cb(r, c, reading == 1);  // pressed if HIGH
      }
    }
  }
}

void matrix_scantest(KeyEventCallback cb) {
  uint8_t c = 0;
  // Drive current row high, others low
  digitalWrite(COL_PINS[c],HIGH);
  delayMicroseconds(3);

  for (uint8_t r = 0; r < 3; ++r) {
    uint8_t reading = digitalRead(ROW_PINS[r]);
    if (reading != keyState[c][r] && (millis() - keyDebounceTime[c][r]) > DEBOUNCE_MS) {
      keyDebounceTime[c][r] = millis();
      keyState[c][r] = reading;
      Serial.print("READ KEY in column");
      Serial.println(c);
      Serial.println(r);
      Serial.println(reading);
      cb(r, c, reading == 1);  // pressed if HIGH
    }
  }
}
