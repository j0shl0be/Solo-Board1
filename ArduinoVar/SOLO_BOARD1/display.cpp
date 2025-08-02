#include <Adafruit_SSD1306.h>
#include <splash.h>

#include "display.h"
#include "keymap.h"
#include "slider.h"
#include <Arduino.h>
#include "Wire.h"

// ---------------------------------------------------------------------------
// SSD1306 OLED Display - Adafruit Library with Rotation
// Using rotation for vertical layout on RP2040
// ---------------------------------------------------------------------------

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void display_begin() {
  Wire.setSDA(8);
  Wire.setSCL(9);
  Wire.begin();
  
  if (!oled.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("OLED not found");
    return;
  }
  
  // Set rotation to 90 degrees clockwise
  oled.setRotation(1);
  
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  oled.setCursor(0, 0);
  oled.println("Macropad Ready");
  oled.display();
}

void display_show(uint8_t layer, uint8_t volumePercent) {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);

  // Get current layer info
  const char* layerName = getCurrentDisplayName();
  SliderFunction sliderFunc = getCurrentSliderFunction();
  
  // Display layer name (top of vertical display)
  oled.setCursor(0, 0);
  oled.println("Layer:");
  oled.setCursor(0, 12);
  oled.print(layerName);

  // Display slider function and value (middle section)
  oled.setCursor(0, 30);
  switch (sliderFunc) {
    case SLIDER_VOLUME:
      oled.println("Vol: ");
      oled.print(volumePercent);
      oled.println('%');
      break;
    case SLIDER_BRIGHTNESS:
      oled.println("Bright: ");
      oled.print(volumePercent);
      oled.println('%');
      break;
  }

  /* vertical slider bar (right side of vertical display) */
  // After 90° rotation: width=64, height=128
  oled.drawRect(50, 0, 8, 128, SSD1306_WHITE);
  int barHeight = map(volumePercent, 0, 100, 0, 124);
  oled.fillRect(51, 127 - barHeight, 6, barHeight, SSD1306_WHITE);

  oled.display();
}
