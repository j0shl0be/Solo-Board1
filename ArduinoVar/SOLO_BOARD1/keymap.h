#pragma once
#include <stdint.h>
#include "slider.h"
#include "leds.h"
#include "Arduino.h"

// Extended HID usage IDs for F‑keys 13‑24 (0x68–0x73)
enum : uint8_t {
  KEY_F13 = 0x68, KEY_F14, KEY_F15, KEY_F16,
  KEY_F17, KEY_F18, KEY_F19, KEY_F20,
  KEY_F21, KEY_F22, KEY_F23, KEY_F24
};

typedef struct{
  uint8_t level,
  String displayName,
  

}
// ------------------------ Layer enumeration ------------------------------
enum Layer : uint8_t { LAYER_DEFAULT = 0, LAYER_F_KEYS, NUM_LAYERS };
extern volatile Layer curLayer;

// ------------------------ Toggle key location ----------------------------
extern const uint8_t TOGGLE_UP_ROW;
extern const uint8_t TOGGLE_UP_COL;
extern const uint8_t TOGGLE_DOWN_ROW;
extern const uint8_t TOGGLE_DOWN_COL;

// ------------------------ Keymaps (const) --------------------------------
extern const uint8_t layerKeycodes[NUM_LAYERS][3][3];
