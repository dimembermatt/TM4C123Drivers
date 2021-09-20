/**
 * File name: I2CExample.h
 * Devices: LM4F120; TM4C123
 * Description: Example program to demonstrate the capabilities of the onboard I2C.
 * Authors: Matthew Yu.
 * Last Modified: 09/20/21
 * 
 * Modify __MAIN__ on L12 to determine which main method is executed.
 * __MAIN__ = 0 - Loopback communication with I2C.
 *          = 1 - I2C communication with a TSL2591 light sensor.
 */
#define __MAIN__ 0

/** General imports. */
#include <stdio.h>

/** Device specific imports. */
#include <inc/PLL.h>
#include <lib/GPIO/GPIO.h>
#include <lib/I2C/I2C.h>
#include <lib/Misc/Misc.h>


void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s

#if __MAIN__ == 0
int main(void) {
    /**
     * This program demonstrates initializing a loopback I2C device and sending
     * bytes to itself.
     */
    PLLInit(BUS_80_MHZ);
    DisableInterrupts();

    /* Initialize SysTick for delay calls.*/
    delayInit();
    
    /* Initialize an I2C device. */
    I2CConfig_t i2cConfig = {
        .module=I2C_MODULE_0, // This uses pins PB2 (SCL) and PB3 (SDA).
        .speed=I2C_SPEED_100_KBPS,
        .I2CErrorHandler=NULL,
        .isLoopback=true
    };
    I2C_t i2c = I2CInit(i2cConfig);


    uint8_t data[3] = {'I', '2', 'C'};
    EnableInterrupts();
    while (1) {
        /* Write 3 bytes . */
        I2CMasterTransmit(i2c, 0x3C, data, 3);
    };
}
#elif __MAIN__ == 1
#endif
