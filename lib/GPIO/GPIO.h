/**
 * @file GPIO.h
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief GPIO peripheral driver.
 * @version 0.1
 * @date 2021-09-28
 * @copyright Copyright (c) 2021
 * @note
 * Fast Mode. Use optional compilation flag (1) or define a preprocessor macro
 * (2) in your project for compiling with GPIOSetBit(Fast) and GPIOGetBit(Fast).
 *      (1) `-D__FAST__`
 *      (2) `#define __FAST__ 1`
 * This is highly recommended for programs using interrupts that call these
 * functions to edit GPIO bits, or for high frequency operation programs. There
 * is a tradeoff of on the order of 100x speedup vs 1KB more lines of DATA.
 *
 * Unsupported Features. This driver does not support DMA control.
 */

#pragma once

/** General imports. */
#include <stdbool.h>
#include <stdint.h>


/** @brief GPIOPin_t is an enumeration that specifies a GPIO pin on the TM4C. */
typedef enum GPIOPin {
    PIN_A0, PIN_A1, PIN_A2, PIN_A3, PIN_A4, PIN_A5, PIN_A6, PIN_A7,
    PIN_B0, PIN_B1, PIN_B2, PIN_B3, PIN_B4, PIN_B5, PIN_B6, PIN_B7,
    PIN_C0, PIN_C1, PIN_C2, PIN_C3, PIN_C4, PIN_C5, PIN_C6, PIN_C7,
    PIN_D0, PIN_D1, PIN_D2, PIN_D3, PIN_D4, PIN_D5, PIN_D6, PIN_D7,
    PIN_E0, PIN_E1, PIN_E2, PIN_E3, PIN_E4, PIN_E5, PIN_E6, PIN_E7,
    PIN_F0, PIN_F1, PIN_F2, PIN_F3, PIN_F4, PIN_F5, PIN_F6, PIN_F7,
    PIN_COUNT, PINS_PER_PORT = 8, PORT_COUNT = PIN_COUNT / PINS_PER_PORT,
} GPIOPin_t;

/**
 * @brief GPIOConfig_t is a user defined struct that specifies a GPIO pin
 *        configuration.
 */
typedef struct GPIOConfig {
    /**
     * @brief The GPIO Pin to enable.
     *
     * Default is PIN_A0.
     */
    GPIOPin_t pin;

    /** ------------- Optional Fields. ------------- */

    /**
     * @brief Whether the GPIO activates the internal pullup or pulldown
     *        resistor, or is open drain.
     *
     * Default is TRI_STATE.
     */
    enum GPIOPull { GPIO_TRI_STATE, GPIO_PULL_UP, GPIO_PULL_DOWN, GPIO_OPEN_DRAIN } pull;

    /**
     * @brief Direction of the pin. True for output, false for input.
     *
     * Default is false (Input).
     */
    bool isOutput;

    /**
     * @brief Alternate function encoding from 1 to 15. A zero value indicates
     *        that alternate function is off. See Table 23-5. GPIO Pins and
     *        Alternative Functions on p. 1351 of the TM4C Datasheet for more
     *        details.
     *
     * Default is 0 (Alternate function is disabled).
     */
    uint8_t alternateFunction;

    /**
     * @brief Whether the pin is digital or analog. True for analog, false for
     *        digital.
     *
     * Default is false (Digital mode).
     */
    bool isAnalog;

    /**
     * @brief The drive strength of the pin.
     *
     * Default is 2-mA drive.
     */
    enum GPIODrive { GPIO_DRIVE_2MA, GPIO_DRIVE_4MA, GPIO_DRIVE_8MA } drive;

    /**
     * @brief Used for 8-mA drive strength; whether slew rate control is enabled.
     *        Essentially increases the GPIO rise and fall time by a couple ns
     *        on 8-mA drive.
     *
     * Default is false (Slew rate control is disabled).
     */
    bool enableSlew;
} GPIOConfig_t;

/**
 * @brief GPIOInterruptConfig_t is a user defined struct that specifies a GPIO
 *        pin interrupt configuration.
 */
typedef struct GPIOInterruptConfig {
    /**
     * @brief GPIO edge interrupt priority. From 0 - 7. Lower value is higher
     *        priority.
     *
     * Default is 0 (Highest priority).
     */
    uint8_t priority;

    /**
     * @brief Pointer to function called on rising edge interrupts. Accepts any
     *        number of arguments, but they must be handled as pointers by the
     *        function.
     *
     * Default is NULL (No function defined).
     */
    void (*touchTask)(uint32_t *args);

    /**
     * @brief The pointer to an array of uint32_t arguments that fed into
     *        touchTask upon being called.
     *
     * Default is NULL (No pointer to any arguments is defined).
     *
     * @note Use with caution. The programmer is responsible for handling
     *       touchArgs inside of their touchTask function implementation.
     */
    uint32_t * touchArgs;

    /**
     * @brief Pointer to function called on falling edge interrupts. Accepts any
     *        number of arguments, but they must be handled as pointers by the function.
     *
     * Default is NULL (No function defined).
     */
    void (*releaseTask)(uint32_t *args);

    /**
     * @brief The pointer to an array of uint32_t arguments that fed into
     *        releaseTask upon being called.
     *
     * Default is NULL (No pointer to any arguments is defined).
     *
     * @note Use with caution. The programmer is responsible for handling
     *       releaseArgs inside of their releaseTask function implementation.
     */
    uint32_t * releaseArgs;

    /**
     * @brief Whether an input pin was previously raised or lowered. Used for
     *        debouncing.
     *
     * Default is LOWERED.
     */
    enum GPIOIntPinStatus {LOWERED, RAISED} pinStatus;
} GPIOInterruptConfig_t;

/**
 * @brief GPIOInit initializes a GPIO pin given a configuration.
 *
 * @param config Configuration for pin.
 * @return A GPIOPin_t pin.
 * @note By default, this driver does not support slew rate or drive strength.
 */
GPIOPin_t GPIOInit(GPIOConfig_t config);

/**
 * @brief GPIOIntInit is a version of the initializer that provides interrupt
 *        capability.
 *
 * @param config Configuration for pin.
 * @param intConfig Configuration for pin interrupt.
 * @return A GPIOPin_t pin.
 * @note Requires the EnableInterrupts() call if edge triggered interrupts are enabled.
 */
GPIOPin_t GPIOIntInit(GPIOConfig_t config, GPIOInterruptConfig_t intConfig);

/**
 * @brief GPIOSetBit sets the bit for an (assumed) output GPIO pin.
 *
 * @param pin GPIOPin_t to set.
 * @param value 0 (false) or 1 (true) value to set pin to.
 * @note This method assumes that the GPIO was configured as an output.
 */
void GPIOSetBit(GPIOPin_t pin, bool value);

/**
 * @brief GPIOGetBit returns the value of the bit at the specified GPIO pin.
 *
 * @param pin GPIOPin_t to set.
 * @return value 0 (false) or 1 (true) value of the pin.
 */
bool GPIOGetBit(GPIOPin_t pin);
