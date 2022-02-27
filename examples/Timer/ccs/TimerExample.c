/**
 * @file TimersExample.c
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief An example project showing how to use the Timer driver.
 * @version 0.1
 * @date 2022-02-27
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
volatile uint32_t counterW0A = 0;

Timer_t timers[4];

void dummyTask1(uint32_t * args) { ++counter0A; }

void dummyTask2(uint32_t * args) { ++counter0B; }

void dummyTask3(uint32_t * args) { ++counter1A; }

void dummyTask4(uint32_t * args) {
    ++counterW0A;

    if (counterW0A == 200) {
        /* 1. Place a breakpoint at L51! Notice that at counterW0A = 200,
              counter1A = 100, counter0B = 50, and counter0A = 25.
              What does this imply about the frequency of TIMER_0A and
              TIMER_0B? The prescale value modifies the effective frequency
              to frequency / (prescale + 1). */
        timers[3].period = freqToPeriod(100, MAX_FREQ);
        TimerUpdatePeriod(timers[3]); 
    }
    if (counterW0A == 205) {
        /* 2. Place a breakpoint at L59! Notice here that when counterW0A = 205,
              counter1A ~= 120, counter0B ~= 60, and counter0A = 30. Why is
              counter0B and counter1A not exactly 120 and 60? They are not
              highest priority! */ 
        uint8_t i = 0;
    }
}

int main(void) {
    /** 
     * This program demonstrates the initialization and operation of four
     * timers at different frequencies.
     */
    PLLInit(BUS_80_MHZ);
    DisableInterrupts();

    TimerConfig_t timerConfigs[4] = {
        /* The first timer has keyed arguments notated to show you what each positional argument means. */
        {.timerID=TIMER_0A, .period=freqToPeriod(1600, MAX_FREQ), .isIndividual=true,  .prescale=15, .timerTask=dummyTask1, .isPeriodic=true, .priority=0, .timerArgs=NULL},
        {         TIMER_0B,         freqToPeriod(1600, MAX_FREQ),               true,             7,            dummyTask2,             true,           1,            NULL},
        {         TIMER_1A,         freqToPeriod(400, MAX_FREQ),                false,            0,            dummyTask3,             true,           2,            NULL},
        {         WTIMER_0A,        freqToPeriod(800, MAX_FREQ),                false,            0,            dummyTask4,             true,           3,            NULL},
    };

    /* Initialize and start four timers based on the timer configuration array above. */
    for (uint8_t i = 0; i < 4; ++i) {
        timers[i] = TimerInit(timerConfigs[i]);
        TimerStart(timers[i]);
    }

    EnableInterrupts();
    while (1) {
        /* View in debugging mode with counter0A, counter0B, counter1A, and
           counterW0A added to watch 1. Put a breakpoint at L51 and L59. Run 
           until the first breakpoint is hit, and check the register value
           ratios. Check to see if your watch values match the expected values
           at L51 and L59! */
        WaitForInterrupt();
    };
}

#elif __MAIN__ == 1
static Timer_t timer;

/** This dummy task increments an internal counter and toggles an LED. */
static void dummyTask(uint32_t * args) {
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
        .isIndividual=false,
        .timerTask=dummyTask,
        .isPeriodic=true, 
        .priority=5, 
        .timerArgs=NULL
    };
    timer = TimerInit(timerConfig);
    TimerStart(timer);

    EnableInterrupts();
    while (1) { WaitForInterrupt(); }
}
#endif
