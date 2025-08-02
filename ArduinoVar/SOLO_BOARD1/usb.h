#pragma once
#include <stdint.h>
#include <Arduino.h>
#include <Adafruit_TinyUSB.h>

#define RID_KEYBOARD 1

// USB HID report descriptor
extern uint8_t const desc_hid_report[];

// USB HID object
extern Adafruit_USBD_HID usb_hid;

// Function declarations
void usb_begin();
void sendHIDKey(uint8_t keycode, bool pressed);
void tapHIDKey(uint8_t keycode);
bool usb_is_ready();
void usb_task();
