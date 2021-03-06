/**
 * Timers.c
 * Devices: LM4F120; TM4C123
 * Description: Low level drivers for using onboard timers.
 * Authors: Matthew Yu.
 * Last Modified: 03/06/21
 **/

/** Device specific imports. */
#include "Timers.h"
#include <TM4C123Drivers/inc/RegDefs.h>

#define TIMERXA_ICR_TATOCINT 0x00000001
#define TIMERXB_ICR_TATOCINT 0x00000100

#define GET_REG(addr) (*((volatile uint32_t *)(addr)))

/** Our handler type used by timers. */
typedef void (*handlerFunctionPtr_t)(void);

/** 
 * This field is either 0, 1, 2, 3, specifying the byte that the interrupt of
 * priority X should be set to.  
 */ 
enum InterruptPriorityIdx {INTA, INTB, INTC, INTD};

/** Struct defining configuration info for each timer. */
struct InterruptSettings {
    /** Priority index of the timer. */
    enum InterruptPriorityIdx priorityIdx;

    /** Address of the relevant priority register. */
    volatile uint32_t * NVIC_PRI_ADDR;

    /** Address of the relevant set enable register. */
    volatile uint32_t * NVIC_EN_ADDR;

    /** Relevant IRQ number of the interrupt. */
    uint32_t IRQ;

    /** User function associated with the interrupt. */
    handlerFunctionPtr_t timerHandlerTask;
};

/** Configuration info for all timers. */
struct InterruptSettings interruptSettings[TIMER_COUNT] = {
    {INTD, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI4_OFFSET),  (uint32_t *)(PERIPHERALS_BASE + NVIC_EN0_OFFSET), 19,    NULL}, /* Timer 0A. */
    {INTA, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI5_OFFSET),  (uint32_t *)(PERIPHERALS_BASE + NVIC_EN0_OFFSET), 20,    NULL}, /* Timer 0B. */
    {INTB, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI5_OFFSET),  (uint32_t *)(PERIPHERALS_BASE + NVIC_EN0_OFFSET), 21,    NULL}, /* Timer 1A. */
    {INTC, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI5_OFFSET),  (uint32_t *)(PERIPHERALS_BASE + NVIC_EN0_OFFSET), 22,    NULL}, /* Timer 1B. */
    {INTD, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI5_OFFSET),  (uint32_t *)(PERIPHERALS_BASE + NVIC_EN0_OFFSET), 23,    NULL}, /* Timer 2A. */
    {INTA, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI6_OFFSET),  (uint32_t *)(PERIPHERALS_BASE + NVIC_EN0_OFFSET), 24,    NULL}, /* Timer 2B. */
    {INTD, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI8_OFFSET),  (uint32_t *)(PERIPHERALS_BASE + NVIC_EN1_OFFSET), 35-32, NULL}, /* Timer 3A. */
    {INTA, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI9_OFFSET),  (uint32_t *)(PERIPHERALS_BASE + NVIC_EN1_OFFSET), 36-32, NULL}, /* Timer 3B. */
    {INTC, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI17_OFFSET), (uint32_t *)(PERIPHERALS_BASE + NVIC_EN2_OFFSET), 70-64, NULL}, /* Timer 4A. */
    {INTD, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI17_OFFSET), (uint32_t *)(PERIPHERALS_BASE + NVIC_EN2_OFFSET), 71-64, NULL}, /* Timer 4B. */
    {INTA, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI23_OFFSET), (uint32_t *)(PERIPHERALS_BASE + NVIC_EN2_OFFSET), 92-64, NULL}, /* Timer 5A. */
    {INTB, (uint32_t *)(PERIPHERALS_BASE + NVIC_PRI23_OFFSET), (uint32_t *)(PERIPHERALS_BASE + NVIC_EN2_OFFSET), 93-64, NULL}, /* Timer 5B. */

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
};


/**
 * TimerInit initializes an arbitrary timer with a handler function reference.
 * @param timer Enum identifying which timer to initialize.
 * @param period Reload time, in cycles.
 * @param handlerTask Function pointer to what should be called by the TimerXX_Handler.
 * @note Potentially add the following parameters:
 *          - clock mode (i.e. 32-bit vs 16-bit config with CFG_R).
 *          - one shot vs periodic timer mode (TAMR, TBMR).
 *          - count down vs count up (TACDIR inside TAMR, TBCDIR inside TBMR).
 *          - Timer priority (NVIC).
 *       Requires the EnableInterrupts() call if edge triggered interrupts are enabled.
 *       By default the timer is priority 2, below SysTick.
 */
void TimerInit(timer_t timer, uint32_t period, void (*handlerTask)(void)) {
    /* Early return on invalid timer enum. */
    if (timer == TIMER_COUNT) return;

    /* We'll generate the timer offset to find the correct addresses for each
     * timer. */
    uint32_t timerOffset = 0;

    /* Timers TIMER_0A to WTIMER_1B. */
    if (timer < WTIMER_2A) timerOffset = 0x1000 * (uint32_t)(timer >> 1);

    /* Timers WTIMER_2A to WTIMER_5B. Jump the base to 0x4004.C000. Our magic
     * number, 16, is the enumerated value of WTIMER_2A. */
    else timerOffset = 0x1000 * (uint32_t)((timer-16) >> 1) + 0x0001C000;

    /* Step 0. Assign the task to the appropriate handler. */
    interruptSettings[timer].timerHandlerTask = handlerTask;

    /* Step 1. Activate the timer. */
    if (timer <= TIMER_5B) { /* 16/32 bit normal timers. */
        SYSCTL_RCGCTIMER_R |= (0x01 << (uint32_t)(timer >> 1));
    } else { /* 32/64 bit wide timers. */
        /* Our magic number 12, is the enumerated value of WTIMER_0A. */
        SYSCTL_RCGCWTIMER_R |= (0x01 << (uint32_t)((timer-12) >> 1));
    }
        
    /* Step 2. Disable timer during setup. */
    GET_REG(GPTM_BASE + timerOffset + GPTMCTL_OFFSET) &=
        ((timer % 2) == 0) ? 0xFFFFFF00 : 0xFFFFFF00FF;

    /* Step 3. Configure for 32-bit mode. */
    GET_REG(GPTM_BASE + timerOffset + GPTMCFG_OFFSET) = 0x00000000;

    /* Step 4. Configure for periodic mode.
     * Step 5. Set reload value.
     * Step 6. Set prescaler to 1. */
    if ((timer % 2) == 0) { /* Timer A. */
        GET_REG(GPTM_BASE + timerOffset + GPTMTAMR_OFFSET)  = 0x00000002;
        GET_REG(GPTM_BASE + timerOffset + GPTMTAILR_OFFSET) = period - 1;
        GET_REG(GPTM_BASE + timerOffset + GPTMTAPR_OFFSET)  = 0x00000000;
    } else { /* Timer B. */
        GET_REG(GPTM_BASE + timerOffset + GPTMTBMR_OFFSET)  = 0x00000002;
        GET_REG(GPTM_BASE + timerOffset + GPTMTBILR_OFFSET) = period - 1;
        GET_REG(GPTM_BASE + timerOffset + GPTMTBPR_OFFSET)  = 0x00000000;
    }

    /* Step 7. Clear timer timeout flag. */
    GET_REG(GPTM_BASE + timerOffset + GPTMICR_OFFSET) |=
        ((timer % 2) == 0) ? TIMERXA_ICR_TATOCINT: TIMERXB_ICR_TATOCINT;

    /* Step 8. Arm timeout interrupt. */
    GET_REG(GPTM_BASE + timerOffset + GPTMIMR_OFFSET) |=
        ((timer % 2) == 0) ? 0x00000001 : 0x00000100;

    /* Step 9. Set timer to interrupt priority 2. */
    uint32_t mask = 0xFFFFFFFF;
    uint32_t intVal = 0x40;

    /* Our magic number is 8 since to shift a hex value one hex position, we do
     * 4 binary shifts. To do it twice; 8 binary shifts. Our output should
     * something like this for priority 2: 0xFF00FFFF, 0x00400000. */
    mask &= ~(0xFF << (interruptSettings[timer].priorityIdx * 8));
    intVal = intVal << (interruptSettings[timer].priorityIdx * 8);
    (*interruptSettings[timer].NVIC_PRI_ADDR) = 
        ((*interruptSettings[timer].NVIC_PRI_ADDR)&mask)|intVal;
    
    /* Step 10. Enable IRQ X in NVIC. */
    (*interruptSettings[timer].NVIC_EN_ADDR) = 1 << interruptSettings[timer].IRQ;

    /* Step 11. Enable timer after setup. */
    GET_REG(GPTM_BASE + timerOffset + GPTMCTL_OFFSET) |=
        ((timer % 2) == 0) ? 0x00000001 : 0x00000100;
}

void Timer0A_Handler(void) {
    TIMER0_ICR_R |= TIMERXA_ICR_TATOCINT;
    if (interruptSettings[0].timerHandlerTask != NULL) {
        interruptSettings[0].timerHandlerTask();
    }
}

void Timer0B_Handler(void) {
    TIMER0_ICR_R |= TIMERXB_ICR_TATOCINT;
    if (interruptSettings[1].timerHandlerTask != NULL) {
        interruptSettings[1].timerHandlerTask();
    }
}

void Timer1A_Handler(void) {
    TIMER1_ICR_R |= TIMERXA_ICR_TATOCINT;
    if (interruptSettings[2].timerHandlerTask != NULL) {
        interruptSettings[2].timerHandlerTask();
    }
}

void Timer1B_Handler(void) {
    TIMER1_ICR_R |= TIMERXB_ICR_TATOCINT;
    if (interruptSettings[3].timerHandlerTask != NULL) {
        interruptSettings[3].timerHandlerTask();
    }
}

void Timer2A_Handler(void) {
    TIMER2_ICR_R |= TIMERXA_ICR_TATOCINT;
    if (interruptSettings[4].timerHandlerTask != NULL) {
        interruptSettings[4].timerHandlerTask();
    }
}

void Timer2B_Handler(void) {
    TIMER2_ICR_R |= TIMERXB_ICR_TATOCINT;
    if (interruptSettings[5].timerHandlerTask != NULL) {
        interruptSettings[5].timerHandlerTask();
    }
}

void Timer3A_Handler(void) {
    TIMER3_ICR_R |= TIMERXA_ICR_TATOCINT;
    if (interruptSettings[6].timerHandlerTask != NULL) {
        interruptSettings[6].timerHandlerTask();
    }
}

void Timer3B_Handler(void) {
    TIMER3_ICR_R |= TIMERXB_ICR_TATOCINT;
    if (interruptSettings[7].timerHandlerTask != NULL) {
        interruptSettings[7].timerHandlerTask();
    }
}

void Timer4A_Handler(void) {
    TIMER4_ICR_R |= TIMERXA_ICR_TATOCINT;
    if (interruptSettings[8].timerHandlerTask != NULL) {
        interruptSettings[8].timerHandlerTask();
    }
}

void Timer4B_Handler(void) {
    TIMER4_ICR_R |= TIMERXB_ICR_TATOCINT;
    if (interruptSettings[9].timerHandlerTask != NULL) {
        interruptSettings[9].timerHandlerTask();
    }
}

void Timer5A_Handler(void) {
    TIMER5_ICR_R |= TIMERXA_ICR_TATOCINT;
    if (interruptSettings[10].timerHandlerTask != NULL) {
        interruptSettings[10].timerHandlerTask();
    }
}

void Timer5B_Handler(void) {
    TIMER5_ICR_R |= TIMERXB_ICR_TATOCINT;
    if (interruptSettings[11].timerHandlerTask != NULL) {
        interruptSettings[11].timerHandlerTask();
    }
}

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

/**
 * freqToPeriod converts a desired frequency into the equivalent period in
 * cycles given the base system clock.
 * @param freq Desired frequency.
 * @param maxFreq Base clock frequency. If freq > maxFreq, period = 1 (saturates
 *                at max frequency). Rounds up if maxFreq is not easily
 *                divisible by freq.
 * @return Output period, in cycles.
 */
uint32_t freqToPeriod(uint32_t freq, uint32_t maxFreq) {
    return (uint32_t) ceil(maxFreq/freq);
}
