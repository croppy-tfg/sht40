/**
 * @file    sht40.h
 * @author  Jose
 * @brief   Driver library for SHT40 temperature & humidity sensor using I2C for STM32.
 * @date    20/9/26
 * @project firmware-root
 */

#ifndef SHT40_H
#define SHT40_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"

#define SHT40_ADDRESS (0x44 << 1)

#define SHT40_CMD_MEASURE_HIGH  0xFD
#define SHT40_CMD_MEASURE_MED   0xF6
#define SHT40_CMD_MEASURE_LOW   0xE0
#define SHT40_CMD_SOFT_RESET    0x94

typedef enum {
    SHT40_OK = 0,
    SHT40_ERR_I2C,
    SHT40_ERR_CRC_TEMP,
    SHT40_ERR_CRC_HUM
} SHT40_Status_t;

typedef void (*SHT40_Delay_Fn)(uint32_t ms);

typedef struct {
    I2C_HandleTypeDef *hi2c;
    uint16_t           addr;
    SHT40_Delay_Fn     delay;
} SHT40_Handle_t;

SHT40_Status_t SHT40_Init(SHT40_Handle_t *h, I2C_HandleTypeDef *hi2c, uint16_t addr);
SHT40_Status_t SHT40_Reset(SHT40_Handle_t *h);
SHT40_Status_t SHT40_Read(SHT40_Handle_t *h, float *temperature, float *humidity);

#endif //SHT40_H
