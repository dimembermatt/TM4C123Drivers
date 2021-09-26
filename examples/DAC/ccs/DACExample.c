/**
 * @file DACExample.c
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief An example project showing how to use the DAC driver.
 * @version 0.1
 * @date 2021-09-24
 * @copyright Copyright (c) 2021
 */

/** General imports. */
#include <stdint.h>
#include <stdlib.h>

/** Device specific imports. */
#include <lib/PLL/PLL.h>
#include <lib/DAC/DAC.h>
#include <lib/GPIO/GPIO.h>


GPIOPin_t pins[2] = { PIN_B0, PIN_COUNT };
DACConfig_t config = {
    .pins=pins,
    .numPins=1
};

/** Initializes both onboard switches to test triggers. */
int main(void) {
    PLLInit(BUS_80_MHZ);
    DAC_t dac = DACInit(config);

    // View in debugging mode with GPIO_PORTB_DATA_R added to watch 1.
    // Step through program to see pins change.
    DACOut(dac, 0); // GPIO_PORTB_DATA_R should be 0x00.
    DACOut(dac, 1); // GPIO_PORTB_DATA_R should be 0x01.
    DACOut(dac, 2); // GPIO_PORTB_DATA_R should be 0x00.

    config.pins[1] = PIN_B5; 
    config.numPins = 2;
    dac = DACInit(config);

    DACOut(dac, 2); // GPIO_PORTB_DATA_R should be 0x20.
    DACOut(dac, 3); // GPIO_PORTB_DATA_R should be 0x21.
    
    while (1) {}
}
