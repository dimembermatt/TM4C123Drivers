/**
 * File name: SwitchExample.h
 * Devices: LM4F120; TM4C123
 * Description: Example program to demonstrate the capabilities of edge triggered switches.
 * Authors: Matthew Yu.
 * Last Modified: 04/17/21
 */

/** General imports. */
#include <stdio.h>

/** Device specific imports. */
#include <inc/PLL.h>
#include <lib/GPIO/GPIO.h>
#include <lib/Switch/Switch.h>


void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s

volatile uint32_t risingCounter = 0;
volatile uint32_t fallingCounter = 0; 
void dummyTaskRising(void) {
    GPIOSetBit(PIN_F1, !GPIOGetBit(PIN_F1));
    ++risingCounter;
}
void dummyTaskFalling(void) {
    GPIOSetBit(PIN_F2, !GPIOGetBit(PIN_F2));
    ++fallingCounter;
}

int main(void) {
    /**
     * This program demonstrates initializing the onboard switches as
     * edge-triggered GPIO and using them to toggle onboard LEDs. This is
     * similar to GPIOExample main #2.
     */
    PLLInit(BUS_80_MHZ);
    DisableInterrupts();
    
    /* Initialize PF0 (SW2) and PF4 (SW1) as edge triggered interrupt switches. */
    SwitchInit(PIN_F0, dummyTaskRising, NULL);
    SwitchInit(PIN_F4, NULL, dummyTaskFalling);

    GPIOConfig_t PF1Config = {
        .GPIOPin=PIN_F1, 
        .pull=PULL_DOWN, 
        .isOutput=true, 
        .isAlternative=false, 
        .alternateFunction=0, 
        .isAnalog=false
    };

    GPIOConfig_t PF2Config = {
        PIN_F2, 
        PULL_DOWN, 
        true, 
        false, 
        0, 
        false
    };

    /* Initialize PF1, PF2 as a normal GPIO LED. */
    GPIOInit(PF1Config);
    GPIOInit(PF2Config);

    GPIOSetBit(PIN_F1, 0);
    GPIOSetBit(PIN_F2, 0);
    EnableInterrupts();
    while (1) {
        /* View in debugging mode with risingCounter and fallingCounter added to
         * watch 1. Press SW2 (right button) and SW1 (left button) to see
         * counters increment. Additionally, pressing SW2 should toggle PF1 (Red 
         * LED while pressing SW1 should toggle PF2 (Blue LED). */
        WaitForInterrupt();
    };
}
