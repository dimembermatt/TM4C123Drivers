/**
 * I2C.h
 * Devices: LM4F120; TM4C123
 * Description: Low level drivers for I2C management.
 * Authors: Matthew Yu.
 * Last Modified: 09/20/21
 */
#pragma once

/** General imports. */
#include <stdint.h>

/** Device specific imports. */
#include <lib/GPIO/GPIO.h>


/**
 * @brief I2CModule is an enumeration specifying a set of pins used for I2C
 *        communication. */
typedef enum I2CModule {
    I2C_MODULE_0,   // PB2 SCL
                    // PB3 SDA

    I2C_MODULE_1,   // PA6 SCL
                    // PA7 SDA

    I2C_MODULE_2,   // PE4 SCL
                    // PE5 SDA

    I2C_MODULE_3    // PD0 SCL
                    // PD1 SDA
} I2CModule_t;

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

/** Configuration of a single DAC object. */
typedef struct I2CConfig {
    /** ------------- Optional Fields. ------------- */

    /**
     * The I2C module used for configuration.
     *
     * Default I2C_MODULE_0.
     */
    I2CModule_t module;

    /**
     * The data transmission speed for the I2C module.
     * 
     * Default I2C_SPEED_100_KBPS.
     */
    enum I2CSpeed speed;

    /**
     * Error handler that is called when the I2C modules encounter a problem.
     * 
     * Default is hardfault handler.
     */
    void (*I2CErrorHandler)(void);

    /**
     * Used for testing purposes. Feeds I2C back into the MCU.
     * 
     * Default is false.
     */
    bool isLoopback;

    /** ------------- Unimplemented Fields. ------------- */

} I2CConfig_t;

typedef struct I2C {
    /** The I2CModule used with communicating to another device. */
    enum I2CModule module;

    /** Error handler that is called when the I2C modules encounter a problem. */
    void (*I2CErrorHandler)(void);
} I2C_t;


/**
 * I2CInit initializes an I2C module given an I2CConfig_t configuration.
 * 
 * @param config The configuration of the I2C module.
 * @return An I2C_t struct instance used for sampling.
 */
I2C_t I2CInit(I2CConfig_t config);

/**
 * I2CMasterTransmit transmits a specified number of bytes across the data line
 * of a provided I2C.
 * 
 * @param i2c The I2C module to transmit across.
 * @param slaveAddress Address of the slave device to contact.
 * @param bytes Pointer to the data array to send.
 * @param numBytes The number of bytes in the data array to send.
 */
void I2CMasterTransmit(I2C_t i2c, uint8_t slaveAddress, uint8_t * bytes, uint8_t numBytes);

/**
 * I2CMasterReceive receives a specified number of bytes across the data line
 * of a provided I2C.
 * 
 * @param i2c The I2C module to receive across.
 * @param slaveAddress Address of the slave device to get data from.
 * @param bytes Pointer to the data array to fill.
 * @param numBytes The number of bytes in the data array to fill.
 */
void I2CMasterReceive(I2C_t i2c, uint8_t slaveAddress, uint8_t * bytes, uint8_t numBytes);

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
void I2CSlaveProcess(I2C_t i2c, uint8_t slaveAddress, void (*readTask)(uint8_t), uint8_t (*writeTask)(void));

