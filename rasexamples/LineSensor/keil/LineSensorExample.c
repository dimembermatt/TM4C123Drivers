/**
 * @file LineSensorExample.c
 * Authors: Dario Jimenez, Matthew Yu (matthewjkyu@gmail.com)
 * @brief Program that demonstrates a simple use of the QTR reflectance sensor.
 * @version 0.1
 * @date 2021-09-28
 * @copyright Copyright (c) 2021
 * @note
 * Modify the value of "__MAIN__" on line 16 to choose which program to run:
 *
 * __MAIN__ = 0: demonstrates initialization and manual triggering of a line sensor.
 * __MAIN__ = 1: demonstrates initialization and interrupt capability of a line sensor.
 *
 * Analog pins used: PE3, PE2, PE1, PE0, PD3, PD2, PD1, and PD0.
 */
#define __MAIN__ 0

/** General imports. */
#include <stdlib.h>

/** Device specific imports. */
#include <lib/PLL/PLL.h>
#include <lib/GPIO/GPIO.h>
#include <raslib/LineSensor/LineSensor.h>


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
     * @brief This program demonstrates intializing a line sensor with 8 pins:
     * PE3, PE2, PE1, PE0, PD3, PD2, PD1, and PD0 and reading from it manually
     * on demand as both integer and boolean values.
     */
    PLLInit(BUS_80_MHZ);
    DisableInterrupts();

    LineSensorConfig_t config = {
        .pins={AIN1, AIN2, AIN3, AIN4, AIN5, AIN6, AIN7, AIN8},
        .numPins=8,
    };

    /* Initialization of ADC */
    LineSensor_t sensor = LineSensorInit(config);

    /* Initialize PF1 as a GPIO output. This is associated with the RED led on
       the TM4C. */
    GPIOConfig_t PF1Config = {
        .pin=PIN_F1,
        .pull=GPIO_PULL_DOWN,
        .isOutput=true,
        .alternateFunction=0,
        .isAnalog=false,
        .drive=GPIO_DRIVE_2MA,
        .enableSlew=false
    };

    /* Initialize PF2 as a GPIO output. This is associated with the BLUE led on
       the TM4C. */
    GPIOConfig_t PF2Config = {
        PIN_F2,
        GPIO_PULL_DOWN,
        true
    };

    /* Initialize PF3 as a GPIO output. This is associated with the GREEN led on
       the TM4C. */
    GPIOConfig_t PF3Config = {
        PIN_F3,
        GPIO_PULL_DOWN,
        true
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

        /* Read from the line sensor again, but this time using a threshold.
           This threshold corresponds to 2048 / 4095 * 3.3 V. */
        LineSensorGetBoolArray(&sensor, 2048);

        uint8_t avgSide = 0;
        uint8_t i;
        for (i = 0; i < 8; ++i) {
            avgSide += sensor.values[i] << i;
        }

        /* Turn on RED LED if sensor data is none across the board. */
        if (avgSide == 0) {
            GPIOSetBit(PIN_F1, 1);
            GPIOSetBit(PIN_F2, 0);
            GPIOSetBit(PIN_F3, 0);
        }
        /* Turn on GREEN LED if sensor data is tending towards the left side. */
        else if (avgSide >= 0x10) {
            GPIOSetBit(PIN_F1, 0);
            GPIOSetBit(PIN_F2, 0);
            GPIOSetBit(PIN_F3, 1);
        }
        /* Turn on BLUE LED if sensor data is tending towards the right side. */
        else {
            GPIOSetBit(PIN_F1, 0);
            GPIOSetBit(PIN_F2, 1);
            GPIOSetBit(PIN_F3, 0);
        }
    }
}

#elif __MAIN__ == 1
int main(void) {
    /**
     * @brief This program demonstrates intializing a line sensor with 8 pins:
     * PE3, PE2, PE1, PE0, PD3, PD2, PD1, and PD0 and reading from it
     * automagically using an interrupt on TIMER0A. It reads at 20 Hz. It also
     * uses a thresholding feature to convert values into a boolean array based
     * on a threshold.
     */
    PLLInit(BUS_80_MHZ);
    DisableInterrupts();

    LineSensorConfig_t config = {
        .pins={AIN1, AIN2, AIN3, AIN4, AIN5, AIN6, AIN7, AIN8},
        .numPins=8,
        .repeatFrequency=20,
        .isThresholded=true,
        .threshold=2048 // This threshold corresponds to 2048 / 4095 * 3.3 V.
        // Uses ADC Module 0, Sequencer 0, Timer 0A by default.
    };

    /* Initialization of ADC. */
    LineSensor_t sensor = LineSensorInit(config);

    /* Initialize PF1 as a GPIO output. This is associated with the RED led on
       the TM4C. */
    GPIOConfig_t PF1Config = {
        .pin=PIN_F1,
        .pull=GPIO_PULL_DOWN,
        .isOutput=true,
        .alternateFunction=0,
        .isAnalog=false,
        .drive=GPIO_DRIVE_2MA,
        .enableSlew=false
    };

    /* Initialize PF2 as a GPIO output. This is associated with the BLUE led on
       the TM4C. */
    GPIOConfig_t PF2Config = {
        PIN_F2,
        GPIO_PULL_DOWN,
        true
    };

    /* Initialize PF3 as a GPIO output. This is associated with the GREEN led on
       the TM4C. */
    GPIOConfig_t PF3Config = {
        PIN_F3,
        GPIO_PULL_DOWN,
        true
    };
    GPIOInit(PF1Config);
    GPIOInit(PF2Config);
    GPIOInit(PF3Config);


    EnableInterrupts();

    /* Main loop: read line sensor and get boolean array, turn on LEDs depending on values from boolean array */
    while(1) {
        uint8_t avgSide = 0;
        uint8_t i;
        for (i = 0; i < 8; ++i) {
            avgSide += sensor.values[i] << i;
        }

        /* Turn on RED LED if sensor data is none across the board. */
        if (avgSide == 0) {
            GPIOSetBit(PIN_F1, 1);
            GPIOSetBit(PIN_F2, 0);
            GPIOSetBit(PIN_F3, 0);
        }
        /* Turn on GREEN LED if sensor data is tending towards the left side. */
        else if (avgSide >= 0x10) {
            GPIOSetBit(PIN_F1, 0);
            GPIOSetBit(PIN_F2, 0);
            GPIOSetBit(PIN_F3, 1);
        }
        /* Turn on BLUE LED if sensor data is tending towards the right side. */
        else {
            GPIOSetBit(PIN_F1, 0);
            GPIOSetBit(PIN_F2, 1);
            GPIOSetBit(PIN_F3, 0);
        }
    }
}
#endif
