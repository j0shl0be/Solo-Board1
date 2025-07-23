#include "leds.h"
#include <Adafruit_NeoPixel.h>

constexpr uint8_t LED_PIN   = 10;
constexpr uint16_t NUM_LEDS = 10;

static Adafruit_NeoPixel pixels(NUM_LEDS, LED_PIN,  NEO_GRB + NEO_KHZ800);

static uint32_t layerColors[] = {
  pixels.Color(0, 80, 255),   // Default – blue
  pixels.Color(255, 0, 100)   // F‑keys – magenta
};

void leds_begin() {
  pixels.begin();
  pixels.fill(layerColors[0]);
  pixels.setBrightness(10);
  pixels.show();
}

void leds_update_layer(){
  pixels.fill(layerColors[curLayer]);
  pixels.show();
}

void leds_brightness(uint8_t brightness){
  pixels.setBrightness(brightness);
  pixels.show();
}