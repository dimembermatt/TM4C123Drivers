/**
 * Timers.h
 * Devices: LM4F120; TM4C123
 * Description: Low level drivers for using onboard timers.
 * Authors: Matthew Yu.
 * Last Modified: 03/13/21
 **/
#pragma once

/** General imports. */
#include <stdint.h>
#include <stdbool.h>


#define MAX_FREQ 80000000 /** 80 MHz. */

/** Our handler type used by timers. */
typedef void (*handlerFunctionPtr_t)(void);
/** Enumerator defining all possible timers, including SysTick. */
enum TimerID {
    TIMER_0A, TIMER_0B,
    TIMER_1A, TIMER_1B,
    TIMER_2A, TIMER_2B,
    TIMER_3A, TIMER_3B,
    TIMER_4A, TIMER_4B,
    TIMER_5A, TIMER_5B,
    WTIMER_0A, WTIMER_0B,
    WTIMER_1A, WTIMER_1B,
    WTIMER_2A, WTIMER_2B,
    WTIMER_3A, WTIMER_3B,
    WTIMER_4A, WTIMER_4B,
    WTIMER_5A, WTIMER_5B,
    SYSTICK, TIMER_COUNT,
};

typedef struct TimerConfig {
    /* Timer to setup. */
    enum TimerID timerID;

    /* Timer reload time, in cycles. */
    uint32_t period;

    /* Whether the Timer executes continuously or not. */
    bool isPeriodic;

    /* Timer priority. From 0 - 7. Lower is higher priority. */
    uint8_t priority;
    
    /* Task executed when Timer interrupts. */
    void (*handlerTask)(void);
} TimerConfig_t;

/**
 * TimerInit initializes an arbitrary timer with a handler function reference.
 * @param timer       Enum identifying which timer to initialize.
 * @param period      Reload time, in cycles. 
 * @param handlerTask Function pointer to what should be called by the
 *                    TimerXX_Handler. 
 * @note Note that B-side timer functionality is currently broken, and WTimers
 *       are not yet supported.
 *       Use freqToPeriod() for frequency conversion.
 *       Requires the EnableInterrupts() call if edge triggered interrupts are enabled.
 *       By default the timer is priority 5, below SysTick.
 * @dev  Potentially add the following parameters:
 *          - clock mode (i.e. 32-bit vs 16-bit config with CFG_R).
 *          - one shot vs periodic timer mode (TAMR, TBMR).
 *          - count down vs count up (TACDIR inside TAMR, TBCDIR inside TBMR).
 *          - Timer priority (NVIC).
 */
void TimerInit(TimerConfig_t timerConfig);

/** Handler definitions for normal timers. */
void Timer0A_Handler(void);
void Timer0B_Handler(void);
void Timer1A_Handler(void);
void Timer1B_Handler(void);
void Timer2A_Handler(void);
void Timer2B_Handler(void);
void Timer3A_Handler(void);
void Timer3B_Handler(void);
void Timer4A_Handler(void);
void Timer4B_Handler(void);
void Timer5A_Handler(void);
void Timer5B_Handler(void);

/** Handler definitions for wide timers. */
void WideTimer0A_Handler(void);
void WideTimer0B_Handler(void);
void WideTimer1A_Handler(void);
void WideTimer1B_Handler(void);
void WideTimer2A_Handler(void);
void WideTimer2B_Handler(void);
void WideTimer3A_Handler(void);
void WideTimer3B_Handler(void);
void WideTimer4A_Handler(void);
void WideTimer4B_Handler(void);
void WideTimer5A_Handler(void);
void WideTimer5B_Handler(void);

/**
 * freqToPeriod converts a desired frequency into the equivalent period in
 * cycles given the base system clock, rounded up.
 * @param freq Desired frequency.
 * @param maxFreq Base clock frequency. 
 * @return Output period, in cycles.
 * @note If freq > maxFreq, the period 0 is returned (an error).
 */ 
uint32_t freqToPeriod(uint32_t freq, uint32_t maxFreq);
