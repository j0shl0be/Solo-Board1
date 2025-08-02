#pragma once
#include <stdint.h>
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// Extended HID usage IDs for F‑keys 13‑24 (0x68–0x73)
enum : uint8_t {
  KEY_F13 = 0x68, KEY_F14, KEY_F15, KEY_F16,
  KEY_F17, KEY_F18, KEY_F19, KEY_F20,
  KEY_F21, KEY_F22, KEY_F23, KEY_F24
};

// Slider function enumeration
enum SliderFunction : uint8_t {
  SLIDER_VOLUME = 0,
  SLIDER_BRIGHTNESS
};

// Comprehensive Layer structure
struct LayerConfig {
  uint8_t keycodes[3][3];        // 3x3 key matrix keycodes
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

extern volatile Layer curLayer;

// ------------------------ Layer toggle key locations ---------------------
extern const uint8_t TOGGLE_UP_ROW;
extern const uint8_t TOGGLE_UP_COL;
extern const uint8_t TOGGLE_DOWN_ROW;
extern const uint8_t TOGGLE_DOWN_COL;

// ------------------------ Layer configurations ---------------------------
extern const LayerConfig layerConfigs[NUM_LAYERS];

// ------------------------ Helper functions -------------------------------
const LayerConfig* getCurrentLayerConfig();
uint8_t getLayerKeycode(uint8_t row, uint8_t col);
SliderFunction getCurrentSliderFunction();
uint32_t getCurrentLedColor();
const char* getCurrentDisplayName();

// ------------------------ Layer management functions ---------------------
void switchToLayer(Layer newLayer);
Layer getCurrentLayer();
bool isValidLayer(Layer layer);
const char* getSliderFunctionName(SliderFunction func);
