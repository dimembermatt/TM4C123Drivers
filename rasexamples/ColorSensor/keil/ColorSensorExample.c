/**
 * File name: ColorSensorExample.c
 * Devices: TM4C123
 * Description: Red, blue, or green board LEDs turn on if sensdor detects a red, blue, or green color object respectively. 
 * Authors: Dario Jimenez
 * Last Modified: 10/18/21
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
#include <lib/I2C/I2C.h>


/** 
 * These function declarations are defined in the startup.s assembly file for
 * managing interrupts. 
 */
void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s

/** Your main execution loop. */
int main(void) {
	
    /* Clock setup. */
    PLLInit(BUS_80_MHZ);
    DisableInterrupts();
		
		/* configuration for red built-in */
		GPIOConfig_t redLED = {
			.pin=PIN_F1,
			.isOutput=true,
			.pull=GPIO_PULL_DOWN,
		};
		
		/* configuration for blue built-in */
		GPIOConfig_t blueLED = {
			.pin=PIN_F2,
			.isOutput=true,
			.pull=GPIO_PULL_DOWN,
		};
		
		/* configuration for green built-in */
		GPIOConfig_t greenLED = {
			.pin=PIN_F3,
			.isOutput=true,
			.pull=GPIO_PULL_DOWN,
		};
		
		/* Initialize GPIO pins */
		GPIOInit(redLED);
		GPIOInit(blueLED);
		GPIOInit(greenLED);

		/* I2C configuration */
    I2CConfig_t i2ccon =  {
				.module=I2C_MODULE_0, // This uses pins PB2 (SCL) and PB3 (SDA).
				.speed=I2C_SPEED_400_KBPS //baud rate of 400 kilobits per second
		};
		
		/* color sensor configuration */
		ColorSensorConfig_t config = {
			.I2CConfig= i2ccon, 
			.isInterrupt=true, //use interrupt 
			.samplingFrequency = 200, //200 hz
			.timerID=TIMER_1A //timer 1A
		};
		
		/* Initialize color sensor */
		ColorSensor_t sensor = ColorSensorInit(config);
		
		EnableInterrupts();
    
    /** Main loop. Put your program here! */
    while (1) {
			
			/* if all RGB values are under 0x10 (no object is likely in front of the sensor) turn off all LEDs */
			if( (sensor.RedValue < 0x10) && (sensor.BlueValue < 0x10) && (sensor.GreenValue < 0x10) ){
				GPIOSetBit(PIN_F1, 0);
				GPIOSetBit(PIN_F2, 0);
				GPIOSetBit(PIN_F3, 0);
			}
			
			else{
				/* turns on red LED if highest RGB value is red (i.e. a red object is placed in front of sensor) */
				if((sensor.RedValue> sensor.GreenValue) && (sensor.RedValue > sensor.BlueValue)) GPIOSetBit(PIN_F1, 1);
				else GPIOSetBit(PIN_F1, 0);
				
				/* turns on blue LED if highest RGB value is blue (i.e. a blue object is placed in front of sensor) */
				if((sensor.BlueValue> sensor.RedValue) && (sensor.BlueValue > sensor.GreenValue)) GPIOSetBit(PIN_F2, 1);
				else GPIOSetBit(PIN_F2, 0);
				
				/* turns on green LED if highest RGB value is green (i.e. a green object is placed in front of sensor) */
				if((sensor.GreenValue> sensor.RedValue) && (sensor.GreenValue > sensor.BlueValue)) GPIOSetBit(PIN_F3, 1);
				else GPIOSetBit(PIN_F3, 0);
									 
			}
    }
}
