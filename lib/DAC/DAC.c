/**
 * DAC.c
 * configures DAC output
 * runs on LM4F120 or TM4C123
 * Author: Matthew Yu
 * Last Modified: 11/28/2018
 * 11/28/18 - standardized comment style, preparing for c++ rewrite
 **/

/** General imports. */
#include <stdint.h>
#include <math.h>

/** Device specific imports. */
#include "DAC.h"

#define MAX_DAC_PINS 6

/**
 * DACInit initializes an N-bit DAC on Port B.
 * Goes up to 6 bits of resolution. Multiple DACs can be configured, but is
 * dependent on the user managing the pins data structures.
 * @param pins A list of pins to initialize, in order of LSB to MSB.
 */
void DACInit(DACConfig pins) {
    SYSCTL_RCGCGPIO_R |= 0x00000002;                // 1) Activate the clock for the relevant port.
    while ((SYSCTL_PRGPIO_R & 0x00000002) == 0) {}; // 2) Stall until clock is ready.
    GPIO_PORTB_AMSEL_R = 0;                         // 3) Disable analog functionality across PORTB.
    GPIO_PORTB_AFSEL_R = 0;                         // 4) Disable alternative function functionality across PORTB.
	for (uint8_t i = 0; i < MAX_DAC_PINS; i++) {
        if (pins.pinList[i] >= PIN_B0 && pins.pinList[i] <= PIN_B7) { // Valid pin.
            uint8_t pinAddress = pow(2, pins.pinList[i] % 8);
            // 5) Set pins to be output (1).
            GPIO_PORTB_DIR_R |= pinAddress;
            // 6) Enable digital IO for pins.
            GPIO_PORTB_DEN_R |= pinAddress;
        }
    }
}

/**
 * DACOut outputs data to the relevant DAC pins set by DACInit.
 * @param data A value from 0 - 63. Scaled based on how many bits are part of the DAC.
 * @param pins The list of pins to write data to, in order of LSB to MSB.
 */
void DACOut(uint8_t data, DACConfig pins) {
	uint8_t numPins = 0;
    for (uint8_t i = 0; i < MAX_DAC_PINS; i++) {
        if (pins.pinList[i] >= PIN_B0 && pins.pinList[i] <= PIN_B7) {
            ++numPins;
        }
    }
    //data /= pow(2, MAX_DAC_PINS-numPins);

    uint32_t adjustedData = 0;
	for (uint8_t i = 0; i < MAX_DAC_PINS; i++) {
        if (pins.pinList[i] >= PIN_B0 && pins.pinList[i] <= PIN_B7) { // Valid pin.
            uint8_t placeVal = (data>>i) & 1;
            pin_t pin = pins.pinList[i];
        	adjustedData |= (uint8_t) (pow(2, pin % 8) * placeVal);
        }
    }
    GPIO_PORTB_DATA_R = adjustedData;
}
