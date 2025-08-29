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


Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

static const char* g_lastKeyName;
static bool g_hasKeyName = false;

// Draw bitmap icon. Supports 1-bit row formats of widths 8, 16, or 32.
// - For size 8:  data is uint8_t[8],  bit mask starts at 0x80
// - For size 16: data is uint16_t[16], bit mask starts at 0x8000
// - For size 32: data is uint32_t[32], bit mask starts at 0x80000000
static void drawIconWithSize(uint8_t x, uint8_t y, const void* icon, uint8_t size) {
  if (size == 8) {
    const uint8_t* data = (const uint8_t*)icon;
    for (int row = 0; row < 8; row++) {
      for (int col = 0; col < 8; col++) {
        if (data[row] & (0x80u >> col)) {
          oled.drawPixel(x + col, y + row, SSD1306_WHITE);
        }
      }
    }
  } else if (size == 16) {
    const uint16_t* data = (const uint16_t*)icon;
    for (int row = 0; row < 16; row++) {
      for (int col = 0; col < 16; col++) {
        if (data[row] & (0x8000u >> col)) {
          oled.drawPixel(x + col, y + row, SSD1306_WHITE);
        }
      }
    }
  } else if (size == 32) {
    const uint32_t* data = (const uint32_t*)icon;
    for (int row = 0; row < 32; row++) {
      for (int col = 0; col < 32; col++) {
        if (data[row] & (0x80000000u >> col)) {
          oled.drawPixel(x + col, y + row, SSD1306_WHITE);
        }
      }
    }
  }
}

void drawLayerSliderIcon() {
  const void* icon = getCurrentLayerIcon();
  uint8_t size = getCurrentLayerIconSize();
  if (icon && size) {
    drawIconWithSize(0, 0, icon, size);
  }
  const void* sliderIcon = getCurrentSliderIcon();
  uint8_t sliderSize = getCurrentSliderIconSize();
  if (sliderIcon && sliderSize) {
    drawIconWithSize(32, 0, sliderIcon, sliderSize);
  }
}

void display_begin() {
  Wire.setSDA(8);
  Wire.setSCL(9);
  Wire.begin();
  
  if (!oled.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS) && Serial) {
    Serial.println("OLED not found");
    return;
  }
  


  oled.setRotation(1);
  
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  oled.setCursor(0, 0);
  if (!Serial) {
    oled.println("Serial not found");
    
  }
  oled.display();
}

void display_show(uint8_t layer, uint8_t volumePercent) {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);

  // Get current layer info
  const char* layerName = getCurrentDisplayName();
  SliderFunction sliderFunc = getCurrentSliderFunction();
  Layer curLayer = getCurrentLayer();
  
  // Draw layer icon (top-left)
  drawLayerSliderIcon();
  // Draw slider icon next to it

  
  // Display layer name (top of vertical display)
    // oled.setCursor(2, 34);
    // oled.println("Layer:");
    // oled.setCursor(2, 46);
    // oled.print(layerName);

  // Display slider function and value (middle section)
  // oled.setCursor(0, 64);
  // switch (sliderFunc) {
  //   case SLIDER_VOLUME:
  //     oled.println("Vol: ");
  //     oled.print(volumePercent);
  //     oled.println('%');
  //     break;
  //   case SLIDER_BRIGHTNESS:
  //     oled.println("Bright: ");
  //     oled.print(volumePercent);
  //     oled.println('%');
  //     break;
  // }

  //Bar to show which layer is active
  oled.drawRect(1, 32, 2, 48, SSD1306_WHITE);
  for (int i = NUM_LAYERS -1 ; i >= 0; i--) {
    if (i != curLayer) {
      oled.fillRect(3, 69 - i * 12, 2, 10, SSD1306_WHITE);
    }
    else {
      oled.fillRect(3, 68 - i * 12, 11, 12, SSD1306_WHITE);
      oled.fillRect(3, 69 - i * 12, 10, 10, SSD1306_BLACK);
    }
  }


  /* vertical slider bar (right side of vertical display) */
  // After 90° rotation: width=64, height=128
  oled.drawRect(48, 32, 16, 96, SSD1306_WHITE);
  int barHeight = map(volumePercent, 0, 100, 0, 94);
  oled.fillRect(49, 127 - barHeight, 14, barHeight, SSD1306_WHITE);

  // Bottom-left: show last pressed key name
  if (g_hasKeyName && g_lastKeyName && g_lastKeyName[0] != '\0') {
    oled.setTextSize(2);
    oled.setCursor(2, 96);
    oled.print(g_lastKeyName);
    oled.setTextSize(1);
  }

  oled.display();
}

void display_set_key_name(const char* name) {
  if (name == nullptr) {
    display_clear_key_name();
    return;
  }
  g_lastKeyName = name;
  g_hasKeyName = true;
}

void display_clear_key_name() {
  g_lastKeyName = nullptr;
  g_hasKeyName = false;
}

void display_update_key_name(uint8_t row, uint8_t col, bool pressed) {
  if (pressed) {
    const char* name = getLayerKeycode(row, col).keyName;
    display_set_key_name(name);
  } else {
    display_clear_key_name();
  }
}
