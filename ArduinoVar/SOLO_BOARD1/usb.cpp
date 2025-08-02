#include "usb.h"

// USB HID report descriptor
uint8_t const desc_hid_report[] = {
  TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(RID_KEYBOARD))
};

// USB HID object
Adafruit_USBD_HID usb_hid;

// Static variables for key tracking
static uint8_t current_keys[6] = { 0 };
static uint8_t modifier = 0;

void usb_begin() {
  // Initialize USB HID
  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usb_hid.setPollInterval(2);
  usb_hid.setBootProtocol(HID_ITF_PROTOCOL_KEYBOARD);
  usb_hid.begin();
  
  if (!TinyUSBDevice.isInitialized()) {
    TinyUSBDevice.begin();
  }

  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }
}

void sendHIDKey(uint8_t keycode, bool pressed) {
  if (!usb_hid.ready()) return;

  if (pressed) {
    // Avoid duplicates
    for (int i = 0; i < 6; ++i) {
      if (current_keys[i] == keycode) return;
    }

    // Add key if there's space
    for (int i = 0; i < 6; ++i) {
      if (current_keys[i] == 0) {
        current_keys[i] = keycode;
        break;
      }
    }
  } else {
    // Remove released key
    for (int i = 0; i < 6; ++i) {
      if (current_keys[i] == keycode) {
        current_keys[i] = 0;
      }
    }
  }

  usb_hid.keyboardReport(RID_KEYBOARD, modifier, current_keys);
}

void tapHIDKey(uint8_t keycode) {
  if (!usb_hid.ready())
    return;
  uint8_t keys[6] = { keycode };
  usb_hid.keyboardReport(RID_KEYBOARD, 0, keys);
  delay(10);
  usb_hid.keyboardRelease(RID_KEYBOARD);  // Release all
}

bool usb_is_ready() {
  return usb_hid.ready();
}

void usb_task() {
#ifdef TINYUSB_NEED_POLLING_TASK
  TinyUSBDevice.task();
#endif
}
