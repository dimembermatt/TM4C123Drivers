/**
 * GPIO.h
 * Devices: LM4F120; TM4C123
 * Description: Low level drivers for GPIO initialization.
 * Authors: Matthew Yu.
 * Last Modified: 03/10/21
 * 
 * By default, this driver does not support slew rate, drive strength, or
 * interrupts.
 */
#pragma once

/** General imports. */
#include <stdbool.h>
#include <stdint.h>


typedef enum pin{
    PIN_A0, PIN_A1, PIN_A2, PIN_A3, PIN_A4, PIN_A5, PIN_A6, PIN_A7,
    PIN_B0, PIN_B1, PIN_B2, PIN_B3, PIN_B4, PIN_B5, PIN_B6, PIN_B7,
    PIN_C0, PIN_C1, PIN_C2, PIN_C3, PIN_C4, PIN_C5, PIN_C6, PIN_C7,
    PIN_D0, PIN_D1, PIN_D2, PIN_D3, PIN_D4, PIN_D5, PIN_D6, PIN_D7,
    PIN_E0, PIN_E1, PIN_E2, PIN_E3, PIN_E4, PIN_E5, PIN_E6, PIN_E7,
    PIN_F0, PIN_F1, PIN_F2, PIN_F3, PIN_F4, PIN_F5, PIN_F6, PIN_F7,
    PIN_COUNT, PORT_COUNT = PIN_COUNT / 8,
} pin_t;

typedef enum pull {
    PULL_UP,
    PULL_DOWN,
    OPEN_DRAIN,
    NONE
} pull_t;

typedef struct GPIOConfig {
    /** The GPIO Pin to set. */
    pin_t GPIOPin;

    /** 
     * Whether the GPIO activates the internal pullup or pulldown resistor, or
     * is open drain.
     */
    pull_t GPIOPull;

    /** Direction of the pin. 1 for output, 0 for input. */
    bool isOutput;

    /** 
     * Whether the pin is using an alternative function. 1 for alternative, 0
     * for default. 
     */
    bool isAlternative;

    /** 
     * Alternate function encoding from 1 to 15. Only used if isAlternative is
     * true. See Table 23-5. GPIO Pins and Alternative Functions on p. 1351 of
     * the TM4C Datasheet for more details.
     */
    uint8_t alternateFunction;

    /** Whether the pin is digital or analog. 1 for analog, 0 for digital. */
    bool isAnalog;
} GPIOConfig_t;

/**
 * GPIOInit initializes a GPIO pin given a configuration.
 * @param pinConfig Configuration for pin.
 * @note By default, this driver does not support slew rate, drive strength, or
 * interrupts.
 */
void GPIOInit(GPIOConfig_t pinConfig);

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
void GPIOSetBit(pin_t pin, bool value);

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
bool GPIOGetBit(pin_t pin);

bool GPIOGetBitFast(pin_t pin);

void GPIOSetBitFast(pin_t pin, bool val);
