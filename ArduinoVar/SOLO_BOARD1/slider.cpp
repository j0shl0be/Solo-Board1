#include "slider.h"
#include "keymap.h"
#include "leds.h"

int lastPotValue = 0;
int lastVolumeStep = -1;
int lastVolumePercent = 0;
bool serialConnected = false;
uint8_t globalBrightness = DEFAULT_BRIGHTNESS;

void slider_begin() {
  analogReadResolution(ADC_ACCURACY);
  lastPotValue = analogRead(POT_PIN);
  uint8_t percent = get_slider_percent();
  display_show(curLayer, percent);
  
  // Initialize EEPROM if needed
  EEPROM.begin(256);
  
  // Load saved brightness from EEPROM
  globalBrightness = load_brightness();
  
  serialConnected = TinyUSBDevice.mounted() && (bool)Serial;
}

uint8_t get_slider_percent() {//returns a calibrated volume
  uint16_t potValue = get_slider_raw();
  if (abs(lastPotValue - potValue) > VOL_CHANGE_THRESHOLD) {
    lastPotValue = potValue;
  }
  uint8_t percent = calibrate_vol(map(lastPotValue, 0, 4095, 0, 100));
  
  // Protect against values > 100 due to potentiometer contact issues
  if (percent > 100) {
    percent = 100;
  }
  
  return percent;
}

uint16_t get_slider_raw() {
  return (uint16_t)analogRead(POT_PIN);
}

uint8_t calibrate_vol(uint8_t uncalibrated) {
  if (uncalibrated > 32) {
    return (33 - (uncalibrated / 3.0));
  } else {
    return (1210.2 * pow(uncalibrated, -1.13224));
  }
}

void send_volume_change(uint8_t percent) {//sends the calibrated volume out if it is a larger difference than the threshold
  // Additional protection: clamp percent to 0-100 range
  if (percent > 100) {
    percent = 100;
  }
  
  int step = map(percent, 0, 100, 0, VOLUME_STEPS);  // use VOLUME_STEPS = number of HID sends
  if (step != lastVolumeStep) {
    lastVolumeStep = step;
    // Refresh serial connection state before calling callback
    serialConnected = TinyUSBDevice.mounted() && (bool)Serial;
    // Call per-layer slider callback
    SliderCallback cb = getCurrentSliderCallback();
    if (cb) cb(percent);
  }
  return;
}

uint8_t load_brightness() {
  uint8_t savedBrightness = EEPROM.read(BRIGHTNESS_EEPROM_ADDR);
  
  // Validate saved brightness (should be between 0-100)
  if (savedBrightness > 100) {
    savedBrightness = DEFAULT_BRIGHTNESS;
    save_brightness(savedBrightness);
  }
  
  return savedBrightness;
}

void save_brightness(uint8_t brightness) {
  EEPROM.write(BRIGHTNESS_EEPROM_ADDR, brightness);
  EEPROM.commit();
}

void update_global_brightness(uint8_t newBrightness) {
  globalBrightness = newBrightness;
  save_brightness(globalBrightness);
  leds_brightness(globalBrightness);
}