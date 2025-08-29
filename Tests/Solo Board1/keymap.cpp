#include "keymap.h"
#include "tusb.h"

const uint8_t TOGGLE_ROW = 2;
const uint8_t TOGGLE_COL = 2;

// ------------------------ Layer key tables -------------------------------
// row x col = 3 × 3
const uint8_t layerKeycodes[NUM_LAYERS][3][3] = {
  // ---------- LAYER 0: Default shortcuts ---------------------------------
  {
    {HID_KEY_CONTROL_LEFT, HID_KEY_SHIFT_LEFT, 'z'},
    {'x', 'c', 'v'},
    {'b', 'n', 0x00}
  },
  // ---------- LAYER 1: F‑Keys -------------------------------------------
  {
    {KEY_F13, KEY_F14, KEY_F15},
    {KEY_F16, KEY_F17, KEY_F18},
    {KEY_F19, KEY_F20, KEY_F21}
  }
};
