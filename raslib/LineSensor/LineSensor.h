/**
 * LineSensor.h 
 * Devices: TM4C123
 * Description: High level driver for QTR Reflectance Sensor
 * Authors: Dario Jimenez, Matthew Yu
 * Last Modified: 09/21/2021
 * 
 * QTR reflectance Sensor information:
 * This sensor detects the reflectance of an object.
 * For instance: white reflects a high amount of light, hence the sensor will
 * detect a white object as high reflectance. In the other hand, black reflects
 * low amount of light, hence sensor will detect a black/dark object as low
 * reflectance. The reflectance captured by the sensor is converted into a
 * voltage level from 0 - 3.3V, which can be captured by the TM4C ADC as a
 * 12-bit value (0 - 4095).
 * 
 * Available TM4C123 pins for QTR sensor: 
 * - PE5, PE4, PE3, PE2, PE1, PE0
 * - PD3, PD2, PD1, PD0
 * - PB5, PB4
 * 
 * Power Specifications:
 * Connect VCC to 3.3V or 5V (VDD)
 * 100 mA input
 * It is STRONGLY RECOMMENDED to use a 5V, 5A regulator at the power input.
 * 
 * Hardware:
 * Connect pins on sensor to analog pins
 * The optimal distance for the sensors is 0.125" (3 mm)
 * Maximum recommended sensing distance: 0.25" (6mm)
 * 
 * Summary of Driver:
 * This driver initializes a specified number of sensors on the QTR as analog inputs
 * and returns the raw data from the sensor as an array of 16-bit values.
 */

#pragma once

/** General imports. */
#include <stdint.h>

/** Device specific imports. */
#include <lib/ADC/ADC.h>


/** Number of total pins in QTR-8A Reflectance Sensor array */
#define MAX_PINS_QTR_8 8 

/** Number of total pins in QTR-3A Reflectance Sensor array */
#define MAX_PINS_QTR_3 3


typedef struct LineSensorConfig {
    /**
     * The ADC pins tied to each sensor in the QTR-8A reflectance sensor array.
     * It can contain up to MAX_PINS_QTR_8, and users are recommended to assign
     * them in order spatially of your sensor array.
     * 
     * Default for each entry is AIN0 (PE3).
     */
    enum ADCPin pins[MAX_PINS_QTR_8];

    /**
     * The number of pins that should be initialized. This has a upper limit of
     * MAX_PINS_QTR_8 and will be internally fixed to it if exceeded. They
     * correspond to each entry in the pins array starting from index 0 of the
     * array.
     * 
     * Default is 0 pins.
     */
    uint8_t numPins;

    /** ------------- Optional Fields. ------------- */

    /**
     * The sample frequency of the line sensor, in hz. When defined to be 
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
    uint32_t repeatFrequency;

    /**
     * Boolean to determine whether the interrupt call for the line sensor uses
     * LineSensorGetBoolArray or LineSensorGetIntArray.
     * 
     * Default false.
     */
    bool isThresholded;

    /**
     * If isThresholded is enabled, then the threshold value (fixed to (0,
     * 4095]) tells the initializer to call LineSensorGetBoolArray instead of
     * LineSensorGetIntArray at a fixed frequency.
     * 
     * Default is 0.
     */
    uint16_t threshold;
} LineSensorConfig_t;

typedef struct LineSensor {
    /** 
     * An array of GPIO pins that are used in the array. Empty entries will
     * have the value PIN_COUNT, or 48.
     */
    ADC_t adcs[MAX_PINS_QTR_8];

    /**
     * An array of uint32_t values that are associated with each ADC pin. These
     * are filled when called by LineSensorGetIntArray or LineSensorGetBoolArray.
     */
    uint32_t values[MAX_PINS_QTR_8];

    /** 
     * The number of pins that are defined in this LineSensor. Includes
     * duplicates.
     */
    uint8_t numPins;
} LineSensor_t;

/**
 * LineSensorInit initializes the QTR-8A reflectance sensor array.
 * 
 * @param config The configuration of the LineSensor module.
 * @return A LineSensor_t struct instance used for sampling.
 */
LineSensor_t LineSensorInit(LineSensorConfig_t config);

/** 
 * LineSensorGetIntArray reads an array of 16-bit integer sensor values from [0,
 * 4095] sampled at the selected pins of the QTR Reflectance Sensor into the
 * values member of the LineSensor object.
 * 
 * @param sensor A pointer to the LineSensor_t struct.
 * @note The sensor values will be stored in the same index as their respsective pins
 *       i.e: value from PD2 will be stored in SensorValues[5] because PD2 is stores in pins[5]
 *       Recommended to be used alongside timer functions to run inside an interrupt service routine
 */
void LineSensorGetIntArray(LineSensor_t * sensor);

/**
 * LineSensorGetIntArray reads an array of 16-bit integer sensor values from [0,
 * 4095] sampled at the selected pins of the QTR Reflectance Sensor into the
 * values member of the LineSensor object. Using an input threshold, those
 * values are converted into boolean integers, [0, 1].
 * 
 * @param sensor A pointer to the LineSensor_t struct.
 * @param threshold A threshold value from [0, 4095] that is used to convert the
 *                  integer samples into boolean values.
 *                  Values beyond this range is fixed to the bounds.
 *                  1 if value >= threshold
 *                  0 if value < threshold
 * @note The sensor values will be stored in the same index as their respsective pins
 *       i.e: value from PD2 will be stored in SensorValues[5] because PD2 is stores in pins[5]
 *       Recommended to be used alongside timer functions to run inside an interrupt service routine
 * 
 */
void LineSensorGetBoolArray(LineSensor_t * sensor, uint16_t threshold);
