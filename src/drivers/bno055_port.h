#ifndef BNO055_PORT_H
#define BNO055_PORT_H

#include "mik32_hal.h"
#include "mik32_hal_i2c.h"
#include "../../lib/BNO055_SensorAPI/bno055.h"

void BNO055_delay_msek(u32 msek);
s8 BNO055_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
s8 BNO055_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);

#endif