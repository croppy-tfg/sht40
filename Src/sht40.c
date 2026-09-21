/**
 * @file    sht40.c
 * @author  Jose
 * @brief   Driver library for SHT40 temperature & humidity sensor using I2C for STM32.
 * @date    20/9/26
 * @project firmware-root
 */

#include "sht40.h"

#include <stddef.h>
#include <stdint.h>
#include "FreeRTOS.h"

#include "stm32f4xx_hal_i2c.h"
#include "task.h"

#define SHT40_CRC8_POLYNOMIAL 0x31
#define SHT40_CRC8_INIT       0xFF

static uint8_t SHT40_CRC8(const uint8_t *data, size_t len)
{
    uint8_t crc = SHT40_CRC8_INIT;
    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t bit = 0; bit < 8; bit++) {
            if (crc & 0x80)
                crc = (crc << 1) ^ SHT40_CRC8_POLYNOMIAL;
            else
                crc = (crc << 1);
        }
    }
    return crc;
}

static void RTOS_Delay(const uint32_t ms) {
    vTaskDelay(pdMS_TO_TICKS(ms));
}

SHT40_Status_t SHT40_Init(SHT40_Handle_t *h, I2C_HandleTypeDef *hi2c, uint16_t addr)
{
    h->hi2c = hi2c;
    h->addr = addr;
    h->delay = RTOS_Delay;
    return SHT40_Reset(h);
}

SHT40_Status_t SHT40_Reset(SHT40_Handle_t *h)
{
    uint8_t cmd = SHT40_CMD_SOFT_RESET;
    if (HAL_I2C_Master_Transmit(h->hi2c, h->addr, &cmd, 1, 100) != HAL_OK)
        return SHT40_ERR_I2C;
    HAL_Delay(2);
    return SHT40_OK;
}

SHT40_Status_t SHT40_Read(SHT40_Handle_t *h, float *temperature, float *humidity)
{
    uint8_t cmd = SHT40_CMD_MEASURE_HIGH;
    uint8_t buf[6];

    if (HAL_I2C_Master_Transmit(h->hi2c, h->addr, &cmd, 1, 100) != HAL_OK)
        return SHT40_ERR_I2C;

    if (h->delay) h->delay(10);

    if (HAL_I2C_Master_Receive(h->hi2c, h->addr, buf, 6, 100) != HAL_OK)
        return SHT40_ERR_I2C;

    if (SHT40_CRC8(&buf[0], 2) != buf[2])
        return SHT40_ERR_CRC_TEMP;
    if (SHT40_CRC8(&buf[3], 2) != buf[5])
        return SHT40_ERR_CRC_HUM;

    uint16_t raw_t = ((uint16_t)buf[0] << 8) | buf[1];
    uint16_t raw_rh = ((uint16_t)buf[3] << 8) | buf[4];

    *temperature = -45.0f + 175.0f * ((float)raw_t / 65535.0f);

    float rh = -6.0f + 125.0f * ((float)raw_rh / 65535.0f);
    if (rh < 0.0f)   rh = 0.0f;
    if (rh > 100.0f) rh = 100.0f;
    *humidity = rh;

    return SHT40_OK;
}