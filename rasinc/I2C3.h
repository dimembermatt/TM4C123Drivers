/**
 * @file      I2C3.h
 * @brief     Low-level software driver for I2C
 * @details   TM4C123 is master. I2C3 is connected to PD0=SCL, PD1=SDA<br>
 * This file originally comes from the TIDA-010021 Firmware (tidcf48.zip) and<br>
 * was modified by Pololu to support the MSP432P401R.  Modified again for TM4C123
 * @version   SSD1306 EE319K
 * @author    Daniel Valvano and Jonathan Valvano, modified by Dario Jimenez on 09/26/21
 * @copyright Copyright 2021 by Jonathan W. Valvano, valvano@mail.utexas.edu,
 * @warning   AS-IS
 * @note      For more information see  http://users.ece.utexas.edu/~valvano/
 * @date      Jan 2, 2021
<table>
<caption id="I2C3ports">I2C connected to I2C3</caption>
<tr><th>Pin   <th>signal
<tr><td>GND   <td>ground
<tr><td>VIN   <td>3.3V
<tr><td>PD1   <td>I2C SDA data (bidirectional)
<tr><td>PD0   <td>I2C SCL clock (TM4C123 output)
</table>
 ******************************************************************************/


/* This example accompanies the books
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
      ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2020
   "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
      ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2020
   "Embedded Systems: Real-Time Operating Systems for ARM Cortex-M Microcontrollers",
      ISBN: 978-1466468863, Jonathan Valvano, copyright (c) 2020

 Copyright 2021 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// TM4C123 hardware
// SDA    PD1 I2C data
// SCL    PD0 I2C clock
/**
 *  I2C0 Connection  | I2C1 Connection  | I2C2 Connection  | I2C3 Connection
 *  ---------------- | ---------------- | ---------------- | ----------------
 *  SCL -------- PB2 | SCL -------- PA6 | SCL -------- PE4 | SCL -------- PD0
 *  SDA -------- PB3 | SDA -------- PA7 | SDA -------- PE5 | SDA -------- PD1
 */
 #ifndef _I2C3_H_
#define _I2C3_H_
#include <stdint.h>

/**
 * I2C3_Init
 * ----------
 * let t be bus period, let F be bus frequency,
 * let f be I2C frequency.
 * at F=80 MHz, I2C period = (TPR+1)*250ns.<br> 
 * f=400kHz,    I2C period = 20*(TPR+1)*12.5ns = 2.5us, with TPR=9<br>
 * I2C period, 1/f = 20*(TPR+1)*t <br>
 * F/f = 20*(TPR+1)<br>
 * TPR = (F/f/20)-1 
 * @param  I2Cfreq  desired frequency of I2C in Hz.
 * @param  busFreq  bus frequency in Hz.
 * ----------
 * @brief write 1 or more bytes to I2C3.
 */
void I2C3_Init(uint32_t I2Cfreq, uint32_t busFreq);

/**
 * I2C3_Send
 * ----------
 * @param  slaveAddr  address of slave device.
 * @param  pData      data address of data to be writen.
 * @param  count      number of bytes to be writen.
 * ----------
 * @brief write 1 or more bytes to I2C3.
 */
int I2C3_Send(uint8_t slaveAddr, uint8_t *pData, uint32_t count);

/**
 * I2C3_Send1
 * ----------
 * @param  slaveAddr  address of slave device.
 * @param  data       data to be writen.
 * ----------
 * @brief write 1 byte to I2C3.
 */
 int I2C3_Send1(uint8_t slaveAddr, uint8_t data);
 
 /**
 * I2C3_Send2
 * ----------
 * @param  slaveAddr  address of slave device.
 * @param  data1      first data to be writen.
 * @param  data2      second data to be writen.
 * ----------
 * @brief write 1 byte to I2C3.
 */
 int I2C3_Send2(int8_t slave, uint8_t data1, uint8_t data2);
/**
 * I2C3_Send4
 * ----------
 * @param  slaveAddr  address of slave device.
 * @param  data1      data to be writen.
 * @param  data2      data to be writen.
 * @param  data3      data to be writen.
 * @param  data4      data to be writen.
 * ----------
 * @brief write 4 bytes to I2C3.
 */
 int I2C3_Send4(uint8_t slaveAddr, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4);

/**
 * I2C3_Recv
 * ----------
 * @param  slaveAddr  address of slave device.
 * @param  pData      data address to store read data.
 * @param  count      number of bytes to be read.
 * ----------
 * @brief read 1 or more bytes from I2C3.
 */
int I2C3_Recv(uint8_t slaveAddr, uint8_t *pData, uint32_t count);

/**
 * I2C3_Recv3
 * ----------
 * @param  slaveAddr  address of slave device.
 * @param  data       data address to store read data.
 * @param  count      number of bytes to be read.
 * @brief read 2 or more bytes from I2C3.
 */
int I2C3_Recv3(uint8_t slaveAddr, uint8_t data[3]);

/**
 * I2C3_SendData
 * ----------
 * @param  slaveAddr  address of slave device.
 * @param  pData      data address of data to be writen.
 * @param  count      number of bytes to be writen.
 * ----------
 * @brief write many bytes to I2C3.
 */
int I2C3_SendData(uint8_t slaveAddr, uint8_t *pData, uint32_t count);


int TransmitAndReceive(uint8_t slaveAddr, uint8_t* receiveData, uint8_t* transmitData);


#endif //_I2C3_H_
