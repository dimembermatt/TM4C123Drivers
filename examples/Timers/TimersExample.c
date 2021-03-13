/**
 * File name: TimersExample.h 
 * Devices: LM4F120; TM4C123 
 * Description: Example program to demonstrate the capabilities of normal and
 * wide timers. Includes SysTick.
 * Authors: Matthew Yu. 
 * Last Modified: 03/13/21
 */

/** Device specific imports. */
#include <TM4C123Drivers/inc/PLL.h>
#include <TM4C123Drivers/lib/Miscellaneous/Misc.h>
#include <TM4C123Drivers/lib/Timers/Timers.h>


void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s

volatile uint32_t counter0A = 0;
volatile uint32_t counter0B = 0;
volatile uint32_t counter1A = 0;
volatile uint32_t counter2A = 0;

void dummyTask(void) {
    ++counter0A;
}

void dummyTask2(void) {
    ++counter1A;
}

void dummyTask3(void) {
	++counter2A;
}

void dummyTask4(void) {
    ++counter0B;
}

int main(void) {
    PLL_Init(Bus80MHz);
    DisableInterrupts();

	TimerConfig_t timers[4] = {
        /* The first timer has keyed arguments notated to show you what each positional argument means. */
		{.timerID=TIMER_0A, .period=freqToPeriod(100, MAX_FREQ), .isPeriodic=true, .priority=5, .handlerTask=dummyTask},
		{		  TIMER_0B,         freqToPeriod(800, MAX_FREQ),             true,           5,              dummyTask4},
		{         TIMER_1A,         freqToPeriod(200, MAX_FREQ),             true,           5,              dummyTask2},
		{         TIMER_2A,         freqToPeriod(400, MAX_FREQ),             true,           5,              dummyTask3},
	};
	
    TimerInit(timers[0]);
    TimerInit(timers[1]);
    TimerInit(timers[2]);
    TimerInit(timers[3]);

    EnableInterrupts();

    while (1) {
        /* View in debugging mode with counter0A, counter0B, counter1A, and
         * counter2A added to watch 1. Run for a while, then check register
         * value ratios. Should be 1 : 0 : 2 : 4. 
         * Note that you can't run both A side and B side timers at once. 
         * Try it again with isPeriod set to false for single trigger mode,
         * or with different priorities on the timers. What do you see?
         * 
         * NOTE: As of (03/13/21) B-side timers do not activate properly. */
        WaitForInterrupt();
    };
}
