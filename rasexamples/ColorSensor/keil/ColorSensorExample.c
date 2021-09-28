/**
 * File name: ColorSensorExample.c
 * Devices: TM4C123
 * Description: Red on-board LED is turned on when color sensor detects a red color
 * Authors: Dario Jimenez
 * Last Modified: 9/18/21
 */

/** General imports. These can be declared the like the example below. */
#include <stdlib.h>

/** 
 * Device specific imports. Include files and library files are accessed like
 * the below examples.
 */
#include <lib/PLL/PLL.h>
#include <lib/GPIO/GPIO.h>
#include <lib/Timer/Timer.h>
#include <raslib/ColorSensor/ColorSensor.h>


#define Bus80MHz     4 //temporary



/** 
 * These function declarations are defined in the startup.s assembly file for
 * managing interrupts. 
 */
void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s


/** Your main execution loop. */
int main(void) {
    /** Hardware and software initializations. */

    /* Clock setup. */
    PLLInit(Bus80MHz);
    DisableInterrupts();

    ColorSensor_t sensor; //create instance of ColorSensor_t

    ColorSensor_init(&sensor); //initialize color sensor

    GPIOConfig_t redLED = {
        .pin=PIN_F1, 
        .pull=GPIO_PULL_DOWN, 
        .isOutput=true, 
    };

    GPIOInit(redLED);

    /* sets up an interrupt that updates and checks if values from red sensor surpassed thresholds */
    ColorSensor_SetInterrupt(&sensor, 0, 250, RED, TIMER_0A); //redInterrupt field in sensor will be set to 1 if red value > 250

    EnableInterrupts();
    
    /** Main loop. Put your program here! */
    while (1) {

        // if interrupt flag for red color sensor is set turn on red on board LED
        if(sensor.redInterrupt == 1) GPIOSetBit(PIN_F1, 1); //turn on red LED            

        else GPIOSetBit(PIN_F1, 0); //turn off LED if not set
    }
}
