/**
 * @file TSL2591.c
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

/** General imports. */
#include <assert.h>

/** Device specific imports. */
#include "./TSL2591.h"
#include <lib/Timer/Timer.h>
#include <lib/GPIO/GPIO.h>


#define TSL2591_CMD         0xA0
#define TSL2591_POFF        0x00
#define TSL2591_PON         0x01
#define TSL2591_AEN         0x02
#define TSL2591_AIEN        0x10
#define TSL2591_SAI         0x40
#define TSL2591_NPIEN       0x80

#define TSL2591_I2C_ADDR    0x29
#define TSL2591_DEV_ID      0x50

/** @brief A set of registers for I2C communication. */
enum TSL2591Reg {
    TSL2591_REG_ENABLE          = 0x00,
    TSL2591_REG_CONTROL         = 0x01,
    TSL2591_REG_THRES_AILTL     = 0x04,
    TSL2591_REG_THRES_AILTH     = 0x05,
    TSL2591_REG_THRES_AIHTL     = 0x06,
    TSL2591_REG_THRES_AIHTH     = 0x07,
    TSL2591_REG_THRES_NPAILTL   = 0x08,
    TSL2591_REG_THRES_NPAILTH   = 0x09,
    TSL2591_REG_THRES_NPAIHTL   = 0x0A,
    TSL2591_REG_THRES_NPAIHTH   = 0x0B,
    TSL2591_REG_PERSIST         = 0x0C,
    TSL2591_REG_PID             = 0x11,
    TSL2591_REG_ID              = 0x12,
    TSL2591_REG_STATUS          = 0x13,
    TSL2591_REG_CHAN0_L         = 0x14,
    TSL2591_REG_CHAN0_H         = 0x15,
    TSL2591_REG_CHAN1_L         = 0x16,
    TSL2591_REG_CHAN1_H         = 0x17,
};

TSL2591_t TSL2591Init(TSL2591Config_t config) {
    /* Initialization asserts. */
    assert(config.gain <= TSL2591_GAIN_MAX);
    assert(config.time <= TSL2591_INTT_600MS);

    DelayInit();
    I2C_t i2c = I2CInit(config.i2cConfig);

    /* Check for matching Device ID. */
    uint8_t transmit[] = { TSL2591_CMD | TSL2591_REG_ID };
    I2CMasterTransmit(i2c, TSL2591_I2C_ADDR, transmit, 1);
    uint8_t read[] = { 0 };
    I2CMasterReceive(i2c, TSL2591_I2C_ADDR, read, 1);
    if (read[0] == TSL2591_DEV_ID) {
        /* Set gain and integration time. */
        uint8_t transmit2[2] = { TSL2591_CMD | TSL2591_REG_CONTROL, 0 };
        transmit2[1] = (config.gain << 4) | config.time;
        I2CMasterTransmit(i2c, TSL2591_I2C_ADDR, transmit2, 2);

        TSL2591_t sensor = {
            .i2c=i2c,
            .raw=0,
            .ir=0,
            .full=0,
            .visible=0,
            .lux=0,
            .irradiance=0,
            .gain=config.gain,
            .time=config.time
        };
        
        /* Disable. */
        TSL2591Disable(&sensor);
        return sensor;
    }
    assert(0); // Configuration error. Cannot extract ID.
}

void TSL2591Enable(TSL2591_t * sensor) {
    /* Activate internal oscillator and ALS sampler. */
    uint8_t transmit[] = { TSL2591_CMD | TSL2591_REG_ENABLE , TSL2591_PON | TSL2591_AEN | TSL2591_AIEN | TSL2591_NPIEN };
    I2CMasterTransmit(sensor->i2c, TSL2591_I2C_ADDR, transmit, 2);
}

void TSL2591Disable(TSL2591_t * sensor) {
    /* Deactivate internal oscillator. */
    uint8_t transmit[] = { TSL2591_CMD | TSL2591_REG_ENABLE , TSL2591_POFF };
    I2CMasterTransmit(sensor->i2c, TSL2591_I2C_ADDR, transmit, 2);
}

void TSL2591Sample(TSL2591_t * sensor) {
    TSL2591Enable(sensor);

    /* Delay for configured integration time. */
    uint8_t i;
    for (i = 0; i <= sensor->time + 1; ++i) {
        DelayMillisec(100);
    }

    /* Capture Channel 1. */
    uint8_t transmit[] = { TSL2591_CMD | TSL2591_REG_CHAN1_L };
    uint8_t read1[2] = { 0 };
    I2CMasterTransmit(sensor->i2c, TSL2591_I2C_ADDR, transmit, 1);
    I2CMasterReceive(sensor->i2c, TSL2591_I2C_ADDR, read1, 2);

    /* Capture Channel 0. */
    transmit[0] = TSL2591_CMD | TSL2591_REG_CHAN0_L;
    uint8_t read2[2] = { 0 };
    I2CMasterTransmit(sensor->i2c, TSL2591_I2C_ADDR, transmit, 1);
    I2CMasterReceive(sensor->i2c, TSL2591_I2C_ADDR, read2, 2);

    TSL2591Disable(sensor);

    /* Set up values. */
    sensor->raw = (((read1[1] << 8) | read1[0]) << 16) | ((read2[1] << 8) | read2[0]);
    sensor->full = sensor->raw & 0xFFFF;
    sensor->ir = sensor->raw >> 16;
    sensor->visible = sensor->full - sensor->ir;

    if ((sensor->full == 0xFFFF) | (sensor->ir == 0xFFFF)) {
        sensor->lux = 0;
        return;
    } else {
        float atime, again, cpl, lux1, lux2, lux3;
        
        atime = 100.0F;
        if (sensor->time <= TSL2591_INTT_600MS) {
            atime += 100.0F * sensor->time;
        } else {
            assert(0);
        }
        switch(sensor->gain) {
            case TSL2591_GAIN_LOW:
                again = 1.0F;
                break;
            case TSL2591_GAIN_MED:
                again = 25.0F;
                break;
            case TSL2591_GAIN_HIGH:
                again = 428.0F;
                break;
            case TSL2591_GAIN_MAX:
                again = 9876.0F;
                break;
            default:
                assert(0);
        }

        #define TSL2591_LUX_DF      (408.0F)
        #define TSL2591_LUX_COEFB   (1.64F)  // CH0 coefficient 
        #define TSL2591_LUX_COEFC   (0.59F)  // CH1 coefficient A
        #define TSL2591_LUX_COEFD   (0.86F)  // CH2 coefficient B

        cpl = (atime * again) / TSL2591_LUX_DF;
        lux1 = ((float) sensor->full - (TSL2591_LUX_COEFB * (float) sensor->ir)) / cpl;
        lux2 = ((TSL2591_LUX_COEFC * (float) sensor->full) - (TSL2591_LUX_COEFD * (float) sensor->ir)) / cpl;
        lux3 = lux1 > lux2 ? lux1 : lux2;
        sensor->lux = (uint32_t) lux3;
        sensor->irradiance = (uint32_t)(lux3 * 0.0083333);

        #undef TSL2591_LUX_DF
        #undef TSL2591_LUX_COEFB
        #undef TSL2591_LUX_COEFC
        #undef TSL2591_LUX_COEFD
    }
}
