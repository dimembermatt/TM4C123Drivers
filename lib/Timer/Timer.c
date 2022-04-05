/**
 * @file Timer.c
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief Timer peripheral driver.
 * @version 0.1
 * @date 2021-10-28
 * @copyright Copyright (c) 2021
 * @note
 * Unsupported Features. This driver does not support multiple clock
 * modes, nor count up vs count down.
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

    {INTC, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI23_OFFSET), (uint32_t *)(PERIPHERALS_BASE + NVIC_EN2_OFFSET), 94-64, NULL,   NULL}, /* WTimer 0A. */
    {INTD, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI23_OFFSET), (uint32_t *)(PERIPHERALS_BASE + NVIC_EN2_OFFSET), 95-64, NULL,   NULL}, /* WTimer 0B. */
    {INTA, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI24_OFFSET), (uint32_t *)(PERIPHERALS_BASE + NVIC_EN3_OFFSET), 96-96, NULL,   NULL}, /* WTimer 1A. */
    {INTB, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI24_OFFSET), (uint32_t *)(PERIPHERALS_BASE + NVIC_EN3_OFFSET), 97-96, NULL,   NULL}, /* WTimer 1B. */
    {INTC, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI24_OFFSET), (uint32_t *)(PERIPHERALS_BASE + NVIC_EN3_OFFSET), 98-96, NULL,   NULL}, /* WTimer 2A. */
    {INTD, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI24_OFFSET), (uint32_t *)(PERIPHERALS_BASE + NVIC_EN3_OFFSET), 99-96, NULL,   NULL}, /* WTimer 2B. */
    {INTA, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI25_OFFSET), (uint32_t *)(PERIPHERALS_BASE + NVIC_EN3_OFFSET), 100-96, NULL,  NULL}, /* WTimer 3A. */
    {INTB, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI25_OFFSET), (uint32_t *)(PERIPHERALS_BASE + NVIC_EN3_OFFSET), 101-96, NULL,  NULL}, /* WTimer 3B. */
    {INTC, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI25_OFFSET), (uint32_t *)(PERIPHERALS_BASE + NVIC_EN3_OFFSET), 102-96, NULL,  NULL}, /* WTimer 4A. */
    {INTD, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI25_OFFSET), (uint32_t *)(PERIPHERALS_BASE + NVIC_EN3_OFFSET), 103-96, NULL,  NULL}, /* WTimer 4B. */
    {INTA, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI26_OFFSET), (uint32_t *)(PERIPHERALS_BASE + NVIC_EN3_OFFSET), 104-96, NULL,  NULL}, /* WTimer 5A. */
    {INTB, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI26_OFFSET), (uint32_t *)(PERIPHERALS_BASE + NVIC_EN3_OFFSET), 105-96, NULL,  NULL}, /* WTimer 5B. */

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

    /* 3. Configure for individual or concatenated mode. */
    GET_REG(GPTM_BASE + timerOffset + GPTMCFG_OFFSET) = config.isIndividual ? 0x4 : 0x0;

    /* 4. Configure for periodic mode.
       5. Set reload value.
       6. Set prescaler. */
    if ((ID % 2) == 0) { /* Timer A. */
        GET_REG(GPTM_BASE + timerOffset + GPTMTAMR_OFFSET)  =
            config.isPeriodic ? 0x00000002 : 0x00000001;
        GET_REG(GPTM_BASE + timerOffset + GPTMTAILR_OFFSET) = config.period - 1;
        GET_REG(GPTM_BASE + timerOffset + GPTMTAPR_OFFSET)  = config.prescale;
    } else { /* Timer B. */
        GET_REG(GPTM_BASE + timerOffset + GPTMTBMR_OFFSET)  =
            config.isPeriodic ? 0x00000002 : 0x00000001;
        GET_REG(GPTM_BASE + timerOffset + GPTMTBILR_OFFSET) = config.period - 1;
        GET_REG(GPTM_BASE + timerOffset + GPTMTBPR_OFFSET)  = config.prescale;
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

    return timer;
}

void TimerStart(Timer_t timer) {
    /* Initialization asserts. */
    assert(timer.timerID < TIMER_COUNT);

    uint8_t ID = timer.timerID;

    /* Special case for SYSTICK. */
    if (ID == SYSTICK) {
        /* Enable timer. */
        GET_REG(PERIPHERALS_BASE + SYSTICK_CTRL_OFFSET) = 0x00000007;
        return;
    }

    /* 1. We'll generate the timer offset to find the correct addresses for each
       timer. */
    uint32_t timerOffset = 0;
    /* Timers TIMER_0A to WTIMER_1B. */
    if (ID < WTIMER_2A) timerOffset = 0x1000 * (uint32_t)(ID >> 1);
    /* Timers WTIMER_2A to WTIMER_5B. Jump the base to 0x4004.C000. Our magic
       number, 16, is the enumerated value of WTIMER_2A. */
    else timerOffset = 0x1000 * (uint32_t)((ID-16) >> 1) + 0x0001C000;

    /* 2. Enable timer. */
    GET_REG(GPTM_BASE + timerOffset + GPTMCTL_OFFSET) |=
        ((ID % 2) == 0) ? 0x00000001 : 0x00000100;
}

void TimerStop(Timer_t timer) {
    /* Initialization asserts. */
    assert(timer.timerID < TIMER_COUNT);

    uint8_t ID = timer.timerID;

    /* Special case for SYSTICK. */
    if (ID == SYSTICK) {
        /* Disable during setup. */
        GET_REG(PERIPHERALS_BASE + SYSTICK_CTRL_OFFSET) = 0x00000000;
        return;
    }

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
}

uint64_t TimerGetValue(Timer_t timer) {
    /* Initialization asserts. */
    assert(timer.timerID < TIMER_COUNT);

    /* Special case for SYSTICK. */
    if (timer.timerID == SYSTICK) {
        /* Get current value. */
        return GET_REG(PERIPHERALS_BASE + SYSTICK_CURR_OFFSET) & 0x00FFFFFF;
    }

    uint8_t ID = timer.timerID;
    uint32_t timerOffset = 0;

    /* Timers TIMER_0A to WTIMER_1B. */
    if (ID < WTIMER_2A) timerOffset = 0x1000 * (uint32_t)(ID >> 1);
    /* Timers WTIMER_2A to WTIMER_5B. Jump the base to 0x4004.C000. Our magic
       number, 16, is the enumerated value of WTIMER_2A. */
    else timerOffset = 0x1000 * (uint32_t)((ID-16) >> 1) + 0x0001C000;

    /* Normal timers, 16 bit mode:
           - [63:56] Doesn't matter
           - [55:48] Prescaler B
           - [47:32] B side timer values
           - [31:24] Doesn't matter
           - [23:16] Prescaler A
           - [15:00] A side timer values
       Normal timers, 32 bit mode:
           - [63:32] B side timer values
           - [31:00] A side timer values
       Wide timers, 32 bit mode:
           - [63:32] B side timer values
           - [31:00] A side timer values
       Wide timers, 64 bit mode:
           - [63:00] A/B side timer value. */ 
    uint32_t lower = GET_REG(GPTM_BASE + timerOffset + GPTMTAV_OFFSET);
    uint32_t upper = GET_REG(GPTM_BASE + timerOffset + GPTMTBV_OFFSET);
    return (((uint64_t)upper << 32) | (uint64_t)lower);
}

uint32_t freqToPeriod(uint32_t freq, uint32_t maxFreq) {
    /* https://stackoverflow.com/a/2745086 */
    return (uint32_t) (1 + ((maxFreq - 1) / freq));
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
void WideTimer0A_Handler(void) {
    GET_REG(GPTM_BASE + 0x6000 + GPTMICR_OFFSET) |= TIMERXA_ICR_TATOCINT;
    if (TimerInterruptSettings[12].timerTask != NULL) {
        TimerInterruptSettings[12].timerTask(TimerInterruptSettings[12].timerArgs);
    }
}
void WideTimer0B_Handler(void) {
    GET_REG(GPTM_BASE + 0x6000 + GPTMICR_OFFSET) |= TIMERXB_ICR_TATOCINT;
    if (TimerInterruptSettings[13].timerTask != NULL) {
        TimerInterruptSettings[13].timerTask(TimerInterruptSettings[13].timerArgs);
    }
}
void WideTimer1A_Handler(void) {
    GET_REG(GPTM_BASE + 0x7000 + GPTMICR_OFFSET) |= TIMERXA_ICR_TATOCINT;
    if (TimerInterruptSettings[14].timerTask != NULL) {
        TimerInterruptSettings[14].timerTask(TimerInterruptSettings[14].timerArgs);
    }
}
void WideTimer1B_Handler(void) {
    GET_REG(GPTM_BASE + 0x7000 + GPTMICR_OFFSET) |= TIMERXB_ICR_TATOCINT;
    if (TimerInterruptSettings[15].timerTask != NULL) {
        TimerInterruptSettings[15].timerTask(TimerInterruptSettings[15].timerArgs);
    }
}
void WideTimer2A_Handler(void) {
    GET_REG(GPTM_BASE + 0x1C000 + GPTMICR_OFFSET) |= TIMERXA_ICR_TATOCINT;
    if (TimerInterruptSettings[16].timerTask != NULL) {
        TimerInterruptSettings[16].timerTask(TimerInterruptSettings[16].timerArgs);
    }
}
void WideTimer2B_Handler(void) {
    GET_REG(GPTM_BASE + 0x1C000 + GPTMICR_OFFSET) |= TIMERXB_ICR_TATOCINT;
    if (TimerInterruptSettings[17].timerTask != NULL) {
        TimerInterruptSettings[17].timerTask(TimerInterruptSettings[17].timerArgs);
    }
}
void WideTimer3A_Handler(void) {
    GET_REG(GPTM_BASE + 0x1D000 + GPTMICR_OFFSET) |= TIMERXA_ICR_TATOCINT;
    if (TimerInterruptSettings[18].timerTask != NULL) {
        TimerInterruptSettings[18].timerTask(TimerInterruptSettings[18].timerArgs);
    }
}
void WideTimer3B_Handler(void) {
    GET_REG(GPTM_BASE + 0x1D000 + GPTMICR_OFFSET) |= TIMERXB_ICR_TATOCINT;
    if (TimerInterruptSettings[19].timerTask != NULL) {
        TimerInterruptSettings[19].timerTask(TimerInterruptSettings[19].timerArgs);
    }
}
void WideTimer4A_Handler(void) {
    GET_REG(GPTM_BASE + 0x1E000 + GPTMICR_OFFSET) |= TIMERXA_ICR_TATOCINT;
    if (TimerInterruptSettings[20].timerTask != NULL) {
        TimerInterruptSettings[20].timerTask(TimerInterruptSettings[20].timerArgs);
    }
}
void WideTimer4B_Handler(void) {
    GET_REG(GPTM_BASE + 0x1E000 + GPTMICR_OFFSET) |= TIMERXB_ICR_TATOCINT;
    if (TimerInterruptSettings[21].timerTask != NULL) {
        TimerInterruptSettings[21].timerTask(TimerInterruptSettings[21].timerArgs);
    }
}
void WideTimer5A_Handler(void) {
    GET_REG(GPTM_BASE + 0x1F000 + GPTMICR_OFFSET) |= TIMERXA_ICR_TATOCINT;
    if (TimerInterruptSettings[22].timerTask != NULL) {
        TimerInterruptSettings[22].timerTask(TimerInterruptSettings[22].timerArgs);
    }
}
void WideTimer5B_Handler(void) {
    GET_REG(GPTM_BASE + 0x1F000 + GPTMICR_OFFSET) |= TIMERXB_ICR_TATOCINT;
    if (TimerInterruptSettings[23].timerTask != NULL) {
        TimerInterruptSettings[23].timerTask(TimerInterruptSettings[23].timerArgs);
    }
}

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

Timer_t DelayInit(void) {
    TimerConfig_t config = {
        .timerID=SYSTICK,
        .period=freqToPeriod(MAX_FREQ/80, MAX_FREQ),
        .isIndividual=false,
        .timerTask=NULL,
        .isPeriodic=true,
        .priority=1,
        .timerArgs=NULL
    };
    Timer_t timer = TimerInit(config);
    TimerStart(timer);
    return timer;
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
