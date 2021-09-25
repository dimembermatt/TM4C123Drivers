/**
 * @file Timer.h
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief Timer peripheral driver.
 * @version 0.1
 * @date 2021-09-24
 * @copyright Copyright (c) 2021
 * @note
 * Unsupported Features. This driver does not support WTimers, multiple clock
 * modes, nor count up vs count down. B-side timers are currently broken.
 */
#pragma once

/** General imports. */
#include <stdint.h>
#include <stdbool.h>


#define MAX_FREQ 80000000 /** 80 MHz. */

/** @brief TimerID_t is an enumeration defining all possible timers, including SysTick. */
typedef enum TimerID {
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
} TimerID_t;

/** @brief TimerConfig_t is a user defined struct that specifies a timer configuration. */
typedef struct TimerConfig {
    /**
     * @brief The Timer module to enable.
     *
     * Default is TIMER_0A.
     */
    TimerID_t timerID;

    /**
     * @brief Timer reload time, in cycles.
     *
     * This value must be specified and be greater than zero. Failing this
     * condition will trigger an internal assert in debug mode. In production,
     * this causes undefined behavior.
     */
    uint32_t period;

    /** ------------- Optional Fields. ------------- */

    /**
     * @brief Pointer to function called on timer interrupt. Accepts any
     *        number of arguments, but they must be handled as pointers by the
     *        function. 
     *
     * Default is NULL (No function defined).
     * 
     * @note This is optional, but is typically recommended (unless you're just
     *       initializing Systick). 
     */
    void (*timerTask)(uint32_t *args);

    /**
     * @brief Whether the Timer executes continuously or not.
     *
     * Default is false (Aperiodic).
     */
    bool isPeriodic;

    /**
     * @brief Timer interrupt priority. From 0 - 7. Lower value is higher
     *        priority.
     *
     * Default is 0 (Highest priority).
     */
    uint8_t priority;

    /**
     * @brief The pointer to an array of uint32_t arguments that fed into
     *        timerTask upon being called.
     *
     * Default is NULL (No pointer to any arguments is defined).
     *
     * @note Use with caution. The programmer is responsible for handling
     *       timerArgs inside of their timerTask function implementation.
     */
    uint32_t * timerArgs;
} TimerConfig_t;

/** @brief Timer_t is a struct containing user relevant data of a timer. */
typedef struct Timer {
    /** @brief The Timer module configured. */
    TimerID_t timerID;

    /** @brief Timer reload time, in cycles. */
    uint32_t period;
} Timer_t;

/**
 * @brief TimerInit initializes a Timer module given a configuration.
 *
 * @param config Configuration for the timer.
 * @return A Timer_t struct containing relevant data.
 * @note Use freqToPeriod() for frequency conversion.
 * @note Requires the EnableInterrupts() call since the interrupts are
 *       enabled.
 */
Timer_t TimerInit(TimerConfig_t config);

/**
 * @brief TimerUpdatePeriod adjusts the timer period.
 *
 * @param timer A timer configuration to adjust.
 */
void TimerUpdatePeriod(Timer_t timer);

/**
 * @brief TimerStop halts execution of the timer specified.
 *
 * @param timer A timer configuration to adjust.
 */
void TimerStop(Timer_t timer);

/**
 * @brief TimerStart Starts execution of the timer specified. This is only
 *        needed when the Timer has stopped prior for any reason.
 *
 * @param timer A timer configuration to adjust.
 */
void TimerStart(Timer_t timer);

/**
 * @brief freqToPeriod converts a desired frequency into the equivalent period
 *        in cycles given the base system clock, rounded up.
 *
 * @param freq Desired frequency.
 * @param maxFreq Base clock frequency.
 * @return Output period, in cycles.
 * @note If freq > maxFreq, the period 0 is returned (an error).
 */
uint32_t freqToPeriod(uint32_t freq, uint32_t maxFreq);

/** Handler declarations for normal timers. */
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

/** Handler declarations for wide timers. */
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

/** @brief Systick Handler definition. */
void SysTick_Handler(void);

/**
 * @brief SysTickGetTick returns the number of total ticks.
 * @return Number of ticks since startup.
 */
uint64_t SysTickGetTick(void);

/**
 * @brief DelayInit initializes the SysTick timer to run at 1 MHz and start an
 *        internal ticker.
 *
 * @note This can be overwritten by other SysTick initializations. If another
 * SysTick initializer is not at 1 MHz, this will break.
 */
void DelayInit(void);

/**
 * @brief DelayMillisec delays the process by 1 ms. This is BLOCKING.
 *
 * @param n The number of ms to delay the process.
 * @note Ideally, this method is not used in interrupt handlers.
 */
void DelayMillisec(uint32_t n);

/**
 * @brief DelayMillisec delays the process by 1 us. This is BLOCKING.
 *
 * @param n The number of us to delay the process.
 * @note Ideally, this method is not used in interrupt handlers.
 */
void DelayMicrosec(uint32_t n);
