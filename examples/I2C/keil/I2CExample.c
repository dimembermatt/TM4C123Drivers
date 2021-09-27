/**
 * File name: I2CExample.h
 * Devices: LM4F120; TM4C123
 * Description: Example program to demonstrate the capabilities of the onboard I2C.
 * Authors: Matthew Yu.
 * Last Modified: 09/20/21
 * I2C communication with a TSL2591 light sensor.
 */
#define __MAIN__ 0

/** General imports. */
#include <stdio.h>

/** Device specific imports. */
#include <lib/PLL/PLL.h>
#include <lib/GPIO/GPIO.h>
#include <lib/I2C/I2C.h>
#include <lib/Timer/Timer.h>


void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s

int main(void) {
    /**
     * This program demonstrates initializing a loopback I2C device and sending
     * bytes to itself.
     */
    PLLInit(BUS_80_MHZ);
    DisableInterrupts();

    /* Initialize SysTick for delay calls.*/
    DelayInit();
    
    /* Our visual indicator (Green LED) that the I2C transaction worked. */
    GPIOConfig_t PF3Config = {
        PIN_F3, 
        GPIO_PULL_DOWN,
        true
    };
    GPIOPin_t PF3 = GPIOInit(PF3Config);

    /* Initialize an I2C device. */
    I2CConfig_t i2cConfig = {
        .module=I2C_MODULE_0, // This uses pins PB2 (SCL) and PB3 (SDA).
        .speed=I2C_SPEED_100_KBPS,
        .I2CErrorHandler=NULL,
        .isLoopback=true
    };
    I2C_t i2c = I2CInit(i2cConfig);


    uint8_t data[1] = { 0xA0 | 0x12 };
    uint8_t received[1] = { 0 };
    EnableInterrupts();
    while (1) {
        /* Write 3 bytes and check the ID register for 50. */
        I2CMasterTransmit(i2c, 0x29, data, 1);
        I2CMasterReceive(i2c, 0x29, received, 1);

        if (received[0] == 0x50) {
            GPIOSetBit(PF3, 1);
            while(1) {}
        }
    };
}
