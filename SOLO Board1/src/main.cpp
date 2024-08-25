#include <Arduino.h>
#include "Adafruit_TinyUSB.h"

uint8_t D0 = 0;
uint8_t D1 = 0;
uint8_t D2 = 0;
uint8_t D3 = 0;
uint8_t D4 = 0;
uint8_t D5 = 0;

uint8_t setpins[] = {D0, D1, D2};
uint8_t readpins[] = {D3, D4, D5};

void setup() {
    TinyUSBDevice.begin(0);


}

void loop() {

}
