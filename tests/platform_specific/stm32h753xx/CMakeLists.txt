cmake_minimum_required(VERSION 3.22)
project(libparams_stm32h753xx C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXECUTABLE_SUFFIX ".elf")

include(${CMAKE_CURRENT_LIST_DIR}/../cmake/stm32_platform_test.cmake)
add_libparams_stm32_platform_test(libparams_stm32h753xx
    stm32h753xx
    STM32H753xx
    cortex-m7
)
add_libparams_stm32_platform_test(libparams_stm32h753xx_spifram
    stm32h753xx/stm32h753xx_spifram
    STM32H753xx
    cortex-m7
)
