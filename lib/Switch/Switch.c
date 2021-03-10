/**
 * File name: Switch.c
 * Devices: LM4F120; TM4C123
 * Description: Low level drivers for onboard and offboard switches. Performs
 * internal debounceing.
 * Authors: Matthew Yu.
 * Last Modified: 03/10/21
 * TODO: update interrupts and handlers to use any generic pin provided. Remove
 * tm4c123gh6pm.
 */

/** General imports. */
#include <math.h>
#include <stdio.h>

/** Device specific imports. */
#include "Switch.h"
#include <TM4C123Drivers/inc/RegDefs.h>
#include <TM4C123Drivers/inc/tm4c123gh6pm.h>


/** Boolean status of a switch, used for debouncing. */
typedef enum PinStatus {
    RAISED,
    LOWERED
} PinStatus;

/** Configuration of a single switch. */
struct SwitchConfig {
    /** Pin ID. */
    pin_t pin;
	
    /** Pointer to function called on rising edge interrupt. */
    void (*touchTask)(void);
	
    /** Pointer to function called on falling edge interrupt. */
    void (*releaseTask)(void);
	
    /** Last known status of the pin. */
	PinStatus status;
};

/** Array of potential switch configurations. */
static struct SwitchConfig switchConfig[] = {
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
 *       Requires the EnableInterrupts() call if edge triggered interrupts are enabled.
 */
void SwitchInit(pin_t pin, void (*touchTask)(void), void (*releaseTask)(void)) {
    /* 1. Generate the config. */
    GPIOConfig_t pinConfig = {pin, PULL_DOWN, false, false, 0, false};

    /* 2. Enable pull up resistors for PF0 and PF4. */
    if (pin == PIN_F0 || pin == PIN_F4) pinConfig.GPIOPull = PULL_UP;

    /* 3. Initialize the GPIO. */
    GPIOInit(pinConfig);

    /* 4. Enable PortF edge triggered interrupts if we specify a task for the pin. */
    if (touchTask != NULL || releaseTask != NULL) {
        /* 5. Generate the port offset and pinAddress to find the correct addresses. */
        uint32_t portOffset = 0;
        if (pinConfig.GPIOPin < PIN_E0) portOffset = 0x1000 * (pinConfig.GPIOPin/8);
        /* Jump to 0x4002.4000 for PORTE. */
        else portOffset = 0x00020000 + 0x1000 * ((pinConfig.GPIOPin-PIN_E0)/8);
        uint8_t pinAddress = pow(2, pinConfig.GPIOPin % 8);

        /* 6. Set pin as edge sensitive. */
        GET_REG(GPIO_PORT_BASE + portOffset + GPIO_IS_OFFSET) &= ~pinAddress;
        if (touchTask != NULL && releaseTask != NULL) {   
            /* 6b. Set pin to interrupt on both edges. */
            GET_REG(GPIO_PORT_BASE + portOffset + GPIO_IBE_OFFSET) |= pinAddress;
        } else {
            /* 6c. Set pin to interrupt as dictated by GPIOIEV. */
            GET_REG(GPIO_PORT_BASE + portOffset + GPIO_IBE_OFFSET) &= ~pinAddress;
            if (touchTask != NULL) {
                /* 6d. Set pin to interrupt on a rising edge. */
                GET_REG(GPIO_PORT_BASE + portOffset + GPIO_IEV_OFFSET) |= pinAddress;
            } else {
                /* 6e. Set pin to interrupt on a falling edge. */
                GET_REG(GPIO_PORT_BASE + portOffset + GPIO_IEV_OFFSET) &= ~pinAddress;
            }
        }
        /* 6f. Clear flag for the pin. TODO: see p. 657 step 7 in the
         * initialization example and follow those instructions. */
        GET_REG(GPIO_PORT_BASE + portOffset + GPIO_ICR_OFFSET) |= pinAddress;
        /* 6g. Arm interrupt for pin. */
        GET_REG(GPIO_PORT_BASE + portOffset + GPIO_IM_OFFSET) |= pinAddress;
        /* 6h. Give the interrupt priority 5. TODO: THIS IS SPECIFICALLY FOR
         * PORTF GPIO INTERRUPTS. See p. 104 Table 2-9 and Timer.c implementation
         * for doing this generically. */
        NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF00FFFF) | 0x00A00000;
        /* 6i. Enable interrupt 30 in NVIC. */
        NVIC_EN0_R = 0x40000000;
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
uint8_t SwitchGetValue(pin_t pin) {
    uint8_t pinAddress = pow(2, pin % 8);
    return (~GPIO_PORTF_DATA_R & pinAddress) >> (pin % 8);
}
