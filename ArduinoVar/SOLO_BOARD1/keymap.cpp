#include "keymap.h"
#include <Adafruit_NeoPixel.h>

const uint8_t TOGGLE_UP_ROW = 0;
const uint8_t TOGGLE_UP_COL = 2;

const uint8_t TOGGLE_DOWN_ROW = 2;
const uint8_t TOGGLE_DOWN_COL = 2;

// ------------------------ Layer configurations ---------------------------
const LayerConfig layerConfigs[NUM_LAYERS] = {
  // ---------- LAYER 0: Default shortcuts ---------------------------------
  {
    // Keycodes
    {
      {0x05, 0x06, 0x07},  // b, c, d
      {0x04, 0x08, 0x09},  // a, e, f
      {0x0C, 0x0B, 0x0A}   // l, k, j
    },
    SLIDER_VOLUME,                    // Slider function
    0x0050FF,                        // Blue color
    "DEFAULT"                        // Display name
  },
  
  // ---------- LAYER 1: F‑Keys -------------------------------------------
  {
    // Keycodes
    {
      {KEY_F13, KEY_F14, KEY_F15},
      {KEY_F16, KEY_F17, KEY_F18},
      {KEY_F19, KEY_F20, KEY_F21}
    },
    SLIDER_VOLUME,                   // Slider function
    0xFF0064,                        // Magenta color
    "F-KEYS"                         // Display name
  },
  
  // ---------- LAYER 2: Media controls -----------------------------------
  {
    // Keycodes
    {
      {0x4F, 0x50, 0x51},           // Media play/pause, next, previous
      {0x52, 0x53, 0x54},           // Volume up, down, mute
      {0x55, 0x56, 0x57}            // Media controls
    },
    SLIDER_BRIGHTNESS,               // Slider function
    0x00FF00,                        // Green color
    "MEDIA"                          // Display name
  }
};

// ------------------------ Helper functions -------------------------------
const LayerConfig* getCurrentLayerConfig() {
  return &layerConfigs[curLayer];
}

uint8_t getLayerKeycode(uint8_t row, uint8_t col) {
  return layerConfigs[curLayer].keycodes[row][col];
}

SliderFunction getCurrentSliderFunction() {
  return layerConfigs[curLayer].sliderFunc;
}

uint32_t getCurrentLedColor() {
  return layerConfigs[curLayer].ledColor;
}

const char* getCurrentDisplayName() {
  return layerConfigs[curLayer].displayName;
}



// ------------------------ Layer management functions ---------------------
void switchToLayer(Layer newLayer) {
  if (isValidLayer(newLayer)) {
    curLayer = newLayer;
  }
}

Layer getCurrentLayer() {
  return curLayer;
}

bool isValidLayer(Layer layer) {
  return layer < NUM_LAYERS;
}

const char* getSliderFunctionName(SliderFunction func) {
  switch (func) {
    case SLIDER_VOLUME:
      return "Volume";
    case SLIDER_BRIGHTNESS:
      return "Brightness";
    default:
      return "Unknown";
  }
}
