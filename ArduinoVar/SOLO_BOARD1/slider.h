#pragma once
#include <stdint.h>
#include "Arduino.h"
#include "display.h"
#include "keymap.h"
#include "Adafruit_TinyUSB.h"

enum SliderFunction: uint8_t{
  Volume,
  Brightness
};
constexpr uint8_t POT_PIN = A0;
constexpr int VOLUME_STEPS = 50;        // Granularity of volume mapping
constexpr int VOL_CHANGE_THRESHOLD = 3; // ADC delta noise filter
constexpr uint8_t ADC_ACCURACY = 12;  //ADC bit depth
extern volatile Layer curLayer;
extern int lastPotValue;
extern int lastVolumeStep;
extern int lastVolumePercent;

void slider_begin();
uint8_t get_slider_percent();
uint16_t get_slider_raw();
uint8_t calibrate_vol(uint8_t volume);
void send_volume_change(uint8_t volume);