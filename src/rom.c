/*
 * Copyright (c) 2022-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "rom.h"
#include <stddef.h>
#include <string.h>
#include "libparams_error_codes.h"

int32_t libparams_rom_error_code = 0;

static bool isFlashOpsValid(const FlashDriverOps* flash) {
    return flash != NULL &&
           flash->init != NULL &&
           flash->unlock != NULL &&
           flash->lock != NULL &&
           flash->erase != NULL &&
           flash->write != NULL &&
           flash->read != NULL &&
           flash->get_number_of_pages != NULL &&
           flash->get_page_size != NULL;
}

RomDriverInstance romInit(const FlashDriverOps* flash, int32_t first_page_idx, size_t pages_amount) {
    RomDriverInstance rom = {0};
    if (!isFlashOpsValid(flash)) {
        return rom;
    }

    if (first_page_idx < 0) {
        first_page_idx += flash->get_number_of_pages();
    }

    if (first_page_idx < 0 ||
            first_page_idx + pages_amount > flash->get_number_of_pages() ||
            pages_amount == 0) {
        return rom;
    }

    flash->init();

    rom.flash = flash;
    rom.addr = flash->start_addr + first_page_idx * flash->get_page_size();
    rom.first_page_idx = first_page_idx;
    rom.total_size = pages_amount * flash->get_page_size();
    rom.pages_amount = pages_amount;
    rom.inited = true;
    rom.write_protected = true;
    return rom;
}

size_t romRead(const RomDriverInstance* rom, size_t offset, uint8_t* data, size_t requested_size) {
    if (rom == NULL || rom->flash == NULL || data == NULL ||
            offset >= rom->total_size || requested_size == 0) {
        return 0;
    }

    size_t allowed_size = rom->total_size - offset;
    size_t bytes_to_read;
    if (allowed_size < requested_size) {
        bytes_to_read = allowed_size;
    } else {
        bytes_to_read = requested_size;
    }

    return rom->flash->read(data,
                            rom->first_page_idx * rom->flash->get_page_size() + offset,
                            bytes_to_read);
}

void romBeginWrite(RomDriverInstance* rom) {
    if (rom == NULL || rom->flash == NULL) {
        return;
    }

    if (rom->flash->unlock() < 0) {
        return;
    }

    if (rom->flash->erase((uint32_t)rom->first_page_idx, (uint32_t)rom->pages_amount) < 0) {
        return;
    }

    rom->write_protected = false;
}

int32_t romWrite(const RomDriverInstance* rom, size_t offset, const uint8_t* data, size_t size) {
    if (rom == NULL || rom->flash == NULL || data == NULL ||
            offset >= rom->total_size || size == 0 || offset + size > rom->total_size) {
        libparams_rom_error_code = LIBPARAMS_ROM_WRITE_BAD_ARGS_ERROR;
        return LIBPARAMS_ROM_WRITE_BAD_ARGS_ERROR;
    }

    if (rom->write_protected) {
        libparams_rom_error_code = LIBPARAMS_ROM_WRITE_PROTECTED_ERROR;
        return LIBPARAMS_ROM_WRITE_PROTECTED_ERROR;
    }

    int32_t status = 0;
    status = rom->flash->write(data, (uint32_t)offset + rom->addr, size);

    if (status < 0) {
        libparams_rom_error_code = -1000 + status;
        return status;
    }

    return (int32_t)size;
}

uint32_t romGetAvailableMemory(const RomDriverInstance* rom) {
    if (rom == NULL) {
        return 0;
    }

    return (uint32_t)rom->total_size;
}

void romEndWrite(RomDriverInstance* rom) {
    if (rom == NULL || rom->flash == NULL) {
        return;
    }

    if (!rom->write_protected) {
        rom->flash->lock();
        rom->write_protected = true;
    }
}

int32_t romGetErrorCode() {
    int32_t temp = libparams_rom_error_code;
    libparams_rom_error_code = 0;
    return temp;
}
