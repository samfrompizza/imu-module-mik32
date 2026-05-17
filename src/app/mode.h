#ifndef APP_MODE_H
#define APP_MODE_H

#include "imu_frame.h"
#include "../../hardware/mik32-hal/utilities/Include/mik32_hal_ssd1306.h"

typedef struct
{
    const char *name;
    void (*enter)(void);
    void (*render)(HAL_SSD1306_HandleTypeDef *display, const imu_frame_t *frame);
} mode_t;

#endif