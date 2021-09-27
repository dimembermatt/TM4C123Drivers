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

/** General imports. */
#include <assert.h>

/** Device Specific imports. */
#include <lib/PWM/PWM.h>
#include <inc/RegDefs.h>


/**
 * @brief PWMTimerHandler is the generic handler passed to the Timer interrupt
 *        functions. It toggles the state of the PWM pin based on a duty cycle.
 *
 * @param args A pointer to a list of arguments. In this function, args[0] should
 *             be the GPIOPin_t selected as the PWMPin and args[1] should be the
 *             duty cycle, from [0, 100].
 */
void PWMTimerHandler(uint32_t * args) {
    static uint8_t idx = 0;
    static bool on = false;
    

    /* i.e. 0 - 69: ON; 70 - 99: OFF for a 70% duty cycle. */
    if (on != idx <= (uint8_t) args[1])
        GPIOSetBit((GPIOPin_t) args[0], on);

    on = idx <= (uint8_t) args[1];
    idx = (idx + 1) % 100;
}

/** @brief pwmSettings is a set of PWM configurations. */
static struct PWMSettings {
    /** @brief Pin associated with the PWM enum. */
    GPIOPin_t pin;

    /** @brief PWM generator associated with the PWM enum. */
    uint8_t generator;
} pwmSettings[PWM_COUNT] = {
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

/** @brief pwmTimerSettings hold the args for the timer interrupt handler. Index
 *         0 holds the GPIOPin_t and index 1 holds the duty cycle. */
static uint32_t pwmTimerSettings[TIMER_COUNT][2];

PWM_t PWMInit(PWMConfig_t config) {
    /* Initialization asserts. */
    assert(config.source <= PWM_SOURCE_TIMER);
    assert(0 < config.period);
    assert(config.dutyCycle <= 100);

    PWM_t pwm = {
        .source=config.source,
    };
    
    if (config.source == PWM_SOURCE_DEFAULT) {
        assert(config.period <= 0xFFFF);
        assert(config.sourceInfo.pin < PWM_COUNT);
        PWMPin_t pwmPin = config.sourceInfo.pin;
        pwm.sourceInfo.pin=pwmPin;

        /* 1. Enable PWM clock and stall until ready. */
        GET_REG(SYSCTL_BASE + SYSCTL_RCGCPWM_OFFSET) =
            0x1 * (pwmPin <= M0_PD1) | 0x2 * (pwmPin > M0_PD1);
        while ((GET_REG(SYSCTL_BASE + SYSCTL_PRPWM_OFFSET) &
            (0x1 * (pwmPin <= M0_PD1) | 0x2 * (pwmPin > M0_PD1))) == 0) {};

        /* 2. Enable appropriate GPIO. */
        GPIOConfig_t gpioConfig = {
            .pin=pwmSettings[pwmPin].pin,
            .pull=GPIO_TRI_STATE,
            .isOutput=true,
            .alternateFunction=4 * (pwmPin <= M0_PD1) +
                               5 * (pwmPin > M0_PD1),
            .isAnalog=false,
            .drive=GPIO_DRIVE_2MA,
            .enableSlew=false
        };
        GPIOInit(gpioConfig);

        /* 3. Cut the clock to TODO: this */
        GET_REG(SYSCTL_BASE + SYSCTL_RCC_OFFSET) &= ~0x00100000;

        /* 4. Select the PWM base based on the PWM pin. */
        uint32_t PWMBase =
            PWM0_BASE * (pwmPin <= M0_PD1) +
            PWM1_BASE * (pwmPin > M0_PD1);

        uint32_t generatorOffset = (pwmSettings[pwmPin].generator >> 1);

        /* 5. Shut down the PWM temporarily. */
        GET_REG(PWMBase + PWM_CTL_OFFSET + PWM_OFFSET * generatorOffset) = 0x0;

        /* 6. Set the period. PWM clock source is SYSCLK. */
        GET_REG(PWMBase + PWM_LOAD_OFFSET + PWM_OFFSET * generatorOffset) = config.period-1;

        /* 7. Configure PWM for countdown mode. and set their pulse width.
              By default, both PWMA and PWMB signal generation are initialized
              to count down. */
        if (pwmSettings[pwmPin].generator % 2 == 0) {
            GET_REG(PWMBase + PWM_GENA_OFFSET + PWM_OFFSET * generatorOffset) = 0x000000C8; /* Drive high at cmpA, drive low at LOAD. */
            GET_REG(PWMBase + PWM_CMPA_OFFSET + PWM_OFFSET * generatorOffset) =
                (config.period * config.dutyCycle / 100) - 1;
        } else {
            GET_REG(PWMBase + PWM_GENB_OFFSET + PWM_OFFSET * generatorOffset) = 0x00000C08; /* Drive high at cmpB, drive low at LOAD. */
            GET_REG(PWMBase + PWM_CMPB_OFFSET + PWM_OFFSET * generatorOffset) =
                (config.period * config.dutyCycle / 100) - 1;
        }

        /* 8. Start the timers. */
        GET_REG(PWMBase + PWM_CTL_OFFSET + PWM_OFFSET * generatorOffset) = 0x1;

        /* 9. Enable the PWM outputs. */
        GET_REG(PWMBase + PWM_ENABLE_OFFSET) |= 1 << pwmSettings[pwmPin].generator;
    } else {
        assert(config.sourceInfo.timerSelect.timerID < TIMER_COUNT);
        assert(config.sourceInfo.timerSelect.pin < PIN_COUNT);

        /* Timer based PWM. */
        GPIOPin_t gpioPin = config.sourceInfo.timerSelect.pin;
        TimerID_t timerID = config.sourceInfo.timerSelect.timerID;
        pwmTimerSettings[timerID][0] = gpioPin;
        pwmTimerSettings[timerID][1] = config.dutyCycle;

        GPIOConfig_t pinConfig = {
            .pin=gpioPin,
            .pull=GPIO_TRI_STATE,
            .isOutput=true,
            .alternateFunction=0,
            .isAnalog=false,
            .drive=GPIO_DRIVE_2MA,
            .enableSlew=false
        };
        GPIOInit(pinConfig);

        TimerConfig_t timerConfig = {
            .timerID=timerID,
            .period=config.period/100, /* Execute handler at 100 x the frequency. */
            .timerTask=PWMTimerHandler,
            .isPeriodic=true,
            .priority=3,
            .timerArgs=pwmTimerSettings[timerID]
        };
        pwm.sourceInfo.timerInfo.timer = TimerInit(timerConfig);
    }

    return pwm;
}

void PWMUpdateConfig(PWM_t pwm, uint32_t period, uint8_t dutyCycle) {
    /* Initialization asserts. */
    assert(0 < period);
    assert(dutyCycle <= 100);
    assert(pwm.source <= PWM_SOURCE_TIMER);
    if (pwm.source == PWM_SOURCE_DEFAULT) {
        assert(period <= 0xFFFF);
        assert(pwm.sourceInfo.pin < PWM_COUNT);

        PWMPin_t pin = pwm.sourceInfo.pin;

        /* 0. Select the PWM base based on the PWM pin. */
        uint32_t PWMBase =
            PWM0_BASE * (pin <= M0_PD1) +
            PWM1_BASE * (pin > M0_PD1);

        uint32_t generatorOffset = (pwmSettings[pin].generator >> 1);

        /* 1. Disable the timers. */
        GET_REG(PWMBase + PWM_CTL_OFFSET + PWM_OFFSET * generatorOffset) = 0;

        /* 2. Set the period. PWM clock source is SYSCLK. */
        GET_REG(PWMBase + PWM_LOAD_OFFSET + PWM_OFFSET * generatorOffset) = period-1;

        /* 3. Set the pulse width. The offset is dependent on which comparator is
           selected. Each module can have two comparators running. */
        if (pwmSettings[pin].generator % 2 == 0) {
            GET_REG(PWMBase + PWM_CMPA_OFFSET + PWM_OFFSET * generatorOffset) =
                (period*dutyCycle/100)-1;
        } else {
            GET_REG(PWMBase + PWM_CMPB_OFFSET + PWM_OFFSET * generatorOffset) =
                (period*dutyCycle/100)-1;
        }

        /* 4. Start the timers. */
        GET_REG(PWMBase + PWM_CTL_OFFSET + PWM_OFFSET * generatorOffset) = 1;
    } else {        
        assert(pwm.sourceInfo.timerInfo.timer.timerID < TIMER_COUNT);
        assert(pwm.sourceInfo.timerInfo.timer.period > 0);
        assert(pwm.sourceInfo.timerInfo.pin < PIN_COUNT);

        /* Timer based PWM. */
        pwmTimerSettings[pwm.sourceInfo.timerInfo.timer.timerID][1] = dutyCycle;
        pwm.sourceInfo.timerInfo.timer.period = period/100;
        TimerUpdatePeriod(pwm.sourceInfo.timerInfo.timer);
    }
}

void PWMStop(PWM_t pwm) {
    /* Initialization asserts. */
    assert(pwm.source <= PWM_SOURCE_TIMER);
    if (pwm.source == PWM_SOURCE_DEFAULT) {
        assert(pwm.sourceInfo.pin < PWM_COUNT);

        PWMPin_t pin = pwm.sourceInfo.pin;

        /* 0. Select the PWM base based on the PWM pin. */
        uint32_t PWMBase =
            PWM0_BASE * (pin <= M0_PD1) +
            PWM1_BASE * (pin > M0_PD1);

        uint32_t generatorOffset = (pwmSettings[pin].generator >> 1);

        /* 1. Disable the timers. */
        GET_REG(PWMBase + PWM_CTL_OFFSET + PWM_OFFSET * generatorOffset) &= ~0x1;
    } else {
        assert(pwm.sourceInfo.timerInfo.timer.timerID < TIMER_COUNT);
        assert(pwm.sourceInfo.timerInfo.timer.period > 0);
        assert(pwm.sourceInfo.timerInfo.pin < PIN_COUNT);

        /* Timer based PWM. */
        TimerStop(pwm.sourceInfo.timerInfo.timer);
    }
}

void PWMStart(PWM_t pwm) {
    /* Initialization asserts. */
    assert(pwm.source <= PWM_SOURCE_TIMER);
    if (pwm.source == PWM_SOURCE_DEFAULT) {
        assert(pwm.sourceInfo.pin < PWM_COUNT);
        
        PWMPin_t pin = pwm.sourceInfo.pin;

        /* 0. Select the PWM base based on the PWM pin. */
        uint32_t PWMBase =
            PWM0_BASE * (pin <= M0_PD1) +
            PWM1_BASE * (pin > M0_PD1);

        uint32_t generatorOffset = (pwmSettings[pin].generator >> 1);

        /* 1. Enable the timers. */
        GET_REG(PWMBase + PWM_CTL_OFFSET + PWM_OFFSET * generatorOffset) |= 0x1;
    } else {
        assert(pwm.sourceInfo.timerInfo.timer.timerID < TIMER_COUNT);
        assert(pwm.sourceInfo.timerInfo.timer.period > 0);
        assert(pwm.sourceInfo.timerInfo.pin < PIN_COUNT);
        
        /* Timer based PWM. */
        TimerStart(pwm.sourceInfo.timerInfo.timer);    
    }
}
