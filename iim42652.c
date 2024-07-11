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

iim_status dev;

void IIM_init(SPI_HandleTypeDef *spi_handler, GPIO_TypeDef *cs_port, uint16_t cs_pin)
{
    dev.spi_h = spi_handler;
    dev.cs_port = cs_port;
    dev.cs_pin = cs_pin;

    dev.gyro_fs = SET_GYRO_FS_SEL_2000_dps;
    dev.gyro_odr = SET_GYRO_ODR_1kHz;
    dev.acc_fs = SET_ACCEL_FS_SEL_16g;
    dev.acc_odr = SET_ACCEL_ODR_1kHz;
}

void IIM_readTemperature(float *temperature)
{
    uint8_t txBuffer[3] = {0, 0xFF, 0xFF};
    uint8_t rxBuffer[3] = {0, 0, 0};

    txBuffer[0] = TEMP_DATA1_UI | 0x80;

    HAL_GPIO_WritePin(dev.cs_port, dev.cs_pin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(dev.spi_h, txBuffer, rxBuffer, 3, 10);
    HAL_GPIO_WritePin(dev.cs_port, dev.cs_pin, GPIO_PIN_SET);

    int16_t temp_int = (rxBuffer[1] << 8 | rxBuffer[2]);
    *temperature = (float)temp_int;
    *temperature = ((*temperature) / 132.48) + 25.0;
}

void IIM_powerOn()
{
    uint8_t txBuffer[2];                       // Transmit buffer (1: address 2:3: data)
    txBuffer[0] = PWR_MGMT0; // In write oprerations, first bit in SPI transfer has to be 1
    txBuffer[1] = 0x1F;                 // Assign command to transmit buffer

    HAL_GPIO_WritePin(dev.cs_port, dev.cs_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(dev.spi_h, txBuffer, 2, 10);
    HAL_GPIO_WritePin(dev.cs_port, dev.cs_pin, GPIO_PIN_SET);

    HAL_Delay(500);
}

void IIM_readAccel(iim_raw_data *data)
{
    uint16_t temp;

    uint8_t txBuffer[7] = {0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t rxBuffer[7] = {0, 0, 0, 0, 0, 0, 0};

    txBuffer[0] = ACCEL_DATA_X1_UI | 0x80;

    HAL_GPIO_WritePin(dev.cs_port, dev.cs_pin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(dev.spi_h, txBuffer, rxBuffer, 7, 10);
    HAL_GPIO_WritePin(dev.cs_port, dev.cs_pin, GPIO_PIN_SET);

    temp = (rxBuffer[1] << 8 | rxBuffer[2]);
    data->x = (int16_t)temp;

    temp = (rxBuffer[3] << 8 | rxBuffer[4]);
    data->y = (int16_t)temp;

    temp = (rxBuffer[5] << 8 | rxBuffer[6]);
    data->z = (int16_t)temp;
}

void IIM_readGyro(iim_raw_data *data)
{
    uint16_t temp;

    uint8_t txBuffer[7] = {0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t rxBuffer[7] = {0, 0, 0, 0, 0, 0, 0};

    txBuffer[0] = GYRO_DATA_X1_UI | 0x80;

    HAL_GPIO_WritePin(dev.cs_port, dev.cs_pin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(dev.spi_h, txBuffer, rxBuffer, 7, 10);
    HAL_GPIO_WritePin(dev.cs_port, dev.cs_pin, GPIO_PIN_SET);

    temp = (rxBuffer[1] << 8 | rxBuffer[2]);
    data->x = (int16_t)temp;

    temp = (rxBuffer[3] << 8 | rxBuffer[4]);
    data->y = (int16_t)temp;

    temp = (rxBuffer[5] << 8 | rxBuffer[6]);
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
    dev.acc_fs = fs;
    dev.acc_odr = odr;
    uint8_t tmp;
    tmp = fs << 5;
    tmp |= odr;
    HAL_I2C_Mem_Write(dev.spi_h, IIM_ADR, ACCEL_CONFIG0, 1, &tmp, 1, 10);
}

void IIM_configGyro(uint8_t fs, uint8_t odr)
{
    dev.gyro_fs = fs;
    dev.gyro_odr = odr;
    uint8_t tmp;
    tmp = fs << 5;
    tmp |= odr;
    HAL_I2C_Mem_Write(dev.spi_h, IIM_ADR, GYRO_CONFIG0, 1, &tmp, 1, 10);
}

#endif
