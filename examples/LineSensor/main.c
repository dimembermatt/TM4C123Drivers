/**
 * File name: main.c
 * Devices: TM4C123
 * Description: Program that demostrates a simple use of the QTR reflectance sensor
 * Authors: Dario Jimenez
 * Last Modified: 04/23/2021
 * 
 * Modify the value of "__MAIN__" on line 15 to choose which program to run:
 * 
 * __MAIN__ = 0: demonstrates initialization & LineSensor_GetBoolArray function
 *							- turn on LEDs depending on boolean values
 * __MAIN__ = 1: demostrates initalization & LineSensor_GetIntegerarray function
 *							- turn on LEDs depending on analog values
 * 
 * Analog pins used: PD3, PD2
 **/

#define __MAIN__ 0

/* Standard C Library Import */
#include <stdlib.h>

/* UTRASWARE Library Imports*/
#include <inc/PLL.h>
#include <lib/LineSensor/LineSensor.h>
#include <lib/GPIO/GPIO.h>
#include <lib/Misc/Misc.h>
#include <lib/ADC/ADC.h>


void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s

/* Program 1: LineSensor_GetBoolArray */
#if __MAIN__ == 0

int main(void) {

    PLL_Init(Bus80MHz); //make clock speed 80Mhz
    DisableInterrupts(); //disable interrupts before initializations

		LineSensorData_t sensor;

		/* Choose microcontroller pins for the QTR Sensor */
		sensor.pins[0] = PD3;
		sensor.pins[1] = PD2;

		/* Initialize PD3 as a GPIO analog input with alternate function 8 (ADC) */
		GPIOConfig_t PD3Config = {
        PIN_D3, 
        NONE, 
    	false, 
        true, 
        8, 
        true
    };

		/* Initialize PD2 as a GPIO analog input with alternate function 8 (ADC) */
		GPIOConfig_t PD2Config = {
        PIN_D2, 
        NONE, 
    	false, 
        true, 
        8, 
        true
    };

		/* Initialize PF1 as a GPIO output */
		GPIOConfig_t PF1Config = {
        PIN_F1, 
        PULL_DOWN, 
        true, 
        false, 
        0, 
        false
    };

	/* Initialize PF2 as a GPIO output */
    GPIOConfig_t PF2Config = {
        PIN_F2, 
        PULL_DOWN, 
        true, 
        false, 
        0, 
        false
    };
		
		/* Initialize PF3 as a GPIO output */
		GPIOConfig_t PF3Config = {
        PIN_F3, 
        PULL_DOWN, 
        true, 
        false, 
        0, 
        false
    };

	/* Initialzation of ADC */
	ADC_Init();

	/* Initialization of GPIO pins */
	GPIOInit(PD3Config);
	GPIOInit(PD2Config);
	GPIOInit(PF3Config);
	GPIOInit(PF2Config);
	GPIOInit(PF1Config);

	/* Initialize QTR sensor with 3 pins (PD3, PD2, PD1) */	
	LineSensor_Init(&sensor, 3);

	/* Initialize SysTick for delay calls.*/
    delayInit();
	
    EnableInterrupts(); //enables interrupts after initializations

	/* Main loop: read line sensor and get boolean array, turn on LEDs depending on values from boolean array */
    while (1) {
			
		//Read from the QTR Reflectance sensors and get boolean array using a threshold of 2048
		LineSensor_GetBoolArray(2048, &sensor);

		/* turn on red LED on board if data from both sensors more than set threshold */
		if(sensor.SensorValues[0] == 1 && sensor.SensorValues[1] == 1) {
			GPIOSetBit(PIN_F1, 1);
   		GPIOSetBit(PIN_F2, 0);
			GPIOSetBit(PIN_F3, 0);
		}
		
		/*	Turn on green LED if one of the sensors' data surpasses the set threshold */
		else if(sensor.SensorValues[0] == 1 || sensor.SensorValues[1] == 1){
				GPIOSetBit(PIN_F3, 1);
				GPIOSetBit(PIN_F1, 0);
				GPIOSetBit(PIN_F2, 0);
			}
			
		/*  Turn on blue LED if none of the sensor's data is more than the threshold */
		else{
			GPIOSetBit(PIN_F1, 0);
 			GPIOSetBit(PIN_F2, 1);
			GPIOSetBit(PIN_F3, 0);
		}
		
		/* Update every 10 milliseconds */
		delayMillisec(10);
    }
}

/* Program 2: LineSensor_GetIntegerArray */

#elif __MAIN__ == 1

int main(void) {

    PLL_Init(Bus80MHz); //make clock speed 80Mhz
    DisableInterrupts(); //disable interrupts before initializations

		LineSensorData_t sensor;

		/* Choose microcontroller pins for the QTR Sensor */
		sensor.pins[0] = PD3;
		sensor.pins[1] = PD2;

		/* Initialize PD3 as a GPIO analog input with alternate function 8 (ADC) */
		GPIOConfig_t PD3Config = {
        PIN_D3, 
        NONE, 
    	false, 
        true, 
        8, 
        true
    };

		/* Initialize PD2 as a GPIO analog input with alternate function 8 (ADC) */
		GPIOConfig_t PD2Config = {
        PIN_D2, 
        NONE, 
    	false, 
        true, 
        8, 
        true
    };

		/* Initialize PF1 as a GPIO output */
		GPIOConfig_t PF1Config = {
        PIN_F1, 
        PULL_DOWN, 
        true, 
        false, 
        0, 
        false
    };

	/* Initialize PF2 as a GPIO output */
    GPIOConfig_t PF2Config = {
        PIN_F2, 
        PULL_DOWN, 
        true, 
        false, 
        0, 
        false
    };
		
		/* Initialize PF3 as a GPIO output */
		GPIOConfig_t PF3Config = {
        PIN_F3, 
        PULL_DOWN, 
        true, 
        false, 
        0, 
        false
    };


	/* Initialzation of ADC */
	ADC_Init();
	
	/* Initialization of GPIO pins */
	GPIOInit(PD3Config);
	GPIOInit(PD2Config);
	GPIOInit(PF3Config);
	GPIOInit(PF2Config);
	GPIOInit(PF1Config);

	/* Initialize QTR sensor with 3 pins (PD3, PD2, PD1) */	
	LineSensor_Init(&sensor, 3);

	/* Initialize SysTick for delay calls.*/
    delayInit();
	
    EnableInterrupts(); //enables interrupts after initializations
		
		/* Main loop: read line sensor and get integer array, turn on LEDs depending on values from integer array */
		while(1){
			
			/* Read raw data (0-4095) from the two sensors and have it stores in "sensor.SensorValues" array */
			LineSensor_GetIntegerArray(&sensor);
			
			/* turn on red LED */
			if(sensor.SensorValues[0] > 2048 && sensor.SensorValues[1] < 1028){
				GPIOSetBit(PIN_F3, 0);
				GPIOSetBit(PIN_F1, 1);
				GPIOSetBit(PIN_F2, 0);
			}
			
			/* turn on green LED */
			else if(sensor.SensorValues[0] < 512 && sensor.SensorValues[1] > 3000){
				GPIOSetBit(PIN_F3, 1);
				GPIOSetBit(PIN_F1, 0);
				GPIOSetBit(PIN_F2, 0);
			}
			
			/* turn on blue LED */
			else if(sensor.SensorValues[0] < 400 && sensor.SensorValues[1] < 400){
				GPIOSetBit(PIN_F3, 0);
				GPIOSetBit(PIN_F1, 0);
				GPIOSetBit(PIN_F2, 1);
			}
			
			/* turn on all LEDs (white) if none of the conditional statements are satisfied */
			else{
				GPIOSetBit(PIN_F3, 1);
				GPIOSetBit(PIN_F1, 1);
				GPIOSetBit(PIN_F2, 1);
			}
		}
	}
#endif
