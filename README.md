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
> Under construction as of 09/24/21.

C: Completed
I: Implementing
D: Debugging
P: Changes Proposed
N: Needs Changes

| Name              | Status    | Last Modified    | Further Details                                                                                    |
|-------------------|-----------|------------------|----------------------------------------------------------------------------------------------------|
| Example           | C         | 09/24/21         | Working as expected.                                                                               |
| GPIO              | C         | 09/24/21         | Working as expected.                                                                               |
| Timers            | C         | 09/24/21         | Working as expected.                                                                               |
| SSI               | C         | 09/24/21         | Working as expected.                                                                               |
| DAC               | C         | 09/24/21         | Working as expected.                                                                               |
| ADC               | C         | 09/24/21         | Working as expected.                                                                               |
| PWM               | C         | 09/24/21         | Working as expected.                                                                               |
| I2C               | I         |                  | Working on driver implementation.                                                                  |
| UART              | N         |                  | Needs driver implementation. 1st in queue.                                                         |
| USB               | N         |                  | Needs driver implementation. See HapticMouse src. 2nd in queue.                                    |
| CAN               | N         |                  | Needs driver implementation. 3rd in queue.                                                         |
| DMA               | N         |                  | Needs driver implementation.                                                                       |
| Low Power Mode    | N         |                  | In planning.                                                                                       |

Drivers should potentially be obfuscated into a lib.


See [this source](https://renenyffenegger.ch/notes/development/languages/C-C-plus-plus/GCC/create-libraries/index) for figuring out how to hide implementation file details, in case this becomes an issue later.


Features for the refactor branch.
- standardization of API to ADC drivers (config generates data struct which is managed by user)
- doxygen supported javadoc documentation styling
- ASSERT statements for config inputs and so forth
- variable argument inputs for interrupts (GPIO, Timer, etc)
- rename Timers to Timer library
- remove Blynk, ST7735, esp8266, switch from library
- move Systick delay from Misc into Timers. 
- performance measurement on DelayMillisec, DelayMicrosec, and optimize.