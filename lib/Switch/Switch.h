/**
 * File name: Switch.h
 * Devices: LM4F120; TM4C123
 * Description: Low level drivers for onboard and offboard switches. Performs
 * internal debouncing.
 * Authors: Matthew Yu.
 * Last Modified: 03/03/21
 */
#pragma once

/** General imports. */
#include <stdio.h>
#include <stdint.h>
#include <math.h>

/** Device specific imports. */
#include <TM4C123Drivers/inc/tm4c123gh6pm.h>
#include <TM4C123Drivers/inc/GPIO.h>


/**
 * SwitchInit initializes a switch on Port F with provided function pointers to tasks
 * executed when the switch is pressed or released.
 * @param pin Pin that the switch is initialized to.
 * @param touchTask Function executed when switch is pressed (falling edge).
 * @param releaseTask Function executed when switch is released (rising edge).
 * @note Functions must take no explicit parameters and return values.
 *       Only Port F pins are supported (including onboard pins).
 *       Default interrupt priority is 5.
 *       PORTF pin analog and alternative functionality is disabled after initialization.
 *       Requires the EnableInterrupts() call if edge triggered interrupts are enabled.
 */
void SwitchInit(pin_t pin, void (*touchTask)(void), void (*releaseTask)(void));

/**
 * SwitchGetValue gets the current value of the switch.
 * @param pin Pin that switch is tied to.
 * @return False if pressed, True if released.
 */
unsigned long SwitchGetValue(pin_t pin);
