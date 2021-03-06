/**
 * File name: Misc.h
 * Devices: LM4F120; TM4C123
 * Description: Miscellaneous functions for general use.
 * Authors: Jonathan Valvano. Revised by Matthew Yu.
 * Last Modified: 03/04/21
 */
#pragma once

/** General imports. */
#include <stdint.h>


/**
 * delayMillisec delays the process by 1 ms. Tuned to the 80 MHz TM4C clock.
 * @param n The number of ms to delay the process.
 */
void delayMillisec(uint32_t n);
