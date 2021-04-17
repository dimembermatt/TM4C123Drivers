/**
 * PWM.c
 * Devices: LM4F120; TM4C123
 * Description: Middle level drivers for generating square waves.
 * Authors: Matthew Yu.
 * Last Modified: 04/17/21
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

struct PWMSettings {
    /** Pin associated with the PWM enum. */
    pin_t pin;

    /** PWM generator associated with the PWM enum. */
    uint8_t generator;
};

static struct PWMSettings pwmSettings[PWM_COUNT] = {
    {PIN_B6, 0},
    {PIN_B7, 1},
    {PIN_B4, 2},
    {PIN_B5, 3},
    {PIN_E4, 4},
    {PIN_E5, 5},
    {PIN_C4, 6},
    {PIN_D0, 6},
    {PIN_C5, 7},
    {PIN_D1, 7},

    {PIN_D0, 0},
    {PIN_D1, 1},
    {PIN_A6, 2},
    {PIN_E4, 2},
    {PIN_A7, 3},
    {PIN_E5, 3},
    {PIN_F0, 4},
    {PIN_F1, 5},
    {PIN_F2, 6},
    {PIN_F3, 7},
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
void PWMInit(struct PWMConfig pwmConfig, uint32_t period, uint32_t dutyCycle) {
    // TODO: #4 debug this until it is in a working state.
    if (pwmConfig.source == DEFAULT) {
        PWMPin_t pwmPin = pwmConfig.config.pwmSelect.pwmPin;

        /* In-built TM4C PWM. */
        if (pwmPin == PWM_COUNT) return;

        /* 1. Enable PWM clock and stall until ready. */
        GET_REG(SYSCTL_BASE + SYSCTL_RCGCPWM_OFFSET) =
            0x1 * (pwmPin <= M0_PD1) | 0x2 * (pwmPin > M0_PD1);
        while ((GET_REG(SYSCTL_BASE + SYSCTL_PRPWM_OFFSET) & 
            (0x1 * (pwmPin <= M0_PD1) | 0x2 * (pwmPin > M0_PD1))) == 0) {};

        /* 2. Enable appropriate GPIO. */
        GPIOConfig_t gpioConfig = {
            pwmSettings[pwmPin].pin,
            NONE,
            true,
            true,
            4 * (pwmPin <= M0_PD1) + 
            5 * (pwmPin > M0_PD1),
            false
        };
        GPIOInit(gpioConfig);

        /* 3. We don't perform this step, but you could modify RCC_R to base the
           PWM clock on the PWM clock divider, and adjust its divisor. */
        GET_REG(SYSCTL_BASE + SYSCTL_RCC_OFFSET) &= ~0x00100000;

        /* 4. Select the PWM base based on the PWM pin. */
        uint32_t PWMBase = 
            PWM0_BASE * (pwmPin <= M0_PD1) + 
            PWM1_BASE * (pwmPin > M0_PD1);

        uint32_t generatorOffset = (pwmSettings[pwmPin].generator >> 1) + 1;

        /* 5. Configure PWM for countdown mode. By default, both PWMA and PWMB
           signal generation are initialized to count down. */
        uint32_t address = PWMBase + PWM_CTL_OFFSET * generatorOffset;
        GET_REG(PWMBase + PWM_CTL_OFFSET * generatorOffset) = 0;
        GET_REG(PWMBase + PWM_GENA_OFFSET * generatorOffset) = 0x0000008C;
        GET_REG(PWMBase + PWM_GENB_OFFSET * generatorOffset) = 0x0000080C;

        /* 6. Set the period. PWM clock source is SYSCLK. */
        GET_REG(PWMBase + PWM_LOAD_OFFSET * generatorOffset) = period-1;
            
        /* 7. Set the pulse width. The offset is dependent on which comparator is
           selected. Each module can have two comparators running. */
        GET_REG(PWMBase +
            (PWM_CMPA_OFFSET * (pwmConfig.config.pwmSelect.comparator == PWMA) +
             PWM_CMPB_OFFSET * (pwmConfig.config.pwmSelect.comparator == PWMB)) *
                generatorOffset) = (period*dutyCycle/100)-1;

        /* 8. Start the timers. */
        GET_REG(PWMBase + PWM_CTL_OFFSET * generatorOffset) = 1;

        /* 9. Enable the PWM outputs. */
        GET_REG(PWMBase + PWM_ENABLE_OFFSET) |= 1 << pwmSettings[pwmPin].generator;

    } else {
        /* Timer based PWM. */
        PWMTimerSetting.dutyCycle = dutyCycle;
        PWMTimerSetting.pin = pwmConfig.config.timerSelect.pin;

        GPIOConfig_t pinConfig = {
            PWMTimerSetting.pin, 
            NONE,
            true, 
            false, 
            0,
            false
        };
        GPIOInit(pinConfig);

        TimerConfig_t timerConfig = {
            pwmConfig.config.timerSelect.timerID,
            period/100, /* Execute handler at 100 x the frequency. */
            true,
            3,
            PWMTimerHandler
        };
        TimerInit(timerConfig);
    }
}

/**
 * PWMUpdateConfig updates the PWM period and duty cycle. 
 * Does not check if the PWM was previously initialized.
 * 
 * @param pwmConfig The PWM configuration that should be started.
 * @param period The period of one cycle of the PWM.
 * @param dutyCycle The duty cycle of one cycle of the PWM, from 0 to 100.
 * @note Only one Timer based PWM can be on at a time.
 */
void PWMUpdateConfig(struct PWMConfig pwmConfig, uint32_t period, uint32_t dutyCycle) {
    if (pwmConfig.source == DEFAULT) {
        PWMPin_t pwmPin = pwmConfig.config.pwmSelect.pwmPin;

        /* In-built TM4C PWM. */
        if (pwmPin == PWM_COUNT) return;

        /* 0. Select the PWM base based on the PWM pin. */
        uint32_t PWMBase = 
            PWM0_BASE * (pwmPin <= M0_PD1) + 
            PWM1_BASE * (pwmPin > M0_PD1);

        uint32_t generatorOffset = (pwmSettings[pwmPin].generator >> 1) + 1;

        /* 1. Disable the timers. */
        GET_REG(PWMBase + PWM_CTL_OFFSET * generatorOffset) = 0;

        /* 2. Set the period. PWM clock source is SYSCLK. */
        GET_REG(PWMBase + PWM_LOAD_OFFSET * generatorOffset) = period-1;
            
        /* 3. Set the pulse width. The offset is dependent on which comparator is
           selected. Each module can have two comparators running. */
        GET_REG(PWMBase +
            (PWM_CMPA_OFFSET * (pwmConfig.config.pwmSelect.comparator == PWMA) +
             PWM_CMPB_OFFSET * (pwmConfig.config.pwmSelect.comparator == PWMB)) *
                generatorOffset) = (period*dutyCycle/100)-1;

        /* 4. Start the timers. */
        GET_REG(PWMBase + PWM_CTL_OFFSET * generatorOffset) = 1;
    } else {
        /* Timer based PWM. */
        PWMTimerSetting.dutyCycle = dutyCycle;
        TimerUpdatePeriod(pwmConfig.config.timerSelect.timerID, period/100);
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
        PWMPin_t pwmPin = pwmConfig.config.pwmSelect.pwmPin;

        /* In-built TM4C PWM. */
        if (pwmPin == PWM_COUNT) return;

        /* 0. Select the PWM base based on the PWM pin. */
        uint32_t PWMBase = 
            PWM0_BASE * (pwmPin <= M0_PD1) + 
            PWM1_BASE * (pwmPin > M0_PD1);

        uint32_t generatorOffset = (pwmSettings[pwmPin].generator >> 1) + 1;

        /* 1. Disable the timers. */
        GET_REG(PWMBase + PWM_CTL_OFFSET * generatorOffset) = 0;

    } else {
        /* Timer based PWM. */
        TimerStop(pwmConfig.config.timerSelect.timerID);
    }
}
