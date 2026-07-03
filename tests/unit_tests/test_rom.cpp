/*
 * Copyright (c) 2022-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <gtest/gtest.h>
#include <cstring>
#include <iostream>
#include "rom.h"
#include "libparams_error_codes.h"
#include "flash_driver.h"
#include "platform_flash_driver.h"

namespace {

constexpr size_t CUSTOM_FLASH_START_ADDR = 0x1000U;
constexpr size_t CUSTOM_FLASH_PAGE_SIZE = 32U;
constexpr size_t CUSTOM_FLASH_NUM_PAGES = 2U;

uint8_t custom_flash[CUSTOM_FLASH_PAGE_SIZE * CUSTOM_FLASH_NUM_PAGES];
bool custom_unlocked = false;
bool custom_erase_called = false;
bool custom_write_called = false;
bool custom_read_called = false;

void customFlashInit() {
    memset(custom_flash, 0, sizeof(custom_flash));
    custom_unlocked = false;
    custom_erase_called = false;
    custom_write_called = false;
    custom_read_called = false;
}

int8_t customFlashUnlock() {
    custom_unlocked = true;
    return LIBPARAMS_OK;
}

int8_t customFlashLock() {
    custom_unlocked = false;
    return LIBPARAMS_OK;
}

int8_t customFlashErase(uint32_t start_page_idx, uint32_t num_of_pages) {
    if (!custom_unlocked ||
            num_of_pages == 0U ||
            start_page_idx + num_of_pages > CUSTOM_FLASH_NUM_PAGES) {
        return LIBPARAMS_WRONG_ARGS;
    }
    custom_erase_called = true;
    memset(&custom_flash[start_page_idx * CUSTOM_FLASH_PAGE_SIZE],
           0xFF,
           num_of_pages * CUSTOM_FLASH_PAGE_SIZE);
    return LIBPARAMS_OK;
}

int32_t customFlashWrite(const uint8_t* data, size_t offset, size_t bytes_to_write) {
    if (!custom_unlocked ||
            data == nullptr ||
            offset < CUSTOM_FLASH_START_ADDR ||
            offset + bytes_to_write > CUSTOM_FLASH_START_ADDR + sizeof(custom_flash)) {
        return LIBPARAMS_WRONG_ARGS;
    }
    custom_write_called = true;
    memcpy(&custom_flash[offset - CUSTOM_FLASH_START_ADDR], data, bytes_to_write);
    return static_cast<int32_t>(bytes_to_write);
}

size_t customFlashRead(uint8_t* data, size_t offset, size_t bytes_to_read) {
    if (data == nullptr || offset + bytes_to_read > sizeof(custom_flash)) {
        return 0U;
    }
    custom_read_called = true;
    memcpy(data, &custom_flash[offset], bytes_to_read);
    return bytes_to_read;
}

uint16_t customFlashGetNumberOfPages() {
    return CUSTOM_FLASH_NUM_PAGES;
}

uint32_t customFlashGetPageSize() {
    return CUSTOM_FLASH_PAGE_SIZE;
}

const FlashDriverOps custom_flash_ops = {
    customFlashInit,
    customFlashUnlock,
    customFlashLock,
    customFlashErase,
    customFlashWrite,
    customFlashRead,
    customFlashGetNumberOfPages,
    customFlashGetPageSize,
    CUSTOM_FLASH_START_ADDR,
};

}  // namespace

class RomDriverMultiplePagesTest : public ::testing::Test {
protected:
    RomDriverInstance rom;

    void SetUp() override {
        const FlashDriverOps* flash = ubuntuFlashGetOps();
        rom = romInit(flash, 0, flash->get_number_of_pages());
    }
};

// Test Case 1. Initialize ROM Driver Instance
// Test 1.1: Initialize with Single Latest Page, Negative Number
TEST(TestRom, initializeWithSingleLatestPageNegativeNumber) {
    auto rom = romInit(ubuntuFlashGetOps(), -1, 1);
    ASSERT_TRUE(rom.inited)<< "Failed to init with single latest page using negative number";
}
// Test 1.2: Initialize with Single Latest Page, Positive Number
TEST(TestRom, initializeWithSingleLatestPagePositiveNumber) {
    const FlashDriverOps* flash = ubuntuFlashGetOps();
    auto rom = romInit(flash, flash->get_number_of_pages() - 1, 1);
    ASSERT_TRUE(rom.inited) << "Failed to init with single latest page using positive number";
}
// Test 1.3: Initialize with Multiple Pages
TEST_F(RomDriverMultiplePagesTest, initializeWithMultiplePages) {
    ASSERT_TRUE(rom.inited) << "Failed to init with multiple pages";
}
// Test 1.4: Initialize with Invalid Page Index
TEST(TestRom, initializeWithInvalidPageIndex) {
    const FlashDriverOps* flash = ubuntuFlashGetOps();
    auto rom = romInit(flash, flash->get_number_of_pages(), 1);
    ASSERT_FALSE(rom.inited) << "Initialized with invalid page index";
}
// Test 1.5: Initialize with Zero Pages
TEST(TestRom, initializeWithZeroPages) {
    auto rom = romInit(ubuntuFlashGetOps(), -1, 0);
    ASSERT_FALSE(rom.inited) << "Initialized with zero pages";
}
// Test 1.6: Initialize with null flash ops
TEST(TestRom, initializeWithNullFlashOps) {
    auto rom = romInit(nullptr, -1, 1);
    ASSERT_FALSE(rom.inited) << "Initialized with null flash ops";
}


// Test Case 2: Read from ROM
// Test 2.1: Read Data within Bounds
TEST_F(RomDriverMultiplePagesTest, readDataWithingBounds) {
    const auto ROM_SIZE = romGetAvailableMemory(&rom);
    uint8_t data[ROM_SIZE];

    // Read a chank of data
    ASSERT_EQ(romRead(&rom, 0, data, 8), 8) << "Failed to read data within bounds";

    // Read everything
    ASSERT_EQ(romRead(&rom, 0, data, ROM_SIZE), ROM_SIZE) << "Failed to read entire ROM size";
}
// Test 2.2: Read Data Exceeding Bounds
TEST_F(RomDriverMultiplePagesTest, readDataExceedingBounds) {
    const auto ROM_SIZE = romGetAvailableMemory(&rom);
    uint8_t data[ROM_SIZE];

    // Normal, clamped read
    ASSERT_EQ(romRead(&rom, 0, data, ROM_SIZE + 1), ROM_SIZE) << "Failed to clamp read";
    ASSERT_EQ(romRead(&rom, 1, data, ROM_SIZE), ROM_SIZE - 1) << "Failed to clamp read";

    // Wrong inputs
    ASSERT_EQ(romRead(nullptr, 0, data, ROM_SIZE), 0) << "Read should fail with nullptr ROM";
    ASSERT_EQ(romRead(&rom, 0, nullptr, ROM_SIZE), 0) << "Read should fail with nullptr data";
    ASSERT_EQ(romRead(&rom, ROM_SIZE, data, 1), 0) << "Read should fail: offset out of bounds";
    ASSERT_EQ(romRead(&rom, 0, data, 0), 0) << "Read should fail with zero size";
}

// Test Case 3: Write to ROM
// Test 3.1: Write Data within Bounds
TEST_F(RomDriverMultiplePagesTest, writeDataWithingBounds) {
    const uint8_t SAMPLE_DATA[] = {1, 2, 3, 4, 5, 6, 7, 8};
    uint8_t read_data[sizeof(SAMPLE_DATA)] = {};

    romBeginWrite(&rom);
    ASSERT_EQ(romWrite(&rom, 0, SAMPLE_DATA, sizeof(SAMPLE_DATA)), sizeof(SAMPLE_DATA));
    romEndWrite(&rom);

    ASSERT_EQ(romRead(&rom, 0, read_data, sizeof(SAMPLE_DATA)), sizeof(SAMPLE_DATA));
    ASSERT_EQ(memcmp(SAMPLE_DATA, read_data, sizeof(SAMPLE_DATA)), 0);
}

TEST(TestRom, usesInstanceFlashOps) {
    RomDriverInstance rom = romInit(&custom_flash_ops, 1, 1);
    ASSERT_TRUE(rom.inited);
    ASSERT_EQ(rom.flash, &custom_flash_ops);
    ASSERT_EQ(rom.addr, CUSTOM_FLASH_START_ADDR + CUSTOM_FLASH_PAGE_SIZE);

    const uint8_t data[] = {9U, 8U, 7U, 6U};
    uint8_t read_data[sizeof(data)] = {};

    romBeginWrite(&rom);
    ASSERT_EQ(romWrite(&rom, 0, data, sizeof(data)), sizeof(data));
    romEndWrite(&rom);
    ASSERT_TRUE(custom_erase_called);
    ASSERT_TRUE(custom_write_called);

    ASSERT_EQ(romRead(&rom, 0, read_data, sizeof(read_data)), sizeof(read_data));
    ASSERT_TRUE(custom_read_called);
    ASSERT_EQ(memcmp(data, read_data, sizeof(data)), 0);
}
// Test 3.2: Write Data Exceeding Bounds
TEST_F(RomDriverMultiplePagesTest, writeDataExceedingBounds) {
    const uint8_t SAMPLE_DATA[] = {1, 2, 3, 4, 5, 6, 7, 8};

    // 1. ROM nullptr
    romBeginWrite(&rom);
    ASSERT_EQ(romWrite(nullptr, 0, SAMPLE_DATA, sizeof(SAMPLE_DATA)), LIBPARAMS_ROM_WRITE_BAD_ARGS_ERROR);
    romEndWrite(&rom);

    // 2. Offset out of bound
    ASSERT_EQ(romWrite(&rom, rom.total_size, SAMPLE_DATA, sizeof(SAMPLE_DATA)), LIBPARAMS_ROM_WRITE_BAD_ARGS_ERROR);

    // 3. Data nullptr
    romBeginWrite(&rom);
    ASSERT_EQ(romWrite(&rom, 0, nullptr, sizeof(SAMPLE_DATA)), LIBPARAMS_ROM_WRITE_BAD_ARGS_ERROR);
    romEndWrite(&rom);

    // 4. Zero wring size
    romBeginWrite(&rom);
    ASSERT_EQ(romWrite(&rom, 0, SAMPLE_DATA, 0), LIBPARAMS_ROM_WRITE_BAD_ARGS_ERROR);
    romEndWrite(&rom);

    // 5. Offset and size out of bound
    romBeginWrite(&rom);
    ASSERT_EQ(romWrite(&rom, 3000, SAMPLE_DATA, 3000), LIBPARAMS_ROM_WRITE_BAD_ARGS_ERROR);
    romEndWrite(&rom);

    // 6. Without romBeginWrite() and romEndWrite()
    ASSERT_EQ(romWrite(&rom, 0, SAMPLE_DATA, sizeof(SAMPLE_DATA)), LIBPARAMS_ROM_WRITE_PROTECTED_ERROR);

    // 7. romBeginWrite() and romEndWrite() nullptr
    romBeginWrite(nullptr);
    ASSERT_EQ(romWrite(&rom, 0, SAMPLE_DATA, sizeof(SAMPLE_DATA)), LIBPARAMS_ROM_WRITE_PROTECTED_ERROR);
    romEndWrite(nullptr);
}

// Test Case 4: Get Available Memory
// Test 4.1: Verify Available Memory Calculation with correct_input
TEST_F(RomDriverMultiplePagesTest, verifyAvaliableMemoryCalculationWithCorrectInput) {
    const FlashDriverOps* flash = ubuntuFlashGetOps();
    ASSERT_EQ(romGetAvailableMemory(&rom), flash->get_number_of_pages() * flash->get_page_size());
}
// Test 4.2: Verify Available Memory Calculation with nullptr
TEST_F(RomDriverMultiplePagesTest, test_4_2_verifyAvaliableMemoryCalculationWithNullptr) {
    ASSERT_EQ(romGetAvailableMemory(nullptr), 0);
}

int main (int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
