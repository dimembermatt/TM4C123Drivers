/**
 * @file RGB.c
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief Flashes the onboard TM4C RGB (PF1-3) many colors, using a state machine.
 * @version 0.1
 * @date 2021-09-27
 * @copyright Copyright (c) 2021
 */

/** General imports. */
#include <stdlib.h>
#include <stdbool.h>

/** Device specific imports. */
#include <lib/GPIO/GPIO.h>
#include <lib/Timer/Timer.h>
#include <raslib/RGB/RGB.h>


static Timer_t timer;
static uint8_t states[8][4] = {
    // R, B, G, next state
      {0, 0, 0, 1},
      {0, 0, 1, 5},
      {0, 1, 0, 3},
      {0, 1, 1, 1}, // We never go back to the off state :)
      {1, 0, 0, 6},
      {1, 0, 1, 4},
      {1, 1, 0, 2},
      {1, 1, 1, 0}
};

static uint8_t state = 0;
static void RGBStep(uint32_t * _args) {
    // Set the LEDs.
    GPIOSetBit(PIN_F1, states[state][0]); 
    GPIOSetBit(PIN_F2, states[state][1]); 
    GPIOSetBit(PIN_F3, states[state][2]); 

    // Set new state.
    state = states[state][3];
}

void RGBInit(void) {
    GPIOConfig_t pf1 = { // RED
        PIN_F1,
        GPIO_PULL_DOWN,
        true,
        0,
        false,
        GPIO_DRIVE_2MA,
        false
    };

    GPIOConfig_t pf2 = { // BLUE
        PIN_F2,
        GPIO_PULL_DOWN,
        true,
        0,
        false,
        GPIO_DRIVE_2MA,
        false
    };

    GPIOConfig_t pf3 = { // GREEN
        PIN_F3,
        GPIO_PULL_DOWN,
        true,
        0,
        false,
        GPIO_DRIVE_2MA,
        false
    };

    GPIOInit(pf1);
    GPIOInit(pf2);
    GPIOInit(pf3);

    TimerConfig_t tConf = {
        TIMER_5A,
        freqToPeriod(15, MAX_FREQ),
        RGBStep,
        true,
        7,
        NULL
    };
    timer = TimerInit(tConf);
}

void RGBStart(void) {
    TimerStart(timer);
}

void RGBStop(void) {
    TimerStop(timer);
    state = 0;
}
