#ifndef DRIVERS_IMU_SERVICE_H
#define DRIVERS_IMU_SERVICE_H

#include <stdbool.h>
#include "../app/imu_frame.h"

bool             imu_service_init(void);
bool             imu_service_poll(void);
const imu_frame_t *imu_service_get(void);

#endif