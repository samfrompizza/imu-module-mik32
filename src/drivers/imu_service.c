#include "imu_service.h"
#include "bno055_port.h"
#include "../../lib/BNO055_SensorAPI/bno055.h"

#include <math.h>
#include <string.h>

#define GRAVITY_FULL_LSB          (981)

#define HEADING_FULL_CIRCLE_LSB   (360 * IMU_LSB_PER_DEG)

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif
#define DEG_TO_RAD                (M_PI / 180.0f)

static struct bno055_t  s_bno055;
static imu_frame_t      s_frame;
static bool             s_have_frame = false;

bool imu_service_init(void)
{
    s_bno055.bus_write  = BNO055_I2C_bus_write;
    s_bno055.bus_read   = BNO055_I2C_bus_read;
    s_bno055.delay_msec = BNO055_delay_msek;
    s_bno055.dev_addr   = BNO055_I2C_ADDR2;

    if (bno055_init(&s_bno055) != BNO055_SUCCESS) {
        return false;
    }
    if (bno055_set_operation_mode(BNO055_OPERATION_MODE_NDOF) != BNO055_SUCCESS) {
        return false;
    }
    BNO055_delay_msek(50);
    return true;
}

static float heading_lsb_to_rad(int16_t heading_lsb)
{
    int32_t lsb = heading_lsb;
    while (lsb < 0) {
        lsb += HEADING_FULL_CIRCLE_LSB;
    }
    while (lsb >= HEADING_FULL_CIRCLE_LSB) {
        lsb -= HEADING_FULL_CIRCLE_LSB;
    }
    return ((float)lsb / (float)IMU_LSB_PER_DEG) * DEG_TO_RAD;
}

static void project_gravity_to_display(const struct bno055_gravity_t *g,
                                      float *out_u, float *out_v)
{
    float gx = (float)g->x;
    float gy = (float)g->y;
    float scale = 1.0f / (float)GRAVITY_FULL_LSB;
    *out_u = gy * scale;
    *out_v = gx * scale;
}

bool imu_service_poll(void)
{
    imu_frame_t frame;
    memset(&frame, 0, sizeof(frame));

    struct bno055_accel_t   accel   = {0};
    struct bno055_gravity_t gravity = {0};
    struct bno055_mag_t     mag     = {0};
    struct bno055_euler_t   euler   = {0};

    if (bno055_read_accel_xyz(&accel) != BNO055_SUCCESS)       return false;
    if (bno055_read_gravity_xyz(&gravity) != BNO055_SUCCESS)   return false;
    if (bno055_read_mag_xyz(&mag) != BNO055_SUCCESS)           return false;
    if (bno055_read_euler_hrp(&euler) != BNO055_SUCCESS)       return false;

    bno055_get_sys_calib_stat(&frame.calib_sys);
    bno055_get_gyro_calib_stat(&frame.calib_gyro);
    bno055_get_accel_calib_stat(&frame.calib_accel);
    bno055_get_mag_calib_stat(&frame.calib_mag);

    frame.raw_accel_x   = accel.x;
    frame.raw_accel_y   = accel.y;
    frame.raw_accel_z   = accel.z;
    frame.raw_gravity_x = gravity.x;
    frame.raw_gravity_y = gravity.y;
    frame.raw_gravity_z = gravity.z;
    frame.raw_mag_x     = mag.x;
    frame.raw_mag_y     = mag.y;
    frame.raw_mag_z     = mag.z;
    frame.raw_euler_h   = euler.h;
    frame.raw_euler_r   = euler.r;
    frame.raw_euler_p   = euler.p;

    project_gravity_to_display(&gravity, &frame.tilt_u, &frame.tilt_v);
    frame.heading_rad = heading_lsb_to_rad(euler.h);

    s_frame = frame;
    s_have_frame = true;
    return true;
}

const imu_frame_t *imu_service_get(void)
{
    return s_have_frame ? &s_frame : NULL;
}
