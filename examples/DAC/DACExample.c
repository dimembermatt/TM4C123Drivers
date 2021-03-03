/**
 * File name: DACExample.h
 * Devices: LM4F120; TM4C123
 * Description: Example program to demonstrate digital to analog converters.
 * Authors: Matthew Yu.
 * Last Modified: 03/03/21
 */

/** General imports. */
#include <stdio.h>

/** Device specific imports. */
#include "../inc/tm4c123gh6pm.h"
#include "../inc/PLL.h"
#include "../lib/DAC/DAC.h"
#include "../inc/GPIO.h"

DACConfig config = {
    {PIN_B0, PIN_COUNT, PIN_COUNT, PIN_COUNT, PIN_COUNT, PIN_COUNT}
};

/** Initializes both onboard switches to test triggers. */
int main(void) {
    PLL_Init(Bus80MHz);
    DACInit(config);

    // View in debugging mode with GPIO_PORTB_DATA_R added to watch 1.
    // Step through program to see pins change.
    DACOut(0, config); // GPIO_PORTB_DATA_R should be 0x00.
    DACOut(1, config); // GPIO_PORTB_DATA_R should be 0x01.
    DACOut(2, config); // GPIO_PORTB_DATA_R should be 0x00.

    config.pinList[1] = PIN_B2;
    DACInit(config);
    DACOut(2, config); // GPIO_PORTB_DATA_R should be 0x04.
    DACOut(3, config); // GPIO_PORTB_DATA_R should be 0x05.
	
	while(1) {}
}
