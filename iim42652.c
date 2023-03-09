/**
 * @file iim42652.c
 * @author Michał Słomiany (m.slomiany@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-01-26
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef IIM42652_C
#define IIM42652_C

/* Includes */
#include "iim42652.h"
#include "stm32f4xx_hal.h"

iim_status status;

void IIM_init(I2C_HandleTypeDef *i2c_handler)
{
    status.i2c_h = i2c_handler;
    status.gyro_fs = SET_GYRO_FS_SEL_2000_dps;
    status.gyro_odr = SET_GYRO_ODR_1kHz;
    status.acc_fs = SET_ACCEL_FS_SEL_16g;
    status.acc_odr = SET_ACCEL_ODR_1kHz;
}

void IIM_readTemperature(float *temperature)
{
    uint8_t tmp[2];
    HAL_I2C_Mem_Read(status.i2c_h, IIM_ADR, TEMP_DATA1_UI, 1, tmp, 2, 10);

    int16_t temp_int = (tmp[0] << 8 | tmp[1]);
    *temperature = (float)temp_int;
    *temperature = ((*temperature) / 132.48) + 25.0;
}

void IIM_powerOn()
{
    uint8_t msg = 0x1F;
    HAL_I2C_Mem_Write(status.i2c_h, IIM_ADR, PWR_MGMT0, 1, &msg, 1, 10);
    HAL_Delay(500);
}

void IIM_readAccel(iim_raw_data *data)
{
    uint8_t tmp[6];
    uint16_t temp;

    HAL_I2C_Mem_Read(status.i2c_h, IIM_ADR, ACCEL_DATA_X1_UI, 1, tmp, 6, 10);

    temp = (tmp[0] << 8 | tmp[1]);
    data->x = (int16_t)temp;

    temp = (tmp[2] << 8 | tmp[3]);
    data->y = (int16_t)temp;

    temp = (tmp[4] << 8 | tmp[5]);
    data->z = (int16_t)temp;
}

void IIM_readGyro(iim_raw_data *data)
{
    uint8_t tmp[6];
    uint16_t temp;

    HAL_I2C_Mem_Read(status.i2c_h, IIM_ADR, GYRO_DATA_X1_UI, 1, tmp, 6, 10);

    temp = (tmp[0] << 8 | tmp[1]);
    data->x = (int16_t)temp;

    temp = (tmp[2] << 8 | tmp[3]);
    data->y = (int16_t)temp;

    temp = (tmp[4] << 8 | tmp[5]);
    data->z = (int16_t)temp;
}

void IIM_convertAccel(iim_scaled_data *output, iim_raw_data input)
{
    output->x = ((float)input.x * 16.0) / 32768.0;
    output->y = ((float)input.y * 16.0) / 32768.0;
    output->z = ((float)input.z * 16.0) / 32768.0;
}

void IIM_convertGyro(iim_scaled_data *output, iim_raw_data input)
{
    output->x = ((float)input.x * 2000.0) / 32768.0;
    output->y = ((float)input.y * 2000.0) / 32768.0;
    output->z = ((float)input.z * 2000.0) / 32768.0;
}

void IIM_configAccel(uint8_t fs, uint8_t odr)
{
    status.acc_fs = fs;
    status.acc_odr = odr;
    uint8_t tmp;
    tmp = fs << 5;
    tmp |= odr;
    HAL_I2C_Mem_Write(status.i2c_h, IIM_ADR, ACCEL_CONFIG0, 1, &tmp, 1, 10);
}

void IIM_configGyro(uint8_t fs, uint8_t odr)
{
    status.gyro_fs = fs;
    status.gyro_odr = odr;
    uint8_t tmp;
    tmp = fs << 5;
    tmp |= odr;
    HAL_I2C_Mem_Write(status.i2c_h, IIM_ADR, GYRO_CONFIG0, 1, &tmp, 1, 10);
}

#endif
