/**
 * DAC.h
 * Devices: LM4F120; TM4C123
 * Description: Low level drivers Digital to Analog Output.
 * Authors: Matthew Yu.
 * Last Modified: 03/03/21
 **/
#pragma once

/** General imports. */
#include <stdint.h>

/** Device specific imports. */
#include "../../inc/tm4c123gh6pm.h"
#include "../../inc/GPIO.h"

/** Configuration of a single DAC object. */
typedef struct DACConfig {
    pin_t pinList[6];
} DACConfig;

/**
 * DACInit initializes an N-bit DAC on Port B.
 * Goes up to 6 bits of resolution. Multiple DACs can be configured, but is
 * dependent on the user managing the pins data structures.
 * @param pins A list of pins to initialize, in order of LSB to MSB.
 */
void DACInit(DACConfig pins);

/**
 * DACOut outputs data to the relevant DAC pins set by DACInit.
 * @param data A value from 0 - 63. Clipped based on how many bits are part of the DAC.
 * @param pins The list of pins to write data to, in order of LSB to MSB.
 */
void DACOut(uint8_t data, DACConfig pins);
