/**
 * @file I2CExample.c
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief An example project demonstrating the capabilities of onboard I2C. This
 * particular example requires the TSL2591 AMS light sensor.
 * @version 0.1
 * @date 2021-10-07
 * @copyright Copyright (c) 2021
 */

/** General imports. */
#include <stdio.h>

/** Device specific imports. */
#include <lib/PLL/PLL.h>
#include <lib/I2C/I2C.h>
#include <lib/Timer/Timer.h>
#include "./TSL2591.h"


void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s

int main(void) {
    /**
     * This program demonstrates initializing a TSL2591 and polling it
     * periodically for data.
     */
    PLLInit(BUS_80_MHZ);
    DisableInterrupts();

    /* Initialize SysTick for delay calls. */
    DelayInit();

    /* Initialize an I2C device. */
    I2CConfig_t i2cConfig = {
        .module=I2C_MODULE_0, // This uses pins PB2 (SCL) and PB3 (SDA).
        .speed=I2C_SPEED_100_KBPS
    };

    /* Initialize a TSL2591 sensor. */
    TSL2591Config_t sensorConfig = {
        .i2cConfig=i2cConfig,
        .gain=TSL2591_GAIN_LOW,
        .time=TSL2591_INTT_100MS
    };

    TSL2591_t sensor = TSL2591Init(sensorConfig);

    EnableInterrupts();
    while (1) {
        /* Sample every half second. */
        TSL2591Sample(&sensor);
        DelayMillisec(500);
    };
}
