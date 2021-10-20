/**
 * @file I2C.h
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

#pragma once

/** General imports. */
#include <stdint.h>
#include <stdbool.h>


/**
 * @brief I2CModule is an enumeration specifying a set of pins used for I2C
 *        communication. */
enum I2CModule {
    I2C_MODULE_0,   // PB2 SCL
                    // PB3 SDA

    I2C_MODULE_1,   // PA6 SCL
                    // PA7 SDA

    I2C_MODULE_2,   // PE4 SCL
                    // PE5 SDA

    I2C_MODULE_3    // PD0 SCL
                    // PD1 SDA
};

/** @brief I2CMode is an enumeration specifying the I2C function. */
enum I2CMode {
    I2C_MODE_MASTER_TRANSMIT,
    I2C_MODE_MASTER_RECEIVE,
    I2C_MODE_SLAVE_TRANSMIT,
    I2C_MODE_SLAVE_RECEIVE
};

/** @brief I2CSpeed is an enumeration specifying the I2C transmission speed. */
enum I2CSpeed {
    I2C_SPEED_100_KBPS,
    I2C_SPEED_400_KBPS,
    I2C_SPEED_1_MBPS,
    I2C_SPEED_3_33_MBPS,
};

/** @brief I2CConfig_t is a user defined struct that specifies an I2C config. */
typedef struct I2CConfig {
    /** ------------- Optional Fields. ------------- */

    /**
     * @brief The I2C module used for configuration.
     *
     * Default I2C_MODULE_0.
     */
    enum I2CModule module;

    /**
     * @brief The data transmission speed for the I2C module.
     * 
     * Default I2C_SPEED_100_KBPS.
     */
    enum I2CSpeed speed;

    /** ------------- Unimplemented Fields. ------------- */

    // /**
    //  * Used for testing purposes. Feeds I2C back into the MCU.
    //  * 
    //  * Default is false.
    //  */
    // bool isLoopback;

} I2CConfig_t;

/** @brief I2C_t is a struct containing user relevant data of a I2C peripheral. */
typedef struct I2C {
    /** @brief The I2CModule used with communicating to another device. */
    enum I2CModule module;
} I2C_t;

/**
 * @brief I2CInit initializes an I2C module given an I2CConfig_t configuration.
 * 
 * @param config The configuration of the I2C module.
 * @return An I2C_t struct instance used for communication.
 */
I2C_t I2CInit(I2CConfig_t config);

/**
 * @brief I2CMasterTransmit transmits a specified number of bytes across the
 *        data line of a provided I2C.
 * 
 * @param i2c The I2C module to transmit across.
 * @param slaveAddress Address of the slave device to contact.
 * @param bytes Pointer to the data array to send.
 * @param numBytes The number of bytes in the data array to send.
 * @return Whether the transmission was successful or not.
 */
bool I2CMasterTransmit(I2C_t i2c, uint8_t slaveAddress, uint8_t * bytes, uint8_t numBytes);

/**
 * @brief I2CMasterReceive receives a specified number of bytes across the data
 *        line of a provided I2C.
 * 
 * @param i2c The I2C module to receive across.
 * @param slaveAddress Address of the slave device to get data from.
 * @param bytes Pointer to the data array to fill.
 * @param numBytes The number of bytes in the data array to fill.
 * @return Whether the transmission was successful or not.
 */
bool I2CMasterReceive(I2C_t i2c, uint8_t slaveAddress, uint8_t * bytes, uint8_t numBytes);
