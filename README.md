# TM4C123Drivers

This repository contains a selection of drivers originally written by Jonathan Valvano and refactored by Matthew Yu. Some inspiration taken from IEEE RAS' [RASWare](https://github.com/ut-ras/Rasware).

This is meant to be a future replacement for RASWare, with its own CLI build system and higher level drivers for devices such as color sensors, line sensors, and so on. I am optimistic that this set of drivers will be rolled out by the Fall 2021 RAS Robotathon competition for students to use.

See [this source](https://renenyffenegger.ch/notes/development/languages/C-C-plus-plus/GCC/create-libraries/index) for figuring out how to hide implementation file details, in case this becomes an issue later.

---

## Resources

Please see [RESOURCES.md](resources/RESOURCES.md) for additional content that may help with bufgixing, device documentation, and so on.

---

## Driver Status
> Up to date as of 04/17/21.

- DAC: Working
- GPIO: Working
- PWM: Partially Working
    - Bug: PWM module version needs fixing. See issue #4.
- SSI: Working
- ST7735: Working
    - Bug: Some minor bugs. See issue #3.
    - Feature: tm4c213gh6pm.h dependency needs removing. See issue #2.
- Switch: Working
- Timers: Partially Working
    - Bug: B side timers not working. See issue #6.
    - Feature: Wide timers need implementation. See issue #6.

- UART: Not Implemented
- ESP8266: Not Implemented
- Blynk: Move from this repo into Lab_4 repo.
- ADC: Not Implemented.
- USB: Not Implemented.
- I2C: Not Implemented.
- Bluetooth: Not Implemented.
