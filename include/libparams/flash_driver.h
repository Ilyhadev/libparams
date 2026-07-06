/*
 * Copyright (c) 2022-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef LIBPARAM_ROM_FLASH_DRIVER_H_
#define LIBPARAM_ROM_FLASH_DRIVER_H_

#include <stdint.h>
#include <stddef.h>

#define FLASH_START_ADDR            0x08000000

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void (*init)(void);
    int8_t (*unlock)(void);
    int8_t (*lock)(void);
    int8_t (*erase)(uint32_t start_page_idx, uint32_t num_of_pages);
    int32_t (*write)(const uint8_t* data, size_t offset, size_t bytes_to_write);
    size_t (*read)(uint8_t* data, size_t offset, size_t bytes_to_read);
    uint16_t (*get_number_of_pages)(void);
    uint32_t (*get_page_size)(void);
    size_t start_addr;
} FlashDriverOps;


#ifdef __cplusplus
}
#endif

#endif  // LIBPARAM_ROM_FLASH_DRIVER_H_
