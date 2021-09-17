/**
 * File name: GPIOExample.h
 * Devices: LM4F120; TM4C123
 * Description: Example program to demonstrate the low level GPIO driver.
 * Authors: Matthew Yu.
 * Last Modified: 09/13/21
 *
 * Modify __MAIN__ on L12 to determine which main method is executed.
 * __MAIN__ = 0 - Initialization, getting, and setting of GPIO pins.
 *          = 1 - Initialization of a GPIO pin with an edge triggered interrupt.
 */
#define __MAIN__ 0

/** General imports. */
#include <stdlib.h>

/** Device specific imports. */
#include <inc/PLL.h>
#include <lib/GPIO/GPIO.h>
#include <lib/Misc/Misc.h>


void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s

#if __MAIN__ == 0
int main(void) {
    /**
     * This program demonstrates initializing a GPIO pin and updating its value.
     * Every 1000ms, PF1 and PF2 - associated with the Red and Blue LED,
     * respectively - are toggled between an ON and OFF state.
     */
    PLLInit(BUS_80_MHZ);
    DisableInterrupts();

    /* Initialize SysTick for delay calls. This is required, since the delay
     * calls utilize the SysTick hardware. */
    delayInit();

    /* RED LED. */
    GPIOConfig_t PF1Config = {
        .GPIOPin=PIN_F1,
        .pull=PULL_DOWN,
        .isOutput=true,
        .isAlternative=false,
        .alternateFunction=0,
        .isAnalog=false
    };

    /* BLUE LED. */
    GPIOConfig_t PF2Config = {
        PIN_F2,
        PULL_DOWN,
        true,
        false,
        0,
        false
    };

    /* Initialize a GPIO LED on PF1 and PF2. */
    GPIOInit(PF1Config);
    GPIOInit(PF2Config);

    EnableInterrupts();

    /* PF1 is default on. */
    GPIOSetBit(PIN_F1, 1);
    GPIOSetBit(PIN_F2, 0);
    while (1) {
        /* Every 1s, toggle PF1 and PF2 LEDs. */
        delayMillisec(1000);

        /* See when running that a blue and red light flash alternately. */
        GPIOSetBit(PIN_F1, !GPIOGetBit(PIN_F1));
        GPIOSetBit(PIN_F2, !GPIOGetBit(PIN_F2));
    };
}
#elif __MAIN__ == 1
/** This dummy task toggles the LED on PF2. */
void toggleLED(void) { GPIOSetBit(PIN_F2, !GPIOGetBit(PIN_F2)); }

int main(void) {
    /**
     * This program demonstrates initializing a GPIO pin with an edge triggered
     * interrupt and executing on it. Every time PF0 is pressed, the toggleLED
     * function is called and PF2 (BLUE LED) is toggled.
     */
    PLLInit(BUS_80_MHZ);
    DisableInterrupts();

    /* Initialize SysTick for delay calls. This is required, since the delay
     * calls utilize the SysTick hardware. */
    delayInit();

    /* RED LED. */
    GPIOConfig_t PF1Config = {
        .GPIOPin=PIN_F1,
        .pull=PULL_DOWN,
        .isOutput=true,
        .isAlternative=false,
        .alternateFunction=0,
        .isAnalog=false
    };

    /* BLUE LED. */
    GPIOConfig_t PF2Config = {
        PIN_F2,
        PULL_DOWN,
        true,
        false,
        0,
        false
    };

    /* SW2. */
    GPIOConfig_t PF0Config = {
        PIN_F0,
        PULL_UP,
        false,
        false,
        0,
        false
    };

    /* SW2 Interrupt. */
    GPIOInterruptConfig_t PF0IntConfig = {
        .priority=3,
        .touchTask=toggleLED,
        .releaseTask=NULL,
        .pinStatus=LOWERED
    };

    /* Initialize a GPIO LED on PF1 and PF2. */
    GPIOInit(PF1Config);
    GPIOInit(PF2Config);

    /* Initialize PF0 (SW2) as an edge triggered switch. */
    GPIOIntInit(PF0Config, PF0IntConfig);

    EnableInterrupts();

    /* PF1 is default on. */
    GPIOSetBit(PIN_F1, 1);
    GPIOSetBit(PIN_F2, 0);
    while (1) {
        /* Every 1s, toggle PF1 LED on and off. */
        delayMillisec(1000);
        GPIOSetBit(PIN_F1, !GPIOGetBit(PIN_F1));
    };
}
#endif
