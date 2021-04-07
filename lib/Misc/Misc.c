/**
 * File name: Misc.h
 * Devices: LM4F120; TM4C123
 * Description: Misc functions for general use.
 * Authors: Jonathan Valvano. Revised by Matthew Yu.
 * Last Modified: 04/07/21
 */

/** General imports. */
#include <stdlib.h>

/** Device specific imports. */
#include <lib/Misc/Misc.h>
#include <lib/Timers/Timers.h>


/** 
 * Initializes SysTick to run at 1 MHz and start an internal ticker. 
 * Can be overwritten by other Systick initializers. If another Systick
 * initializer is not at 1 MHz, this breaks.
 */
void delayInit(void) {
    TimerConfig_t config = {
        .timerID=SYSTICK,
        .period=freqToPeriod(MAX_FREQ/80, MAX_FREQ),
        .isPeriodic=true,
        .priority=1,
        .handlerTask=NULL
    };
    TimerInit(config);
}

/**
 * delayMillisec delays the process by 1 ms. Uses SYSTICK.
 * 
 * @param n The number of ms to delay the process.
 */
void delayMillisec(uint32_t n) {
	uint64_t tick = getTick();

    /* 1_000 ticks is 1 ms. Tuned to 1 MHz Systick. */
    while (getTick() - tick < 1000 * n);
}

/**
 * delayMicrosec delays the process by 1 us. Uses SYSTICK.
 * 
 * @param n The number of us to delay the process.
 */
void delayMicrosec(uint32_t n) {
	uint64_t tick = getTick();

    /* 1 tick is 1 us. Tuned to 1 MHz Systick. */
    while (getTick() - tick < n);
}
