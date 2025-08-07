// ============================================================================
// KB2040 RP2040 Macropad Firmware – Modular v2.0 (2025‑07‑18)
// Main entry point (PlatformIO / Arduino framework)
// ----------------------------------------------------------------------------
// Split across multiple source files for clarity:
//   * keymap.h / keymap.cpp     – layer definitions & key tables
//   * matrix.h / matrix.cpp     – 3×3 key‑matrix scanning & debounce
//   * leds.h   / leds.cpp       – NeoPixel feedback per layer
//   * display.h / display.cpp   – SSD1306 OLED status
//   * slider.h / slider.cpp     – potentiometer handling
//   * usb.h    / usb.cpp        – USB HID implementation
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
#include "usb.h"

// ---------------- Global variables -----------------------------------------
volatile Layer curLayer = LAYER_DEFAULT;
volatile uint8_t curVolume = 0;

uint8_t lastShownVolume = 255;                  // impossible value for first check
Layer lastShownLayer = static_cast<Layer>(-1);  // force update initially
unsigned long lastDisplayUpdate = 0;
const unsigned long DISPLAY_UPDATE_INTERVAL_MS = 100;

// Layer hold variables
bool layerUpHeld = false;
bool layerDownHeld = false;
bool layerUpTriggered = false;
bool layerDownTriggered = false;
bool layerUpProcessed = false;
bool layerDownProcessed = false;
unsigned long layerUpHoldStart = 0;
unsigned long layerDownHoldStart = 0;
const unsigned long LAYER_HOLD_TIME_MS = 1500;  // 1.5 seconds

// ---------------- Callback for matrix events ------------------------------
void onKeyEvent(uint8_t row, uint8_t col, bool pressed) {
  // Handle layer up key (top-right)
  if (row == TOGGLE_UP_ROW && col == TOGGLE_UP_COL) {
    if (pressed && !layerUpProcessed) {
      layerUpHeld = true;
      layerUpTriggered = false;
      layerUpProcessed = true;
      layerUpHoldStart = millis();
    } else if (!pressed && layerUpProcessed) {
      // Key released - check if we should send regular key
      if (layerUpHeld && !layerUpTriggered) {
        // Was a short tap, send regular key
        KeyCombo code = getLayerKeycode(row, col);
        if (code.keycode == 0) return;
        tapHIDKey(code.keycode, code.modifier1 | code.modifier2 | code.modifier3);
      }
      layerUpHeld = false;
      layerUpTriggered = false;
      layerUpProcessed = false;
    }
    return;
  }
  
  // Handle layer down key (bottom-right)
  if (row == TOGGLE_DOWN_ROW && col == TOGGLE_DOWN_COL) {
    if (pressed && !layerDownProcessed) {
      layerDownHeld = true;
      layerDownTriggered = false;
      layerDownProcessed = true;
      layerDownHoldStart = millis();
    } else if (!pressed && layerDownProcessed) {
      // Key released - check if we should send regular key
      if (layerDownHeld && !layerDownTriggered) {
        // Was a short tap, send regular key
        KeyCombo code = getLayerKeycode(row, col);
        if (code.keycode == 0) return;
        tapHIDKey(code.keycode, code.modifier1 | code.modifier2 | code.modifier3);
      }
      layerDownHeld = false;
      layerDownTriggered = false;
      layerDownProcessed = false;
    }
    return;
  }

  // Handle regular key presses (only if not holding layer keys)
  if (!layerUpHeld && !layerDownHeld) {
    KeyCombo code = getLayerKeycode(row, col);
    if (code.keycode == 0) return;
    sendHIDKey(code.keycode, pressed, code.modifier1 | code.modifier2 | code.modifier3);
  }
}

void setLayer(Layer layer) {
  switchToLayer(layer);
  leds_update_layer();
}

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  // Initialize USB HID
  usb_begin();

  // Initialize peripherals
  slider_begin();
  leds_begin();
  setLayer(curLayer);
  display_begin();
  matrix_begin();

  digitalWrite(D0, HIGH);
}

void loop() {
  // USB task
  usb_task();
  
  // Process any key transitions (debounce handled inside matrix_scan)
  matrix_scan(onKeyEvent);

  // Check for layer changes based on hold time
  if (layerUpHeld && !layerUpTriggered && (millis() - layerUpHoldStart) >= LAYER_HOLD_TIME_MS) {
    Layer nextLayer = static_cast<Layer>((getCurrentLayer() + 1) % NUM_LAYERS);
    switchToLayer(nextLayer);
    leds_update_layer();
    display_show(getCurrentLayer(), lastShownVolume);
    layerUpTriggered = true; // Mark as triggered to prevent regular key press
  }
  
  if (layerDownHeld && !layerDownTriggered && (millis() - layerDownHoldStart) >= LAYER_HOLD_TIME_MS) {
    Layer prevLayer = static_cast<Layer>((getCurrentLayer() + NUM_LAYERS - 1) % NUM_LAYERS);
    switchToLayer(prevLayer);
    leds_update_layer();
    display_show(getCurrentLayer(), lastShownVolume);
    layerDownTriggered = true; // Mark as triggered to prevent regular key press
  }

  // Update display and slider value
  uint8_t currentPercent = get_slider_percent();

  unsigned long now = millis();
  if ((currentPercent != lastShownVolume || getCurrentLayer() != lastShownLayer) && 
      (now - lastDisplayUpdate > DISPLAY_UPDATE_INTERVAL_MS)) {
    send_volume_change(currentPercent);
    display_show(getCurrentLayer(), currentPercent);
    lastShownVolume = currentPercent;
    lastShownLayer = getCurrentLayer();
    lastDisplayUpdate = now;
  }
}
  