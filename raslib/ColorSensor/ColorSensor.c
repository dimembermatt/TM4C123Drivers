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

#include <assert.h>

/* library imports */
#include <raslib/ColorSensor/ColorSensor.h>
#include <lib/I2C/I2C.h>
#include <lib/GPIO/GPIO.h>
#include <lib/Timer/Timer.h>
#include <rasinc/I2C3.h>


#define MAX_COLORSENSOR_TIMERS 4 // Max number of interrupts (1 for each color)
#define MAX_COLORSENSOR_FREQ 30 //max frequency for color sensor
#define COLORSENSOR_PRIORITY 4

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

static GPIOConfig_t INT_Pin ={PIN_A7, GPIO_TRI_STATE}; //GPIO configuration for int pin
static TimerConfig_t timersConfig[MAX_COLORSENSOR_TIMERS]; //array of timer of configuration for each color sensor
static Timer_t timers[MAX_COLORSENSOR_TIMERS]; //array of timer for each color sensor
//static ColorSensor_t* sensors[MAX_COLORSENSOR_TIMERS] = {0x0, 0x0, 0x0, 0x0}; //array of sensors used for interrupts

/* interrupt counters for persistance buffer */
static uint8_t redCount = 0;
static uint8_t greenCount = 0;
static uint8_t blueCount = 0;

//indicates if I2C is on
uint8_t isI2COn = 0;

/* values to transmit for setting up color sensor */
static uint8_t EnableWriteValues[2] = {TCS34725_COMMAND | TCS34725_ENABLE_R, TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN};
static uint8_t ATimeWriteValues[2] = {TCS34725_COMMAND | TCS34725_ATIME_R, TCS34725_ATIME_10};
static uint8_t ControlWriteValues[2] = {TCS34725_COMMAND | TCS34725_CONTROL_R, TCS34725_CONTROL_1X_GAIN};

/* Values to transmit for setting up clear interrupt */
static uint8_t setInterruptValues[2] = {TCS34725_COMMAND | TCS34725_ENABLE_R, TCS34725_ENABLE_AIEN | TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN};
static uint8_t persistenceValues[2] = { TCS34725_COMMAND | TCS34725_PERS_R, TCS34725_PERS_5CYCLES};



TCS34725_t TCS34725Init(TCS34725Config_t config){
  I2C_t i2c = I2CInit(config.I2CConfig);

  /* check device ID */
  uint8_t TransmitData[2] = {TCS34725_COMMAND | TCS34725_ID_R, config.Gain};
  uint8_t read = 0;

	I2CMasterTransmit(i2c, TCS34725_ADDRESS, TransmitData, 1);
	I2CMasterReceive(i2c, TCS34725_ADDRESS, &read, 1);

	
  if(read != TCS34725_ID1 && read != TCS34725_ID2){ //if ID is the same, then incorrect sensor is plugged in
    assert(0);
  }

  /* transmit gain settings */
  TransmitData[0] = TCS34725_COMMAND | TCS34725_CONTROL_R;
  I2CMasterTransmit(i2c, TCS34725_ADDRESS, TransmitData, 2);

  /* transmit integration time settings */
  TransmitData[0] =  TCS34725_COMMAND | TCS34725_ATIME_R;
  TransmitData[1] = config.IntegrationTime;
  I2CMasterTransmit(i2c, TCS34725_ADDRESS, TransmitData, 2);

  /* Transmit long integration time setting*/
  if(config.isLong == true){
      TransmitData[0] =  TCS34725_COMMAND | TCS34725_CONFIG_R;
      TransmitData[1] = 1;
      I2CMasterTransmit(i2c, TCS34725_ADDRESS, TransmitData, 2);
  }

  /* transmit Enable settings (enables the sensor) */
  TransmitData[0] = TCS34725_COMMAND | TCS34725_ENABLE_R;
  TransmitData[1] = TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN | TCS34725_ENABLE_WEN;
  I2CMasterTransmit(i2c, TCS34725_ADDRESS, TransmitData, 2);

  TCS34725_t sensor = {i2c,0,0,0,0,0,0,0,0};

  return sensor;
}

void TCS34725Read(TCS34725_t * sensor){

	
  volatile uint8_t dataL[] = { 0 };
  volatile uint8_t dataH[] = { 0 };
  volatile uint16_t data;

  /* Read clear bytes */
  uint8_t transmit[] = { TCS34725_COMMAND| TCS34725_CDATAH_R};
	I2CMasterTransmit(sensor->i2c, TCS34725_ADDRESS, transmit, 1);
	I2CMasterReceive(sensor->i2c, TCS34725_ADDRESS, dataH, 1);
	transmit[0] = TCS34725_COMMAND| TCS34725_CDATAL_R;
	I2CMasterTransmit(sensor->i2c, TCS34725_ADDRESS, transmit, 1);
	I2CMasterReceive(sensor->i2c, TCS34725_ADDRESS, dataL, 1);
	
	data = dataH[0];
	data = data << 8;
	data |= dataL[0];
	sensor->ClearValue = data;
	
	data = 0;
	
	/* Read red bytes */
	
  transmit[0] = TCS34725_COMMAND| TCS34725_RDATAH_R;
	I2CMasterTransmit(sensor->i2c, TCS34725_ADDRESS, transmit, 1);
	I2CMasterReceive(sensor->i2c, TCS34725_ADDRESS, dataH, 1);
	transmit[0] = TCS34725_COMMAND| TCS34725_RDATAL_R;
	I2CMasterTransmit(sensor->i2c, TCS34725_ADDRESS, transmit, 1);
	I2CMasterReceive(sensor->i2c, TCS34725_ADDRESS, dataL, 1);
	
	data = dataH[0];
	data = data << 8;
	data |= dataL[0];
	sensor->RedValue = data;
	
	data = 0;
	
	/* Read green bytes */
	
  transmit[0] = TCS34725_COMMAND| TCS34725_GDATAH_R;
	I2CMasterTransmit(sensor->i2c, TCS34725_ADDRESS, transmit, 1);
	I2CMasterReceive(sensor->i2c, TCS34725_ADDRESS, dataH, 1);
	transmit[0] = TCS34725_COMMAND| TCS34725_GDATAL_R;
	I2CMasterTransmit(sensor->i2c, TCS34725_ADDRESS, transmit, 1);
	I2CMasterReceive(sensor->i2c, TCS34725_ADDRESS, dataL, 1);
	
	data = dataH[0];
	data = data << 8;
	data |= dataL[0];
	sensor->GreenValue = data;
	
	data = 0;
	
	/* Read blue bytes */
	
  transmit[0] = TCS34725_COMMAND| TCS34725_BDATAH_R;
	I2CMasterTransmit(sensor->i2c, TCS34725_ADDRESS, transmit, 1);
	I2CMasterReceive(sensor->i2c, TCS34725_ADDRESS, dataH, 1);
	transmit[0] = TCS34725_COMMAND| TCS34725_BDATAL_R;
	I2CMasterTransmit(sensor->i2c, TCS34725_ADDRESS, transmit, 1);
	I2CMasterReceive(sensor->i2c, TCS34725_ADDRESS, dataL, 1);
	
	data = dataH[0];
	data = data << 8;
	data |= dataL[0];
	sensor->BlueValue = data;
	
	data = 0;
	
	
}
