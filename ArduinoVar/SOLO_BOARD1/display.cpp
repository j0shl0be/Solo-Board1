#include <Adafruit_SSD1306.h>
#include <splash.h>

#include "display.h"
#include <Arduino.h>
#include "Wire.h"

// ---------------------------------------------------------------------------
// SSD1306 helper – now 100 % core‑agnostic.
// We use the *default* I²C bus (Wire = I²C0) so no `setSDA()` / `setSCL()`
// calls are required (they don’t exist in the Arduino‑MBED core).
// On a Pico/KB2040 that means:
//   • SDA0 = GP4  (pin 6 on Pico header, "SCL" pad on KB2040 STEMMA)
//   • SCL0 = GP5  (pin 7 on Pico header, "SDA" pad on KB2040 STEMMA)
// ---------------------------------------------------------------------------

constexpr uint8_t SCREEN_WIDTH = 128;
constexpr uint8_t SCREEN_HEIGHT = 64;
constexpr uint8_t OLED_ADDR = 0x3C;

TwoWire myWire(i2c0, 8, 9);  // SDA = GP8, SCL = GP9

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &myWire, -1);

void display_begin() {
  myWire.begin();
  if (!oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
      Serial.println("OLED not found");
  }
  oled.setRotation(0);
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

  oled.setCursor(0, 0);
  oled.print("Layer: ");
  oled.print(layer == 0 ? "DEF" : "F-KEYS");

  oled.setCursor(0, 16);
  oled.print("Vol: ");
  oled.print(volumePercent);
  oled.println('%');

  /* volume bar */
  oled.drawRect(0, 32, 128, 10, SSD1306_WHITE);
  oled.fillRect(1, 33, map(volumePercent, 0, 100, 0, 126), 8, SSD1306_WHITE);

  oled.display();
}
