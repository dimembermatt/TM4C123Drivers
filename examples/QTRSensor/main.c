/**
 * File name: main.c
 * Devices: TM4C123
 * Description: Program that demostrates use of QTR reflectance sensor
 * Authors: Dario Jimenez
 * Last Modified: 04/23/2021
 * 
 * Brief Summary of program: 
 * Built-in turns blue is sensor value is more than 2047 and red if less then 2047.
 * Range of values for the sensor are 0-4095.
 * In order to get the sensor to detect an object, you need to place object VERY close to the sensor.
 * DO NOT remove/disable interrupts or else sensor won't work
 */

/** General imports. */
#include <stdlib.h>

/** Device specific imports. */
#include <inc/PLL.h>
#include <lib/GPIO/GPIO.h>
#include <lib/Misc/Misc.h>
#include <lib/QTRSensor/QTRSensor.h>


void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s

int main(void) {

    PLL_Init(Bus80MHz); //make clock speed 80Mhz
    DisableInterrupts(); //disable interrupts during initialization

    /* Initialize SysTick for delay calls.*/
    delayInit();
    
	/* configuration for pin PF1 (built-in LED) */
    GPIOConfig_t PF1Config = {
        PIN_F1, 
        PULL_DOWN, 
        true, 
        false, 
        0, 
        false
    };

		/* configuration for pin PF1 (built-in LED) */
    GPIOConfig_t PF2Config = {
        PIN_F2, 
        PULL_DOWN, 
        true, 
        false, 
        0, 
        false
    };

    /* Initialize a GPIO LED on PF1 and PF2. */
    GPIOInit(PF1Config);
    GPIOInit(PF2Config);
		
		/* Initializes QTR sensor on pin PE3 */
		QTRSensorInit(PE3);
    
    EnableInterrupts(); //enables interrupts after initialization


    while (1) {
			
			  /* turn off LED */
			GPIOSetBit(PIN_F1, 0);
			GPIOSetBit(PIN_F2, 0);
			
       
			/* read data from QTR sensor */
			volatile uint32_t data = readQTRSensor();
			
			/* conditional statement */
			if(data < 2047){
				GPIOSetBit(PIN_F1, 1); // turn on red LED
				delayMillisec(1000); //wait a second until reading data from sensor again
			}
			else{
				GPIOSetBit(PIN_F2, 1); //turn on blue LED
				delayMillisec(1000); //wait a second until reading data from sensor again
			}
    }
}
