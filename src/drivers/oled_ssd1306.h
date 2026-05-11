#ifndef OLED_SSD1306_H
#define OLED_SSD1306_H

#include <stdint.h>
#include <stdbool.h>
#include "../../hardware/mik32-hal/utilities/Include/mik32_hal_ssd1306.h"
#include "mik32_hal.h"
#include "board_config.h"
#include "mik32_hal_spi.h"

#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_BUF_SIZE (OLED_WIDTH * OLED_HEIGHT / 8)

extern uint8_t oled_buffer[OLED_BUF_SIZE];

void oled_init(SPI_HandleTypeDef *hspi);

void oled_clear();
void oled_fill();

void oled_update();

void oled_draw_pixel(uint8_t x, uint8_t y, bool color);

void oled_draw_bitmap_8x8(uint8_t x, uint8_t y, const uint8_t *bitmap);

void oled_test_screen();

#endif