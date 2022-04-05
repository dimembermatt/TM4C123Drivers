/**
 * @file Timer.h
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief Timer peripheral driver.
 * @version 0.1
 * @date 2021-10-28
 * @copyright Copyright (c) 2021
 * @note
 * Unsupported Features. This driver does not support multiple clock
 * modes, nor count up vs count down.
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
     * @brief The base timer reload time, in cycles.
     *
     * This value must be specified and be greater than zero. Failing this
     * condition will trigger an internal assert in debug mode. In production,
     * this causes undefined behavior.
     * 
     * @note Values greater than 0xFFFF or 0xFFFFFFFF are truncated for 16 bit
     *       timers and 32 bit timers, respectively. This period is extended by
     *       the prescale value, which is optional.
     * @note For Systick, the maximum period is 0xFFFFFF. At 80 MHz, this works
     *       out to be around 0.2097151875s or 4.768 Hz.
     */
    uint64_t period;

    /** ------------- Optional Fields. ------------- */

    /**
     * @brief Whether the Timer is separated from the B side timer to make a 16/32
     *        bit timer.
     * 
     * Default is false (Timers are concatenated and are 32/64 bits wide).
     * 
     */
    bool isIndividual;

    /**
     * @brief A multiple extension of the base timer reload period, in cycles.
     * For example, a prescale value of 1 doubles the base period, effectively
     * halving the frequency.
     * 
     * Default is 0 (No scaling).
     * 
     * @note Values greater than 0xFF are truncated for non wide timers. Values
     *       are offset by one. I.E. a prescale value of 1 doubles the period,
     *       and a prescale value of 2 triples the period, and so on.
     */
    uint16_t prescale;

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
 * @brief TimerInit initializes a Timer module given a configuration. Does not
 *        start execution. Call TimerStart(timer) to begin execution. 
 *
 * @param config Configuration for the timer.
 * @return A Timer_t struct containing relevant data.
 * @note Use freqToPeriod() for frequency conversion.
 */
Timer_t TimerInit(TimerConfig_t config);

/**
 * @brief TimerStart Starts execution of the timer specified.
 *
 * @param timer A timer configuration to adjust.
 */
void TimerStart(Timer_t timer);

/**
 * @brief TimerStop halts execution of the timer specified.
 *
 * @param timer A timer configuration to adjust.
 */
void TimerStop(Timer_t timer);

/**
 * @brief TimerUpdatePeriod adjusts the timer period. Does not affect whether
 *        the timer is running or stopped.
 *
 * @param timer A timer configuration to adjust.
 */
void TimerUpdatePeriod(Timer_t timer);

/**
 * @brief TimerGetValue returns the current register value of the timer specified. 
 * 
 * @param timer Timer to get value from.
 * @return uint64_t Value of register dependent on configuration.
 * @note
 *     Normal timers, 16 bit mode:
 *         - [63:56] Doesn't matter
 *         - [55:48] Prescaler B
 *         - [47:32] B side timer values
 *         - [31:24] Doesn't matter
 *         - [23:16] Prescaler A
 *         - [15:00] A side timer values
 *     Normal timers, 32 bit mode:
 *         - [63:32] B side timer values
 *         - [31:00] A side timer values
 *     Wide timers, 32 bit mode:
 *         - [63:32] B side timer values
 *         - [31:00] A side timer values
 *     Wide timers, 64 bit mode:
 *         - [63:00] A/B side timer value.
 */
uint64_t TimerGetValue(Timer_t timer);

/**
 * @brief freqToPeriod converts a desired frequency into the equivalent period
 *        in cycles given the base system clock, rounded up.
 * 
 * @param freq Desired frequency.
 * @param maxFreq Base clock frequency (typically 80 MHZ - 80_000_000).
 * @return uint32_t Period of the desired frequency, in cycles.
 * @note If freq > maxFreq, freq is throttled to maxFreq. The output is 1.
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
 * @return Configuration for SysTick.
 * @note This can be overwritten by other SysTick initializations. If another
 * SysTick initializer is not at 1 MHz, this will break.
 */
Timer_t DelayInit(void);

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
