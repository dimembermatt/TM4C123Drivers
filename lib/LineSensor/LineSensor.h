/**
 * LineSensor.h 
 * Devices: TM4C123
 * Description: Low level driver for QTR Reflectance Sensor
 * Authors: Dario Jimenez
 * Last Modified: 06/28/2021
 * 
 * QTR reflectance Sensor information:
 * This sensor detects the reflectance of an object.
 * For instance: white reflects a high amount of light, hence the sensor will detect a white object as high reflectance.
 * In the other hand, black reflects low amount of light, hence sensor will detect a black/dark object as low reflectance.
 * 
 * Available TM4C123 pins for QTR sensor: PE5, PE3, PE2, PE1, PE0, PD3, PD2, PD1, PD0
 * 
 * Note: Pins must be initialized as GPIO input, analog on, and alternate function 8 (ADC).
 * 
 * Power Specifications:
 * Connect VCC to 3.3V or 5V (VDD)
 * 100 mA input
 * It is STRONGLY RECOMMENDED to use a 5V, 5A regulator at the power input.
 * 
 * Hardware:
 * Connect pins on sensor to analog pins
 * The optimal distance for the sensors is 0.125" (3 mm)
 * Maximum recommended sensing distance: 0.25" (6mm)
 * 
 * Summary of Driver:
 * This driver initializes a specified number of sensors on the QTR as analog inputs;
 * returns the raw data from the sensor as an array of 16-bit values
 * */

#ifndef LineSensor_H
#define LineSensor_H

/* ADC import */
#include <lib/ADC/ADC.h>

/* Standard Library import */
#include <stdint.h>


/* number of total pins in QTR-8A Reflectance Sensor array */
#define MAX_PINS_QTR_8 8 

/* number of total pins in QTR-3A Reflectance Sensor array */
#define MAX_PINS_QTR_3 3



/* Data Structure that holds an array of the pins being used and their corresponding values */
typedef struct LineSensorData{
	AnalogPort_t pins[MAX_PINS_QTR_8];
	uint16_t SensorValues[MAX_PINS_QTR_8];
	uint8_t numPins; 
}LineSensorData_t;

/**
 *  Initiliaze the LineSensor on the chosen pins/ports
 * 
 * Inputs: 1) lineSensor: the linesensor struct with the desired microcontroller pins stored in the "pins" array passed as a pointer (by reference)
 * 		   2) numPins: the number of pins being used
 * 
 * Output: 1) ERROR (1) if unable to initialize properly
 * 		   2) NO_ERROR (0) if everything was initialized properly
 * 
 * note: If an incorrect number is given for numPins parameter, the sensor will not work properly.
 * 		 Similarly, if the pins are not given in the proper order, the sensor will not work properly
 */

Error_t LineSensor_Init(LineSensorData_t *lineSensor, uint8_t numPins);

/** 
 * Read an array if 16-bit integer sensor values from the selected pins of the QTR Reflectance Sensor
 * 
 * Line Sensor must be initialized first.
 * 
 * Inputs: 1) lineSensor: lineSensor struct for the desired sensor passed as a pointer (by reference)
 * 
 * Output: no output, the read values are stored in the SensorValues array of the struct passed as a parameter
 * 
 * Note: the sensor values will be stored in the same index as their respsective pins
 * 		 i.e: value from PD2 will be stored in SensorValues[5] because PD2 is stores in pins[5]
 * 		 Recommended to be used alongside timer functions to run inside an interrupt service routine
 */

void LineSensor_GetIntegerArray(LineSensorData_t *lineSensor);

/**
 * Read an array of boolean sensor values from the selected pins of the QTR Reflectance Sensor Array
 * 
 * Line Sensor must be initialized first.
 * 
 * Inputs: 1) threshold: an integer value between 0 and 4095 to determine boolean values
 * 		   2) lineSensor: the lineSensor structfor the desired sensor passed as a pointer (by reference)
 * 
 * Output: no output, the boolean values will be stored in the SensorValues array of the struct passes as a parameter
 * 		   1) 1 will be stored if sensor data from pin is greater than of equal to the set threshold
 * 		   2) 0 will be stores if sensor data from pin is less than the set threshold
 * 
 * Note: This mehtod can be useful if a filter needs to be added  for preprocessing after the data.
 * 		 the sensor values will be stored in the same index as their respsective pins
 * 		 i.e: value from PD2 will be stored in SensorValues[5] because PD2 is stores in pins[5]
 * 		 Recommended to be used alongside timer functions to run inside an interrupt service routine
 * 
 */

void LineSensor_GetBoolArray(uint16_t threshold, LineSensorData_t *lineSensor);

#endif

