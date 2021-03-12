/**
 * GPIO.c
 * Devices: LM4F120; TM4C123
 * Description: Low level drivers for GPIO initialization.
 * Authors: Matthew Yu.
 * Last Modified: 03/10/21
 * 
 * By default, this driver does not support slew rate, drive strength, or
 * interrupts.
 */

/** General imports. */
#include <math.h>

/** Device imports. */
#include "GPIO.h"
#include <TM4C123Drivers/inc/RegDefs.h>
#include <TM4C123Drivers/inc/tm4c123gh6pm.h>

/**
 * GPIOInit initializes a GPIO pin given a configuration.
 * @param pinConfig Configuration for pin.
 * @note By default, this driver does not support slew rate, drive strength, or
 * interrupts.
 */
void GPIOInit(GPIOConfig_t pinConfig) {
    /* Early return on invalid pin_t value. */
    if (pinConfig.GPIOPin >= PIN_COUNT) return;

    /* 1. Activate the clock for the relevant port. */
    GET_REG(SYSCTL_BASE + SYSCTL_RCGCGPIO_OFFSET) |= 
        1 << (pinConfig.GPIOPin / 8); /* 8 pins per port. */

    /* 2. Stall until clock is ready. */
    while ((GET_REG(SYSCTL_BASE + SYSCTL_PRGPIO_OFFSET) & 
           (1 << (pinConfig.GPIOPin / 8))) == 0) {};

    /* 3. Generate the port offset to find the correct addresses. */
    uint32_t portOffset = 0;
    if (pinConfig.GPIOPin < PIN_E0) portOffset = 0x1000 * (pinConfig.GPIOPin/8);
    /* Jump to 0x4002.4000 for PORTE. */
    else portOffset = 0x00020000 + 0x1000 * ((pinConfig.GPIOPin-PIN_E0)/8);

    /* 4. If PC0-3, PD7, PF0, unlock the port. */
    if (pinConfig.GPIOPin == PIN_C0 ||
        pinConfig.GPIOPin == PIN_C1 ||
        pinConfig.GPIOPin == PIN_C2 ||
        pinConfig.GPIOPin == PIN_C3 ||
        pinConfig.GPIOPin == PIN_D7 ||
        pinConfig.GPIOPin == PIN_F0 ) {
        GET_REG(GPIO_PORT_BASE + portOffset + GPIO_LOCK_OFFSET) = GPIO_LOCK_KEY;
    }

    /* 5. Determine the pin address. I.E. PF1 (41) % 8 = 1. */
    uint8_t pinAddress = pow(2, pinConfig.GPIOPin % 8);
    
    /* 6. Allow changes to selected pin. */
    GET_REG(GPIO_PORT_BASE + portOffset + GPIO_CR_OFFSET) |= pinAddress;

    /* 7. Set direction of pin. */
    GET_REG(GPIO_PORT_BASE + portOffset + GPIO_DIR_OFFSET) &= ~pinAddress;
    if (pinConfig.isOutput)
        GET_REG(GPIO_PORT_BASE + portOffset + GPIO_DIR_OFFSET) |= pinAddress;

    /* 8. Set alternative function if required. */
    if (pinConfig.isAlternative) {
        GET_REG(GPIO_PORT_BASE + portOffset + GPIO_AFSEL_OFFSET) |= pinAddress;
        uint32_t mask = 0xFFFFFFFF;
        mask &= ~(0xF << ((pinConfig.GPIOPin % 8) * 4));
        GET_REG(GPIO_PORT_BASE + portOffset + GPIO_PCTL_OFFSET) &= mask;
        GET_REG(GPIO_PORT_BASE + portOffset + GPIO_PCTL_OFFSET) |=
            (pinConfig.alternateFunction << ((pinConfig.GPIOPin % 8) * 4));
    }

    /* 9. Set pin drive strength to 8mA. This step is performed by default. */
    GET_REG(GPIO_PORT_BASE + portOffset + GPIO_DR8R_OFFSET) |= pinAddress;

    /* 10. Set pullup, pulldown, or open drain. */
    if (pinConfig.GPIOPull == PULL_UP) {
        GET_REG(GPIO_PORT_BASE + portOffset + GPIO_PUR_OFFSET) |= pinAddress;
    } else if (pinConfig.GPIOPull == PULL_DOWN) {
        GET_REG(GPIO_PORT_BASE + portOffset + GPIO_PDR_OFFSET) |= pinAddress;
    } else if (pinConfig.GPIOPull == OPEN_DRAIN) {
        GET_REG(GPIO_PORT_BASE + portOffset + GPIO_ODR_OFFSET) |= pinAddress;
    }

    /* 11. Enable as digital or as analog pins. */
    if (pinConfig.isAnalog) {
        GET_REG(GPIO_PORT_BASE + portOffset + GPIO_AMSEL_OFFSET) |= pinAddress;
    } else {
        GET_REG(GPIO_PORT_BASE + portOffset + GPIO_DEN_OFFSET) |= pinAddress;
    }
}

/**
 * GPIOSetBit sets the bit for an (assumed) output GPIO pin.
 * Substitute for the following:
 * 
 * Option 1:
 *      #define PF1       (*((volatile uint32_t *)0x40025008))
 *      PF1 ^= 0x2;
 * 
 * Option 2:
 *      GET_REG(GPIO_PORT_BASE + portOffset + GPIO_DATA_OFFSET) ^= 0x2;
 * 
 * @param pin Pin to set. Assumes it's an output.
 * @param value 0 (false) or 1 (true) value to set pin to.
 */
void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s
void GPIOSetBit(pin_t pin, bool value) {
    /* Early return on invalid pin_t value. */
    if (pin >= PIN_COUNT) return;

    /* 1. Generate the port offset to find the correct addresses. */
    uint32_t portOffset = 0;
    if (pin < PIN_E0) portOffset = 0x1000 * (pin/8);
    /* Jump to 0x4002.4000 for PORTE. */
    else portOffset = 0x00020000 + 0x1000 * ((pin-PIN_E0)/8);

    /* 2. Determine the pin address. I.E. PF1 (41) % 8 = 1. */
    uint8_t pinAddress = pow(2, pin % 8);

	uint8_t val = value ? pow(2, pin % 8) : 0;
    GET_REG(GPIO_PORT_BASE + portOffset + (pinAddress << 2)) = val;
}

/**
 * GPIOGetBit returns the value of the bit at the specified GPIO pin.
 * Substitute for the following:
 * 
 * Option 1:
 *      #define PF1       (*((volatile uint32_t *)0x40025008))
 *      val = PF1 & 0x2;
 * 
 * Option 2:
 *      val = GET_REG(GPIO_PORT_BASE + portOffset + GPIO_DATA_OFFSET) 
 *              &= 0x2;
 * 
 * @param pin Pin to set. Assumes it's an output.
 * @return value 0 (false) or 1 (true) value of the pin.
 */
bool GPIOGetBit(pin_t pin) {
    /* Early return on invalid pin_t value. */
    if (pin >= PIN_COUNT) return 0;

    /* 1. Generate the port offset to find the correct addresses. */
    uint32_t portOffset = 0;
    if (pin < PIN_E0) portOffset = 0x1000 * (pin/8);
    /* Jump to 0x4002.4000 for PORTE. */
    else portOffset = 0x00020000 + 0x1000 * ((pin-PIN_E0)/8);

    /* 2. Determine the pin address. I.E. PF1 (41) % 8 = 1. */
    uint8_t pinAddress = pow(2, pin % 8);

    return GET_REG(GPIO_PORT_BASE + portOffset + (pinAddress << 2));
}
