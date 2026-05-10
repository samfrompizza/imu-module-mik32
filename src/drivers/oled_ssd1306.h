#ifndef OLED_SSD1306_H
#define OLED_SSD1306_H

#include <stdint.h>
#include "../../hardware/mik32-hal/utilities/Include/mik32_hal_ssd1306.h"
#include "board_config.h"

void oled_hw_init();
void oled_send_cmd();
void oled_send_data();
void oled_init();
void oled_clear();

#endif