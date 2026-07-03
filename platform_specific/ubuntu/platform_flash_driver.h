/*
 * Copyright (c) 2026 Ilia Kliantsevich <iliawork112005@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef LIBPARAMS_UBUNTU_PLATFORM_FLASH_DRIVER_H_
#define LIBPARAMS_UBUNTU_PLATFORM_FLASH_DRIVER_H_

#include "flash_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

const FlashDriverOps* ubuntuFlashGetOps(void);

#ifdef __cplusplus
}
#endif

#endif  // LIBPARAMS_UBUNTU_PLATFORM_FLASH_DRIVER_H_
