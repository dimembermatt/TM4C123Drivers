/**
 * @file LineSensor.h
 * @author Dario Jimenez, Matthew Yu (matthewjkyu@gmail.com)
 * @brief High level driver for the QTR Reflectance Sensor
 * @version 0.1
 * @date 2021-09-27
 * @copyright Copyright (c) 2021
 * @note
 * QTR reflectance Sensor information:
 * This sensor detects the reflectance of an object.
 * For instance: white reflects a high amount of light, hence the sensor will
 * detect a white object as high reflectance. In the other hand, black reflects
 * low amount of light, hence sensor will detect a black/dark object as low
 * reflectance. The reflectance captured by the sensor is converted into a
 * voltage level from 0 - 3.3V, which can be captured by the TM4C ADC as a
 * 12-bit value (0 - 4095).
 * - The optimal distance for the sensors is 3 mm
 * - Maximum recommended sensing distance is 6 mm
 * 
 * Available TM4C123 pins for QTR sensor: 
 * - PE5, PE4, PE3, PE2, PE1, PE0
 * - PD3, PD2, PD1, PD0
 * - PB5, PB4
 * 
 * Power Specifications:
 * - Supply voltage: 3.3 - 5.0 V
 * - Supply current: 100 mA
 * - Use your 5V regulator! Do not draw from TM4C.
 * 
 * Pinout:
 * - VCC pin: 5V input line or 3.3V input line. If latter, short the 3.3V bypass.
 * - GND pin: GND input line
 * - 1-8 pins: ADC inputs, in order
 * 
 * Summary of driver:
 * - this driver allows you to initialize line sensors, read an unsigned
 *   16-bit value from the sensors, and set up periodic interrupts where data
 *   is read from a sensor at a desired frequency.
 */
#pragma once

/** General imports. */
#include <stdint.h>
#include <stdbool.h>

/** Device specific imports. */
#include <lib/ADC/ADC.h>
#include <lib/Timer/Timer.h>


/** @brief Number of total pins in QTR-8A Reflectance Sensor array. */
#define MAX_PINS_QTR_8 8 

/** @brief Number of total pins in QTR-3A Reflectance Sensor array. */
#define MAX_PINS_QTR_3 3


/**
 * @brief LineSensorConfig_t is a user defined struct that specifies a line
 *        sensor configuration.
 */
typedef struct LineSensorConfig {
    /**
     * @brief The ADC pins tied to each sensor in the QTR-8A reflectance sensor
     * array. It can contain up to MAX_PINS_QTR_8, and users are recommended to
     * assign them in order spatially of your sensor array.
     * 
     * Default for each entry is AIN0 (PE3).
     */
    enum ADCPin pins[MAX_PINS_QTR_8];

    /**
     * @brief The number of pins that should be initialized. This has a upper
     * limit of MAX_PINS_QTR_8 and will be internally fixed to it if exceeded.
     * They correspond to each entry in the pins array starting from index 0 of
     * the array.
     * 
     * Failure to set this a value greater than zero or less than or equal to
     * MAX_PINS_QTR_8 will fail the internal assert.
     */
    uint8_t numPins;

    /** ------------- Optional Fields. ------------- */

    /**
     * @brief The sample frequency of the line sensor, in hz. When defined to be
     * [1, 100] hz, timer 5 with priority 5 is set to execute 
     * LineSensorGetIntArray at that frequency. It fills up the values array.
     * If defined outside of this range, the timer is not enabled.
     * 
     * Default is 0, which is OFF.
     * 
     * Note that calling LineSensorGetIntArray or LineSensorGetBoolArray
     * manually may have its results in the LineSensor struct values member
     * overwritten immediately.
     */
    uint8_t repeatFrequency;

    /**
     * @brief Boolean to determine whether the interrupt call for the line
     * sensor uses LineSensorGetBoolArray or LineSensorGetIntArray.
     * 
     * Default false.
     */
    bool isThresholded;

    /**
     * @brief If isThresholded is enabled, then the threshold value (fixed to
     * (0, 4095]) is provided to the ensuing LineSensorGetBoolArray interrupt
     * call.
     * 
     * Default is 0.
     */
    uint16_t threshold;

    /**
     * @brief The ADC module associated with the ADC Pin. There are two ADC
     * modules in the TM4C123GH6PM and both can operate simultaneously.
     *
     * Default ADC_MODULE_0.
     */
    enum ADCModule module;

    /**
     * @brief The ADC sequencer associated with the ADC Pin. There are four sequencers
     *        PER ADC module. They do not work simultaneously relative to other
     *        sequencers in the module. Multiple enabled sequencers are given
     *        priority in reverse order (e.g. SS0 goes after SS3).
     *
     * Default ADC_SS_0.
     */
    enum ADCSequencer sequencer;

    /**
     * @brief The timer ID used for this line sensor. B side timers and wide
     *        timers are currently unusable.
     * 
     * Default is TIMER_0A.
     */
    TimerID_t timer;
} LineSensorConfig_t;

/**
 * @brief The pregenerated struct used for containing the relevant info used for
 *        manage the LineSensor instance.
 */
typedef struct LineSensor {
    /** 
     * @brief An array of ADC objects that are used in the line sensor. Empty
     * entries will have the value PIN_COUNT, or 48.
     */
    ADC_t adcs[MAX_PINS_QTR_8];

    /**
     * @brief An array of uint16_t values that are associated with each ADC pin.
     * These are filled when called by LineSensorGetIntArray or
     * LineSensorGetBoolArray.
     */
    uint16_t values[MAX_PINS_QTR_8];

    /** 
     * @brief The number of pins that are defined in this LineSensor. Includes
     * duplicates.
     */
    uint8_t numPins;
} LineSensor_t;

/**
 * @brief LineSensorInit initializes the QTR-8A reflectance sensor array.
 * 
 * @param config The configuration of the LineSensor module.
 * @return A LineSensor_t struct instance used for sampling.
 */
LineSensor_t LineSensorInit(LineSensorConfig_t config);

/** 
 * @brief LineSensorGetIntArray reads an array of 16-bit integer sensor values
 * from [0, 4095] sampled at the selected pins of the QTR Reflectance Sensor
 * into the values member of the LineSensor object.
 * 
 * @param sensor A pointer to the LineSensor_t struct.
 * @note The sensor values will be stored in the same index as their respective
 *       pins (i.e: value from PD2 will be stored in SensorValues[5] because PD2
 *       is stores in pins[5]).
 */
void LineSensorGetIntArray(LineSensor_t * sensor);

/**
 * @brief LineSensorGetIntArray reads an array of 16-bit integer sensor values
 * from [0, 4095] sampled at the selected pins of the QTR Reflectance Sensor
 * into the values member of the LineSensor object. Using an input threshold,
 * those values are converted into boolean integers, [0, 1].
 * 
 * @param sensor A pointer to the LineSensor_t struct.
 * @param threshold A threshold value from [0, 4095] that is used to convert the
 *                  integer samples into boolean values.
 *                  Values beyond this range is fixed to the bounds.
 *                  1 if value >= threshold
 *                  0 if value < threshold
 * @note The sensor values will be stored in the same index as their respective
 *       pins (i.e: value from PD2 will be stored in SensorValues[5] because PD2
 *       is stores in pins[5]).
 */
void LineSensorGetBoolArray(LineSensor_t * sensor, uint16_t threshold);
