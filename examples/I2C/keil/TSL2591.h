/**
 * @file TSL2591.h
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief High level drivers to control the AMS TSL2591 light sensor.
 * @version 0.1
 * @date 2021-10-07
 * @copyright Copyright (c) 2021
 * @note
 * Unsupported Features. This driver does not support interrupts. This device
 * may require user tuning of the output. A feature to tune the device may be
 * provided at some point in the future.
 */
#pragma once

#include <lib/I2C/I2C.h>


/**
 * @brief TSL2591Gain is an enumeration specifying possible gains used for the
 *        device. */
enum TSL2591Gain {
    TSL2591_GAIN_LOW    = 0x00,
    TSL2591_GAIN_MED    = 0x01,
    TSL2591_GAIN_HIGH   = 0x02,
    TSL2591_GAIN_MAX    = 0x03
};

/**
 * @brief TSL2591Gain is an enumeration specifying possible integration times
 *        used for the device. */
enum TSL2591IntegrationTime {
    TSL2591_INTT_100MS  = 0x00,
    TSL2591_INTT_200MS  = 0x01,
    TSL2591_INTT_300MS  = 0x02,
    TSL2591_INTT_400MS  = 0x03,
    TSL2591_INTT_500MS  = 0x04,
    TSL2591_INTT_600MS  = 0x05
};

/** @brief TSL2591Config_t is a user defined struct that specifies a TSL2591
 *         sensor configuration. */
typedef struct TSL2591Config {
    /** @brief The I2CConfig tied to this sensor. */
    I2CConfig_t i2cConfig;

    /**
     * @brief The gain of the sensor.
     * 
     * Default TSL2591_GAIN_LOW.
     */
    enum TSL2591Gain gain;

    /**
     * @brief The integration time of the sensor.
     * 
     * Default TSL2591_INTT_100MS. (100 ms)
     */
    enum TSL2591IntegrationTime time;
} TSL2591Config_t;

/** @brief TSL2591_t is a struct containing user relevant data for a TSL2591
 *         sensor. */
typedef struct TSL2591 {
    /** @brief The i2C object associated with the sensor. */
    I2C_t i2c;

    /** @brief The raw data of the sensor. */
    uint32_t raw;

    /** @brief The IR component data of the sensor. */
    uint16_t ir;

    /** @brief The full spectrum component data of the sensor. */
    uint16_t full;

    /** @brief The visible spectrum component data of the sensor. */
    uint16_t visible;

    /** @brief The estimated lux measurement of the sensor. */
    uint32_t lux;

    /** @brief The estimated W/m^2 measurement of the sensor. */
    uint32_t irradiance;

    /** @brief The gain of the sensor. */
    enum TSL2591Gain gain;

    /** @brief The integration time of the sensor. */
    enum TSL2591IntegrationTime time;

} TSL2591_t;

/**
 * @brief TSL2591Init initializes a TSL2591 AMS light sensor given a
 *        configuration.
 * 
 * @param config Configuration of the light sensor.
 * @return TSL2591_t User data structure for communication and data collection.
 */
TSL2591_t TSL2591Init(TSL2591Config_t config);

/**
 * @brief TSL2591Disable disables a given TSL2591 sensor.
 * 
 * @param sensor Sensor to disable.
 */
void TSL2591Disable(TSL2591_t * sensor);

/**
 * @brief TSL2591Enable enables a given TSL2591 sensor.
 * 
 * @param sensor Sensor to enable.
 */
void TSL2591Enable(TSL2591_t * sensor);

/**
 * @brief TSL2591Sample samples a given TSL2591 sensor and updates the
 *        measurement values inside the sensor data struct.
 * 
 * @param sensor Sensor to sample.
 */
void TSL2591Sample(TSL2591_t * sensor);
