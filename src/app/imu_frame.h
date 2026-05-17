#ifndef APP_IMU_FRAME_H
#define APP_IMU_FRAME_H

#include <stdint.h>

#define IMU_LSB_PER_MS2 (100)
#define IMU_LSB_PER_DEG (16)
#define IMU_LSB_PER_UT (16)

#define IMU_CALIB_LEVEL_MAX (3U)

typedef struct
{
    int16_t raw_accel_x, raw_accel_y, raw_accel_z;
    int16_t raw_gravity_x, raw_gravity_y, raw_gravity_z;
    int16_t raw_mag_x, raw_mag_y, raw_mag_z;
    int16_t raw_euler_h, raw_euler_r, raw_euler_p;

    uint8_t calib_sys;
    uint8_t calib_gyro;
    uint8_t calib_accel;
    uint8_t calib_mag;

    float tilt_u;
    float tilt_v;

    float heading_rad;
} imu_frame_t;

#endif
