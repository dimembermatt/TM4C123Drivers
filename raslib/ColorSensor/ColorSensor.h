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
#include <lib/I2C/I2C.h>


/**
 * @brief enumeration that specifies the available gain configurations
 *        for the TCS34725 color sensor
 **/
typedef enum TCS34725Gain{
    TCS34725_Gain_1X = 0x0,
    TCS34725_Gain_4X = 0x1,
    TCS34725_Gain_16X = 0x2,
    TCS34725_Gain_60X = 0x3,
}TCS34725Gain_t;

/**
 * @brief enumeration that specifies the integration time/cycles for the
 * TCS34725 color sensor
 * 
 * @note the higher the integration time, the more precise the sensor 
 *       values are, but it takes longer to update values.
 **/
typedef enum TCS34725IntegrationTime{
    TCS34725_INT_TIME_2MS = 0xFF,
    TCS34725_INT_TIME_24MS = 0xF6,
    TCS34725_INT_TIME_101MS = 0xD5,
    TCS34725_INT_TIME_154MS = 0xC0,
    TCS34725_INT_TIME_700MS = 0x00,
}TCS34725IntegrationTime_t;

typedef enum TCS34725WaitTime{
    TCS34725_WAIT_2MS = 0xFF,
    TCS34725_WAIT_204MS = 0xAB,
    TCS34725_WAIT_256MS = 0x00,
}TCS34725WaitTime_t;




typedef struct TCS34725Config{

    I2CConfig_t I2CConfig;

    TCS34725Gain_t Gain;

    TCS34725IntegrationTime_t IntegrationTime;

    bool isLong; //if true, increases wait time by 12x
    
}TCS34725Config_t;


/**
 * data structure for color sensor.
 * 
 * This is where RGBC values, and interrupt flags
 * will be stored
 **/
typedef struct TCS34725{

    I2C_t i2c;

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
    //PrivateColorSensor_t priv;

}TCS34725_t;

/* Color enumeration for available colors */
typedef enum TCS34725WColors{
    RED,
    GREEN,
    BLUE,
    CLEAR
}TCS34725Colors_t;



TCS34725_t TCS34725Init(TCS34725Config_t config);


void TCS34725Read(TCS34725_t * sensor);
