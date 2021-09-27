/**
 * DAC.h
 * Devices: LM4F120; TM4C123
 * Description: Low level drivers to configure a resistor ladder DAC output.
 * Authors: Matthew Yu.
 * Last Modified: 04/17/21
 */
#pragma once

/** General imports. */
#include <stdint.h>

/** Device specific imports. */
#include <lib/GPIO/GPIO.h>


/** @brief DACConfig_t is a user defined struct that specifies an up to 6 bit
 *         DAC configuration. */
typedef struct DACConfig {
    /** @brief pins is a pointer to an array defined by the user. It must be at
     *         least numPins length and each entry should be a valid GPIOPin_t
     *         value, otherwise an internal assert will be executed.
     */
    GPIOPin_t * pins;

    /** @brief numPins is the number of pins specified for the DAC. */
    uint8_t numPins;
} DACConfig_t;

/** @brief DAC_t is a struct containing user relevant data of an up to 6 bit
 *         DAC. */
typedef struct DAC {
	/** @brief Pointer to an array of pins. */
    GPIOPin_t * pins;

    /** @brief numPins is the number of pins specified for the DAC. */
    uint8_t numPins;
} DAC_t;

/**
 * DACInit initializes an N-bit DAC on Port B.
 * @param pins A list of pins to initialize, in order of LSB to MSB.
 * @note Assumes that the first pin that is invalid (PIN_COUNT) means all
 *       following pins are invalid. 
 *       Goes up to 6 bits of resolution. 
 *       Multiple DACs can be configured, but the user retains responsibility
 *       for managing the pins data structures.
 */
DAC_t DACInit(DACConfig_t config);

/**
 * DACOut outputs data to the relevant DAC pins set by DACInit.
 * @param pins The list of pins to write data to, in order of LSB to MSB.
 * @param data A value from 0 - 255. Scaled based on how many bits are part of the DAC.
 * @note Assumes that the first pin that is invalid (PIN_COUNT) means all
 *       following pins are invalid. 
 */
void DACOut(DAC_t dac, uint8_t data);
