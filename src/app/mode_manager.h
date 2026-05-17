#ifndef APP_MODE_MANAGER_H
#define APP_MODE_MANAGER_H

#include "mode.h"

typedef enum
{
    APP_MODE_WELCOME = 0,
    APP_MODE_DEBUG,
    APP_MODE_LEVEL,
    APP_MODE_COMPASS,
    APP_MODE_BALL,
    APP_MODE_COUNT
} app_mode_id_t;

void mode_manager_init();
void mode_manager_handle_button();
void mode_manager_render(HAL_SSD1306_HandleTypeDef *display,
                         const imu_frame_t *frame);
app_mode_id_t mode_manager_current();

#endif
