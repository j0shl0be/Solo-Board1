#include "matrix.h"
#include <Arduino.h>

// ------------------------ Hardware pin map ------------------------------
constexpr uint8_t ROW_PINS[3] = {0, 1, 2}; // Outputs
constexpr uint8_t COL_PINS[3] = {3, 4, 5}; // Inputs w/ pull‑ups

static uint8_t keyState[3][3] = {{1}}; // 1=released (pull‑up)
static uint32_t keyDebounceTime[3][3] = {{0}};
constexpr uint16_t DEBOUNCE_MS = 10;

void matrix_begin()
{
    for (uint8_t r = 0; r < 3; ++r)
    {
        pinMode(ROW_PINS[r], OUTPUT);
        for (uint8_t c = 0; c < 3; ++c)
        {
            pinMode(COL_PINS[c], INPUT_PULLUP);
            keyState[r][c] = 1;
        }
    }
}

void matrix_scan(KeyEventCallback cb)
{
    for (uint8_t r = 0; r < 3; ++r)
    {
        // Drive current row high, others low
        for (uint8_t ri = 0; ri < 3; ++ri)
            digitalWrite(ROW_PINS[ri], ri == r ? HIGH : LOW);
        delayMicroseconds(3);

        for (uint8_t c = 0; c < 3; ++c)
        {
            uint8_t reading = digitalRead(COL_PINS[c]);
            if (reading != keyState[r][c] && (millis() - keyDebounceTime[r][c]) > DEBOUNCE_MS)
            {
                keyDebounceTime[r][c] = millis();
                keyState[r][c] = reading;
                cb(r, c, reading == 1); // pressed if HIGH
            }
        }
    }
}
