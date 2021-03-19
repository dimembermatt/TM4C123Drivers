# Resources

A couple extra resources are provided here for those looking to learn more about how the TM4C and other components work, as well as those looking to develop drivers for the TM4C.

---

## General Documentation

Of course, users are always encouraged to take a look at the actual implementation and header files to further understand how the codebase works and how to use different drivers and devices. Looking at implementation files aren't required, if you just want it to just work (TM); but the header files provide important information on usage and context that might affect your user experience.

Additionally, check out the examples folder! Some have been designed to only show results on Keil (namely, viewing programs in debug mode with the stack trace), but all should provide useful examples on how to use the API and have been tested and work. Flashing the code should not be an issue.

---

## TM4C Documentation

The [TM4C MCU Datasheet](TM4C_Datasheet.pdf) is the end all, do all for learning about writing drivers for the TM4C. It's best consumed in little chunks as reference, so don't be intimidated by its size.

### Interrupts

A particularly useful page is the [interrupt page](http://shukra.cedt.iisc.ernet.in/edwiki/EmSys:Interrupts_in_TM4C123GH6PM_Launchpad) for the TM4C. Highly suggested when looking at how interrupt sources are configured. Table 7.3, The relationship between vectors and NVIC definitions, provides an easy to read partial list of interrupts and their relevant components.

For the full list, see the Interrupts table (2-9, pg. 104) on the TM4C MCU Datasheet.

### Timer Register Map

It's sometimes not obvious what TM4C register maps to the register definitions defined in [tm4c123gh6pm.h](../inc/tm4c123gh6pm.h). The [TM4C123 Timer Programming page](http://shukra.cedt.iisc.ernet.in/edwiki/EmSys:TM4C123_Timer_Programming) makes it easy to identify which GPTMxxx register belongs to what.

### SSI Register Map

Similarly, the [SPI Programming in TI ARM TIVA page](http://shukra.cedt.iisc.ernet.in/edwiki/EmSys:SPI_programming_in_TI_ARM_Tiva) provides useful information for developing SPI drivers and other SSI drivers for the TM4C.

### GPIO Selection

If your project requires a lot of pins and you're needing to be selective, check out pg 1351 in the datasheet (Table 23-5, GPIO Pins and Alternate Functions) to see what analog and alternate functions can be assigned to each pin. The Recommended Pin Allocation Table below is a summary on a potential combinations you can have for your connected devices, but is not the only set of combinations.

#### Recommended Pin Allocation

| Type                         | Port |
|------------------------------|------|
| ST7735, CAN                  | A    |
| DAC, audio, CAN, I2C         | B    |
| Comparators, UART, USB       | C    |
| ADC, USB                     | D    |
| Wifi, UART, CAN, I2C         | E    |
| Buttons, Switches            | F    |

### Onboard Switches and LEDs

To be clear, the onboard switches for the TM4C is in the following pattern (from [microcontrollers lab](https://microcontrollerslab.com/controlling-led-push-button-tiva-launchpad/)):
- Left Button <=> SW1 <=> PF4
- Right Button <=> SW2 <=> PF0

LEDs are as follows:
- Red <=> PF1
- Blue <=> PF2
- Green <=> PF3

---

## ST7735 TFT Display

Documentation on HW connections and internal functionality is provided in the lib/ST7735/ folder, as [ST7735_Documentation.md](../lib/ST7735/ST7735_Documentation.md) and as [ST7735_v2.1.pdf](../lib/ST7735/ST7735_v2.1.pdf).

Those also wanting to make use of the ST7735DrawBitmap function in ST7735.h, should take a look at [BmpConvert16Readme.txt](../lib/BMP/BmpConvert16Readme.txt) and its accompanying Windows(!) executable, [BMPConvert16.exe](../lib/BMP/BmpConvert.cpp).

Maybe a linux binary will be provided in the future.

### Development

For developing applications or firmware that use this project, I recommend using
the guide in [DEVELOPMENT.md](DEVELOPMENT.md).