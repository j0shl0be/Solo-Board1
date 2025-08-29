#pragma once
#include <stdint.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

extern Adafruit_SSD1306 oled;


void display_begin();
void display_show(uint8_t layer, uint8_t volumePercent);
void drawLayerSliderIcon();
static void drawIconWithSize(uint8_t x, uint8_t y, const void* icon, uint8_t size);
void display_set_key_name(const char* name);
void display_clear_key_name();
void display_update_key_name(uint8_t row, uint8_t col, bool pressed);
