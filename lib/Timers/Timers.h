/**
 * Timers.h
 * Devices: LM4F120; TM4C123
 * Description: Low level drivers for using onboard timers.
 * Authors: Matthew Yu.
 * Last Modified: 03/06/21
 **/
#pragma once

/** General imports. */
#include <stdint.h>
#include <stdio.h>
#include <math.h>

/** Device specific imports. */
#include <TM4C123Drivers/inc/tm4c123gh6pm.h>


#define MAX_FREQ 80000000 // 80 MHz

/** Enumerator defining all possible timers sans SysTick. */
typedef enum {
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
    TIMER_COUNT,
} timer_t;

/**
 * TimerInit initializes an arbitrary timer with a handler function reference.
 * @param timer       Enum identifying which timer to initialize.
 * @param period      Reload time, in cycles.
 * @param handlerTask Function pointer to what should be called by the
 *                    TimerXX_Handler. 
 * @note Note the following potential conflicts:
 *       - The TExaS scope (Texas.c) uses Timer5A.
 *       Note that B-side timer functionality is currently broken, and WTimers
 *       are not yet supported.
 */
void TimerInit(timer_t timer, uint32_t period, void (*handlerTask)(void));

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
 * cycles given the base system clock. Rounded up
 * @param freq Desired frequency.
 * @param maxFreq Base clock frequency. If freq > maxFreq, period = 0 (an error).
 * @return Output period, in cycles.
 */ 
uint32_t freqToPeriod(uint32_t freq, uint32_t maxFreq);
