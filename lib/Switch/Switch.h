/**
 * File name: Switch.h
 * Devices: LM4F120; TM4C123
 * Description: Low level drivers for onboard and offboard switches. Performs
 * internal debouncing.
 * Authors: Matthew Yu.
 * Last Modified: 04/05/21
 */
#pragma once

/** General imports. */
#include <stdbool.h>

/** Device specific imports. */
#include <lib/GPIO/GPIO.h>


/**
 * SwitchInit initializes a switch with provided function pointers to tasks
 * executed when the switch is pressed or released.
 * @param pin Pin that the switch is initialized to.
 * @param touchTask Function executed when switch is pressed (falling edge).
 * @param releaseTask Function executed when switch is released (rising edge).
 * @note Functions must take no explicit parameters and return values.
 *		 Requires the EnableInterrupts() call if edge triggered interrupts are enabled.
 */
void SwitchInit(pin_t pin, void (*touchTask)(void), void (*releaseTask)(void));

/**
 * SwitchGetValue gets the current value of the switch.
 * @param pin Pin that switch is tied to.
 * @return False if pressed, True if released.
 */
bool SwitchGetValue(pin_t pin);
