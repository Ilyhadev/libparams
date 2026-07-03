# stm32h753xx flash driver examples

This example checks that both STM32H753xx flash backends build with the ARM embedded compiler.

## Purpose

The application is intended to verify that libparams and the STM32H753xx platform flash drivers compile without warnings or errors:

- `stm32h753xx` internal flash
- `stm32h753xx/stm32h753xx_spifram` external SPI FRAM

## Usage


```bash
cd libparams
make stm32h753xx
```

It will generate `.elf`, `.hex`, and `.bin` files in the `libparams/build` folder.

## Notes

- The test uses local HAL and SPI mocks from this directory.
- The binaries are compile/link smoke tests, not hardware-in-the-loop tests.
- Related workflow: [![CI](https://github.com/PonomarevDA/libparams/actions/workflows/ci.yml/badge.svg)](https://github.com/PonomarevDA/libparams/actions/workflows/ci.yml)
