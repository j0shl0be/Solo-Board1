#pragma once
#include <stdint.h>
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "usb_hid_keys.h"

// Slider function enumeration
enum SliderFunction : uint8_t {
  SLIDER_VOLUME = 0,
  SLIDER_BRIGHTNESS
};

struct KeyCombo {
  uint8_t keycode;
  uint8_t modifier1;
  uint8_t modifier2;
  uint8_t modifier3;
};

// Comprehensive Layer structure
struct LayerConfig {
  KeyCombo keycodes[3][3];        // 3x3 key matrix keycodes
  SliderFunction sliderFunc;      // What the slider controls
  uint32_t ledColor;             // NeoPixel color for this layer
  const char* displayName;       // Name shown on OLED display
};

// ------------------------ Layer enumeration ------------------------------
enum Layer : uint8_t { 
  LAYER_DEFAULT = 0, 
  LAYER_F_KEYS, 
  LAYER_MEDIA,
  NUM_LAYERS 
};

// ------------------------ Layer toggle key locations ---------------------
extern const uint8_t TOGGLE_UP_ROW;
extern const uint8_t TOGGLE_UP_COL;
extern const uint8_t TOGGLE_DOWN_ROW;
extern const uint8_t TOGGLE_DOWN_COL;

// Global layer variable
extern volatile Layer curLayer;

// ------------------------ Layer configurations ---------------------------
extern const LayerConfig layerConfigs[NUM_LAYERS];

// ------------------------ Helper functions -------------------------------
const LayerConfig* getCurrentLayerConfig();
KeyCombo getLayerKeycode(uint8_t row, uint8_t col);
SliderFunction getCurrentSliderFunction();
uint32_t getCurrentLedColor();
const char* getCurrentDisplayName();

// ------------------------ Layer management functions ---------------------
void switchToLayer(Layer newLayer);
Layer getCurrentLayer();
bool isValidLayer(Layer layer);
const char* getSliderFunctionName(SliderFunction func);
