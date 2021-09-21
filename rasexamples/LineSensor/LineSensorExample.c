/**
 * File name: LineSensorExample.c
 * Devices: TM4C123
 * Description: Program that demonstrates a simple use of the QTR reflectance sensor.
 * Authors: Dario Jimenez, Matthew Yu
 * Last Modified: 09/21/2021
 *
 * Modify the value of "__MAIN__" on line 15 to choose which program to run:
 *
 * __MAIN__ = 0: demonstrates initialization and manual triggering of a line sensor.
 * __MAIN__ = 1: demostrates initalization and interrupt capability of a line sensor.
 *
 * Analog pins used: PD3, PD2.
 **/
#define __MAIN__ 0

/** General imports. */
#include <stdlib.h>

/** Device specific imports. */
#include <inc/PLL.h>
#include <raslib/LineSensor/LineSensor.h>
#include <lib/Misc/Misc.h>
#include <lib/GPIO/GPIO.h>


void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s

#if __MAIN__ == 0
int main(void) {
    /**
     * This program demonstrates intializing a line sensor with 2 pins, PD3 and
     * PD2 and reading from it manually on demand as both integer and boolean
     * values.
     */
    PLLInit(BUS_80_MHZ);
    DisableInterrupts();

    LineSensorConfig_t config = {
        .pins={AIN4, AIN5},
        .numPins=2,
    };

    /* Initialzation of ADC */
    LineSensor_t sensor = LineSensorInit(config);

    /* Initialize SysTick for delay calls.*/
    delayInit();

    /* Initialize PF1 as a GPIO output. This is associated with the RED led on
       the TM4C. */
    GPIOConfig_t PF1Config = {
        PIN_F1, 
        PULL_DOWN, 
        true, 
        false, 
        0, 
        false
    };

    /* Initialize PF2 as a GPIO output. This is associated with the BLUE led on
       the TM4C. */
    GPIOConfig_t PF2Config = {
        PIN_F2, 
        PULL_DOWN, 
        true, 
        false, 
        0, 
        false
    };
    
    /* Initialize PF3 as a GPIO output. This is associated with the GREEN led on
       the TM4C. */
    GPIOConfig_t PF3Config = {
        PIN_F3, 
        PULL_DOWN, 
        true, 
        false, 
        0, 
        false
    };
	GPIOInit(PF1Config);
	GPIOInit(PF2Config);
	GPIOInit(PF3Config);

    EnableInterrupts();

    /* Main loop: read line sensor and get boolean array, turn on LEDs depending
       on values from boolean array. */
    while(1) {
        /* Read from the line sensor. */
        LineSensorGetIntArray(&sensor);

        /* Here, you should check your debugger to see what is inside the sensor
           values array! */

        delayMillisec(50);

        /* Read from the line sensor again, but this time using a threshold.
           This threshold corresponds to 2048 / 4095 * 3.3 V. */
        LineSensorGetBoolArray(&sensor, 2048);

        /* Turn on GREEN LED on board if data from both sensors more than set threshold */
        if(sensor.values[0] == 1 && sensor.values[1] == 1) {
            GPIOSetBit(PIN_F1, 0);
            GPIOSetBit(PIN_F2, 0);
            GPIOSetBit(PIN_F3, 1);
        }

        /* Turn on BLUE LED if one of the sensors' data surpasses the set threshold. */
        else if(sensor.values[0] == 1 || sensor.values[1] == 1) {
            GPIOSetBit(PIN_F1, 0);
            GPIOSetBit(PIN_F2, 1);
            GPIOSetBit(PIN_F3, 0);
        }

        /* Turn on RED LED if none of the sensor's data is more than the threshold. */
        else {
            GPIOSetBit(PIN_F1, 1);
            GPIOSetBit(PIN_F2, 0);
            GPIOSetBit(PIN_F3, 0);
        }

        /* Delay another 50 ms. */
        delayMillisec(50);
    }
}

#elif __MAIN__ == 1
int main(void) {
    /**
     * This program demonstrates intializing a line sensor with 2 pins, PD3 and
     * PD2 and having it read from the sensor automagically using an interrupt
     * on TIMER3. It reads at 20 Hz. It also uses a thresholding feature to
     * convert values into a boolean array based on a threshold.
     */
    PLLInit(BUS_80_MHZ);
    DisableInterrupts();

    LineSensorConfig_t config = {
        .pins={AIN4, AIN5},
        .numPins=2,
        .repeatFrequency=20,
        .isThresholded=true,
        .threshold=2048 // This threshold corresponds to 2048 / 4095 * 3.3 V.
    };

    /* Initialzation of ADC */
    LineSensor_t sensor = LineSensorInit(config);

    /* Initialize SysTick for delay calls.*/
    delayInit();

    /* Initialize PF1 as a GPIO output. This is associated with the RED led on
       the TM4C. */
    GPIOConfig_t PF1Config = {
        PIN_F1, 
        PULL_DOWN, 
        true, 
        false, 
        0, 
        false
    };

    /* Initialize PF2 as a GPIO output. This is associated with the BLUE led on
       the TM4C. */
    GPIOConfig_t PF2Config = {
        PIN_F2, 
        PULL_DOWN, 
        true, 
        false, 
        0, 
        false
    };
    
    /* Initialize PF3 as a GPIO output. This is associated with the GREEN led on
       the TM4C. */
    GPIOConfig_t PF3Config = {
        PIN_F3, 
        PULL_DOWN, 
        true, 
        false, 
        0, 
        false
    };
	GPIOInit(PF1Config);
	GPIOInit(PF2Config);
	GPIOInit(PF3Config);

    EnableInterrupts();

    /* Main loop: read line sensor and get boolean array, turn on LEDs depending on values from boolean array */
    while(1) {
        /* Turn on GREEN LED on board if data from both sensors more than set threshold */
        if(sensor.values[0] == 1 && sensor.values[1] == 1) {
            GPIOSetBit(PIN_F1, 0);
            GPIOSetBit(PIN_F2, 0);
            GPIOSetBit(PIN_F3, 1);
        }

        /* Turn on BLUE LED if one of the sensors' data surpasses the set threshold. */
        else if(sensor.values[0] == 1 || sensor.values[1] == 1) {
            GPIOSetBit(PIN_F1, 0);
            GPIOSetBit(PIN_F2, 1);
            GPIOSetBit(PIN_F3, 0);
        }

        /* Turn on RED LED if none of the sensor's data is more than the threshold. */
        else {
            GPIOSetBit(PIN_F1, 1);
            GPIOSetBit(PIN_F2, 0);
            GPIOSetBit(PIN_F3, 0);
        }
    }
}
#endif
