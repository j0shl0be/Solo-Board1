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
      {{KEY_B, KEY_MOD_LSHIFT, 0, 0}, {KEY_C, KEY_MOD_LSHIFT, 0, 0}, {KEY_D, KEY_MOD_LSHIFT, 0, 0}},  // b, c, d
      {{KEY_A, KEY_MOD_LSHIFT, 0, 0}, {KEY_E, KEY_MOD_LSHIFT, 0, 0}, {KEY_F, KEY_MOD_LSHIFT, 0, 0}},  // a, e, f
      {{KEY_I, KEY_MOD_LSHIFT, 0, 0}, {KEY_H, KEY_MOD_LSHIFT, 0, 0}, {KEY_G, KEY_MOD_LSHIFT, 0, 0}}   // i, h, g
    },
    SLIDER_VOLUME,                    // Slider function
    0x0050FF,                        // Blue color
    "DEFAULT"                        // Display name
  },
  
  // ---------- LAYER 1: F‑Keys -------------------------------------------
  {
    // Keycodes
    {
      {{KEY_TAB, KEY_MOD_LALT, 0, 0}, {KEY_F14, 0, 0, 0}, {KEY_F15, 0, 0, 0}},
      {{KEY_F16, 0, 0, 0}, {KEY_F17, 0, 0, 0}, {KEY_F18, 0, 0, 0}},
      {{KEY_F19, 0, 0, 0}, {KEY_F20, 0, 0, 0}, {KEY_F21, 0, 0, 0}}
    },
    SLIDER_VOLUME,                   // Slider function
    0xFF0064,                        // Magenta color
    "F-KEYS"                         // Display name
  },
  
  // ---------- LAYER 2: Media controls -----------------------------------
  {
    // Keycodes
    {
      {{KEY_MEDIA_PLAYPAUSE, 0, 0, 0}, {KEY_MEDIA_NEXTSONG, 0, 0, 0}, {KEY_MEDIA_PREVIOUSSONG, 0, 0, 0}},  // Media play/pause, next, previous
      {{KEY_MEDIA_VOLUMEUP, 0, 0, 0}, {KEY_MEDIA_VOLUMEDOWN, 0, 0, 0}, {KEY_MEDIA_MUTE, 0, 0, 0}},  // Volume up, down, mute
      {{KEY_MEDIA_PLAYPAUSE, 0, 0, 0}, {KEY_MEDIA_NEXTSONG, 0, 0, 0}, {KEY_MEDIA_PREVIOUSSONG, 0, 0, 0}}   // Media controls
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

KeyCombo getLayerKeycode(uint8_t row, uint8_t col) {
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
