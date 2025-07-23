#pragma once
#include <stdint.h>
#include "keymap.h"


extern volatile Layer curLayer;

void leds_begin();
void leds_update_layer();
void leds_brightness(uint8_t brightness);