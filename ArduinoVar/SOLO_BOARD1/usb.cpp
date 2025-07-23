#include "usb.h"
#include <Arduino.h>

void usb_begin() {
  tusb_init();
}

void send_key_press(uint8_t keycode) {
  if (!tud_hid_ready()) return;

  uint8_t report[6] = {0};
  report[0] = keycode;
  tud_hid_keyboard_report(0, 0, report);
}

void send_key_release() {
  if (tud_hid_ready()) {
    tud_hid_keyboard_report(0, 0, NULL);
  }
}

void send_volume_change(int step) {
  if (!tud_hid_ready()) return;

  uint8_t code = (step > 0) ? HID_USAGE_CONSUMER_VOLUME_INCREMENT : HID_USAGE_CONSUMER_VOLUME_DECREMENT;

  tud_hid_report(0, &code, sizeof(code));
  delay(10);
  uint8_t release = 0;
  tud_hid_report(0, &release, sizeof(release));
}
