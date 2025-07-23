#include "slider.h"

int lastPotValue = 0;
int lastVolumeStep = -1;
int lastVolumePercent = 0;

void slider_begin() {
  analogReadResolution(ADC_ACCURACY);
  lastPotValue = analogRead(POT_PIN);
  display_show(curLayer, map(lastPotValue, 0, 4095, 0, 100));
}

uint8_t get_slider_percent() {//returns a calibrated volume
  uint16_t potValue = get_slider_raw();
  if (abs(lastPotValue - potValue) > VOL_CHANGE_THRESHOLD) {
    lastPotValue = potValue;
  }
  uint8_t percent = calibrate_vol(map(lastPotValue, 0, 4095, 0, 100));
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
  //if (!tud_hid_ready())
  //  Serial.println("USB not ready");
  //  return;
  int step = map(percent, 0, 100, 0, VOLUME_STEPS);  // use VOLUME_STEPS = number of HID sends
  if (step != lastVolumeStep) {
    lastVolumeStep = step;
    // Send absolute volume message to Mac via USB serial
    Serial.print("VOL:");
    Serial.println(percent);
  }
  return;
}