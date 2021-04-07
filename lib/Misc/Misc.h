/**
 * File name: Misc.h
 * Devices: LM4F120; TM4C123
 * Description: Misc functions for general use.
 * Authors: Jonathan Valvano. Revised by Matthew Yu.
 * Last Modified: 04/07/21
 */
#pragma once

/** General imports. */
#include <stdint.h>


/** 
 * Initializes SysTick to run at 80 MHz and start an internal ticker. 
 * Can be overwritten by other Systick initializers.
 */
void delayInit(void);

/**
 * delayMillisec delays the process by 1 ms.
 * 
 * @param n The number of ms to delay the process.
 */
void delayMillisec(uint32_t n);

/**
 * delayMicrosec delays the process by 1 us.
 * 
 * @param n The number of us to delay the process.
 */
void delayMicrosec(uint32_t n);
