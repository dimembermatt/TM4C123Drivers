/**
 * DAC.c
 * Devices: LM4F120; TM4C123
 * Description: Low level drivers to configure a resistor ladder DAC output.
 * Authors: Matthew Yu.
 * Last Modified: 04/17/21
 */

/** General Imports. */
#include <stdbool.h>

/** Device specific imports. */
#include <inc/tm4c123gh6pm.h>
#include <lib/DAC/DAC.h>


#define MAX_DAC_PINS 6

/**
 * DACInit initializes an N-bit DAC on Port B.
 * @param pins A list of pins to initialize, in order of LSB to MSB.
 * @note Assumes that the first pin that is invalid (PIN_COUNT) means all
 *       following pins are invalid.
 *       Goes up to 6 bits of resolution.
 *       Multiple DACs can be configured, but the user retains responsibility
 *       for managing the pins data structures.
 */
void DACInit(DACConfig_t pins) {
    for (uint8_t i = 0; i < MAX_DAC_PINS; i++) {
        if (pins.pinList[i] == PIN_COUNT) return;
        GPIOConfig_t config = {
            pins.pinList[i],
            PULL_DOWN,
            true,
            false,
            0,
            false
        };
        GPIOInit(config);
    }
}

/**
 * DACOut outputs data to the relevant DAC pins set by DACInit.
 * @param pins The list of pins to write data to, in order of LSB to MSB.
 * @param data A value from 0 - 255. Scaled based on how many bits are part of the DAC.
 * @note Assumes that the first pin that is invalid (PIN_COUNT) means all
 *       following pins are invalid.
 */
void DACOut(DACConfig_t pins, uint8_t data) {
    for (uint8_t i = 0; i < MAX_DAC_PINS; i++) {
        if (pins.pinList[i] == PIN_COUNT) return;
        GPIOSetBit(pins.pinList[i], (data>>i) & 0x1);
    }
}
