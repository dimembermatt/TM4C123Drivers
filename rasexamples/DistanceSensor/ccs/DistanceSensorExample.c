/**
 * @file DistanceSensor.c
 * @author Dario Jimenez, Matthew Yu (matthewjkyu@gmail.com)
 * @brief Program that demonstrates the uses of the GP2Y0A60SZ distance sensor and its driver.
 * @version 0.1
 * @date 2021-09-28
 * @copyright Copyright (c) 2021
 * @note
 * Modify the value of "__MAIN__" on line 17 to choose which program to run:
 *
 * __MAIN__ = 0: demonstrates initialization and manual triggering of a distance sensor.
 * __MAIN__ = 1: demonstrates initialization and interrupt capability of a distance sensor.
 *
 * See raslib/DistanceSensor/DistanceSensor.h for wiring and power instructions.
 * Analog pins used: PE3. See lib/ADC/ADC.h for other AINx pins that can be used.
 */
#define __MAIN__ 0

/** General imports. */
#include <stdlib.h>

/** Device specific imports. */
#include <lib/PLL/PLL.h>
#include <lib/GPIO/GPIO.h>
#include <raslib/DistanceSensor/DistanceSensor.h>


/**
 * These function declarations are defined in the startup.s assembly file for
 * managing interrupts.
 */
void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s

#if __MAIN__ == 0
int main(void) {
    /**
     * @brief This program demonstrates intializing a distance sensor on pin PE3
     * and reading from it manually on demand as both integer and boolean values.
     */
    PLLInit(BUS_80_MHZ);
    DisableInterrupts();

    DistanceSensorConfig_t config = {
        .pin=AIN0,
    };

    DistanceSensor_t sensor = DistanceSensorInit(config);

    /* Initialize PF2 as a GPIO output. This is associated with the BLUE led on
       the TM4C. */
    GPIOConfig_t PF2Config = {
        .pin=PIN_F2,
        .pull=GPIO_PULL_DOWN,
        .isOutput=true,
        .alternateFunction=0,
        .isAnalog=false,
        .drive=GPIO_DRIVE_2MA,
        .enableSlew=false
    };

    /* Initialize PF3 as a GPIO output. This is associated with the GREEN led on
       the TM4C. */
    GPIOConfig_t PF3Config = {
        PIN_F3,
        GPIO_PULL_DOWN,
        true
    };
    GPIOInit(PF2Config);
    GPIOInit(PF3Config);

    EnableInterrupts();

    /* Main loop: read line sensor and get boolean value, turn on LEDs depending
       on values from boolean value. */
    while(1) {
        /* Read from the distance sensor. */
        DistanceSensorGetInt(&sensor);

        /* Here, you should check your debugger to see what is inside the sensor
           values array! */

        /* Read from the distance sensor again, but this time using a threshold.
           This threshold corresponds to 2048 / 4095 * 3.3 V. */
        DistanceSensorGetBool(&sensor, 2048);

        /* Turn on GREEN LED if the sensor value is more than set threshold */
        if (sensor.value == 1) {
            GPIOSetBit(PIN_F2, 0);
            GPIOSetBit(PIN_F3, 1);
        }

        /* Turn on BLUE LED if the sensor value is less than the set threshold. */
        else {
            GPIOSetBit(PIN_F2, 1);
            GPIOSetBit(PIN_F3, 0);
        }
    }
}

#elif __MAIN__ == 1
int main(void) {
    /**
     * @brief This program demonstrates initializing a distance sensor on pin
     * PE3, and reading from the sensor automagically using an interrupt on
     * TIMER_0A. It reads at 50 Hz, and does not use thresholding, although it
     * can be enabled.
     */
    PLLInit(BUS_80_MHZ);
    DisableInterrupts();

    DistanceSensorConfig_t config = {
        .pin=AIN0,
        .repeatFrequency=50,
        .isThresholded=false
    };

    DistanceSensor_t sensor = DistanceSensorInit(config);

    /* Initialize PF2 as a GPIO output. This is associated with the BLUE led on
       the TM4C. */
    GPIOConfig_t PF2Config = {
        .pin=PIN_F2,
        .pull=GPIO_PULL_DOWN,
        .isOutput=true,
        .alternateFunction=0,
        .isAnalog=false,
        .drive=GPIO_DRIVE_2MA,
        .enableSlew=false
    };

    /* Initialize PF3 as a GPIO output. This is associated with the GREEN led on
       the TM4C. */
    GPIOConfig_t PF3Config = {
        PIN_F3,
        GPIO_PULL_DOWN,
        true
    };
    GPIOInit(PF2Config);
    GPIOInit(PF3Config);

    EnableInterrupts();

    /* Main loop. */
    while (1) {
        /* Turn on the blue LED if an analog value from sensor is below 2048
           (around 1.65 V). */
        if (sensor.value < 2048){
            GPIOSetBit(PIN_F2, 1); //set PF2 to high to turn on blue LED
            GPIOSetBit(PIN_F3, 0); //set PF3 to low to turn off green LED
        }

        /* Turn on green LED if an analog value from sensor is above or
           equal to 2048 (around 1.65 V). */
        else if (sensor.value >= 2048){
            GPIOSetBit(PIN_F2, 0); //set PF2 to low to turn off blue LED
            GPIOSetBit(PIN_F3, 1); //set PF3 to high to turn on gren LED
        }
    }
}
#endif
