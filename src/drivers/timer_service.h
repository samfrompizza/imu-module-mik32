#ifndef DRIVERS_TIMER_SERVICE_H
#define DRIVERS_TIMER_SERVICE_H

#include <stdbool.h>
#include "mik32_hal_timer32.h"

#define TIMER_IMU_HZ (20U)
#define TIMER_DISPLAY_HZ (20U)

void timer_service_init();

bool timer_service_take_imu_tick();
bool timer_service_take_display_tick();

TIMER32_HandleTypeDef *timer_service_imu_handle();
TIMER32_HandleTypeDef *timer_service_display_handle();
void timer_service_isr_on_imu_tick();
void timer_service_isr_on_display_tick();

#endif
