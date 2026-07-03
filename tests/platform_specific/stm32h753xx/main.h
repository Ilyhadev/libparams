/*
 * Copyright (c) 2022-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef STM32H753XX_TEST_MAIN_H_
#define STM32H753XX_TEST_MAIN_H_

#include "stm32h7xx_hal.h"

#define SPI5_NCS1_FRAM_GPIO_Port  ((GPIO_TypeDef*)0x50000000U)
#define SPI5_NCS1_FRAM_Pin        7U

#endif  // STM32H753XX_TEST_MAIN_H_
