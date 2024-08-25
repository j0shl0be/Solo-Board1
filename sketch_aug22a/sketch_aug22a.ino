#include "Arduino.h"
#include "Adafruit_TinyUSB.h"
#include "RP2040.h"
#define activeState true
#define GPIO_OUT_SET (*(volatile uint32_t *)(SIO_BASE + 0x14))
#define GPIO_OUT_CLR (*(volatile uint32_t *)(SIO_BASE + 0x18))
#define GPIO_OUT_XOR (*(volatile uint32_t *)(SIO_BASE + 0x1C))
#define GPIO_DIR (*(volatile uint32_t *)(SIO_BASE + 0x20))
#define GPIO_DIR_SET (*(volatile uint32_t *)(SIO_BASE + 0x24))
#define GPIO_DIR_CLR (*(volatile uint32_t *)(SIO_BASE + 0x28))


uint8_t setpins[3] = { D0, D1, D2 };
uint8_t readpins[3] = { D3, D4, D5 };
uint8_t volumepin = A0;

Adafruit_USBD_HID usb_hid;

uint8_t const desc_hid_report[] = {
  TUD_HID_REPORT_DESC_KEYBOARD()
};

uint8_t hid_value[] = {
  HID_KEY_0, HID_KEY_1, HID_KEY_2, HID_KEY_3, HID_KEY_4, HID_KEY_5, HID_KEY_6, HID_KEY_7, HID_KEY_8
};

void setup() {
  // put your setup code here, to run once:
  for (int i = 0; i < 3; i++) {
    pinMode(setpins[i], OUTPUT);
    digitalWrite(setpins[i], LOW);
  }

  for (int i = 0; i < 3; i++) {
    pinMode(readpins[i], INPUT);
  }
  if (!TinyUSBDevice.isInitialized()) {
    TinyUSBDevice.begin(0);
  }

  usb_hid.setBootProtocol(HID_ITF_PROTOCOL_KEYBOARD);
  usb_hid.setPollInterval(2);
  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usb_hid.setStringDescriptor("SOLO Board1");
  // Set up output report (on control endpoint) for Capslock indicator
  usb_hid.setReportCallback(NULL, hid_report_callback);

  usb_hid.begin();
}

void set_volume(uint8_t volume, uint8_t *output, uint8_t *count) {
  static uint8_t prevVolume = 0x0F;

  if (volume == prevVolume) return;
  if (abs(volume - prevVolume) == 1) return;

  if (volume > prevVolume) {
    output[*count++] = 0x80;
    prevVolume++;
  } else {
    output[*count++] = 0x81;
    prevVolume--;
  }

  return;
}

void process_hid() {

  static bool keyPressedPreviously = false;

  uint8_t count = 0;
  uint8_t output[6] = { 0 };

  int sensorVal = analogRead(volumepin);
  uint8_t volume = map(sensorVal, 0, 1023, 0, 100);

  set_volume(volume, output, &count);

  //scan all keys
  for (int i = 0; i < 3; i++) {
    gpio_put(setpins[i], true);
    for (int j = 0; j < 3; j++) {
      if (activeState == gpio_get(readpins[j])) {
        output[count++] = hid_value[3 * i + j];
        if (count == 6) break;
      }
    }
    gpio_put(setpins[i], false);

    if (count == 6) break;
  }
  if (TinyUSBDevice.suspended() && count) {
    TinyUSBDevice.remoteWakeup();
  }

  if (!usb_hid.ready()) return;

  if (count) {

    uint8_t const report_id = 0;
    uint8_t const modifier = 0;

    keyPressedPreviously = true;
    usb_hid.keyboardReport(report_id, modifier, output);
  } else {
    if (keyPressedPreviously) {
      keyPressedPreviously = false;
      usb_hid.keyboardRelease(0);
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  TinyUSBDevice.task();

  if (!TinyUSBDevice.mounted()) return;

  //poll gpio every 2ms

  static uint32_t ms = 0;
  if (millis() - ms > 2) {
    ms = millis();
    process_hid();
  }
}

void hid_report_callback(uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize) {
  (void)report_id;
  (void)bufsize;

  if (report_type != HID_REPORT_TYPE_OUTPUT) return;
  uint8_t ledIndicator = buffer[0];
}