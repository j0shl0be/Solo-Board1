#pragma once
#include <stdint.h>
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "usb_hid_keys.h"

// Slider function enumeration (kept for compatibility/UI text if needed)
enum SliderFunction : uint8_t {
  SLIDER_VOLUME = 0,
  SLIDER_BRIGHTNESS
};

// Callback type for per-layer slider handling (0-100 percent)
typedef void (*SliderCallback)(uint8_t percent);

struct KeyStruct {
  uint8_t keycode;
  uint8_t modifier1;
  uint8_t modifier2;
  uint8_t modifier3;
  const char* keyName;
};

// Comprehensive Layer structure
struct LayerConfig {
  KeyStruct keycodes[3][3];        // 3x3 key matrix keycodes
  SliderFunction sliderFunc;      // What the slider controls (legacy/UI)
  uint32_t ledColor;              // NeoPixel color for this layer
  const char* displayName;        // Name shown on OLED display
  const void* icon;               // Layer icon (8x8 or 16x16)
  uint8_t iconSize;               // Icon size in pixels (8, 16, 32)
  const void* sliderIcon;         // Slider icon (8x8 or 16x16)
  uint8_t sliderIconSize;         // Slider icon size in pixels (8, 16, 32)
  SliderCallback sliderCallback;  // Callback to handle 0-100 slider value
};

// ------------------------ Layer enumeration ------------------------------
enum Layer : uint8_t { 
  LAYER_DEFAULT = 0, 
  LAYER_SECOND,
  LAYER_F_KEYS, 
  LAYER_MEDIA,
  NUM_LAYERS // KEEP THIS LAST !!!!!!!!!!!!!!!!!!!!will break if not last!!!!!!!!!!!!!!!!!!
};

// ------------------------ Layer toggle key locations ---------------------
extern const uint8_t TOGGLE_UP_ROW;
extern const uint8_t TOGGLE_UP_COL;
extern const uint8_t TOGGLE_DOWN_ROW;
extern const uint8_t TOGGLE_DOWN_COL;

// Global layer variable
extern volatile Layer curLayer;

// ------------------------ Layer configurations ---------------------------
extern const LayerConfig layerConfigs[];

// ------------------------ Helper functions -------------------------------
const LayerConfig* getCurrentLayerConfig();
KeyStruct getLayerKeycode(uint8_t row, uint8_t col);
SliderFunction getCurrentSliderFunction();
uint32_t getCurrentLedColor();
const char* getCurrentDisplayName();
const void* getCurrentLayerIcon();
const void* getCurrentSliderIcon();
uint8_t getCurrentLayerIconSize();
uint8_t getCurrentSliderIconSize();
SliderCallback getCurrentSliderCallback();
String getKeyName(uint8_t keycode);

// ------------------------ Layer management functions ---------------------
void switchToLayer(Layer newLayer);
Layer getCurrentLayer();
bool isValidLayer(Layer layer);
const char* getSliderFunctionName(SliderFunction func);
