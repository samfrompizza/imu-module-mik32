#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#include "mik32_hal_gpio.h"
#include "mik32_memory_map.h"

#define SPI_PORT (SPI_1)

#define OLED_D0_PIN (2)
#define OLED_D0_PORT (GPIO_0)
#define OLED_D1_PIN (1)
#define OLED_D1_PORT (GPIO_0)

#define OLED_RES_PIN (GPIO_PIN_9)
#define OLED_RES_PORT (GPIO_1)
#define OLED_DC_PIN (GPIO_PIN_8)
#define OLED_DC_PORT (GPIO_1)
#define OLED_CS_PIN (GPIO_PIN_10)
#define OLED_CS_PORT (GPIO_0)

#define BTN_PIN_NUM (8)
#define BTN_PIN_PORT GPIO_0

#endif