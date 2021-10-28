/**
 * @file PWM.h
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief PWM peripheral driver.
 * @version 0.1
 * @date 2021-10-28
 * @copyright Copyright (c) 2021
 * @note
 * Modes. This driver will support both the TM4C's existing PWM modules as well
 * as a GPIO + Timer configuration.
 * Unsupported Features. This driver does not support PWM module interrupts, or
 * different priorities for Timer based PWM.
 */
#pragma once

/** General imports. */
#include <stdbool.h>
#include <stdint.h>

/** Device Specific imports. */
#include <lib/GPIO/GPIO.h>
#include <lib/Timer/Timer.h>


/** @brief PWMPin is an enumeration that specifies one of the available PWM pins
 * on the TM4C. */
typedef enum PWMPin {
    M0_PB6,
    M0_PB7,
    M0_PB4,
    M0_PB5,
    M0_PE4,
    M0_PE5,
    M0_PC4,
    M0_PD0,
    M0_PC5,
    M0_PD1,
    M1_PD0,
    M1_PD1,
    M1_PA6,
    M1_PE4,
    M1_PA7,
    M1_PE5,
    M1_PF0,
    M1_PF1,
    M1_PF2,
    M1_PF3,
    PWM_COUNT
} PWMPin_t;

/** @brief PWMSource is an enumeration that specifies the type of PWM being
 *         generated. */
enum PWMSource { 
    PWM_SOURCE_DEFAULT, 
    PWM_SOURCE_TIMER
};

/** @brief PWMDivisor is an enumeration that specifies the PWM unit clock
 *         divisor. */
enum PWMDivisor {
    PWM_DIV_OFF,
    PWM_DIV_2,
    PWM_DIV_4,
    PWM_DIV_8,
    PWM_DIV_16,
    PWM_DIV_32,
    PWM_DIV_64
};

/** @brief PWMModuleConfig is a struct defined by the user to specify a PWM
 *         module configuration. */
struct PWMModuleConfig {
    /** 
     * @brief Pin to output PWM with.
     *
     * Default M0_PB6.
     */
    PWMPin_t pin;

    /** 
     * @brief PWM reload period, in cycles. This period is dependent on
     *        the system clock, which is typically defined at 80 MHZ by
     *        PLLInit. This period can be extended by using the divisor.
     * 
     * This value must be specified and be greater than zero. Failing
     * these conditions will trigger an internal assert in debug mode. 
     * In production, this causes undefined behavior. 
     * 
     * @note PWM modules can only have a maximum period of 16 bits. At
     *       80 MHz, the maximum period is 819,187.5 ns if not using the divisor
     *       field.
     */
    uint16_t period;

    /** ------------- Optional Fields. ------------- */

    /**
     * @brief The divisor of the PWM clock. For example, a divisor value
     *        of 2 will drop the effective system clock used by the PWM
     *        module in half, doubling the effective period and halving
     *        the effective frequency.
     * 
     * Default is PWM_DIV_OFF, or disabled.
     */
    enum PWMDivisor divisor;
};

/** @brief PWMTimerConfig is a struct defined by the user to specify a PWM timer
 *         based configuration. */
struct PWMTimerConfig {
    /** 
     * @brief Pin to output PWM with.
     * 
     * Default PIN_A0.
     */
    GPIOPin_t pin;

    /**
     * @brief The Timer module to enable.
     *
     * Default is TIMER_0A.
     */
    TimerID_t timerID;

    /**
     * @brief The timer reload period, in cycles. This period is
     *        multiplied by the prescale value, which is optional.
     *
     * This value must be specified and be greater than zero. Failing
     * this condition will trigger an internal assert in debug mode. In
     * production, this causes undefined behavior.
     * 
     * @note Values greater than 0xFFFF or 0xFFFFFFFF are truncated for
     *       16 bit timers and 32 bit timers, respectively. 
     */
    uint64_t period;

    /** ------------- Optional Fields. ------------- */

    /**
     * @brief Whether the Timer is separated from the B side timer to
     *        make a 16/32 bit timer.
     * 
     * Default is false (Timers are concatenated and are 32/64 bits
     * wide). 
     */
    bool isIndividual;

    /**
     * @brief A multiplier for the timer reload period. For example, a
     *        prescale value of 1 doubles the base period, effectively
     *        halving the frequency. A prescale value of 2 triples the
     *        period, and so on. The multiplier is the prescale value +
     *        1.
     * 
     * Default is 0 (No scaling, or 1x scaling).
     * 
     * @note Values greater than 0xFF are truncated for non wide timers.
     */
    uint16_t prescale;
};

/** @brief PWMConfig_t is a user defined struct that specifies a PWM pin
 *         configuration. */
typedef struct PWMConfig {
    /** 
     * @brief Enumerator defining what type of PWM should be configured.
     * 
     * Default is PWM, which requires a PWM pin specified.
     */
    enum PWMSource source;

    /** @brief A union that specifies the PWM based or TIMER based configuration. */
    union {
        /**
         * @brief Struct configuration used with a PWM module source.
         * 
         * Default M0_PB6, with the divisor off.
         */
        struct PWMModuleConfig pwmSelect;

        /** 
         * @brief Struct configuration used with a TIMER source.
         * 
         * Default PIN_A0, TIMER_0A, concatenated A and B sides, with no
         * prescaling. 
         */
        struct PWMTimerConfig timerSelect;
    } sourceInfo;

    /**
     * @brief Percentage of the PWM that is on. This is a value from [0, 100].
     * 
     * Default value is 0, but the recommended value is 50 for most applications.
     */
    uint8_t dutyCycle;
} PWMConfig_t;

/** @brief PWM_t is a struct containing user relevant data of a PWM. */
typedef struct PWM {
    /** @brief The source of the PWM. */
    enum PWMSource source;

    union {
        /** @brief PWM pin used as the source. */
        PWMPin_t pin;

        /** @brief Timer info generated for this PWM. */
        struct {
            /** @brief The user relevant data of the timer being used. */
            Timer_t timer;

            /** @brief The pin acting as a PWM associated with the timer. */
            GPIOPin_t pin;
        } timerInfo;
    } sourceInfo;
} PWM_t;

/**
 * @brief PWMInit initializes a PWM configuration with a given frequency and duty
 * cycle.
 * 
 * @param config The PWM configuration that should be started.
 * @return The PWM struct.
 * @note Calling a Timer based PWM requires calling EnableInterrupts() after
 *       initialization. Make sure the timer specified for TIMER based PWMs is
 *       not already in use.
 * 
 *       Note that users should use the provided PWM divisor fields or Timer 
 *       prescaler fields when defining low PWM frequencies with periods large 
 *       enough that may overflow the period registers. See the period field 
 *       documentation for each config on the register size.
 */
PWM_t PWMInit(PWMConfig_t config);

/**
 * @brief PWMStop disables a PWM configuration.
 * 
 * @param pwm The PWM instance that should be updated and restarted.
 */
void PWMStop(PWM_t pwm);

/**
 * @brief PWMStart enables a PWM configuration.
 * 
 * @param pwm The PWM instance that should be updated and restarted.
 */
void PWMStart(PWM_t pwm);
