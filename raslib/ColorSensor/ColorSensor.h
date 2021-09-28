/**
 * @file ColorSensor.h
 * @details deigned for TM4C microcontroller. This driver/library allows you to initialize color sensors,
 *  update and read their RGBC values and set up interrupts for each color where you can set a low and 
 *  high data threshold.
 * @brief Low-level driver for TCS34725 color sensor
 * @author Dario Jimenez
 * @date 09/16/21
 * 
 *************************************************************************************************************
 *
 * TCS34725 Color Sensor information:
 *    - 3.3V pin: connect to the +3.3V
 *    - GND pin: connect to ground
 *    - SCL pin: connect to PD0
 *    - SDA pin: connect to PD1
 *    - LED pin: connect to ground if you want LED off
 *    - INT pin: connect to PA7 if activating Clear interrupt
 * 
 **/

#pragma once

/* imports */
#include <stdint.h>
#include <lib/Timer/Timer.h>


#define TCS34725_ADDRESS 0x29 //I2C Slave Adress for TCS34725

#define TCS34725_COMMAND 0x80 //Command bit for command register
#define TCS34725_ENABLE_PON 0x01 //Power On bit on Enable Register: writing 1 activates internal oscillator, 0 disables it
#define TCS34725_ENABLE_AEN 0x02 //RGBC Enable bit on Enable Register. 1: enable TCS34725 ADC. 0: disable TCS34725 ADC
#define TCS34725_ENABLE_WEN 0x08 //Wait Enable bit on Enable Register. 1: activates wait timer. 0: disables it.
#define TCS34725_ENABLE_AIEN 0x10 //RGBC Interrupt Enable bit on Enable Register. 1: permits interrupts. 0: does not permit interrupt
#define TCS34725_ATIME_10 0xF6 // integration of 10 cycles
#define TCS34725_CONTROL_1X_GAIN 0x00 //1x gain for ADC
#define TCS34725_STATUS_AVALID 0x01 //indicates whether ADC has finished integration cycle
#define TCS34725_PERS_5CYCLES 0x04 //makes interrupt not trigger until 5 consecutive values past threshold

#define TCS34725_ID1 0x44 //TCS34725 and TCS34721 ID number
#define TCS34725_ID2 0x4D //TCS34723 and TCS34727 ID number


/* Register addresses for color sensor */ 
#define TCS34725_ENABLE_R 0x00 //Enable Register
#define TCS34725_ATIME_R 0x01 //RGBC Timimg Register
#define TCS34725_WTIME_R 0x03 //Wait timer Register
#define TCS34725_AILTL_R 0x04 //RGBC clear channel low threshold lower byte
#define TCS34725_AILTH_R 0x05 //RGBC clear channel low threshold upper byte
#define TCS34725_AIHTL_R 0x06 //RGBC clear channel high threshold lower byte
#define TCS34725_AIHTH_R 0x07 //RGBC clear channel high threshold upper byte
#define TCS34725_PERS_R 0x0C //Persistence Register
#define TCS34725_CONFIG_R 0x0D //Configuration Register
#define TCS34725_CONTROL_R 0x0F // Control Register
#define TCS34725_ID_R 0x12 //ID Register
#define TCS34725_STATUS_R 0x13 //Status Register
#define TCS34725_CDATAL_R 0x14 //Clear data low byte
#define TCS34725_CDATAH_R 0x15 //Clear data high byte
#define TCS34725_RDATAL_R 0x16 //Red data low byte
#define TCS34725_RDATAH_R 0x17 //Red data high byte
#define TCS34725_GDATAL_R 0x18 //Green data low byte
#define TCS34725_GDATAH_R 0x19 //Green data high byte
#define TCS34725_BDATAL_R 0x1A //Blue data low byte
#define TCS34725_BDATAH_R 0x1B //Blue data high byte

/** 
 * data structure for private fields for ColorSensor_t.
 * 
 * DO NOT MODIFY OR ACCESS any of these fields, doing so
 * can cause color sensor to not work correctly.
 **/
typedef struct PrivateColorSensor{
    int isInitialized;

    /* red high and low threshold, if set */
    uint16_t redHigh;
    uint16_t redLow;

    /* green high and low threshold, if set */
    uint16_t greenHigh;
    uint16_t greenLow;

    /* blue high and low threshold, if set */
    uint16_t blueHigh;
    uint16_t blueLow;

    /**
     * indicate if interrupt for 
     * the corresponding color are 
     * active
     **/
    uint8_t isClearInt;
    uint8_t isGreenInt;
    uint8_t isBlueInt;
    uint8_t isRedInt;

}PrivateColorSensor_t;

/**
 * data structure for color sensor.
 * 
 * This is where RGBC values, and interrupt flags
 * will be stored
 **/
typedef struct ColorSensor{

    /* 16-bit value from color sensors' ADC */
    uint16_t RedValue;
    uint16_t GreenValue;
    uint16_t BlueValue;
    uint16_t ClearValue;

    /* Indicate if an a threshold from a interrupt has been triggered */ 
    uint8_t clearInterrupt;
    uint8_t redInterrupt;
    uint8_t greenInterrupt;
    uint8_t blueInterrupt;

    /* private field data structure, do not modify or access */
    PrivateColorSensor_t priv;

}ColorSensor_t;

/* Color enumeration for available colors */
typedef enum ColorSensorColors{
    RED,
    GREEN,
    BLUE,
    CLEAR
}ColorSensorColors_t;

/** 
 * @brief Initializes color sensor to its default settings and I2C interface
 * 
 * @param sensor instance of ColorSensor_t that you want to initialize passed as a pointer
 * 
 * @return value 1 if correctly intialized, value 0 is not
 **/
int ColorSensor_init(ColorSensor_t* sensor);

/**
 * @brief updates 16-bit values from red, green, blue, and clear ADC color sensors.
 * 
 * @param sensor an instance of ColorSensor_t passed as a pointer
 * 
 * @note If color sensor is not initalized, value won't update
 * @note ClearValue, RedValue, GreenValue, BlueValue variables in sensor will be updated
 **/

void ColorSensor_Read(ColorSensor_t* sensor);

/**
 * @brief initializes an interrupt that sets a flag in ColorSensor_t instance if a value from a specified color sensor is greater than high threshold
 *        less than low threshold
 * 
 * @param sensor instance of ColorSensor_t passed as a pointer.
 * @param low low threshold
 * @param high high threshold
 * @param color desired color to set interrupt for
 * @param priority priority of the interrupt
 * @param timerID desired timer for interrupt. Check Timers.h library for a full list of all available timers
 * 
 * @note interrupts will run at 30 Hz
 * @note PA7 GPIO pin will needed if interrupt for clear sensor is initialized and must be connected to "int" pin in color sensor device
 * @note interrupt priority for all interrupts will be 4. 
 * 
 **/

void ColorSensor_SetInterrupt(ColorSensor_t* sensor, uint16_t low, uint16_t high, ColorSensorColors_t color, TimerID_t timerID);

/**
 * @brief disable a specified interrupt 
 * 
 * @param sensor instance of ColorSensor_t passed as a pointer.
 * 
 * @param color color for the interrupt you want to disable
 * 
 **/

void ColorSensor_DisableInterrupt(ColorSensorColors_t color);
