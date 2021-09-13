# TM4C123Drivers

This repository contains a selection of drivers originally written by Jonathan Valvano and refactored by Matthew Yu. Some inspiration taken from IEEE RAS' [RASWare](https://github.com/ut-ras/Rasware).

This is meant to be a future replacement for RASWare, with its own CLI build system and higher level drivers for devices such as color sensors, line sensors, and so on. I am optimistic that this set of drivers will be rolled out by the Fall 2021 RAS Robotathon competition for students to use.

See [this source](https://renenyffenegger.ch/notes/development/languages/C-C-plus-plus/GCC/create-libraries/index) for figuring out how to hide implementation file details, in case this becomes an issue later.

---

## Resources

Please see [RESOURCES.md](resources/RESOURCES.md) for additional content that may help with bufgixing, device documentation, and so on.

---

## Driver Dependency Graph


## Driver Status
> Under construction as of 09/13/21.

C: Completed
I: Implementing
D: Debugging
P: Changes Proposed
N: Needs Changes

| Name              | Status    | Last Modified    | Further Details                                                                                    |
|-------------------|-----------|------------------|----------------------------------------------------------------------------------------------------|
| Example           | D         | 09/13/21         | CCS has undefined references on another machine.                                                   |
| GPIO              | D         | 09/13/21         | CCS has undefined references on another machine.                                                   |
| Switch            | P         |                  | Needs CCS example project.                                                                         |
| Timers            | P         |                  | Needs CCS example project. B side timers, wide timers need implementation/are not working. See #6. |
| SSI               | P         |                  | Needs CCS example project.                                                                         |
| DAC               | D         | 9/13/21          | CCS has undefined references on another machine.                                                   |
| ADC               | N         |                  | Needs driver implementation.                                                                       |
| PWM               | P         |                  | Needs CCS example project. PWM module may need bugfixes. See #4.                                   |
| UART              | N         |                  | Needs driver implementation.                                                                       |
| USB               | N         |                  | Needs driver implementation. See HapticMouse src.                                                  |
| I2C               | N         |                  | Needs driver implementation.                                                                       |
| CAN               | N         |                  | Needs driver implementation.                                                                       |
| DMA               | N         |                  | Needs driver implementation.                                                                       |
| ST7735            | P         |                  | Needs CCS example project. Minor bugs. See #3. Needs tm4c123gh6pm dependency removed. See #2.      |
| ESP8266           | N         |                  | Needs driver implementation. And maybe pull out of repo.                                           |
| Blynk             | N         |                  | Needs CCS example project. And maybe pull out of repo.                                             |
| Bluetooth         | N         |                  | Needs driver implementation. And maybe pull out of repo.                                           |

ST7735, ESP8266, Blynk should potentially be pulled out of this repo into a private repo.
Higher level drivers for Robotathon are required in RASWare - Line sensor, motor, distance sensor, color sensor..
Motor driver can be ported from 445L lab.

Drivers should potentially be obfuscated into a lib.