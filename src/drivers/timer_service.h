#ifndef DRIVERS_TIMER_SERVICE_H
#define DRIVERS_TIMER_SERVICE_H

#include <stdbool.h>
#include "mik32_hal_timer32.h"

#define TIMER_IMU_HZ (20U)
#define TIMER_DISPLAY_HZ (20U)

void timer_service_init(void);

bool timer_service_take_imu_tick(void);
bool timer_service_take_display_tick(void);

TIMER32_HandleTypeDef *timer_service_imu_handle(void);
TIMER32_HandleTypeDef *timer_service_display_handle(void);
void timer_service_isr_on_imu_tick(void);
void timer_service_isr_on_display_tick(void);

#endif
