#include "mik32_hal.h"
#include "mik32_hal_i2c.h"
#include "../../lib/BNO055_SensorAPI/bno055.h"

extern I2C_HandleTypeDef hi2c;

void BNO055_delay_msek(u32 msek)
{
    HAL_DelayMs(msek);
}

s8 BNO055_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
    uint8_t buffer[cnt + 1];
    buffer[0] = reg_addr;
    for (int i = 0; i < cnt; i++) {
        buffer[i + 1] = reg_data[i];
    }

    if (HAL_I2C_Master_Transmit(&hi2c, (uint16_t)dev_addr, buffer, cnt + 1, I2C_TIMEOUT_DEFAULT) != 0)
    {
        return 1;
    }
    return 0;
}

s8 BNO055_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
    HAL_StatusTypeDef status;
    HAL_I2C_AutoEndModeTypeDef old_autoend = hi2c.Init.AutoEnd;

    uint8_t addr_buf = reg_addr;

    HAL_I2C_AutoEnd(&hi2c, I2C_AUTOEND_DISABLE);

    status = HAL_I2C_Master_Transmit(&hi2c, (uint16_t)dev_addr, &addr_buf, 1, I2C_TIMEOUT_DEFAULT);
    if (status != 0) {
        HAL_I2C_AutoEnd(&hi2c, old_autoend);
        return 1;
    }

    HAL_I2C_AutoEnd(&hi2c, old_autoend);

    status = HAL_I2C_Master_Receive(&hi2c, (uint16_t)dev_addr, reg_data, cnt, I2C_TIMEOUT_DEFAULT);
    if (status != 0) {
        return 1;
    }

    return 0;
}