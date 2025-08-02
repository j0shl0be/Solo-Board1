#pragma once
#include <stdint.h>
#include "Arduino.h"
#include "display.h"
#include "keymap.h"
#include "leds.h"
#include "Adafruit_TinyUSB.h"
#include <EEPROM.h>

constexpr uint8_t POT_PIN = A0;
constexpr int VOLUME_STEPS = 50;        // Granularity of volume mapping
constexpr int VOL_CHANGE_THRESHOLD = 6; // ADC delta noise filter
constexpr uint8_t ADC_ACCURACY = 12;  //ADC bit depth

// EEPROM addresses for persistent storage
constexpr int BRIGHTNESS_EEPROM_ADDR = 0;
constexpr uint8_t DEFAULT_BRIGHTNESS = 20;

extern volatile Layer curLayer;
extern int lastPotValue;
extern int lastVolumeStep;
extern int lastVolumePercent;
extern uint8_t globalBrightness;

void slider_begin();
uint8_t get_slider_percent();
uint16_t get_slider_raw();
uint8_t calibrate_vol(uint8_t volume);
void send_volume_change(uint8_t volume);
uint8_t load_brightness();
void save_brightness(uint8_t brightness);
void update_global_brightness(uint8_t newBrightness);