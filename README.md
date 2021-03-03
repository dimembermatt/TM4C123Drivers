# TM4C123Drivers

This repository contains a selection of drivers originally written by Jonathan Valvano and refactored by Matthew Yu for the Spring 2021 EE445L course. Some inspiration taken from IEEE RAS' [RASWare](https://github.com/ut-ras/Rasware).

---

## Recommended Pin Allocation

| Type                         | Port |
|------------------------------|------|
| ST7735, CAN                  | A    |
| DAC, audio, CAN, I2C         | B    |
| Comparators, UART, USB       | C    |
| ADC, USB                     | D    |
| Wifi, UART, CAN, I2C         | E    |
| Buttons, Switches            | F    |

---

## TODO

Drivers that need to be developed/refactored:
- DAC
  - Audio generation
- ADC
- SysTick
- USB
- I2C
- Bluetooth(?)
- Motor Drivers