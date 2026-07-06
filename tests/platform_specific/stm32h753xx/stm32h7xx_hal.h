/*
 * Copyright (c) 2026 Ilia Kliantsevich <iliawork112005@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef STM32H7XX_HAL_MOCK_H_
#define STM32H7XX_HAL_MOCK_H_

#include <stdint.h>

#define FLASH_TYPEERASE_SECTORS              0x00U
#define FLASH_TYPEPROGRAM_FLASHWORD          0x01U
#define FLASH_BANK_1                         1U
#define FLASH_BANK_2                         2U
#define FLASH_VOLTAGE_RANGE_3                3U
#define FLASH_NB_32BITWORD_IN_FLASHWORD      8U
#define FLASH_SECTOR_SIZE                    (128U * 1024U)
#define FLASH_SECTOR_TOTAL                   8U
#ifndef FLASH_START_ADDR
#define FLASH_START_ADDR                     0x08000000U
#endif
#define GPIO_PIN_RESET                       0U
#define GPIO_PIN_SET                         1U

typedef enum
{
    HAL_OK       = 0x00U,
    HAL_ERROR    = 0x01U,
    HAL_BUSY     = 0x02U,
    HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;

typedef uint32_t GPIO_PinState;

typedef struct
{
    uint32_t TypeErase;
    uint32_t Banks;
    uint32_t Sector;
    uint32_t NbSectors;
    uint32_t VoltageRange;
} FLASH_EraseInitTypeDef;

typedef struct
{
    uint32_t dummy;
} GPIO_TypeDef;

typedef struct
{
    uint32_t dummy;
} SPI_HandleTypeDef;

uint32_t HAL_GetTick(void);
HAL_StatusTypeDef HAL_FLASH_Unlock(void);
HAL_StatusTypeDef HAL_FLASH_Lock(void);
HAL_StatusTypeDef HAL_FLASHEx_Erase(FLASH_EraseInitTypeDef* pEraseInit, uint32_t* SectorError);
HAL_StatusTypeDef HAL_FLASH_Program(uint32_t TypeProgram, uint32_t Address, uint32_t DataAddress);
void HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
HAL_StatusTypeDef HAL_SPI_Transmit(SPI_HandleTypeDef* hspi,
                                   uint8_t* pData,
                                   uint16_t Size,
                                   uint32_t Timeout);
HAL_StatusTypeDef HAL_SPI_Receive(SPI_HandleTypeDef* hspi,
                                  uint8_t* pData,
                                  uint16_t Size,
                                  uint32_t Timeout);

#endif  // STM32H7XX_HAL_MOCK_H_
