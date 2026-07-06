/*
 * Copyright (c) 2022-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "rom.h"
#include "flash_driver.h"
#include "platform_flash_driver.h"

void test_flash_wr(void) {
    const FlashDriverOps* flash = stm32h753xxInternalFlashGetOps();
    RomDriverInstance rom = romInit(flash, 0, 1);

    const FlashDriverOps* spifram = stm32h753xxSpiFramGetOps();
    RomDriverInstance spifram_rom = romInit(spifram, 0, 1);

    const uint8_t first_buf[32] = {0};
    romWrite(&rom, 0, first_buf, sizeof(first_buf));
    romWrite(&spifram_rom, 0, first_buf, sizeof(first_buf));

    uint8_t second_buf[32];
    romRead(&rom, 0, second_buf, sizeof(second_buf));
    romRead(&spifram_rom, 0, second_buf, sizeof(second_buf));
}

int main(void) {
    test_flash_wr();

    return 0;
}
