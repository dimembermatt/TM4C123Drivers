/**
 * File name: main.c
 * Devices: TM4C123
 * Description: program that demostrates the uses of the GP2Y0A60SZ distance sensor and its driver
 * Authors: Dario Jimenez
 * Last Modified: 7/27/2021
 *
 **/


/* Standard C library import */
#include <stdlib.h>

/* TM4C library import */
#include <inc/PLL.h>

/* UTRASWARE library imports */
#include <lib/GPIO/GPIO.h>
#include <lib/ADC/ADC.h>
#include <lib/DistanceSensor/DistanceSensor.h>
#include <lib/Switch/Switch.h>

/** 
 * These function declarations are defined in the startup.s assembly file for
 * managing interrupts. 
 */
void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s

/* Global variables */
DistanceSensor_t sensor; // declaration of distance sensor

/* main program */
int main(void) {
    /** Hardware and software initializations. */

    /* Clock setup. */
    PLL_Init(Bus80MHz);
    DisableInterrupts(); //interrupts should be disabled before initialization

    /* Initialize PD2 GPIO to read analog voltage OUT pin of sensor */
    GPIOConfig_t PD2Config = {PIN_D2, NONE, false, true, 8, true};
	
	/* Initialize PF2 for on-board blue LED */
	GPIOConfig_t PF2Config = {PIN_F2, PULL_DOWN, true, false, 0, false};
	
	/*Initialize PF3 for on-board green LED */
	GPIOConfig_t PF3Config = {PIN_F3, PULL_DOWN, true, false, 0, false};
	
	/* Initialize GPIO pins */
	GPIOInit(PD2Config);
	GPIOInit(PF2Config);
	GPIOInit(PF3Config);
		
	/* initialize the ADC for analog voltage reading */
	ADC_Init();
	
	/** Initialize "sensor" using the optional EN pin (PA7) and using analog PD2 pin for analog reading 
	 *
	 * Must always be called after GPIOInit
	 **/
	DistanceSensor_Init(&sensor, PD2);
	
	/**
	 * setup an interrupt for distance sensor that triggers and reads the analog voltage from OUT
	 * at 200 Hz (every 50 microseconds) using TIMER_0A for the interrupt and setting the interrupt 
	 * to priority 0 (highest priority).
	 *
	 * Must always be called after DistanceSensor_ENInit
	 **/
	DistanceSensor_SetInterrupt(&sensor, 200, TIMER_0A, 0);
	
	/* setup and initialization is finished and now interrupts may be enabled */
	EnableInterrupts();
	
	/* turn off LEDs before main loop */
	GPIOSetBit(PIN_F2, 0);
	GPIOSetBit(PIN_F3, 0);
		
    
    /* Main loop */
    while (1) {
			
			/* turn on blue LED if an analog value from sensor is below 2048 (around 1.65 V) */
			if(sensor.data < 2048){
				
				GPIOSetBit(PIN_F2, 1); //set PF2 to high to turn on blue LED
				GPIOSetBit(PIN_F3, 0); //set PF3 to low to turn off green LED
			}
				
			/* turn on green LED if analof value from sensor is above or equal to 2048 (around 1.65 V) */
			else if(sensor.data >= 2048){
				
				GPIOSetBit(PIN_F2, 0); //set PF2 to low to turn off blue LED
				GPIOSetBit(PIN_F3, 1); //set PF3 to high to turn on gren LED
			}
				
    }
}
