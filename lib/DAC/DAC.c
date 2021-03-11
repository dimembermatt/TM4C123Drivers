/**
 * DAC.c
 * Devices: LM4F120; TM4C123
 * Description: Low level drivers to configure a resistor ladder DAC output.
 * Authors: Matthew Yu.
 * Last Modified: 03/10/21
 */

/** General Imports. */
#include <stdbool.h>

/** Device specific imports. */
#include "DAC.h"
#include <TM4C123Drivers/inc/tm4c123gh6pm.h>


#define MAX_DAC_PINS 6

/**
 * DACInit initializes an N-bit DAC on Port B.
 * Goes up to 6 bits of resolution. Multiple DACs can be configured, but is
 * dependent on the user managing the pins data structures.
 * @param pins A list of pins to initialize, in order of LSB to MSB.
 */
void DACInit(DACConfig pins) {
    GPIOConfig_t config = {PIN_B0, PULL_DOWN, true, false, 0, false};
    for (uint8_t i = 0; i < MAX_DAC_PINS; i++) {
        if (pins.pinList[i] >= PIN_B0 && pins.pinList[i] <= PIN_B7) { // Valid pin.
            config.GPIOPin = pins.pinList[i];
            GPIOInit(config);
        }
    }
}

/**
 * DACOut outputs data to the relevant DAC pins set by DACInit.
 * @param data A value from 0 - 63. Scaled based on how many bits are part of the DAC.
 * @param pins The list of pins to write data to, in order of LSB to MSB.
 */
void DACOut(uint8_t data, DACConfig pins) {
	for (uint8_t i = 0; i < MAX_DAC_PINS; i++) {
        if (pins.pinList[i] >= PIN_B0 && pins.pinList[i] <= PIN_B7) { // Valid pin.
            uint8_t placeVal = (data>>i) & 0x1;
            GPIOSetBit(pins.pinList[i], placeVal);
        }
    }
}
