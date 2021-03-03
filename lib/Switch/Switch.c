/**
 * File name: Switch.c
 * Devices: LM4F120; TM4C123
 * Description: Low level drivers for onboard and offboard switches. Performs
 * internal debouncing.
 * Authors: Matthew Yu.
 * Last Modified: 03/03/21
 */

/** General imports. */
#include <math.h>

/** Device specific imports. */
#include "Switch.h"

typedef enum PinStatus {
    RAISED,
    LOWERED
} PinStatus;

typedef struct SwitchConfig {
    /** Pin ID. */
    pin_t pin;
	
    /** Pointer to function called on rising edge interrupt. */
    void (*touchTask)(void);
	
    /** Pointer to function called on falling edge interrupt. */
    void (*releaseTask)(void);
	
    /** Last known status of the pin. */
	PinStatus status;
} SwitchConfig;

/** Array of potential switch configurations. */
static SwitchConfig switchConfig[] = {
    PIN_F0, NULL, NULL, LOWERED,
    PIN_F1, NULL, NULL, LOWERED,
    PIN_F2, NULL, NULL, LOWERED,
    PIN_F3, NULL, NULL, LOWERED,
    PIN_F4, NULL, NULL, LOWERED,
    PIN_F5, NULL, NULL, LOWERED,
    PIN_F6, NULL, NULL, LOWERED,
    PIN_F7, NULL, NULL, LOWERED,
};

/**
 * SwitchInit initializes a switch with provided function pointers to tasks
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
void SwitchInit(pin_t pin, void (*touchTask)(void), void (*releaseTask)(void)) {
    /**
     * To initialize the GPIO pin to accept a switch input, we must do the following steps:
     */
    SYSCTL_RCGCGPIO_R |= 0x00000020;                // 1) Activate the clock for the relevant port.
    while ((SYSCTL_PRGPIO_R & 0x00000020) == 0) {}; // 2) Stall until clock is ready.
    if (pin == PIN_F0)
        GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;          // 3) Unlock the port (if PC0-3, PD7, PF0).

    uint8_t pinAddress = pow(2, pin % 8);           // 4) Determine the pin address. I.e. PF1 (41) % 8 = 1. 
    GPIO_PORTF_CR_R |= pinAddress;                  // 5) Allow changes to selected pin. 
    GPIO_PORTF_AMSEL_R = 0;                         // 6) Disable analog functionality across PORTF.
    GPIO_PORTF_AFSEL_R = 0;                         // 7) Disable alternative function functionality across PORTF.
    GPIO_PORTF_PCTL_R = 0;                          // 8) Clear PCTL register to select regular digital function across PORTF.
    GPIO_PORTF_DIR_R &= ~pinAddress;                // 9) Set pin to be an input (0).
    if (pin == PIN_F0 || pin == PIN_F4)             
        GPIO_PORTF_PUR_R |= pinAddress;             // 10) Enable pull-ups for on board switches.
    GPIO_PORTF_DEN_R |= pinAddress;                 // 11) Enable digital IO for the pin.

    // 12) Enable PortF edge triggered interrupts if we specify a task for the pin.
    if (touchTask != NULL || releaseTask != NULL) {
        GPIO_PORTF_IS_R &= ~pinAddress;                         // 12a) Set pin as edge-sensitive.
        if (touchTask != NULL && releaseTask != NULL) {   
            GPIO_PORTF_IBE_R |= pinAddress;                     // 12b) Set pin to interrupt on both edges.
        } else {
            GPIO_PORTF_IBE_R &= ~pinAddress;                    // 12c) Set pin to interrupt as dictated by GPIOIEV.
            if (touchTask != NULL) {
                GPIO_PORTF_IEV_R |= pinAddress;                 // 12d) Set pin to interrupt on a rising edge.
            } else {
                GPIO_PORTF_IEV_R &= ~pinAddress;                // 12e) Set pin to interrupt on a falling edge.
            }
        } 
        GPIO_PORTF_ICR_R |= pinAddress;                         // 12f) Clear flag for the pin.
        GPIO_PORTF_IM_R |= pinAddress;                          // 12g) Arm interrupt for pin.
        NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF00FFFF) | 0x00A00000;  // 12h) Give the interrupt priority 5.
        NVIC_EN0_R = 0x40000000;                                // 12i) Enable interrupt 30 in NVIC.
    }

    switchConfig[pin % 8].touchTask = touchTask;
    switchConfig[pin % 8].releaseTask = releaseTask;
}

/** Internal handler to manage switch interrupts. Can conflict with duplicate GPIOPortFHandlers. */
void GPIOPortF_Handler(void) {
    // Find which pin triggered an interrupt. Could be multiple at once.
    for (uint8_t i = 0; i < 8; i++) {
        if (GPIO_PORTF_RIS_R & (0x1 << i)) {
            // Acknowledge interrupt flag.
            GPIO_PORTF_ICR_R |= 0x1 << i;

            // Figure out if rising or falling edge by reading the data register.
            if (SwitchGetValue((pin_t) i) == 1 && switchConfig[i].status == LOWERED) {
                // Rising edge.
                switchConfig[i].touchTask();
                switchConfig[i].status = RAISED;
            } else if (SwitchGetValue((pin_t) i) == 0 && switchConfig[i].status == RAISED){
                // Falling edge.
                switchConfig[i].releaseTask();
                switchConfig[i].status = LOWERED;
            }
        }
    }
}

/**
 * SwitchGetValue gets the current value of the switch. Useful for 
 * @param pin Pin that switch is tied to.
 * @return False if pressed, True if released.
 */
unsigned long SwitchGetValue(pin_t pin) {
    uint8_t pinAddress = pow(2, pin % 8);
    return (~GPIO_PORTF_DATA_R & pinAddress) >> (pin % 8);
}
