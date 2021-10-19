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
    /** Hardware and software initializations. */

    /* Clock setup. */
    PLLInit(BUS_80_MHZ);
    DisableInterrupts();

    I2CConfig_t i2ccon =  {
				.module=I2C_MODULE_0, // This uses pins PB2 (SCL) and PB3 (SDA).
				.speed=I2C_SPEED_400_KBPS};
		
		TCS34725Config_t config = {
		.I2CConfig= i2ccon, 
		.Gain=TCS34725_Gain_1X, 
		.IntegrationTime=TCS34725_INT_TIME_2MS,
		.isLong=false};
		
		TCS34725_t sensor = TCS34725Init(config);
    
    /** Main loop. Put your program here! */
    while (1) {

      TCS34725Read(&sensor);
			//DelayMillisec(500);
			 
    }
}
