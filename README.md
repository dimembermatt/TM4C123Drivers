# TM4C123Drivers

This repository contains a selection of drivers originally written by Jonathan Valvano and refactored by Matthew Yu. Some inspiration was also taken from IEEE RAS' [RASWare](https://github.com/ut-ras/Rasware).

This set of drivers has been developed for several simultaneous projects, and a few are provided here:
* A future low level driver replacement for RASWare, which is a HAL software library for use by students in the fall IEEE RAS Robotathon competition.
* The EE445L Spring 2021 [Haptic Mouse](https://www.youtube.com/watch?v=ZX0DsBXqy6Q), which was developed by Kevin Mechler, Arshad Bacchus, and Matthew Yu as a final project. It went on to get second place in the 445L competition.
* A planned smart multimeter watch, under development as part of the EE382V Technology for Embedded Internet of Things graduate class.
* And others...

Altogether, this library contains drivers for all sets of peripherals for the TM4C123GH6PM chip, including timers, ADCs, GPIOs, PWM, and so on. A list of driver statuses are found below. These drivers each contain example projects that can be run on Keil or Code Composer Studio with minimal effort, and will accompany a yet to be created set of API documentation that should help developers like myself use this library.

---

## Documentation

Source code documentation can be found at...

<https://github.com/slatedocs/slate/wiki/Using-Slate-Natively>


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
| Switch            | D         | 09/13/21         | Untested on TM4C123GH6PM.                                                                          |
| Timers            | D         | 09/13/21         | Untested on TM4C123GH6PM. B side timers, wide timers need implementation/are not working. See #6.  |
| SSI               | D         | 09/13/21         | Untested on TM4C123GH6PM.                                                                          |
| DAC               | D         | 09/13/21         | CCS has undefined references on another machine.                                                   |
| ADC               | D         | 09/13/21         | Needs CCS example project.                                                                         |
| PWM               | P         | 09/13/21         | Untested on TM4C123GH6PM. PWM module may need bugfixes. See #4.                                    |
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


See [this source](https://renenyffenegger.ch/notes/development/languages/C-C-plus-plus/GCC/create-libraries/index) for figuring out how to hide implementation file details, in case this becomes an issue later.