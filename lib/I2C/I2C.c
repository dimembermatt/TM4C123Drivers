/**
 * @file I2C.c
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief Low level drivers for I2C management.
 * @version 0.1
 * @date 2021-10-07
 * @copyright Copyright (c) 2021
 * @note
 * Unsupported Features. This driver does not support the TM4C as a slave
 * device. This device does not support interrupts. This device does not support
 * loopback mode, glitch filters, high speed mode, or simultaneous master/slave
 * mode.
 */

/** General Imports. */
#include <stdlib.h>
#include <assert.h>

/** Device specific imports. */
#include <inc/RegDefs.h>
#include <lib/I2C/I2C.h>
#include <lib/GPIO/GPIO.h>


static GPIOPin_t I2CPinMapping[4][2] = {
    //SCL    //SDA
    {PIN_B2, PIN_B3},
    {PIN_A6, PIN_A7},
    {PIN_E4, PIN_E5},
    {PIN_D0, PIN_D1},
};

static uint32_t I2CSpeedMapping[4] = {
    100000,
    400000,
    1000000,
    3330000
};

I2C_t I2CInit(I2CConfig_t config) {
    /* Initialization asserts. */
    assert(config.module <= I2C_MODULE_3);
    assert(config.speed <= I2C_SPEED_3_33_MBPS);

    /* 1. Activate the clock for RCGCI2C. */
    GET_REG(SYSCTL_BASE + SYSCTL_RCGCI2C_OFFSET) |=
        1 << config.module;

    /* 2. Stall until clock is ready. */
    while ((GET_REG(SYSCTL_BASE + SYSCTL_PRI2C_OFFSET) &
        (1 << config.module)) == 0) {};

    /* 3. Enable SCL and SDA GPIO pins. */
    GPIOConfig_t sclPin = {
        I2CPinMapping[config.module][0],
        GPIO_TRI_STATE, /* Do NOT configure SCL as open drain. */
        true,
        3,
        false,
        GPIO_DRIVE_2MA,
        false
    };
    GPIOInit(sclPin);
    GPIOConfig_t sdaPin = {
        I2CPinMapping[config.module][1],
        GPIO_OPEN_DRAIN,
        true,
        3,
        false,
        GPIO_DRIVE_2MA,
        false
    };
    GPIOInit(sdaPin);

    uint32_t moduleBase = config.module * 0x1000 + I2C0_BASE;

    /* 4. Set transmission mode. */
    /* Glitch filter is disabled, master mode enabled, and loopback mode is on. */
    GET_REG(moduleBase + I2C_MCR_OFFSET) = 0x10;

    /* If in master mode. */
    /* 5. Select clock speed. */
    /* Default 80 MHz. */
    uint32_t tpr = 0;
    if (config.speed == I2C_SPEED_3_33_MBPS) {
        #define SCL_LP 6
        #define SCL_HP 4
        tpr = 80000000 / (2 * (SCL_LP + SCL_HP) * I2CSpeedMapping[config.speed]) - 1;
        #undef SCL_LP
        #undef SCL_HP
    } else {
        #define SCL_LP 2
        #define SCL_HP 1
        tpr = 80000000 / (2 * (SCL_LP + SCL_HP) * I2CSpeedMapping[config.speed]) - 1;
        #undef SCL_LP
        #undef SCL_HP
    }
    GET_REG(moduleBase + I2C_MTPR_OFFSET) = tpr;

    I2C_t i2c = {
        .module=config.module
    };

    /* If in slave mode. */
    // GET_REG(moduleBase + I2C_SOAR_OFFSET) = 0x3C;
    // GET_REG(moduleBase + I2C_SCSR_OFFSET) = 1;

    return i2c;
}

/* Hidden test methods. Remove eventually. */
bool I2CTx1(I2C_t i2c, uint8_t slaveAddress, uint8_t byte) {
    uint32_t moduleBase = i2c.module * 0x1000 + I2C0_BASE;

    /* 1. Wait for I2C ready. */
    while (GET_REG(moduleBase + I2C_MCS_OFFSET) & 0x1) {}

    /* 2. Write slave address to I2CMSA in transmit mode. */
    GET_REG(moduleBase + I2C_MSA_OFFSET) = (slaveAddress << 1) & 0xFE;

    /* 3. Write data to I2CMDR. */
    GET_REG(moduleBase + I2C_MDR_OFFSET) = byte;

    /* 4. Generate stop, start, run bits. */
    GET_REG(moduleBase + I2C_MCS_OFFSET) = 0b0111;

    /* 5. Wait for I2C ready. */
    while (GET_REG(moduleBase + I2C_MCS_OFFSET) & 0x1) {}

    /* 6. Return result. False is failure. */ 
    return !(GET_REG(moduleBase + I2C_MCS_OFFSET) & 0x0E);
}

bool I2CRx1(I2C_t i2c, uint8_t slaveAddress, uint8_t * byte) {
    uint32_t moduleBase = i2c.module * 0x1000 + I2C0_BASE;

    /* 1. Wait for I2C ready. */
    while (GET_REG(moduleBase + I2C_MCS_OFFSET) & 0x1) {}

    /* 2. Write slave address to I2CMSA in receive mode. */
    GET_REG(moduleBase + I2C_MSA_OFFSET) = ((slaveAddress << 1) & 0xFE) | 0x1;
    
    /* 3. Generate stop, start, run bits. */
    GET_REG(moduleBase + I2C_MCS_OFFSET) = 0b0111;

    /* 4. Wait for I2C ready. */
    while (GET_REG(moduleBase + I2C_MCS_OFFSET) & 0x1) {}

    /* 3. Read data from I2CMDR, regardless if correct or not. */
    *byte = GET_REG(moduleBase + I2C_MDR_OFFSET) & 0xFF;

    /* 6. Return result. False is failure. */ 
    return !(GET_REG(moduleBase + I2C_MCS_OFFSET) & 0x06);
}

bool I2CMasterTransmit(I2C_t i2c, uint8_t slaveAddress, uint8_t * bytes, uint8_t numBytes) {
    uint32_t moduleBase = i2c.module * 0x1000 + I2C0_BASE;

    /* 1. Wait for I2C ready. */
    while (GET_REG(moduleBase + I2C_MCS_OFFSET) & 0x1) {}

    /* 2. Write slave address to I2CMSA in transmit mode. */
    GET_REG(moduleBase + I2C_MSA_OFFSET) = (slaveAddress << 1) & 0xFE;

    /* For each byte of data to send. */
    uint8_t i;
    for (i = 0; i < numBytes; ++i) {
        /* 3. Write data to I2CMDR. */
        GET_REG(moduleBase + I2C_MDR_OFFSET) = bytes[i];

        if (i == 0) {
            /* First byte. */
            if (numBytes == 1) {
                /* Special case of a single byte. */
                /* 4a. Generate stop, start, run bits. */
                GET_REG(moduleBase + I2C_MCS_OFFSET) = 0b0111;
            } else {
                /* 4a. Generate start, run bits. */
                GET_REG(moduleBase + I2C_MCS_OFFSET) = 0b0011;
            }
        } else if (i == numBytes - 1) {
            /* Nth byte. */
            /* 4b. Generate stop, run bits. */
            GET_REG(moduleBase + I2C_MCS_OFFSET) = 0b0101;
        } else {
            /* Kth byte. */
            /* 4c. Generate run bit. */
            GET_REG(moduleBase + I2C_MCS_OFFSET) = 0b0001;
        }

        /* 5. Wait for transmission completion. */
        while (GET_REG(moduleBase + I2C_MCS_OFFSET) & 0x1) {}

        /* 6. Check for error. */
        bool error = GET_REG(moduleBase + I2C_MCS_OFFSET) & 0x0E;
        if (error) {
            /* Generate stop bit. */
            GET_REG(moduleBase + I2C_MCS_OFFSET) = 0b0100;
            while (GET_REG(moduleBase + I2C_MCS_OFFSET) & 0x1) {}
            return false;
        }
    }

    return true;
}

bool I2CMasterReceive(I2C_t i2c, uint8_t slaveAddress, uint8_t bytes[], uint8_t numBytes) {
    uint32_t moduleBase = i2c.module * 0x1000 + I2C0_BASE;

    /* 1. Wait for I2C ready. */
    while (GET_REG(moduleBase + I2C_MCS_OFFSET) & 0x1) {}

    /* 2. Write slave address to I2CMSA in receive mode. */
    GET_REG(moduleBase + I2C_MSA_OFFSET) = ((slaveAddress << 1) & 0xFE) | 0x1;

    /* For each byte of data to receive. */
    uint8_t i;
    for (i = 0; i < numBytes; ++i) {
        if (i == 0) {
            /* First byte. */
            if (numBytes == 1) {
                /* Special case of a single byte. */
                /* 3a. Generate stop, start, run bits. */
                GET_REG(moduleBase + I2C_MCS_OFFSET) = 0b0111;
            } else {
                /* 3a. Generate ack, start, run bits. */
                GET_REG(moduleBase + I2C_MCS_OFFSET) = 0b1011;
            }
        } else if (i == numBytes - 1) {
            /* Nth byte. */
            /* 3b. Generate stop, run bit. */
            GET_REG(moduleBase + I2C_MCS_OFFSET) = 0b0101;
        } else {
            /* Kth byte. */
            /* 3c. Generate ack, run bits. */
            GET_REG(moduleBase + I2C_MCS_OFFSET) = 0b1001;
        }

        /* 4. Wait for transmission completion. */
        while (GET_REG(moduleBase + I2C_MCS_OFFSET) & 0x1) {}

        /* 5. Write data to I2CMDR, regardless if correct or not. */
        bytes[i] = GET_REG(moduleBase + I2C_MDR_OFFSET) & 0xFF;

        /* 6. Check for error. */
        bool error = GET_REG(moduleBase + I2C_MCS_OFFSET) & 0x6;
        if (error) {
            /* Generate stop bit. */
            GET_REG(moduleBase + I2C_MCS_OFFSET) = 0b0100;
            while (GET_REG(moduleBase + I2C_MCS_OFFSET) & 0x1) {}
            return false;
        }
    }

    return true;
}


/**
 * I2CSlaveProcess manages a single byte request across the I2C line.
 * 
 * @param i2c The I2C module to talk with.
 * @param slaveAddress I2C address of the self.
 * @param readTask  Pointer to a function to call when reading the byte from the
 *                  I2C line.
 * @param writeTask Pointer to a function to call to generate the data to write
 *                  across the I2C line.
 * @note readTask and writeTask, when undefined, will do nothing with the read
 *       data or will write 0xFF to the I2C line, respectively.
 * @note It is recommended that readTask and writeTask are written to be
 *       nonintrusive and FAST. Ideally, the function moves data from another
 *       place and performs more CPU intensive calculations outside of the
 *       I2CSlaveProcess parent call.
 */
// void I2CSlaveProcess(I2C_t i2c, uint8_t slaveAddress, void (*readTask)(uint8_t), uint8_t (*writeTask)(void)) {
//     uint32_t moduleBase = i2c.module * 0x1000 + I2C0_BASE;

//     /* Write own slave address to I2CSOAR. */
//     GET_REG(moduleBase + I2C_SOAR_OFFSET) = slaveAddress << 1;

//     /* Write to device activate bit I2CSCSR. */
//     GET_REG(moduleBase + I2C_SCSR_OFFSET) = 1;

//     /* Determine Read request or write request. */
//     while (!(GET_REG(moduleBase + I2C_SCSR_OFFSET) & 0x03)) {}
//     if (GET_REG(moduleBase + I2C_SCSR_OFFSET) & 0x01) {
//         /* Upon read request, read data from I2CSDR. */
//         uint8_t data = GET_REG(moduleBase + I2C_SDR_OFFSET) & 0xFF;
//         if (readTask != NULL)
//             readTask(data);
//     } else if (GET_REG(moduleBase + I2C_SCSR_OFFSET & 0x02)) {
//         /* Upon transmit request, write data to I2CSDR. */

//         if (writeTask != NULL) {
//             uint8_t data = writeTask();
//             GET_REG(moduleBase + I2C_SDR_OFFSET) = data;
//         } else {
//             GET_REG(moduleBase + I2C_SDR_OFFSET) = 0xFF;
//         }
//     } else {
//         /* Illegal state. Goto error handler. */
//         i2c.I2CErrorHandler();
//     }
// }
