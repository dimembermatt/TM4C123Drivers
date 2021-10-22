# TM4C123Drivers

This repository contains a selection of drivers developed by Matthew Yu. This
project spans all of the peripherals defined in the TM4C123GH6PM datasheet, and
is meant to be as generically accessible as possible. Some inspiration for the
implementation of these drivers was taken from Jonathan Valvano's Valvanoware
and from IEEE RAS' [RASWare](https://github.com/ut-ras/Rasware). Their direct
contributions have been noted as such in specific files, including their
copyrights (i.e. PLL.h). 

This set of drivers has been developed for several simultaneous projects, and a
few are provided here: 
* A future low level driver replacement for RASWare, which is a HAL software
  library for use by students in the fall IEEE RAS Robotathon competition. 
* The EE445L Spring 2021 [Haptic
  Mouse](https://www.youtube.com/watch?v=ZX0DsBXqy6Q), which was developed by
  Kevin Mechler, Arshad Bacchus, and Matthew Yu as a final project. It went on
  to get second place in the 445L competition. 
* A planned smart multimeter watch, under development as part of the EE382V
  Technology for Embedded Internet of Things graduate class. 
* And others...

A list of driver statuses are found below. These drivers each contain example
projects that can be run on Keil or Code Composer Studio with minimal effort,
and will accompany a yet to be created set of API documentation that should help
developers like myself use this library. 

---

## Contributors and Authors

- @dimembermatt
- @dariojimenezp
- @GabbyWill

---

## Documentation

Source code documentation can be found at...

<https://github.com/slatedocs/slate/wiki/Using-Slate-Natively>


---

## Resources

Please see [RESOURCES.md](resources/RESOURCES.md) for additional content that
may help with bugfixing, device documentation, and so on.

---

## Driver Dependency Graph

![dependency_graph](resources/driver_dependencies.drawio.png)

## Driver Status
> Under construction as of 10/19/21.

C: Completed
I: Implementing
D: Debugging
P: Changes Proposed
N: Needs Changes

| Name              | Status    | Last Modified    | Further Details                                                   |
|-------------------|-----------|------------------|-------------------------------------------------------------------|
| Example           | C         | 09/24/21         | Working as expected.                                              |
| GPIO              | C         | 09/24/21         | Working as expected.                                              |
| Timers            | C         | 09/24/21         | Working as expected.                                              |
| SSI               | C         | 09/24/21         | Working as expected.                                              |
| DAC               | C         | 09/24/21         | Working as expected.                                              |
| ADC               | C         | 09/24/21         | Working as expected.                                              |
| PWM               | C         | 09/24/21         | Working as expected.                                              |
| I2C               | C         | 10/07/21         | Working as expected.                                              |
| UART              | C         | 10/19/21         | Working as expected.                                              |
| USB               | I         |                  | Reading documentation and MVP. See HapticMouse src.               |
| CAN               | N         |                  | Needs driver implementation. 1st in queue.                        |
| DMA               | N         |                  | Needs driver implementation. 2nd in queue.                        |
| Low Power Mode    | N         |                  | Needs driver implementation. 3nd in queue.                        |
| QEI               | N         |                  | Under consideration.                                              |
| Watchdog Timer    | N         |                  | Under consideration.                                              |

