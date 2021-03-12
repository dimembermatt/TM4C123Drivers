/**
 * DAC.h
 * Devices: LM4F120; TM4C123
 * Description: Low level drivers Digital to Analog Output.
 * Authors: Matthew Yu.
 * Last Modified: 03/10/21
 */
#pragma once

/** General imports. */
#include <stdint.h>

/** Device specific imports. */
#include <TM4C123Drivers/lib/GPIO/GPIO.h>


/** Configuration of a single DAC object. */
typedef struct DACConfig {
    pin_t pinList[6];
    uint8_t numUsedPins;
} DACConfig_t;

/**
 * DACInit initializes an N-bit DAC on Port B.
 * Goes up to 6 bits of resolution. Multiple DACs can be configured, but is
 * dependent on the user managing the pins data structures.
 * @param pins A list of pins to initialize, in order of LSB to MSB.
 */
void DACInit(DACConfig_t pins);

/**
 * DACOut outputs data to the relevant DAC pins set by DACInit.
 * @param pins The list of pins to write data to, in order of LSB to MSB.
 * @param data A value from 0 - 63. Scaled based on how many bits are part of the DAC.
 */
void DACOut(DACConfig_t pins, uint8_t data);
