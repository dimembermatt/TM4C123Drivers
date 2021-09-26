/**
 * @file GPIOExample.c
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief An example project showing how to use the GPIO driver.
 * @version 0.1
 * @date 2021-09-23
 * @copyright Copyright (c) 2021
 * @note 
 * Modify __MAIN__ on L13 to determine which main method is executed.
 * __MAIN__ = 0 - Initialization, getting, and setting of GPIO pins.
 *          = 1 - Initialization of a GPIO pin with an edge triggered interrupt.
 */
#define __MAIN__ 0

/** General imports. */
#include <stdlib.h>

/** Device specific imports. */
#include <lib/PLL/PLL.h>
#include <lib/GPIO/GPIO.h>
#include <lib/Timer/Timer.h>


void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s

#if __MAIN__ == 0
int main(void) {
    /**
     * This program demonstrates initializing a GPIO pin and updating its value.
     */
    PLLInit(BUS_80_MHZ);
    DisableInterrupts();

    /* Initialize SysTick for delay calls.*/
    DelayInit();
    
    GPIOConfig_t PF1Config = {
        .pin=PIN_F1, 
        .pull=GPIO_PULL_DOWN, 
        .isOutput=true,
        .alternateFunction=0,
        .isAnalog=false,
        .drive=GPIO_DRIVE_2MA,
        .enableSlew=false
    };

    GPIOConfig_t PF2Config = {
        PIN_F2, 
        GPIO_PULL_DOWN,
        true
    };

    /* Initialize a GPIO LED on PF1 and PF2. */
    GPIOPin_t PF1 = GPIOInit(PF1Config);
    GPIOPin_t PF2 = GPIOInit(PF2Config);
    
    EnableInterrupts();

    /* PF1 is default on. */
    GPIOSetBit(PF1, 1);
    GPIOSetBit(PF2, 0);
    while (1) {
        /* Every 1s, toggle PF1 and PF2 LEDs. */
        DelayMillisec(1000);
        
        /* See when running that a blue and red light flash alternately. */
        GPIOSetBit(PF1, !GPIOGetBit(PF1));
        GPIOSetBit(PF2, !GPIOGetBit(PF2));
    };
}
#elif __MAIN__ == 1

uint32_t args[2] = {
    1 /* Number of arguments. */, 
    PIN_F2 /* First argument. */
};

/** This dummy task toggles the GPIO pin specified in args[1]. */
void toggleLED(uint32_t *args) { 
    GPIOSetBit((GPIOPin_t)args[1], !GPIOGetBit((GPIOPin_t)args[1]));
}

int main(void) {
    /**
     * This program demonstrates initializing a GPIO pin with an edge triggered
     * interrupt and executing on it.
     */
    PLLInit(BUS_80_MHZ);
    DisableInterrupts();

    /* Initialize SysTick for delay calls. */
    DelayInit();
    
    GPIOConfig_t PF1Config = {
        .pin=PIN_F1, 
        .pull=GPIO_PULL_DOWN, 
        .isOutput=true,
        .alternateFunction=0,
        .isAnalog=false,
        .drive=GPIO_DRIVE_2MA,
        .enableSlew=false
    };

    GPIOConfig_t PF2Config = {
        PIN_F2, 
        GPIO_PULL_DOWN,
        true
    };

    GPIOConfig_t PF0Config = {
        PIN_F0, 
        GPIO_PULL_UP,
    };

    GPIOInterruptConfig_t PF0IntConfig = {
        .priority=3,
        .touchTask=toggleLED,
        .touchArgs=args,
        .releaseTask=NULL,
        .releaseArgs=NULL,
        .pinStatus=LOWERED
    };

    /* Initialize a GPIO LED on PF1 and PF2. */
    GPIOInit(PF1Config);
    GPIOInit(PF2Config);

    /* Initialize PF0 (SW2) as an edge triggered switch. */
    GPIOIntInit(PF0Config, PF0IntConfig);
    
    EnableInterrupts();

    /* PF1 is default on. */
    GPIOSetBit(PIN_F1, 1);
    GPIOSetBit(PIN_F2, 0);
    while (1) {
        /* Every 1s, toggle PF1 LED on and off. */
        DelayMillisec(1000);
        GPIOSetBit(PIN_F1, !GPIOGetBit(PIN_F1));
    };
}
#endif
