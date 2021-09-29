/**
 * @file Timer.c
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief Timer peripheral driver.
 * @version 0.1
 * @date 2021-09-24
 * @copyright Copyright (c) 2021
 * @note
 * Unsupported Features. This driver does not support WTimers, multiple clock
 * modes, nor count up vs count down. B-side timers are currently broken.
 */

/** General Imports. */
#include <stdio.h>
#include <math.h>
#include <assert.h>

/** Device specific imports. */
#include <inc/RegDefs.h>
#include <lib/FaultHandler/FaultHandler.h>
#include <lib/Timer/Timer.h>


void WaitForInterrupt(void);    // Defined in startup.s

/**
 * @brief This field is either 0, 1, 2, 3, specifying the byte that the
 *        interrupt of priority X should be set to.
 */
enum InterruptPriorityIdx {INTA, INTB, INTC, INTD};

/** @brief TimerInterruptSettings is a set of timer configurations. */
static struct TimerInterruptSettings {
    /** @brief Priority index of the timer. */
    enum InterruptPriorityIdx priorityIdx;

    /** @brief Address of the relevant priority register. */
    volatile uint32_t * NVIC_PRI_ADDR;

    /** @brief Address of the relevant set enable register. */
    volatile uint32_t * NVIC_EN_ADDR;

    /** @brief Relevant IRQ number of the interrupt. */
    uint32_t IRQ;

    /** @brief User function associated with the interrupt. */
    void (*timerTask)(uint32_t *args);

    /** @brief User args associated with the interrupt function. */
    uint32_t * timerArgs;
} TimerInterruptSettings[TIMER_COUNT] = {
    {INTD, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI4_OFFSET),  (uint32_t *)(PERIPHERALS_BASE + NVIC_EN0_OFFSET), 19,    NULL,   NULL}, /* Timer 0A. */
    {INTA, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI5_OFFSET),  (uint32_t *)(PERIPHERALS_BASE + NVIC_EN0_OFFSET), 20,    NULL,   NULL}, /* Timer 0B. */
    {INTB, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI5_OFFSET),  (uint32_t *)(PERIPHERALS_BASE + NVIC_EN0_OFFSET), 21,    NULL,   NULL}, /* Timer 1A. */
    {INTC, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI5_OFFSET),  (uint32_t *)(PERIPHERALS_BASE + NVIC_EN0_OFFSET), 22,    NULL,   NULL}, /* Timer 1B. */
    {INTD, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI5_OFFSET),  (uint32_t *)(PERIPHERALS_BASE + NVIC_EN0_OFFSET), 23,    NULL,   NULL}, /* Timer 2A. */
    {INTA, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI6_OFFSET),  (uint32_t *)(PERIPHERALS_BASE + NVIC_EN0_OFFSET), 24,    NULL,   NULL}, /* Timer 2B. */
    {INTD, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI8_OFFSET),  (uint32_t *)(PERIPHERALS_BASE + NVIC_EN1_OFFSET), 35-32, NULL,   NULL}, /* Timer 3A. */
    {INTA, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI9_OFFSET),  (uint32_t *)(PERIPHERALS_BASE + NVIC_EN1_OFFSET), 36-32, NULL,   NULL}, /* Timer 3B. */
    {INTC, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI17_OFFSET), (uint32_t *)(PERIPHERALS_BASE + NVIC_EN2_OFFSET), 70-64, NULL,   NULL}, /* Timer 4A. */
    {INTD, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI17_OFFSET), (uint32_t *)(PERIPHERALS_BASE + NVIC_EN2_OFFSET), 71-64, NULL,   NULL}, /* Timer 4B. */
    {INTA, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI23_OFFSET), (uint32_t *)(PERIPHERALS_BASE + NVIC_EN2_OFFSET), 92-64, NULL,   NULL}, /* Timer 5A. */
    {INTB, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI23_OFFSET), (uint32_t *)(PERIPHERALS_BASE + NVIC_EN2_OFFSET), 93-64, NULL,   NULL}, /* Timer 5B. */

    // {}, /* WTimer 0A. */
    // {}, /* WTimer 0B. */
    // {}, /* WTimer 1A. */
    // {}, /* WTimer 1B. */
    // {}, /* WTimer 2A. */
    // {}, /* WTimer 2B. */
    // {}, /* WTimer 3A. */
    // {}, /* WTimer 3B. */
    // {}, /* WTimer 4A. */
    // {}, /* WTimer 4B. */
    // {}, /* WTimer 5A. */
    // {}, /* WTimer 5B. */

    {INTA, 0, 0, 0, NULL, NULL}, /* SYSTICK. */
};

Timer_t TimerInit(TimerConfig_t config) {
    /* Initialization asserts. */
    assert(config.timerID < TIMER_COUNT);
    assert(config.priority <= 7);
    assert(0 < config.period);

    Timer_t timer = {
        .timerID=config.timerID,
        .period=config.period
    };

    uint8_t ID = config.timerID;

    /* Assign the task to the appropriate handler. */
    TimerInterruptSettings[ID].timerTask = config.timerTask;
    TimerInterruptSettings[ID].timerArgs = config.timerArgs;

    /* Special case for SYSTICK. */
    if (ID == SYSTICK) {
        /* Disable during setup. */
        GET_REG(PERIPHERALS_BASE + SYSTICK_CTRL_OFFSET) = 0x00000000;

        /* Set reload value. */
        GET_REG(PERIPHERALS_BASE + SYSTICK_LOAD_OFFSET) = config.period - 1;

        /* Clear current value. */
        GET_REG(PERIPHERALS_BASE + SYSTICK_CURR_OFFSET) = 0x00000000;

        /* Set priority. */
        GET_REG(PERIPHERALS_BASE + SYS_PRI3_OFFSET) &= 0x00FFFFFF;
        GET_REG(PERIPHERALS_BASE + SYS_PRI3_OFFSET) |= config.priority << 29;

        /* Re-enable after setup. */
        GET_REG(PERIPHERALS_BASE + SYSTICK_CTRL_OFFSET) = 0x00000007;
        return timer;
    }

    /* We'll generate the timer offset to find the correct addresses for each
       timer. */
    uint32_t timerOffset = 0;

    /* Timers TIMER_0A to WTIMER_1B. */
    if (ID < WTIMER_2A) timerOffset = 0x1000 * (uint32_t)(ID >> 1);

    /* Timers WTIMER_2A to WTIMER_5B. Jump the base to 0x4004.C000. Our magic
       number, 16, is the enumerated value of WTIMER_2A. */
    else timerOffset = 0x1000 * (uint32_t)((ID-16) >> 1) + 0x0001C000;

    /* 1. Activate the timer and stall until ready. */
    if (ID <= TIMER_5B) { /* 16/32 bit normal timers. */
        GET_REG(SYSCTL_BASE + SYSCTL_RCGCTIMER_OFFSET) |= (0x01 << (uint32_t)(ID >> 1));
        while ((GET_REG(SYSCTL_BASE + SYSCTL_PRTIMER_OFFSET) &
            (0x01 << (uint32_t)(ID >> 1))) == 0) {}
    } else { /* 32/64 bit wide timers. */
        /* Our magic number 12, is the enumerated value of WTIMER_0A. */
        GET_REG(SYSCTL_BASE + SYSCTL_RCGCWTIMER_OFFSET) |= (0x01 << (uint32_t)((ID-12) >> 1));
        while ((GET_REG(SYSCTL_BASE + SYSCTL_PRWTIMER_OFFSET) &
            (0x01 << (uint32_t)((ID-12) >> 1))) == 0) {}
    }

    /* 2. Disable timer during setup. */
    GET_REG(GPTM_BASE + timerOffset + GPTMCTL_OFFSET) &=
        ((ID % 2) == 0) ? 0xFFFFFF00 : 0xFFFFFF00FF;

    /* 3. Configure for 32-bit mode. */
    GET_REG(GPTM_BASE + timerOffset + GPTMCFG_OFFSET) = 0x00000000;

    /* 4. Configure for periodic mode.
       5. Set reload value.
       6. Set prescaler to 1. */
    if ((ID % 2) == 0) { /* Timer A. */
        GET_REG(GPTM_BASE + timerOffset + GPTMTAMR_OFFSET)  =
            config.isPeriodic ? 0x00000002 : 0x00000001;
        GET_REG(GPTM_BASE + timerOffset + GPTMTAILR_OFFSET) = config.period - 1;
        GET_REG(GPTM_BASE + timerOffset + GPTMTAPR_OFFSET)  = 0x00000000;
    } else { /* Timer B. */
        GET_REG(GPTM_BASE + timerOffset + GPTMTBMR_OFFSET)  =
            config.isPeriodic ? 0x00000002 : 0x00000001;
        GET_REG(GPTM_BASE + timerOffset + GPTMTBILR_OFFSET) = config.period - 1;
        GET_REG(GPTM_BASE + timerOffset + GPTMTBPR_OFFSET)  = 0x00000000;
    }

    /* 7. Clear timer timeout flag. */
    GET_REG(GPTM_BASE + timerOffset + GPTMICR_OFFSET) |=
        ((ID % 2) == 0) ? TIMERXA_ICR_TATOCINT: TIMERXB_ICR_TATOCINT;

    /* 8. Arm timeout interrupt. */
    GET_REG(GPTM_BASE + timerOffset + GPTMIMR_OFFSET) |=
        ((ID % 2) == 0) ? 0x00000001 : 0x00000100;

    /* 9. Set timer interrupt priority. */
    uint32_t mask = 0xFFFFFFFF;
    uint32_t intVal = config.priority << 5;

    /* Our magic number is 8 since to shift a hex value one hex position, we do
       4 binary shifts. To do it twice; 8 binary shifts. Our output should
       something like this for priority 2: 0xFF00FFFF, 0x00400000. */
    mask &= ~(0xFF << (TimerInterruptSettings[ID].priorityIdx * 8));
    intVal = intVal << (TimerInterruptSettings[ID].priorityIdx * 8);
    (*TimerInterruptSettings[ID].NVIC_PRI_ADDR) =
        ((*TimerInterruptSettings[ID].NVIC_PRI_ADDR)&mask)|intVal;

    /* 10. Enable IRQ X in NVIC. */
    (*TimerInterruptSettings[ID].NVIC_EN_ADDR) = 1 << TimerInterruptSettings[ID].IRQ;

    /* 11. Enable timer after setup. */
    GET_REG(GPTM_BASE + timerOffset + GPTMCTL_OFFSET) |=
        ((ID % 2) == 0) ? 0x00000001 : 0x00000100;

    return timer;
}

void TimerUpdatePeriod(Timer_t timer) {
    /* Initialization asserts. */
    assert(timer.timerID < TIMER_COUNT);
    assert(0 < timer.period);

    uint8_t ID = timer.timerID;

    /* Special case for SYSTICK. */
    if (ID == SYSTICK) {
        /* Set reload value. */
        GET_REG(PERIPHERALS_BASE + SYSTICK_LOAD_OFFSET) = timer.period - 1;
        return;
    }

    /* 1. We'll generate the timer offset to find the correct addresses for each
       timer. */
    uint32_t timerOffset = 0;

    /* Timers TIMER_0A to WTIMER_1B. */
    if (ID < WTIMER_2A) timerOffset = 0x1000 * (uint32_t)(ID >> 1);
    /* Timers WTIMER_2A to WTIMER_5B. Jump the base to 0x4004.C000. Our magic
       number, 16, is the enumerated value of WTIMER_2A. */
    else timerOffset = 0x1000 * (uint32_t)((ID - 16) >> 1) + 0x0001C000;

    /* 2. Update the period. */
    GET_REG(GPTM_BASE + timerOffset + GPTMTAILR_OFFSET) = timer.period - 1;

    /* 3. Enable timer after setup. */
    GET_REG(GPTM_BASE + timerOffset + GPTMCTL_OFFSET) |=
        ((ID % 2) == 0) ? 0x00000001 : 0x00000100;
}

void TimerStop(Timer_t timer) {
    /* Initialization asserts. */
    assert(timer.timerID < TIMER_COUNT);

    uint8_t ID = timer.timerID;

    /* 1. We'll generate the timer offset to find the correct addresses for each
       timer. */
    uint32_t timerOffset = 0;
    /* Timers TIMER_0A to WTIMER_1B. */
    if (ID < WTIMER_2A) timerOffset = 0x1000 * (uint32_t)(ID >> 1);
    /* Timers WTIMER_2A to WTIMER_5B. Jump the base to 0x4004.C000. Our magic
       number, 16, is the enumerated value of WTIMER_2A. */
    else timerOffset = 0x1000 * (uint32_t)((ID-16) >> 1) + 0x0001C000;

    /* 2. Disable timer during setup. */
    GET_REG(GPTM_BASE + timerOffset + GPTMCTL_OFFSET) &=
        ((ID % 2) == 0) ? 0xFFFFFF00 : 0xFFFFFF00FF;
}

void TimerStart(Timer_t timer) {
    /* Initialization asserts. */
    assert(timer.timerID < TIMER_COUNT);

    uint8_t ID = timer.timerID;

    /* 1. We'll generate the timer offset to find the correct addresses for each
       timer. */
    uint32_t timerOffset = 0;
    /* Timers TIMER_0A to WTIMER_1B. */
    if (ID < WTIMER_2A) timerOffset = 0x1000 * (uint32_t)(ID >> 1);
    /* Timers WTIMER_2A to WTIMER_5B. Jump the base to 0x4004.C000. Our magic
       number, 16, is the enumerated value of WTIMER_2A. */
    else timerOffset = 0x1000 * (uint32_t)((ID-16) >> 1) + 0x0001C000;

    /* 2. Enable timer during setup. */
    GET_REG(GPTM_BASE + timerOffset + GPTMCTL_OFFSET) |=
        ((ID % 2) == 0) ? 0x00000001 : 0x00000100;
}

uint32_t freqToPeriod(uint32_t freq, uint32_t maxFreq) {
    return (uint32_t) ceil(maxFreq/freq);
}


/** Handler implementations for wide timers. */
void Timer0A_Handler(void) {
    GET_REG(GPTM_BASE + GPTMICR_OFFSET) |= TIMERXA_ICR_TATOCINT;
    if (TimerInterruptSettings[0].timerTask != NULL) {
        TimerInterruptSettings[0].timerTask(TimerInterruptSettings[0].timerArgs);
    }
}
void Timer0B_Handler(void) {
    GET_REG(GPTM_BASE + GPTMICR_OFFSET) |= TIMERXB_ICR_TATOCINT;
    if (TimerInterruptSettings[1].timerTask != NULL) {
        TimerInterruptSettings[1].timerTask(TimerInterruptSettings[1].timerArgs);
    }
}
void Timer1A_Handler(void) {
    GET_REG(GPTM_BASE + 0x1000 + GPTMICR_OFFSET) |= TIMERXA_ICR_TATOCINT;
    if (TimerInterruptSettings[2].timerTask != NULL) {
        TimerInterruptSettings[2].timerTask(TimerInterruptSettings[2].timerArgs);
    }
}
void Timer1B_Handler(void) {
    GET_REG(GPTM_BASE + 0x1000 + GPTMICR_OFFSET) |= TIMERXB_ICR_TATOCINT;
    if (TimerInterruptSettings[3].timerTask != NULL) {
        TimerInterruptSettings[3].timerTask(TimerInterruptSettings[3].timerArgs);
    }
}
void Timer2A_Handler(void) {
    GET_REG(GPTM_BASE + 0x2000 + GPTMICR_OFFSET) |= TIMERXA_ICR_TATOCINT;
    if (TimerInterruptSettings[4].timerTask != NULL) {
        TimerInterruptSettings[4].timerTask(TimerInterruptSettings[4].timerArgs);
    }
}
void Timer2B_Handler(void) {
    GET_REG(GPTM_BASE + 0x2000 + GPTMICR_OFFSET) |= TIMERXB_ICR_TATOCINT;
    if (TimerInterruptSettings[5].timerTask != NULL) {
        TimerInterruptSettings[5].timerTask(TimerInterruptSettings[5].timerArgs);
    }
}
void Timer3A_Handler(void) {
    GET_REG(GPTM_BASE + 0x3000 + GPTMICR_OFFSET) |= TIMERXA_ICR_TATOCINT;
    if (TimerInterruptSettings[6].timerTask != NULL) {
        TimerInterruptSettings[6].timerTask(TimerInterruptSettings[6].timerArgs);
    }
}
void Timer3B_Handler(void) {
    GET_REG(GPTM_BASE + 0x3000 + GPTMICR_OFFSET) |= TIMERXB_ICR_TATOCINT;
    if (TimerInterruptSettings[7].timerTask != NULL) {
        TimerInterruptSettings[7].timerTask(TimerInterruptSettings[7].timerArgs);
    }
}
void Timer4A_Handler(void) {
    GET_REG(GPTM_BASE + 0x4000 + GPTMICR_OFFSET) |= TIMERXA_ICR_TATOCINT;
    if (TimerInterruptSettings[8].timerTask != NULL) {
        TimerInterruptSettings[8].timerTask(TimerInterruptSettings[8].timerArgs);
    }
}
void Timer4B_Handler(void) {
    GET_REG(GPTM_BASE + 0x4000 + GPTMICR_OFFSET) |= TIMERXB_ICR_TATOCINT;
    if (TimerInterruptSettings[9].timerTask != NULL) {
        TimerInterruptSettings[9].timerTask(TimerInterruptSettings[9].timerArgs);
    }
}
void Timer5A_Handler(void) {
    GET_REG(GPTM_BASE + 0x5000 + GPTMICR_OFFSET) |= TIMERXA_ICR_TATOCINT;
    if (TimerInterruptSettings[10].timerTask != NULL) {
        TimerInterruptSettings[10].timerTask(TimerInterruptSettings[10].timerArgs);
    }
}
void Timer5B_Handler(void) {
    GET_REG(GPTM_BASE + 0x5000 + GPTMICR_OFFSET) |= TIMERXB_ICR_TATOCINT;
    if (TimerInterruptSettings[11].timerTask != NULL) {
        TimerInterruptSettings[11].timerTask(TimerInterruptSettings[11].timerArgs);
    }
}

/** Handler implementations for wide timers. */
void WideTimer0A_Handler(void) {}
void WideTimer0B_Handler(void) {}
void WideTimer1A_Handler(void) {}
void WideTimer1B_Handler(void) {}
void WideTimer2A_Handler(void) {}
void WideTimer2B_Handler(void) {}
void WideTimer3A_Handler(void) {}
void WideTimer3B_Handler(void) {}
void WideTimer4A_Handler(void) {}
void WideTimer4B_Handler(void) {}
void WideTimer5A_Handler(void) {}
void WideTimer5B_Handler(void) {}

/** @brief System clock ticks since program start. Used for delay functions. */
static uint64_t systick = 0;

void SysTick_Handler(void) {
    /* Note: This will roll over at 0xFFFFFFFFFFFFFFFF. Be warned. */
    ++systick;

    if (TimerInterruptSettings[24].timerTask != NULL) {
        TimerInterruptSettings[24].timerTask(TimerInterruptSettings[24].timerArgs);
    }
}

uint64_t SysTickGetTick(void) {
    return systick;
}

void DelayInit(void) {
    TimerConfig_t config = {
        .timerID=SYSTICK,
        .period=freqToPeriod(MAX_FREQ/80, MAX_FREQ),
        .timerTask=NULL,
        .isPeriodic=true,
        .priority=1,
        .timerArgs=NULL
    };
    TimerInit(config);
}

void DelayMillisec(uint32_t n) {
    uint64_t tick = systick;

    /* 1_000 ticks is 1 ms. Tuned to 1 MHz SysTick. */
    while (systick - tick < 1000 * n) { WaitForInterrupt(); }
}

void DelayMicrosec(uint32_t n) {
    uint64_t tick = systick;

    /* 1 tick is 1 us. Tuned to 1 MHz SysTick. */
    while (systick - tick < n) { WaitForInterrupt(); }
}
