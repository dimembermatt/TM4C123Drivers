# TM4C123Drivers

This repository contains a selection of drivers originally written by Jonathan Valvano and refactored by Matthew Yu for the Spring 2021 EE445L course. Some inspiration taken from IEEE RAS' [RASWare](https://github.com/ut-ras/Rasware).

---

## Resources

Please see [RESOURCES.md](resources/RESOURCES.md) for additional content that may help with bufgixing, device documentation, and so on.

---

## TODO

Drivers that need to be developed/refactored:
- Switches
    - Need interrupt generalization for pins not from port F.
- Timers
    - Feature: Wide Timer implementation.
    - Bug: Unable to run both A and B timers at once.
        - May be more general issue of B timers not working properly.
    - Feature: Select clock mode.
    - Feature: One shot vs periodic timer mode.
    - Feature: Count down vs count up timers.
    - Feature: Timer priority.
    - Feature: Run on base+offset for driver implementation
    - Feature: Add back in TimerB timers; use dynamic generation instead to save space.
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
- SysTick
- USB
- I2C
- Bluetooth(?)
- Motor Drivers
