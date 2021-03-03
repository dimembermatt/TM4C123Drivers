/**
 * File name: Switch.c
 * Devices: LM4F120; TM4C123
 * Description: Low level drivers for onboard and offboard switches. Performs
 * internal debouncing.
 * Authors: Jonathan Valvano. Revised by Matthew Yu.
 * Last Modified: 2/25/21
 */

/** General imports. */
#include <stdint.h>

/** Device specific imports. */
#include "Switch.h"

/**
 * SwitchInit initializes a switch with provided function pointers to tasks
 * executed when the switch is pressed or released.
 * @param pin Pin that the switch is initialized to.
 * @param touchTask Function executed when switch is pressed (falling edge).
 * @param releaseTask Function executed when switch is released (rising edge).
 * Functions take no parameters and return no values explicitly.
 * We assume a debounce time of about 10ms.
 * 
 * TODO: Integrate pin selection functionality, edge triggered functionality.
 */
void SwitchInit(uint32_t pin, void (*touchTask)(void), void (*releaseTask)(void)) {
    /**
     * To initialize the GPIO pin to accept a switch input, we must do the following steps:
     * For now, only PF4 is initialized by default.
     */
    SYSCTL_RCGCGPIO_R |= 0x00000020;                // 1) Activate the clock for the relevant port.
    while ((SYSCTL_PRGPIO_R & 0x00000020) == 0) {}; // 2) Stall until clock is ready.
    GPIO_PORTF_LOCK_R = 0x4C4F434B;                 // 3) Unlock the port (if PC0-3, PD7, PF0).
    GPIO_PORTF_CR_R = 0x10;                         //    Allow changes to PF4.
    GPIO_PORTF_AMSEL_R = 0;                         // 4) Disable analog functionality.
    GPIO_PORTF_AFSEL_R &= ~0x10;                    // 5) Disable alternative function functionality.
    GPIO_PORTF_PCTL_R &= ~0x000F0000;               // 6) Clear PCTL register to select regular digital function.
    GPIO_PORTF_DIR_R &= ~0x10;                      // 7) Set direction register bits to input.
    GPIO_PORTF_PUR_R |= 0x10;                       // 8) Enable pull-up on PF4.            
    GPIO_PORTF_IS_R &= ~0x10;                       // 9) Set P4 as edge sensitive.
    GPIO_PORTF_IBE_R |= 0x10;                       // 10) Set P4 to be edge sensitive on both sides.
    GPIO_PORTF_DEN_R |= 0x10;                       // 11) Enable the port.
}

/**
 * SwitchWaitForRelease polls until a release is detected. Blocking.
 * @param pin Pin that the switch is tied to.
 */
void SwitchWaitForRelease(uint32_t pin) {
    // while(SwitchGetValue)
    delayMillisec(DEBOUNCE_DELAY);
}

/**
 * SwitchWaitForPress polls until a press is detected. Blocking.
 * @param pin Pin that the switch is tied to.
 */
void SwitchWaitForPress(uint32_t pin) {
    delayMillisec(DEBOUNCE_DELAY);

}

/**
 * SwitchGetValue gets the current value of the switch.
 * @param pin Pin that switch is tied to.
 * @return False if pressed, True if released.
 */
unsigned long SwitchGetValue(uint32_t pin) {
    return 0;
}