/**
 * QTRSensor.h
 * Devices: TM4C123, QTR Reflectance Sensor
 * Description: driver for QTR Reflectance Sensor on the TM4C123 board
 * Authors: Dario Jimenez
 * Last Modified: 04/23/202

 **/
#include <stdint.h> //standard integer C library

#include <lib/QTRSensor/QTRSensor.h> //driver header file
#include <lib/QTRSensor/ADCT0ATrigger.h> //ADC initialization library


/* Initializes a pin to be analog and turns on the ADC on that pin.
 * The ADC data is read from an interrupt with a rate of 10Hz.
 * Every 100 ms the sensor data is updated */
void QTRSensorInit(SensorPin_t pin){
	ADC0_InitTimer0ATriggerSeq3(pin, 8000000);
}

/* Returns the most recent data sampled from the ADC and sensor*/
uint32_t readQTRSensor(void){
	return retADCVal();
}


Error_t LineSensor_Init(LineSensorData_t lineSensor, uint8_t numPins){
	if(numPins <= 0 || numPins > 8) return ERROR;

	// initialiazes each pin as an analog input
	for(int i = 0; i < numPins; i++){
		ADC0_InitTimer0ATriggerSeq3(lineSensor.pins[i], 8000000);
	}

	return NO_ERROR;
}

void Read_LineSensor(LineSensorData_t* lineSensor){
	for(int i=0; i<lineSensor->numPins; i++){
		lineSensor->SensorValues[i] = 
	}
}