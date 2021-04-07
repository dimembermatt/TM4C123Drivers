/**
 * PWM.h
 * Devices: LM4F120; TM4C123
 * Description: Middle level drivers for generating square waves.
 * Authors: Matthew Yu.
 * Last Modified: 04/06/21
 * 
 * This driver will support both the TM4C's existing PWM modules as well as a
 * GPIO + Timer configuration. 
 */
#pragma once

/** General imports. */
#include <stdbool.h>
#include <stdint.h>

/** Device Specific imports. */
#include <lib/GPIO/GPIO.h>
#include <lib/Timers/Timers.h>


/** Enumerator defining all possible PWMs that can be initialized. */
enum PWMPin {
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
};

/**
 * PWM configuration specifying the PWM that needs to be initialized.
 */
struct PWMConfig {
	/** Enumerator defining what type of PWM should be configured. */
	enum PWMSource { DEFAULT, TIMER } source;

	/** Pin and PWM configuration. */
	union {
		/** Configuration used with a DEFAULT source. */
		struct {
			/** Pin to output PWM with. */
			enum PWMPin pwmPin;

			/** Enum determining which comparator on the PWMpin is being used. */
			enum Comparator { PWMA, PWMB } comparator;
		} pwmSelect;

		/** Configuration used with a TIMER source. */
		struct {
			/** Pin to output PWM with. */
			pin_t pin;

			/** Timer to execute the PWM on. */
			enum TimerID timerID;
		} timerSelect;
	} config;
};

/**
 * PWMInit initializes a PWM configuration with a given frequency and duty
 * cycle.
 * 
 * @param pwmConfig The PWM configuration that should be started.
 * @param period The period of one cycle of the PWM.
 * @param dutyCycle The duty cycle of one cycle of the PWM, from 0 to 100.
 * @note Calling a Timer based PWM requires calling EnableInterrupts() after
 * initialization. Only one Timer based PWM can be on at a time.
 */
void PWMInit(struct PWMConfig pwmConfig, uint32_t period, uint32_t dutyCycle);

/**
 * PWMUpdateConfig updates the PWM period and duty cycle. 
 * Does not check if the PWM was previously initialized.
 * 
 * @param pwmConfig The PWM configuration that should be started.
 * @param period The period of one cycle of the PWM.
 * @param dutyCycle The duty cycle of one cycle of the PWM, from 0 to 100.
 * @note Only one Timer based PWM can be on at a time.
 */
void PWMUpdateConfig(struct PWMConfig pwmConfig, uint32_t period, uint32_t dutyCycle);

/**
 * PWMStop disables a PWM configuration.
 * 
 * @param pwmConfig The PWM that should be halted.
 */
void PWMStop(struct PWMConfig pwmConfig);
