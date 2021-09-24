/**
 * @file TimersExample.c
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief An example project showing how to use the Timer driver.
 * @version 0.1
 * @date 2021-09-23
 * @copyright Copyright (c) 2021
 * @note 
 * Modify __MAIN__ on L13 to determine which main method is executed.
 * __MAIN__ = 0 - Initialization and operation of multiple timers at different frequencies.
 *          = 1 - Initialization of a timer, modifying its period, and stopping it.
 */
#define __MAIN__ 0

/** General imports. */
#include <stdlib.h>

/** Device specific imports. */
#include <lib/PLL/PLL.h>
#include <lib/Timer/Timer.h>
#include <lib/GPIO/GPIO.h>


void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s

#if __MAIN__ == 0
volatile uint32_t counter0A = 0;
volatile uint32_t counter0B = 0;
volatile uint32_t counter1A = 0;
volatile uint32_t counter2A = 0;

Timer_t timers[4];

void dummyTask1(uint32_t * args) { ++counter0A; }

void dummyTask2(uint32_t * args) { ++counter1A; }

void dummyTask3(uint32_t * args) {
    ++counter2A;
    
    if (counter2A == 100) {
        /* 1. Place a breakpoint at L45! Notice that at counter2A = 100, counter0A ~= 25, counter1A ~= 50. */
        timers[3].period = freqToPeriod(100, MAX_FREQ);
        TimerUpdatePeriod(timers[3]); 
    }
    if (counter2A == 105) {
        /* 2. Place a breakpoint at L50! Notice here that when counter2A = 105, counter0A ~= 30, counter1A ~= 60. */
        uint8_t i = 0;
    }
}

void dummyTask4(uint32_t * args) { ++counter0B; }

int main(void) {
    /** 
     * This program demonstrates the initialization and operation of four
     * timers at different frequencies.
     */
    PLLInit(BUS_80_MHZ);
    DisableInterrupts();

    TimerConfig_t timerConfigs[4] = {
        /* The first timer has keyed arguments notated to show you what each positional argument means. */
        {.timerID=TIMER_0A, .period=freqToPeriod(100, MAX_FREQ), .timerTask=dummyTask1, .isPeriodic=true, .priority=5, .timerArgs=NULL},
        {         TIMER_0B,         freqToPeriod(800, MAX_FREQ),            dummyTask4,             true,           5,            NULL},
        {         TIMER_1A,         freqToPeriod(200, MAX_FREQ),            dummyTask2,             true,           5,            NULL},
        {         TIMER_2A,         freqToPeriod(400, MAX_FREQ),            dummyTask3,             true,           5,            NULL},
    };

    /* Initialize four timers based on the timer configuration array above. */
    timers[0] = TimerInit(timerConfigs[0]);
    timers[1] = TimerInit(timerConfigs[1]);
    timers[2] = TimerInit(timerConfigs[2]);
    timers[3] = TimerInit(timerConfigs[3]);

    EnableInterrupts();
    while (1) {
        /* View in debugging mode with counter0A, counter0B, counter1A, and
           counter2A added to watch 1. Put a breakpoint at L45 and L50. Run 
           until the first breakpoint is hit, and check the register value
           ratios. It should be 1 : 0 : 2 : 4. When then change Timer 2A to 
           100 Hz, and run it again. See if the counters match comment L49.
           
           Note that you can't run both A side and B side timers at once.
           Try it again with isPeriod set to false for single trigger mode,
           or with different priorities on the timers. What do you see?
          
           NOTE: As of (09/13/21) B-side timers do not activate properly. 
         */
        WaitForInterrupt();
    };
}

#elif __MAIN__ == 1
Timer_t timer;

/** This dummy task increments an internal counter and toggles an LED. */
void dummyTask(uint32_t * args) {
    static uint32_t counter = 0;
    ++counter;
    GPIOSetBit(PIN_F1, !GPIOGetBit(PIN_F1));

    if (counter == 25) {
        /* At about 5 seconds, the LED toggle frequency should double. */
        timer.period = freqToPeriod(10, MAX_FREQ);
        TimerUpdatePeriod(timer); 
    }

    if (counter == 75) {
        /* At about 10 seconds, the LED toggle should stop. */
        TimerStop(timer);
        
        /* We can forcefully delay another 5 seconds using SysTick. */
        DelayMillisec(5000);
        
        /* And then restart the timer, and it'll go on its merry way! */
        TimerStart(timer);
    }
}

int main(void) {
    /**
     * This program demonstrates starting a timer, updating its period, and then
     * stopping the timer.
     */
    PLLInit(BUS_80_MHZ);
    DisableInterrupts();

    DelayInit();
    
    /* Initialize a GPIO LED on PF1. */
    GPIOConfig_t PF1Config = {
        .pin=PIN_F1, 
        .pull=GPIO_PULL_DOWN, 
        .isOutput=true, 
        .alternateFunction=0, 
        .isAnalog=false,
        .drive=GPIO_DRIVE_2MA,
        .enableSlew=false
    };
    GPIOInit(PF1Config);

    /* Initialize a timer executing at 5 Hz. */
    TimerConfig_t timerConfig = {
        .timerID=TIMER_0A,
        .period=freqToPeriod(5, MAX_FREQ), 
        .timerTask=dummyTask,
        .isPeriodic=true, 
        .priority=5, 
        .timerArgs=NULL};
    TimerInit(timerConfig);

    EnableInterrupts();
    while (1) { WaitForInterrupt(); }
}
#endif
