#pragma once
#include <stdint.h>

typedef void (*KeyEventCallback)(uint8_t row, uint8_t col, bool pressed);

void matrix_begin();
void matrix_scan(KeyEventCallback cb);
void matrix_scantest(KeyEventCallback cb);
