/**
 * PWM.c
 * Devices: LM4F120; TM4C123
 * Description: Middle level drivers for generating square waves.
 * Authors: Matthew Yu.
 * Last Modified: 04/06/21
 * 
 * This driver will support both the TM4C's existing PWM modules as well as a
 * GPIO + Timer configuration. 
 */

/** Device Specific imports. */
#include <lib/PWM/PWM.h>
#include <inc/RegDefs.h>


struct PWMTimerSettings {
	/* Duty cycle of the PWM. */
	uint8_t dutyCycle;

	/* Pin output of duty cycle. */
	pin_t pin;
};

static struct PWMTimerSettings PWMTimerSetting;

void PWMTimerHandler(void) {
	static uint8_t idx = 0;

	/* i.e. 0 - 69: ON; 70 - 99: OFF for a 70% duty cycle. */
	GPIOSetBit(PWMTimerSetting.pin, idx < PWMTimerSetting.dutyCycle); 
	idx = (idx + 1) % 100;
}

static pin_t pinMap[PWM_COUNT] = {
	PIN_B6, PIN_B7, PIN_B4, PIN_B5, PIN_E4, PIN_E5, PIN_C4, PIN_D0, PIN_C5, PIN_D1,
	PIN_D0, PIN_D1, PIN_A6, PIN_E4, PIN_A7, PIN_E5, PIN_F0, PIN_F1, PIN_F2, PIN_F3
};

/**
 * PWMInit initializes a PWM configuration with a given frequency and duty
 * cycle.
 * 
 * @param pwmConfig The PWM configuration that should be started.
 * @param frequency The frequency of one cycle of the PWM.
 * @param dutyCycle The duty cycle of one cycle of the PWM.
 */
void PWMInit(struct PWMConfig pwmConfig, uint32_t frequency, uint32_t dutyCycle) {
	if (pwmConfig.source == DEFAULT) {
		/* In-built TM4C PWM. */
		if (pwmConfig.config.configDefault == PWM_COUNT) return;

		/* 1. Enable PWM clock. */
		GET_REG(SYSCTL_BASE + SYSCTL_RCGCPWM_OFFSET) =
			0x1 * (pwmConfig.config.configDefault <= M0_PD1) |
			0x2 * (pwmConfig.config.configDefault > M0_PD1 );

		/* 2. Enable appropriate GPIO. */
		GPIOConfig_t gpioConfig = {
			pinMap[pwmConfig.config.configDefault],
			NONE,
			true,
			true,
			4 * (pwmConfig.config.configDefault <= M0_PD1) + 
			5 * (pwmConfig.config.configDefault > M0_PD1),
			false
		};
		GPIOInit(gpioConfig);

		/* 4. Configure RCC to set the PWM divider to divider by 2. */

		/* 5. Configure PWM for countdown mode. */

		/* 6. Set the period. PWM clock source is MAX_FREQ/2. */

		/* 7. Set the pulse width. */

		/* 8. Start the timers. */

		/* 9. Enable the PWM outputs. */

	} else {
		/* Timer based PWM. */
		PWMTimerSettings.dutyCycle = dutyCycle;
		PWMTimerSettings.pin = pwmConfig.config.configTimer;

		TimerConfig_t timerConfig = {
			pwmConfig.timerID,
			freqToPeriod(frequency * 100, MAX_FREQ),
			true,
			3,
			PWMTimerHandler
		};
		TimerInit(timerConfig);
	}
}

/**
 * PWMStop disables a PWM configuration.
 * 
 * @param pwmConfig The PWM that should be halted.
 */
void PWMStop(struct PWMConfig pwmConfig) {
	if (pwmConfig.source == DEFAULT) {
		/* In-built TM4C PWM. */

	} else {
		/* Timer based PWM. */
		/* TODO: add TimerStop support in Timer.h. */
	}
}