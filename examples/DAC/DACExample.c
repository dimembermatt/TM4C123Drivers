/**
 * File name: DACExample.h
 * Devices: LM4F120; TM4C123
 * Description: Example program to demonstrate digital to analog converters.
 * Authors: Matthew Yu.
 * Last Modified: 03/03/21
 */

/** Device specific imports. */
#include <inc/tm4c123gh6pm.h>
#include <inc/PLL.h>
#include <lib/DAC/DAC.h>
#include <lib/GPIO/GPIO.h>


DACConfig_t config = {
    {PIN_B0, PIN_COUNT, PIN_COUNT, PIN_COUNT, PIN_COUNT, PIN_COUNT},
    1
};

/** Initializes both onboard switches to test triggers. */
int main(void) {
    PLL_Init(Bus80MHz);
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
	
	while(1) {}
}
