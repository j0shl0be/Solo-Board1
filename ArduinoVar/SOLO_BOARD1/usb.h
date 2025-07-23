#pragma once
#include <stdint.h>

void usb_begin();
void send_key_press(uint8_t keycode);
void send_key_release();
void send_volume_change(int step);
