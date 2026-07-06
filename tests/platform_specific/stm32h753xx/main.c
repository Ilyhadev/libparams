/*
 * Copyright (c) 2022-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "main.h"
#include <string.h>
#include "spi.h"

#define TEST_FLASH_SIZE  FLASH_SECTOR_SIZE
#define TEST_SPIFRAM_SIZE  (32U * 1024U)

static uint8_t flash_memory[TEST_FLASH_SIZE];
static uint8_t spifram_memory[TEST_SPIFRAM_SIZE];
static uint16_t spifram_address;

SPI_HandleTypeDef hspi5;

uint32_t HAL_GetTick(void) {
    return 0;
}

HAL_StatusTypeDef HAL_FLASH_Unlock(void) {
    return HAL_OK;
}

HAL_StatusTypeDef HAL_FLASH_Lock(void) {
    return HAL_OK;
}

HAL_StatusTypeDef HAL_FLASHEx_Erase(FLASH_EraseInitTypeDef* pEraseInit, uint32_t* SectorError) {
    if (pEraseInit == 0 || SectorError == 0 ||
            pEraseInit->TypeErase != FLASH_TYPEERASE_SECTORS ||
            pEraseInit->NbSectors == 0U ||
            pEraseInit->Sector + pEraseInit->NbSectors > FLASH_SECTOR_TOTAL ||
            pEraseInit->VoltageRange != FLASH_VOLTAGE_RANGE_3) {
        return HAL_ERROR;
    }

    *SectorError = 0xFFFFFFFFU;
    memset(flash_memory, 0xFF, sizeof(flash_memory));
    return HAL_OK;
}

HAL_StatusTypeDef HAL_FLASH_Program(uint32_t TypeProgram, uint32_t Address, uint32_t DataAddress) {
    if (TypeProgram != FLASH_TYPEPROGRAM_FLASHWORD ||
            Address < FLASH_START_ADDR ||
            Address + FLASH_NB_32BITWORD_IN_FLASHWORD * 4U > FLASH_START_ADDR + TEST_FLASH_SIZE ||
            Address % (FLASH_NB_32BITWORD_IN_FLASHWORD * 4U) != 0U) {
        return HAL_ERROR;
    }

    memcpy(&flash_memory[Address - FLASH_START_ADDR],
           (const void*)(uintptr_t)DataAddress,
           FLASH_NB_32BITWORD_IN_FLASHWORD * 4U);
    return HAL_OK;
}

void HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState) {
    (void)GPIOx;
    (void)GPIO_Pin;
    (void)PinState;
}

HAL_StatusTypeDef HAL_SPI_Transmit(SPI_HandleTypeDef* hspi,
                                   uint8_t* pData,
                                   uint16_t Size,
                                   uint32_t Timeout) {
    (void)Timeout;

    if (hspi != &hspi5 || pData == 0 || Size == 0U) {
        return HAL_ERROR;
    }

    if (Size == 1U && pData[0] == 0x06U) {
        return HAL_OK;
    }

    if (Size == 3U && (pData[0] == 0x02U || pData[0] == 0x03U)) {
        spifram_address = ((uint16_t)pData[1] << 8U) | pData[2];
        return HAL_OK;
    }

    if ((uint32_t)spifram_address + Size > sizeof(spifram_memory)) {
        return HAL_ERROR;
    }

    memcpy(&spifram_memory[spifram_address], pData, Size);
    spifram_address = (uint16_t)(spifram_address + Size);
    return HAL_OK;
}

HAL_StatusTypeDef HAL_SPI_Receive(SPI_HandleTypeDef* hspi,
                                  uint8_t* pData,
                                  uint16_t Size,
                                  uint32_t Timeout) {
    (void)Timeout;

    if (hspi != &hspi5 || pData == 0 ||
            Size == 0U ||
            (uint32_t)spifram_address + Size > sizeof(spifram_memory)) {
        return HAL_ERROR;
    }

    memcpy(pData, &spifram_memory[spifram_address], Size);
    spifram_address = (uint16_t)(spifram_address + Size);
    return HAL_OK;
}
