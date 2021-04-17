/**
 * File name: Switch.c
 * Devices: LM4F120; TM4C123
 * Description: Low level drivers for onboard and offboard switches. Performs
 * internal debounceing.
 * Authors: Matthew Yu.
 * Last Modified: 04/17/21
 */

/** General imports. */
#include <stdio.h>

/** Device specific imports. */
#include <inc/RegDefs.h>
#include <lib/Switch/Switch.h>


/**
 * SwitchInit initializes a switch with provided function pointers to tasks
 * executed when the switch is pressed or released.
 * @param pin Pin that the switch is initialized to.
 * @param touchTask Function executed when switch is pressed (falling edge).
 * @param releaseTask Function executed when switch is released (rising edge).
 * @note Functions must take no explicit parameters and return values.
 *       Requires the EnableInterrupts() call if edge triggered interrupts are enabled.
 */
void SwitchInit(pin_t pin, void (*touchTask)(void), void (*releaseTask)(void)) {
    /* 1. Generate the config. */
    GPIOConfig_t pinConfig = { pin, PULL_DOWN, false, false, 0, false };

    /* 2. Enable pull up resistors for PF0 and PF4 by default. */
    if (pin == PIN_F0 || pin == PIN_F4) pinConfig.pull = PULL_UP;

    /* 3. Initialize the GPIO. Generate interrupt config if touchTask or
       releaseTask are populated. */
    if (touchTask != NULL || releaseTask != NULL) {
        GPIOInterruptConfig_t pinIntConfig = { 3, touchTask, releaseTask, LOWERED };
        GPIOIntInit(pinConfig, pinIntConfig);
    } else {
        GPIOInit(pinConfig);
    }
}

/**
 * SwitchGetValue gets the current value of the switch.
 * @param pin Pin that switch is tied to.
 * @return False if pressed, True if released.
 */
bool SwitchGetValue(pin_t pin) {
    return GPIOGetBit(pin);
}
