/**
 * @file PWM.h
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief An example project showing how to use the PWM driver.
 * @version 0.1
 * @date 2021-09-24
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
         * @brief Pin to output PWM with.
         *
         * Default M0_PB6.
         */
        PWMPin_t pin;
        
        /** 
         * @brief Struct configuration used with a TIMER source.
         * 
         * Default PIN_A0 and TIMER_0A.
         */
        struct {
            /** 
             * @brief Pin to output PWM with.
             * 
             * Default PIN_A0.
             */
            GPIOPin_t pin;

            /** 
             * @brief Timer to execute the PWM on.
             * 
             * Default TIMER_0A.
             */
            TimerID_t timerID;
        } timerSelect;
    } sourceInfo;

    /** 
     * @brief PWM cycle duration. This is dependent on the system
     *        clock, which is typically defined at 80 MHZ by PLLInit.
     * 
     * @note This value must be specified and be greater than zero. PWM modules  
	 *       can only have a maximum period of 16 bit; timers can go up to 32 bit. 
	 *       Failing these conditions will trigger an internal assert in debug 
	 *       mode. In production, this causes undefined behavior.
     */
    uint32_t period;

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
 *       It is highly recommended that a timer based PWM is used when the period
 *       is a value larger than 0xFFFF. The PWM load register saturates at this 
 *       period. This may mean different cutoff frequencies depending on the system
 *       clock. At 80 MHz, this is around 2441 Hz.
 */
PWM_t PWMInit(PWMConfig_t config);

/**
 * @brief PWMUpdateConfig updates the PWM period and duty cycle. And then starts
 *        it. Does not check if the PWM was previously initialized.
 * 
 * @param pwm The PWM instance that should be updated and restarted.
 * @param period The period of one cycle of the PWM.
 *               This value must be specified and be greater than zero. PWM modules 
 *               can only have a maximum period of 16 bit; timers can go up to 32 bit. 
 *               Failing these conditions will trigger an internal assert in debug 
 *               mode. In production, this causes undefined behavior.
 * @param dutyCycle The duty cycle of one cycle of the PWM, from 0 to 100.
 * @note Only one Timer based PWM can be on at a time.  
 */
void PWMUpdateConfig(PWM_t pwm, uint32_t period, uint8_t dutyCycle);

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
