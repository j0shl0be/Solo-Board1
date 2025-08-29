// ============================================================================
// KB2040 RP2040 Macropad Firmware – Modular v2.0 (2025‑07‑18)
// Main entry point (PlatformIO / Arduino framework)
// ----------------------------------------------------------------------------
// Split across multiple source files for clarity:
//   * keymap.h / keymap.cpp     – layer definitions & key tables
//   * matrix.h / matrix.cpp     – 3×3 key‑matrix scanning & debounce
//   * leds.h   / leds.cpp       – NeoPixel feedback per layer
//   * display.h / display.cpp   – SSD1306 OLED status
//   * main.cpp                  – high‑level logic, rotary volume, callbacks
//
// Author: Joshua Lobe
// ============================================================================

#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include "tusb.h"
#include <Adafruit_NeoPixel.h>
#include "keymap.h"
#include "matrix.h"
#include "leds.h"
#include "display.h"
#include "Serial.h"

// ---------------- Potentiometer → system volume ---------------------------
constexpr uint8_t POT_PIN = A0;
constexpr int VOLUME_STEPS = 30;        // Granularity of volume mapping
constexpr int VOL_CHANGE_THRESHOLD = 4; // ADC delta noise filter
int lastPotValue = 0;
int lastVolumeStep = -1;
volatile Layer curLayer = LAYER_DEFAULT;

Adafruit_USBD_HID usb_hid;

void sendHIDKey(uint8_t keycode, bool pressed);

// ---------------- Callback for matrix events ------------------------------
void onKeyEvent(uint8_t row, uint8_t col, bool pressed)
{
  // Toggle layer (only when pressed, not released)
  if (row == TOGGLE_ROW && col == TOGGLE_COL && pressed)
  {
    curLayer = static_cast<Layer>((curLayer + 1) % NUM_LAYERS);
    leds_update_layer();
    int percent = map(lastPotValue, 0, 4095, 0, 100);
    display_show(curLayer, percent);
    return;
  }

  uint8_t code = layerKeycodes[curLayer][row][col];
  if (!code)
    return; // 0x00 = no key
  sendHIDKey(code, pressed);
}

void setLayer(Layer layer)
{
  curLayer = layer;
  leds_update_layer();
}

void sendHIDKey(uint8_t keycode, bool pressed)
{
  static uint8_t current_keys[6] = {0};
  static uint8_t modifier = 0;

  if (!tud_hid_ready())
    return;

  if (pressed)
  {
    // Add to key report if there's space
    for (int i = 0; i < 6; ++i)
    {
      if (current_keys[i] == 0)
      {
        current_keys[i] = keycode;
        break;
      }
    }
  }
  else
  {
    // Remove key from report
    for (int i = 0; i < 6; ++i)
    {
      if (current_keys[i] == keycode)
      {
        current_keys[i] = 0;
      }
    }
  }

  tud_hid_keyboard_report(0, modifier, current_keys);
}

void tapHIDKey(uint8_t keycode)
{
  if (!tud_hid_ready())
    return;
  uint8_t keys[6] = {keycode};
  tud_hid_keyboard_report(0, 0, keys);
  delay(10);
  tud_hid_keyboard_report(0, 0, NULL); // Release all
}

// ---------------- Arduino setup ------------------------------------------
void setup()
{
  Serial.begin(115200);
  while (!Serial)
    delay(10);
  usb_hid.setPollInterval(2);
  usb_hid.begin();
  TinyUSBDevice.begin();

  // begin IO
  leds_begin();
  setLayer(curLayer);

  analogReadResolution(12);
  lastPotValue = analogRead(POT_PIN);
  display_show(curLayer, map(lastPotValue, 0, 4095, 0, 100));
}

// ---------------- Arduino loop -------------------------------------------
void loop()
{
  // Process any key transitions (debounce handled inside matrix_scan)
  matrix_scan(onKeyEvent);

  // Rotary potentiometer → volume
  int potValue = analogRead(POT_PIN);
  if (abs(potValue - lastPotValue) > VOL_CHANGE_THRESHOLD)
  {
    lastPotValue = potValue;
    int percent = map(potValue, 0, 4095, 0, 100);
    display_show(curLayer, percent);

    int step = map(percent, 0, 100, 0, VOLUME_STEPS);
    if (step != lastVolumeStep)
    {
      lastVolumeStep = step;

      // Send absolute volume message to Mac via USB serial
      Serial.print("VOL:");
      Serial.println(percent);
    }
  }
}
