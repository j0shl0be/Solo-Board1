#include "display.h"
#include <Arduino.h>
//#include <Adafruit_GFX.h>
#include <U8g2lib.h>

// ---------------------------------------------------------------------------
// SSD1306 helper – now 100 % core‑agnostic.
// We use the *default* I²C bus (Wire = I²C0) so no `setSDA()` / `setSCL()`
// calls are required (they don’t exist in the Arduino‑MBED core).
// On a Pico/KB2040 that means:
//   • SDA0 = GP4  (pin 6 on Pico header, "SCL" pad on KB2040 STEMMA)
//   • SCL0 = GP5  (pin 7 on Pico header, "SDA" pad on KB2040 STEMMA)
// ---------------------------------------------------------------------------

constexpr uint8_t SCREEN_WIDTH  = 128;
constexpr uint8_t SCREEN_HEIGHT = 64;
constexpr uint8_t OLED_ADDR     = 0x3C;

U8G2_SSD1306_128X64_NONAME_F_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE, 9, 8);

void display_begin() {
  oled.begin();
  oled.clearBuffer();
  oled.setFont(u8g2_font_6x10_tf);
  oled.drawStr(0, 10, "Macropad Ready");
  oled.sendBuffer();
}

void display_show(uint8_t layer, uint8_t volumePercent) {
  oled.clearBuffer();
  oled.setFont(u8g2_font_6x10_tf);

  oled.setCursor(0, 10);
  oled.print("Layer: ");
  oled.print(layer == 0 ? "DEF" : "F-KEYS");

  oled.setCursor(0, 28);
  oled.print("Vol: ");
  oled.print(volumePercent);
  oled.print('%');

  /* volume bar */
  oled.drawFrame(0, 40, 128, 10);
  oled.drawBox(1, 41, map(volumePercent, 0, 100, 0, 126), 8);

  oled.sendBuffer();
}
