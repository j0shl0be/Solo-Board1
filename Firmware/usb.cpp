#include "usb.h"

// USB HID report descriptor
uint8_t const desc_hid_report[] = {
  TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(RID_KEYBOARD)),
  TUD_HID_REPORT_DESC_CONSUMER(HID_REPORT_ID(RID_CONSUMER))
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

void sendHIDKey(uint8_t keycode, bool pressed, uint8_t modifier) {
  if (!usb_hid.ready()) return;
  uint8_t modifierSend = 0;

  // Route media/consumer keys to Consumer Control report
  if (keycode >= 0xE8 && keycode <= 0xFB) {
    uint16_t usage = 0;
    switch (keycode) {
      case 0xE8: usage = HID_USAGE_CONSUMER_PLAY_PAUSE; break;           // KEY_MEDIA_PLAYPAUSE
      case 0xE9: usage = HID_USAGE_CONSUMER_STOP; break;                 // KEY_MEDIA_STOPCD
      case 0xEA: usage = HID_USAGE_CONSUMER_SCAN_PREVIOUS; break;        // KEY_MEDIA_PREVIOUSSONG
      case 0xEB: usage = HID_USAGE_CONSUMER_SCAN_NEXT; break;            // KEY_MEDIA_NEXTSONG
      case 0xEC: usage = 0; break;                                       // EjectCD (not standard in consumer usages list mapped here)
      case 0xED: usage = HID_USAGE_CONSUMER_VOLUME_INCREMENT; break;     // KEY_MEDIA_VOLUMEUP
      case 0xEE: usage = HID_USAGE_CONSUMER_VOLUME_DECREMENT; break;     // KEY_MEDIA_VOLUMEDOWN
      case 0xEF: usage = HID_USAGE_CONSUMER_MUTE; break;                 // KEY_MEDIA_MUTE
      case 0xF0: usage = 0; break; // WWW
      case 0xF1: usage = HID_USAGE_CONSUMER_AC_BACK; break;              // Browser Back
      case 0xF2: usage = HID_USAGE_CONSUMER_AC_FORWARD; break;           // Browser Forward
      case 0xF3: usage = HID_USAGE_CONSUMER_AC_STOP; break;              // Browser Stop
      case 0xF4: usage = HID_USAGE_CONSUMER_AC_SEARCH; break;            // Find/Search
      case 0xF5: usage = 0; break; // Scroll Up (no direct consumer usage)
      case 0xF6: usage = 0; break; // Scroll Down
      case 0xF7: usage = 0; break; // Edit
      case 0xF8: usage = HID_USAGE_CONSUMER_SLEEP; break;                // Sleep
      case 0xF9: usage = 0; break; // Coffee
      case 0xFA: usage = HID_USAGE_CONSUMER_AC_REFRESH; break;           // Refresh
      case 0xFB: usage = HID_USAGE_CONSUMER_AL_CALCULATOR; break;        // Calculator
    }

    if (usage) {
      if (pressed) {
        usb_hid.sendReport16(RID_CONSUMER, usage);
      } else {
        usb_hid.sendReport16(RID_CONSUMER, 0);
      }
      return;
    }
    // If usage is 0 (unsupported), fall-through to keyboard report as no-op
  }
  if (pressed) {
    // Avoid duplicates
    for (int i = 0; i < 6; ++i) {
      if (current_keys[i] == keycode) return;
    }
    modifierSend = modifier;
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

  usb_hid.keyboardReport(RID_KEYBOARD, modifierSend, current_keys);
}

void tapHIDKey(uint8_t keycode, uint8_t modifier) {
  if (!usb_hid.ready())
    return;
  // Handle consumer keys as a tap (press + release)
  if (keycode >= 0xE8 && keycode <= 0xFB) {
    uint16_t usage = 0;
    switch (keycode) {
      case 0xE8: usage = HID_USAGE_CONSUMER_PLAY_PAUSE; break;
      case 0xE9: usage = HID_USAGE_CONSUMER_STOP; break;
      case 0xEA: usage = HID_USAGE_CONSUMER_SCAN_PREVIOUS; break;
      case 0xEB: usage = HID_USAGE_CONSUMER_SCAN_NEXT; break;
      case 0xED: usage = HID_USAGE_CONSUMER_VOLUME_INCREMENT; break;
      case 0xEE: usage = HID_USAGE_CONSUMER_VOLUME_DECREMENT; break;
      case 0xEF: usage = HID_USAGE_CONSUMER_MUTE; break;
      case 0xF1: usage = HID_USAGE_CONSUMER_AC_BACK; break;
      case 0xF2: usage = HID_USAGE_CONSUMER_AC_FORWARD; break;
      case 0xF3: usage = HID_USAGE_CONSUMER_AC_STOP; break;
      case 0xF4: usage = HID_USAGE_CONSUMER_AC_SEARCH; break;
      case 0xF8: usage = HID_USAGE_CONSUMER_SLEEP; break;
      case 0xFA: usage = HID_USAGE_CONSUMER_AC_REFRESH; break;
      case 0xFB: usage = HID_USAGE_CONSUMER_AL_CALCULATOR; break;
      default: usage = 0; break;
    }
    if (usage) {
      usb_hid.sendReport16(RID_CONSUMER, usage);
      delay(10);
      usb_hid.sendReport16(RID_CONSUMER, 0);
      return;
    }
  }
  uint8_t keys[6] = { keycode };
  usb_hid.keyboardReport(RID_KEYBOARD, modifier, keys);
  delay(10);
  usb_hid.keyboardRelease(RID_KEYBOARD);  // Release all
}

void usb_clear_keyboard() {
  // Clear internal state and send a release to host
  for (int i = 0; i < 6; ++i) current_keys[i] = 0;
  modifier = 0;
  if (usb_hid.ready()) {
    usb_hid.keyboardRelease(RID_KEYBOARD);
  }
}

bool usb_is_ready() {
  return usb_hid.ready();
}

void usb_task() {
#ifdef TINYUSB_NEED_POLLING_TASK
  TinyUSBDevice.task();
#endif
}