#include <Arduino.h>
#include "FastLED.h"
#include "pico/multicore.h"
#include "Serial.h"

#define NUM_LEDS 10
#define LED_PIN 10
#define POT_PIN A0

const uint8_t read_pins[3] = {0, 1, 2};
const uint8_t scan_pins[3] = {3, 4, 5};

CRGB leds[10];

void scan_for_key_event();
void gay();

void core0_task(){
    int temp = analogRead(POT_PIN);
    Serial.println(temp);
}

void core1_task(){
  while(true){
    gay();
    FastLED.show();
  }
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("Setup");
  pinMode(POT_PIN, INPUT);
  //init leds, seccond core, hid and screen
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  multicore_launch_core1(core1_task);
}

void loop() {
  core0_task();
}


void scan_for_key_event(){

}

void gay(){
    static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;
 
  uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);
 
  uint16_t hue16 = sHue16;//gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);
  
  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5,9);
  uint16_t brightnesstheta16 = sPseudotime;
  
  for( uint16_t i = 0 ; i < NUM_LEDS; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;
 
    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;
 
    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);
    
    CRGB newcolor = CHSV( hue8, sat8, bri8);
    
    uint16_t pixelnumber = i;
    pixelnumber = (NUM_LEDS-1) - pixelnumber;
    
    nblend( leds[pixelnumber], newcolor, 64);
  }
}