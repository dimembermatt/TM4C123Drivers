/**
 * File name: main.c
 * Devices: TM4C123
 * Description: Program that demostrates a simple use of the QTR reflectance sensor
 * Authors: Dario Jimenez
 * Last Modified: 04/23/2021
 * 
 * Brief Summary of program: 
 * Built-in LED turns blue if sensor value is more than 2047 and red if less then 2047.
 * Range of values for the sensor are 0-4095.
 * In order to get the sensor to detect an object, you need to place object VERY close to the sensor.
 * DO NOT remove/disable interrupts or else sensor won't work
 */

/** General imports. */
#include <stdlib.h>

/** Device specific imports. */
#include <inc/PLL.h>
#include <lib/LineSensor/LineSensor.h>


void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s

int main(void) {

    PLL_Init(Bus80MHz); //make clock speed 80Mhz
    DisableInterrupts(); //disable interrupts during initialization

		LineSensorData_t Sensor;
		Sensor.pins[0] = PD2;
		Sensor.pins[1] = PD1;
		Sensor.numPins = 2;
	
		LineSensor_Init(Sensor);
	
    EnableInterrupts(); //enables interrupts after initialization

		Read_LineSensor(&Sensor);
    while (1) {
			
			 
    }
}

