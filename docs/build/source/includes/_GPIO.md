# GPIO Drivers

This driver API version is 0.1 and is in __BETA__. Those building software on this version may experience breaking changes when migrating to the next minor or major reason.


## Summary

GPIO pins, or General Purpose Input Output pins, are user configurable pins that can be controlled to operate as a number of functional peripherals. A couple of potential uses are provided below.

- Analog to digital input pin (ADC)
- Edge or Level based interrupt signaller
- Communication hardware pins, such as for I2C, SPI, UART, CAN, etc.

This driver is one of the base pillars of TM4C123Drivers and UTRASWare, and is used in many other drivers.

<aside class="notice">
The GPIO driver as of 10/01/21, does not support the following:
- DMA control.
</aside>

<aside class="notice">
The GPIO driver has an optional compilation flag, `__FAST__`, that toggles between a fast and normal version of `GPIOSetBit` and `GPIOGetBit`. The tradoff is on the order of 100x speedup vs 1kB more lines of DATA. Check the 
</aside>

> GPIOPin_t