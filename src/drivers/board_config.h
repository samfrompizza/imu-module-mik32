#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#include "mik32_hal_gpio.h"
#include "mik32_memory_map.h"

#define OLED_D0_PIN (2)
#define OLED_D0_PORT (GPIO_0)
#define OLED_D1_PIN (1)
#define OLED_D1_PORT (GPIO_0)
#define OLED_RES_PIN (10)
#define OLED_RES_PORT (GPIO_0)
#define OLED_DC_PIN (0)
#define OLED_DC_PORT (GPIO_0)
#define OLED_CS_PIN (4)
#define OLED_CS_PORT (GPIO_1)

#endif