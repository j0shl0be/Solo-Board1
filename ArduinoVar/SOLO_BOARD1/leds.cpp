#include "leds.h"
#include "keymap.h"
#include "slider.h"
#include <Adafruit_NeoPixel.h>

constexpr uint8_t LED_PIN   = 10;
constexpr uint16_t NUM_LEDS = 10;

static Adafruit_NeoPixel pixels(NUM_LEDS, LED_PIN,  NEO_GRB + NEO_KHZ800);

void leds_begin() {
  pixels.begin();
  pixels.fill(getCurrentLedColor());
  pixels.setBrightness(globalBrightness);
  pixels.show();
}

void leds_update_layer(){
  pixels.fill(getCurrentLedColor());
  pixels.setBrightness(globalBrightness);
  pixels.show();
}

void leds_brightness(uint8_t brightness){
  // Refill LEDs with current color before setting brightness
  // This ensures LEDs turn back on when brightness increases from 0
  pixels.fill(getCurrentLedColor());
  pixels.setBrightness(brightness);
  pixels.show();
}

void leds_apply_global_brightness() {
  pixels.setBrightness(globalBrightness);
  pixels.show();
}