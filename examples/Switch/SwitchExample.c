/**
 * File name: SwitchExample.h
 * Devices: LM4F120; TM4C123
 * Description: Example program to demonstrate the capabilities of edge triggered switches.
 * Authors: Matthew Yu.
 * Last Modified: 03/03/21
 */

/** General imports. */
#include <stdio.h>

/** Device specific imports. */
#include "../inc/tm4c123gh6pm.h"
#include "../inc/PLL.h"
#include "../lib/Switch/Switch.h"
#include "../inc/GPIO.h"


volatile uint32_t risingCounter = 0;
volatile uint32_t fallingCounter = 0; 
void dummyTaskRising(void) {
    ++risingCounter;
}
void dummyTaskFalling(void) {
    ++fallingCounter;
}


/** Initializes both onboard switches to test triggers. */
int main(void) {
    PLL_Init(Bus80MHz);
    SwitchInit(PIN_F0, dummyTaskRising, dummyTaskFalling);
    SwitchInit(PIN_F4, dummyTaskRising, dummyTaskFalling);

    while (1) {
        // View in debugging mode with risingCounter and fallingCounter added to watch 1.
        // Press SW2 (right button) to see counter increment.
    };
}
