/**
 * DACSPI.h
 * Devices: LM4F120; TM4C123
 * Description: Low level drivers for the TLV5616CP SPI DAC chip.
 * Authors: Matthew Yu.
 * Last Modified: 04/17/21
 */
#pragma once

/** General imports. */
#include <stdint.h>

/** Device specific imports. */
#include <lib/SSI/SSI.h>


/**
 * DACSPIInit initializes a given SSI port.
 * @param SSIConfig Enum determining which SSI port to output to.
 * @note Multiple DACs can be configured, but the user retains responsibility
 *       for managing the SSI data structures.
 */
void DACSPIInit(SSISelect_t SSIConfig);

/**
 * DACSPIOut outputs data to the relevant SSI port initialized by DACSPIInit.
 * @param ssi Enum determining which SSI port to output to.
 * @param data A value from 0 - 4096.
 */
void DACSPIOut(SSISelect_t ssi, uint16_t data);
