/**
 * @file iim42652.h
 * @author Michał Słomiany (m.slomiany@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-01-26
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef IIM42652_H
#define IIM42652_H

/* Includes */
#include "iim42652_regs.h"
#include "main.h"

typedef struct iim_raw_data
{
    int16_t x;
    int16_t y;
    int16_t z;
} iim_raw_data;

typedef struct iim_scaled_data
{
    float x;
    float y;
    float z;
} iim_scaled_data;

typedef struct iim_status
{
    I2C_HandleTypeDef *i2c_h;
    uint8_t gyro_fs;
    uint8_t gyro_odr;
    uint8_t acc_fs;
    uint8_t acc_odr;
} iim_status;

void IIM_init(I2C_HandleTypeDef *i2c_handler);

void IIM_readTemperature(float *temperature);

void IIM_powerOn();

void IIM_readAccel(iim_raw_data *data);

void IIM_readGyro(iim_raw_data *data);

void IIM_convertAccel(iim_scaled_data *output, iim_raw_data input);

void IIM_convertGyro(iim_scaled_data *output, iim_raw_data input);

void IIM_configAccel(uint8_t fs, uint8_t odr);

void IIM_configGyro(uint8_t fs, uint8_t odr);

#endif
