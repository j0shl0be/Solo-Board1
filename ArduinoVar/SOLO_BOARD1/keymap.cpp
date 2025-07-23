#include "keymap.h"
#include <Adafruit_TinyUSB.h>

const uint8_t TOGGLE_UP_ROW = 0;
const uint8_t TOGGLE_UP_COL = 2;

const uint8_t TOGGLE_DOWN_ROW = 2;
const uint8_t TOGGLE_DOWN_COL = 2;

Layer layers[4]


// ------------------------ Layer key tables -------------------------------
// row x col = 3 × 3
const uint8_t layerKeycodes[NUM_LAYERS][3][3] = {
  // ---------- LAYER 0: Default shortcuts ---------------------------------
  {
    {0x05, 0x06, 0x07},
    {0x04, 0x08, 0x09},
    {0x0C, 0x0B, 0x0A}
  },
  // ---------- LAYER 1: F‑Keys -------------------------------------------
  {
    {KEY_F13, KEY_F14, KEY_F15},
    {KEY_F16, KEY_F17, KEY_F18},
    {KEY_F19, KEY_F20, KEY_F21}
  }
};
