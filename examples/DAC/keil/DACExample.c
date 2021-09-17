/**
 * File name: DACExample.h
 * Devices: LM4F120; TM4C123
 * Description: Example program to demonstrate digital to analog converters.
 * Authors: Matthew Yu.
 * Last Modified: 09/13/21
 * Note: If you check the target options, in the C/C++ Misc Options, `-D__FAST__` is defined.
 * __FAST__ is present in GPIO.c; it selects between two definitions of GPIOSetBit and GPIOGetBit.
 * __FAST__ trades speed for space complexity. It's very useful for when you want to do interrupt
 * GPIO bit handling or very fast consecutive GPIO bit handling!
 * 
 * Modify __MAIN__ on L12 to determine which main method is executed.
 * __MAIN__ = 0 - Initialization and use a resistor based DAC.
 *          = 1 - Initialization and use of a SPI based DAC.
 */
#define __MAIN__ 0

/** Device specific imports. */
#include <inc/tm4c123gh6pm.h>
#include <inc/PLL.h>
#include <lib/DAC/DAC.h>
#include <lib/DAC/DACSPI.h>
#include <lib/GPIO/GPIO.h>

#if __MAIN__ == 0
DACConfig_t config = {
    .pinList={PIN_B0, PIN_COUNT, PIN_COUNT, PIN_COUNT, PIN_COUNT, PIN_COUNT}
};

/** Initializes both onboard switches to test triggers. */
int main(void) {
    PLLInit(BUS_80_MHZ);
    DACInit(config);

    // View in debugging mode with GPIO_PORTB_DATA_R added to watch 1.
    // Step through program to see pins change.
    DACOut(config, 0); // GPIO_PORTB_DATA_R should be 0x00.
    DACOut(config, 1); // GPIO_PORTB_DATA_R should be 0x01.
    DACOut(config, 2); // GPIO_PORTB_DATA_R should be 0x00.

    config.pinList[1] = PIN_B2;
    DACInit(config);
    DACOut(config, 2); // GPIO_PORTB_DATA_R should be 0x04.
    DACOut(config, 3); // GPIO_PORTB_DATA_R should be 0x05.
    
    while (1) {}
}
#elif __MAIN__ == 1
SSIConfig_t config = {
    .SSI=SSI2_PB, 
    .frameFormat=FREESCALE_SPI, 
    .isPrimary=true, 
    .isTransmitting=true,
    .isClockDefaultHigh=true,
    .polarity=false,
    .dataBitSize=16};

/** Initializes both onboard switches to test triggers. */
int main(void) {
    PLLInit(BUS_80_MHZ);
    DACSPIInit(SSI2_PB);

    while (1) {
        /** 
         * Check the oscilloscope output when this is connected 
         * to the TLV chip to see the SSI input and output voltage. 
         */
        DACSPIOut(SSI2_PB, 0xFF);
    }
}
#endif
