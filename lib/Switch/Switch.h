/**
 * File name: Switch.h
 * Devices: LM4F120; TM4C123
 * Description: Low level drivers for onboard and offboard switches. Performs
 * internal debouncing.
 * Authors: Jonathan Valvano. Revised by Matthew Yu.
 * Last Modified: 2/25/21
 */
#pragma once

/** General imports. */
#include <stdint.h>

/** Device specific imports. */
#include "tm4c123gh6pm.h"
#include "Misc.h"

#define DEBOUNCE_DELAY 10 /* 10 ms */

/**
 * SwitchInit initializes a switch with provided function pointers to tasks
 * executed when the switch is pressed or released.
 * @param pin Pin that the switch is initialized to. TODO: this param
 * @param touchTask Function executed when switch is pressed (falling edge).
 * @param releaseTask Function executed when switch is released (rising edge).
 * Functions take no parameters and return no values explicitly.
 * We assume a debounce time of about 10ms.
 */
void SwitchInit(uint32_t pin, void (*touchTask)(void), void (*releaseTask)(void));

/**
 * SwitchWaitForRelease polls until a release is detected. Blocking.
 * @param pin Pin that the switch is tied to.
 */
void SwitchWaitForRelease(uint32_t pin);

/**
 * SwitchWaitForPress polls until a press is detected. Blocking.
 * @param pin Pin that the switch is tied to.
 */
void SwitchWaitForPress(uint32_t pin);

/**
 * SwitchGetValue gets the current value of the switch.
 * @param pin Pin that switch is tied to.
 * @return False if pressed, True if released.
 */
unsigned long SwitchGetValue(uint32_t pin);
