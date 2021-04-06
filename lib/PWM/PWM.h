/**
 * PWM.h
 * Devices: LM4F120; TM4C123
 * Description: Middle level drivers for generating square waves.
 * Authors: Matthew Yu.
 * Last Modified: 03/31/21
 * 
 * This driver will support both the TM4C's existing PWM modules as well as a
 * GPIO + Timer configuration. 
 */
#pragma once

/** General imports. */
#include <stdbool.h>
#include <stdint.h>

/** Device Specific imports. */
#include "GPIO.h"   /* GPIO pin to configure. */
#include "Timer.h"  /* Clock for toggling the GPIO pin. */


/** Enumerator defining all possible PWMs that can be initialized. */
enum PWMSelect {
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
    M1_PF3
};

/**
 * PWM configuration specifying the PWM that needs to be initialized.
 */
struct PWMConfig {
    /** Enumerator defining what type of PWM should be configured. */
    enum PWMSource { DEFAULT, TIMER } source;

    /** Pin and PWM configuration. */
    union config {
        enum PWMSelect configDefault;
        pin_t configTimer;
    };

    /** Timer used for the PWM if source is TIMER. */
    enum TimerID timerID;
}

/**
 * PWMInit initializes a PWM configuration with a given frequency and duty
 * cycle.
 * 
 * @param pwmConfig The PWM configuration that should be started.
 * @param frequency The frequency of one cycle of the PWM.
 * @param dutyCycle The duty cycle of one cycle of the PWM.
 */
void PWMInit(struct PWMConfig pwmConfig, uint32_t frequency, uint32_t dutyCycle);

/**
 * PWMStop disables a PWM configuration.
 * 
 * @param pwmConfig The PWM that should be halted.
 */
void PWMStop(struct PWMConfig pwmConfig);