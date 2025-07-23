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
#include <Adafruit_NeoPixel.h>
#include "keymap.h"
#include "matrix.h"
#include "leds.h"
#include "display.h"
#include "slider.h"
#define RID_KEYBOARD 1
// ---------------- Potentiometer → system volume ---------------------------

volatile Layer curLayer = LAYER_DEFAULT;
volatile uint8_t curVolume = 0;

uint8_t lastShownVolume = 255;                  // impossible value for first check
Layer lastShownLayer = static_cast<Layer>(-1);  // force update initially
unsigned long lastDisplayUpdate = 0;
const unsigned long DISPLAY_UPDATE_INTERVAL_MS = 100;
Adafruit_USBD_HID usb_hid;

void sendHIDKey(uint8_t keycode, bool pressed);

uint8_t const desc_hid_report[] = {
  TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(RID_KEYBOARD))
};

// ---------------- Callback for matrix events ------------------------------
void onKeyEvent(uint8_t row, uint8_t col, bool pressed) {
  // Toggle layer (only when pressed, not released) FIX THIS
  if (row == TOGGLE_UP_ROW && col == TOGGLE_UP_COL && pressed) {
    curLayer = static_cast<Layer>((curLayer + 1) % NUM_LAYERS);
    leds_update_layer();
    display_show(curLayer, lastShownVolume);
    return;
  }

  uint8_t code = layerKeycodes[curLayer][row][col];
  if (!code)
    return;  // 0x00 = no key
  sendHIDKey(code, pressed);
}

void setLayer(Layer layer) {
  curLayer = layer;
  leds_update_layer();
}

void sendHIDKey(uint8_t keycode, bool pressed) {
  static uint8_t current_keys[6] = { 0 };
  static uint8_t modifier = 0;

  if (!usb_hid.ready()) return;

  if (pressed) {
    // Avoid duplicates
    for (int i = 0; i < 6; ++i) {
      if (current_keys[i] == keycode) return;
    }

    // Add key if there's space
    for (int i = 0; i < 6; ++i) {
      if (current_keys[i] == 0) {
        current_keys[i] = keycode;
        break;
      }
    }
  } else {
    // Remove released key
    for (int i = 0; i < 6; ++i) {
      if (current_keys[i] == keycode) {
        current_keys[i] = 0;
      }
    }
  }

  usb_hid.keyboardReport(RID_KEYBOARD, modifier, current_keys);
}

void tapHIDKey(uint8_t keycode) {
  if (!usb_hid.ready())
    return;
  uint8_t keys[6] = { keycode };
  usb_hid.keyboardReport(RID_KEYBOARD, 0, keys);
  delay(10);
  usb_hid.keyboardRelease(RID_KEYBOARD);  // Release all
}
// ---------------- Arduino setup ------------------------------------------
void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usb_hid.setPollInterval(2);
  usb_hid.setBootProtocol(HID_ITF_PROTOCOL_KEYBOARD);
  usb_hid.begin();
  if (!TinyUSBDevice.isInitialized()) {
    TinyUSBDevice.begin();
  }

  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }

  // begin IO
  leds_begin();
  Serial.println("leds done");
  setLayer(curLayer);
  Serial.println("layer done");
  display_begin();
  slider_begin();
  matrix_begin();
  Serial.println("init done");

  digitalWrite(D0, HIGH);
}

// ---------------- Arduino loop -------------------------------------------
void loop() {
#ifdef TINYUSB_NEED_POLLING_TASK
  TinyUSBDevice.task();
#endif
  // Process any key transitions (debounce handled inside matrix_scan)
  matrix_scan(onKeyEvent);

  uint8_t currentPercent = get_slider_percent();

  unsigned long now = millis();
  if ((currentPercent != lastShownVolume || curLayer != lastShownLayer) && (now - lastDisplayUpdate > DISPLAY_UPDATE_INTERVAL_MS)) {
    send_volume_change(currentPercent);
    display_show(curLayer, currentPercent);
    lastShownVolume = currentPercent;
    lastShownLayer = curLayer;
    lastDisplayUpdate = now;
  }
}
