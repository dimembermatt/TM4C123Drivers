// I2C3.c
// Runs on TM4C123
// Busy-wait device driver for the I2C3.
// Daniel and Jonathan Valvano
// Modified by Dario Jimenez on 09/16/21
// Jan 2, 2021
// This file originally comes from the TIDA-010021 Firmware (tidcf48.zip) and
// was modified by Pololu to support the MSP432P401R. Modified again for TM4C123

/* This example accompanies the books
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
      ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2020
   "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
      ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2020
   "Embedded Systems: Real-Time Operating Systems for ARM Cortex-M Microcontrollers",
      ISBN: 978-1466468863, Jonathan Valvano, copyright (c) 2020

 Copyright 2020 by Jonathan W. Valvano, valvano@mail.utexas.edu
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
/*
 *  I2C0 Conncection | I2C1 Conncection | I2C2 Conncection | I2C3 Conncection
 *  ---------------- | ---------------- | ---------------- | ----------------
 *  SCL -------- PB2 | SCL -------- PA6 | SCL -------- PE4 | SCL -------- PD0
 *  SDA -------- PB3 | SDA -------- PA7 | SDA -------- PE5 | SDA -------- PD1
 */
#include <stdint.h>
#include <rasinc/I2C3.h>
#include "../inc/tm4c123gh6pm.h"

#define I2C_MCS_ACK             0x00000008  // Data Acknowledge Enable
#define I2C_MCS_DATACK          0x00000008  // Acknowledge Data
#define I2C_MCS_ADRACK          0x00000004  // Acknowledge Address
#define I2C_MCS_STOP            0x00000004  // Generate STOP
#define I2C_MCS_START           0x00000002  // Generate START
#define I2C_MCS_ERROR           0x00000002  // Error
#define I2C_MCS_RUN             0x00000001  // I2C Master Enable
#define I2C_MCS_BUSY            0x00000001  // I2C Busy
#define I2C_MCR_MFE             0x00000010  // I2C Master Function Enable
#define MAXRETRIES              5           // number of receive attempts before giving up

// let t be bus period, let F be bus frequency
// let f be I2C frequency
// at F=80 MHz, I2C period = (TPR+1)*250ns 
// f=400kHz,    I2C period = 20*(TPR+1)*12.5ns = 2.5us, with TPR=9
// I2C period, 1/f = 20*(TPR+1)*t 
// F/f = 20*(TPR+1)
// TPR = (F/f/20)-1 
void I2C3_Init(uint32_t I2Cfreq, uint32_t busFreq){
  SYSCTL_RCGCI2C_R |= 0x0008;           // activate I2C3
  SYSCTL_RCGCGPIO_R |= 0x0008;          // activate port D
  while((SYSCTL_PRGPIO_R&0x0008) == 0){};// ready?
  GPIO_PORTD_AFSEL_R |= 0x03;           // 3) enable alt funct on PD1,0
  GPIO_PORTD_ODR_R |= 0x02;             // 4) enable open drain on PD1 only
  GPIO_PORTD_DR8R_R |= 0x03;            //  high current on PD1,0
  GPIO_PORTD_DEN_R |= 0x03;             // 5) enable digital I/O on PD1,0
                                        // 6) configure PD1,0 as I2C
  GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R&0xFFFFFF00)+0x00000033;
  GPIO_PORTD_AMSEL_R &= ~0x03;          // 7) disable analog functionality on PD1,0
  I2C3_MCR_R = I2C_MCR_MFE;             // 9) master function enable, no glitch
  I2C3_MCR2_R = I2C_MCR2_GFPW_BYPASS;   // bypass glitch
//  I2C3_MCR_R = I2C_MCR_MFE|I2C_MCR_GFE;             // 9) master function enable, glitch
//  I2C3_MCR2_R = I2C_MCR2_GFPW_4; // 4 clock glitch
  I2C3_MTPR_R = ((busFreq/I2Cfreq)/20)-1; // 8) configure clock speed
}

int I2C3_Send(uint8_t slaveAddr, uint8_t *pData, uint32_t count){
  while(I2C3_MCS_R & I2C_MCS_BUSY){};                // wait for I2C ready
  I2C3_MSA_R = (slaveAddr << 1) & I2C_MSA_SA_M;      // MSA[7:1] is slave address
  I2C3_MSA_R &= ~I2C_MSA_RS;                         // MSA[0] is 0 for send
    
  if(count == 1) {
    I2C3_MDR_R = pData[0] & I2C_MDR_DATA_M;          // prepare data byte
    I2C3_MCS_R = (I2C_MCS_STOP |                     // generate stop
    
                  I2C_MCS_START |                    // generate start/restart
   
                  I2C_MCS_RUN);                      // master enable
    while (I2C3_MCS_R & I2C_MCS_BUSY){};             // wait for transmission done
    // return error bits
    return (I2C3_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
        
  }else {
    I2C3_MDR_R = pData[0] & I2C_MDR_DATA_M;           // prepare data byte
    I2C3_MCS_R = I2C_MCS_RUN|I2C_MCS_START;           // run and start
    while (I2C3_MCS_R & I2C_MCS_BUSY){};              // wait for transmission done
    if((I2C3_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0){
      I2C3_MCS_R = I2C_MCS_STOP;                      // stop transmission
       // return error bits if nonzero
      return (I2C3_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
    }
    for(int i = 1; i < count-1; i++) {
      I2C3_MDR_R = pData[i] & I2C_MDR_DATA_M;         // prepare data byte
      I2C3_MCS_R = I2C_MCS_RUN;                       // master enable
      while (I2C3_MCS_R & I2C_MCS_BUSY){};            // wait for transmission done
      // check error bits
      if((I2C3_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0){
        I2C3_MCS_R = I2C_MCS_STOP;                    // stop transmission
       // return error bits if nonzero
        return (I2C3_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
      }
    }      
    I2C3_MDR_R = pData[count-1] & I2C_MDR_DATA_M;     // prepare last byte
    I2C3_MCS_R = (I2C_MCS_STOP |                      // generate stop
                  I2C_MCS_RUN);                       // master enable
    while (I2C3_MCS_R & I2C_MCS_BUSY) {};             // wait for transmission done
    // return error bits
    return (I2C3_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
  }
}


int I2C3_Send1(uint8_t slaveAddr, uint8_t data){
  while(I2C3_MCS_R & I2C_MCS_BUSY){};  // wait for I2C ready
  I2C3_MSA_R = (slaveAddr << 1);       // MSA[7:1] is slave address
                                       // MSA[0] is 0 for send
  I2C3_MDR_R = data;                   // prepare data byte
  I2C3_MCS_R = (I2C_MCS_STOP |         // generate stop
                I2C_MCS_START |        // generate start/restart
                I2C_MCS_RUN);          // master enable
  while (I2C3_MCS_R & I2C_MCS_BUSY){}; // wait for transmission done
    // return error bits
  return (I2C3_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));     
}

// sends two bytes to specified slave
// Returns 0 if successful, nonzero if error
int I2C3_Send2(int8_t slave, uint8_t data1, uint8_t data2){
  while(I2C3_MCS_R&0x01){}; // wait for I2C ready
  I2C3_MSA_R = slave<<1;    // MSA[7:1] is slave address
                            // MSA[0] is 0 for send
  I2C3_MDR_R = data1;       // prepare first byte
  I2C3_MCS_R = 0x03; // no stop, generate start, enable
  while(I2C3_MCS_R&0x01){}; // wait for transmission done
  if((I2C3_MCS_R&0x0E) != 0){ 
    I2C3_MCS_R = 0x04;      // send stop if error
    return I2C3_MCS_R&0x0E; // return error bits
  }
  I2C3_MDR_R = data2;       // prepare second byte
  I2C3_MCS_R = 0x05;        // generate stop, no start, enable
  while(I2C3_MCS_R&0x01){}; // wait for transmission done
  return I2C3_MCS_R&0x0E;   // return error bits, if any
}


int I2C3_Send4(uint8_t slaveAddr, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4){
  while(I2C3_MCS_R & I2C_MCS_BUSY){};       // wait for I2C ready
  I2C3_MSA_R = (slaveAddr << 1);            // MSA[7:1] is slave address
                                            // MSA[0] is 0 for send
    
  I2C3_MDR_R = data1;                       // prepare data byte
  I2C3_MCS_R = I2C_MCS_RUN|I2C_MCS_START;   // run and start
  while (I2C3_MCS_R & I2C_MCS_BUSY){};      // wait for transmission done
  if((I2C3_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0){
    I2C3_MCS_R = I2C_MCS_STOP;              // stop transmission
       // return error bits if nonzero
    return (I2C3_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
  }
  // second
  I2C3_MDR_R = data2;                       // prepare data byte
  I2C3_MCS_R = I2C_MCS_RUN;                 // master enable
  while (I2C3_MCS_R & I2C_MCS_BUSY){};      // wait for transmission done
      // check error bits
  if((I2C3_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0){
    I2C3_MCS_R = I2C_MCS_STOP;              // stop transmission
       // return error bits if nonzero
    return (I2C3_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
  }
   // third
  I2C3_MDR_R = data3;                       // prepare data byte
  I2C3_MCS_R = I2C_MCS_RUN;                 // master enable
  while (I2C3_MCS_R & I2C_MCS_BUSY){};      // wait for transmission done
      // check error bits
  if((I2C3_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0){
    I2C3_MCS_R = I2C_MCS_STOP;              // stop transmission
       // return error bits if nonzero
    return (I2C3_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
  }
   // fourth      
  I2C3_MDR_R = data4;                       // prepare last byte
  I2C3_MCS_R = (I2C_MCS_STOP |              // generate stop
                I2C_MCS_RUN);               // master enable
  while (I2C3_MCS_R & I2C_MCS_BUSY) {};     // wait for transmission done
  // return error bits
  return (I2C3_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
}




// receives one byte from specified slave
// Note for HMC6352 compass only:
// Used with 'r' and 'g' commands
// Note for TMP102 thermometer only:
// Used to read the top byte of the contents of the pointer register
//  This will work but is probably not what you want to do.
uint8_t I2C3_Recv1(int8_t slave){
  int retryCounter = 1;
  do{
    while(I2C3_MCS_R&I2C_MCS_BUSY){};// wait for I2C ready
    I2C3_MSA_R = (slave<<1)&0xFE;    // MSA[7:1] is slave address
    I2C3_MSA_R |= 0x01;              // MSA[0] is 1 for receive
    I2C3_MCS_R = (0
                        // & ~I2C_MCS_ACK     // negative data ack (last byte)
                         | I2C_MCS_STOP     // generate stop
                         | I2C_MCS_START    // generate start/restart
                         | I2C_MCS_RUN);    // master enable
    while(I2C3_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
    retryCounter = retryCounter + 1;        // increment retry counter
  }                                         // repeat if error
  while(((I2C3_MCS_R&(I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0) && (retryCounter <= MAXRETRIES));
  return (I2C3_MDR_R&0xFF);          // usually returns 0xFF on error
}
int I2C3_Recv(uint8_t slaveAddr, uint8_t *pData, uint32_t count){
  while(I2C3_MCS_R & I2C_MCS_BUSY){};                    // wait for I2C ready
  switch(count){
    case 1:
        I2C3_MSA_R = (slaveAddr << 1) & I2C_MSA_SA_M;    // MSA[7:1] is slave address
        I2C3_MSA_R |= I2C_MSA_RS;                        // MSA[0] is 1 for receive
              
        I2C3_MCS_R = (I2C_MCS_STOP  |                    // generate stop
                      I2C_MCS_START |                    // generate start/restart
                      I2C_MCS_RUN);                      // master enable
        while (I2C3_MCS_R & I2C_MCS_BUSY) {};            // wait for transmission done
        pData[0] = (I2C3_MDR_R & I2C_MDR_DATA_M);        // usually 0xFF on error
        break;
    case 2:
        I2C3_MSA_R = (slaveAddr << 1) & I2C_MSA_SA_M;    // MSA[7:1] is slave address
        I2C3_MSA_R |= I2C_MSA_RS;                        // MSA[0] is 1 for receive
            
        I2C3_MCS_R = (I2C_MCS_ACK   |                    // positive data ack
                      I2C_MCS_START |                    // generate start/restart
                      I2C_MCS_RUN);                      // master enable
        while (I2C3_MCS_R & I2C_MCS_BUSY) {};            // wait for transmission done
        pData[0] = (I2C3_MDR_R & I2C_MDR_DATA_M);        // most significant byte
              
        I2C3_MCS_R = (I2C_MCS_STOP |                     // generate stop
                      I2C_MCS_RUN);                      // master enable
        while(I2C3_MCS_R & I2C_MCS_BUSY){};              // wait for transmission done
        pData[1] = (I2C3_MDR_R & I2C_MDR_DATA_M);        // least significant byte                                                       // repeat if error
        break;
    default:
        I2C3_MSA_R = (slaveAddr << 1) & I2C_MSA_SA_M;    // MSA[7:1] is slave address
        I2C3_MSA_R |= I2C_MSA_RS;                        // MSA[0] is 1 for receive
              
        I2C3_MCS_R = (I2C_MCS_ACK   |                    // positive data ack
                      I2C_MCS_START |                    // generate start/restart
                      I2C_MCS_RUN);                      // master enable
        while (I2C3_MCS_R & I2C_MCS_BUSY) {};            // wait for transmission done
        pData[0] = (I2C3_MDR_R & I2C_MDR_DATA_M);        // most significant byte               
        for(int i = 1; i < count-1; i++){
          I2C3_MCS_R = (I2C_MCS_ACK |                    // positive data ack
                        I2C_MCS_RUN);                    // master enable
          while (I2C3_MCS_R & I2C_MCS_BUSY) {};          // wait for transmission done
          pData[i] = (I2C3_MDR_R & I2C_MDR_DATA_M);      // read byte
        }
             
        I2C3_MCS_R = (I2C_MCS_STOP |                     // generate stop
                      I2C_MCS_RUN);                      // master enable
        while (I2C3_MCS_R & I2C_MCS_BUSY) {};            // wait for transmission done
        pData[count-1] = (I2C3_MDR_R & I2C_MDR_DATA_M);  // least significant byte
        break;
  }
  return (I2C3_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
}
// receives two bytes from specified slave
// Note for HMC6352 compass only:
// Used with 'A' commands
// Note for TMP102 thermometer only:
// Used to read the contents of the pointer register
uint16_t I2C3_Recv2(int8_t slave){
  uint8_t data1,data2;
  int retryCounter = 1;
  do{
    while(I2C3_MCS_R&I2C_MCS_BUSY){};// wait for I2C ready
    I2C3_MSA_R = (slave<<1)&0xFE;    // MSA[7:1] is slave address
    I2C3_MSA_R |= 0x01;              // MSA[0] is 1 for receive
    I2C3_MCS_R = (0
                         | I2C_MCS_ACK      // positive data ack
                       //  & ~I2C_MCS_STOP    // no stop
                         | I2C_MCS_START    // generate start/restart
                         | I2C_MCS_RUN);    // master enable
    while(I2C3_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
    data1 = (I2C3_MDR_R&0xFF);       // MSB data sent first
    I2C3_MCS_R = (0
                       //  & ~I2C_MCS_ACK     // negative data ack (last byte)
                         | I2C_MCS_STOP     // generate stop
                       //  & ~I2C_MCS_START   // no start/restart
                         | I2C_MCS_RUN);    // master enable
    while(I2C3_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
    data2 = (I2C3_MDR_R&0xFF);       // LSB data sent last
    retryCounter = retryCounter + 1;        // increment retry counter
  }                                         // repeat if error
  while(((I2C3_MCS_R&(I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0) && (retryCounter <= MAXRETRIES));
  return (data1<<8)+data2;                  // usually returns 0xFFFF on error
}

int I2C3_Recv3(uint8_t slaveAddr, uint8_t data[3]){
  while(I2C3_MCS_R & I2C_MCS_BUSY){};         // wait for I2C ready
  I2C3_MSA_R = (slaveAddr << 1)|I2C_MSA_RS;   // MSA[7:1] is slave address
                                              // MSA[0] is 1 for receive
// first              
  I2C3_MCS_R = (I2C_MCS_ACK   |               // positive data ack
                I2C_MCS_START |               // generate start/restart
                I2C_MCS_RUN);                 // master enable
  while (I2C3_MCS_R & I2C_MCS_BUSY) {};       // wait for transmission done
  data[0] = I2C3_MDR_R ;                      // most significant byte               
 // second
  I2C3_MCS_R = (I2C_MCS_ACK |                 // positive data ack
                I2C_MCS_RUN);                 // master enable
  while (I2C3_MCS_R & I2C_MCS_BUSY) {};       // wait for transmission done
  data[1] = I2C3_MDR_R;                       // read byte
// third     
  I2C3_MCS_R = (I2C_MCS_STOP |                // generate stop
                I2C_MCS_RUN);                 // master enable
  while (I2C3_MCS_R & I2C_MCS_BUSY) {};       // wait for transmission done
  data[2] = I2C3_MDR_R;                       // least significant byte
  return (I2C3_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
}


int I2C3_SendData(uint8_t slaveAddr, uint8_t *pData, uint32_t count){
  while(I2C3_MCS_R & I2C_MCS_BUSY){};                // wait for I2C ready
  I2C3_MSA_R = slaveAddr << 1;      // MSA[7:1] is slave address
//  I2C3_MSA_R = (slaveAddr << 1) & I2C_MSA_SA_M;      // MSA[7:1] is slave address
 // I2C3_MSA_R &= ~I2C_MSA_RS;                         // MSA[0] is 0 for send
    

  I2C3_MDR_R = 0x40;           // first byte 0x40 means data bytes
  I2C3_MCS_R = I2C_MCS_RUN|I2C_MCS_START;           // run and start
  while (I2C3_MCS_R & I2C_MCS_BUSY){};              // wait for transmission done
//  if((I2C3_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0){
//    I2C3_MCS_R = I2C_MCS_STOP;                      // stop transmission
//     // return error bits if nonzero
//    return (I2C3_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
//  }
  for(int i = 0; i < count-1; i++) {
    I2C3_MDR_R = pData[i];         // prepare data byte
//    I2C3_MDR_R = pData[i] & I2C_MDR_DATA_M;         // prepare data byte
    I2C3_MCS_R = I2C_MCS_RUN;                       // master enable
    while (I2C3_MCS_R & I2C_MCS_BUSY){};            // wait for transmission done
//      // check error bits
//    if((I2C3_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0){
//      I2C3_MCS_R = I2C_MCS_STOP;                    // stop transmission
//       // return error bits if nonzero
//      return (I2C3_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
//    }
  }      
  I2C3_MDR_R = pData[count-1];     // prepare last byte
//  I2C3_MDR_R = pData[count-1] & I2C_MDR_DATA_M;     // prepare last byte
  I2C3_MCS_R = (I2C_MCS_STOP |                      // generate stop
                I2C_MCS_RUN);                       // master enable
  while (I2C3_MCS_R & I2C_MCS_BUSY) {};             // wait for transmission done
  // return error bits
  return (I2C3_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));

}


int TransmitAndReceive(uint8_t slaveAddr, uint8_t* receiveData, uint8_t* transmitData){

	
   while(I2C3_MCS_R & I2C_MCS_BUSY){}; // wait until I2C ready

   I2C3_MSA_R = slaveAddr << 1;  //set slave adress 
   I2C3_MSA_R &= ~I2C_MSA_RS;  //clear MSA[0] to transmit data

   I2C3_MDR_R = transmitData[0];

   while ((I2C3_MCS_R & (0x40)) != 0){}; //wait for bus line to be ready

   I2C3_MCS_R = I2C_MCS_RUN|I2C_MCS_START; //Start and run transmission

   while (I2C3_MCS_R & I2C_MCS_BUSY){} //wait for transmission to finish

   I2C3_MSA_R = slaveAddr << 1; //set slave adress again
   I2C3_MSA_R |= I2C_MSA_RS; //set MSA[0] to receive data

   I2C3_MCS_R = I2C_MCS_RUN|I2C_MCS_START|I2C_MCS_STOP; // Start and Run transmission and end it after

   while (I2C3_MCS_R & I2C_MCS_BUSY){} //wait for transmission to finish

   receiveData[0] =  I2C3_MDR_R&0xFF;

   return (I2C3_MCS_R & (I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
}
