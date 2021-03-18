# TM4C123Drivers

This repository contains a selection of drivers originally written by Jonathan Valvano and refactored by Matthew Yu. Some inspiration taken from IEEE RAS' [RASWare](https://github.com/ut-ras/Rasware).

This is meant to be a future replacement for RASWare, with its own CLI build system and higher level drivers for devices such as color sensors, line sensors, and so on.

---

## Resources

Please see [RESOURCES.md](resources/RESOURCES.md) for additional content that may help with bufgixing, device documentation, and so on.

---

## TODO

Drivers that need to be developed/refactored:
- Complete GPIOSetBitFast/GPIOGetBitFast and figure out how to add compiler flags to select between GPIOGetBit and GPIOGetBitFast and SetBit equivalents.
    - Unfortunately, dynamic generation of addresses is too slow for high speed ISRs. For applications like audio or SSI, this is not tolerable. Ergo we need a drop in speedy replacement for said applications. GPIOGet/SetBitFast does this with a switch statement (LUTs are slow).
- Switches
    - Need interrupt generalization for pins not from port F.
    - Potentially move interrupt generation and handling to GPIO.h.
- Timers
    - Bug: B timers not working.
    - Feature: Wide Timer implementation.
    - Feature: Select clock mode.
    - Feature: Count down vs count up timers.
- ST7735
    - Bug: L648, line for setAddrWindow with unknown purpose.
    - Feature: Speedup ST7735DrawBitmap.
    - Bug: ST7735On and ST7735Off don't work as expected. In particular, I expect ST7735Off to have backlight off.
- DAC
  - Feature: Fix input scaling to num of available pins allocated in the config.
  - Audio generation
    - Bug: Audio kind of works, but not really.
- UART
    - Feature: Convert from inc/ to lib/.
- ESP8266
    - Feature: Convert from inc/ to lib/.
- Blynk
  - Feature: Dependency on UART, ESP8266 to update includes currently pointing to inc/.
- ADC
- USB
- I2C
- Bluetooth(?)
- Motor Drivers

General improvements
- Gradually remove dependencies on tm4c123gh6pm.h. 
    - The rationale for this is that a lot of initialization code can be generalized to improve user API, at the expense of extra initial cycles to do register address calculation using defines in RegDefs.h. tm4c123gh6pm becomes redundant in this case, with defines for hardcoded events (not to mention, 12870 lines of it).
