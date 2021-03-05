// ADCSWTrigger.h
// Runs on TM4C123
// Provide functions that initialize ADC0 SS3 to be triggered by
// software and trigger a conversion, wait for it to finish,
// and return the result.
// Daniel Valvano
// January 15, 2020

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2020

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

// There are many choices to make when using the ADC, and many
// different combinations of settings will all do basically the
// same thing.  For simplicity, this function makes some choices
// for you.  When calling this function, be sure that it does
// not conflict with any other software that may be running on
// the microcontroller.  Particularly, ADC0 sample sequencer 3
// is used here because it only takes one sample, and only one
// sample is absolutely needed.  Sample sequencer 3 generates a
// raw interrupt when the conversion is complete, but it is not
// promoted to a controller interrupt.  Software triggers the
// ADC0 conversion and waits for the conversion to finish.  If
// somewhat precise periodic measurements are required, the
// software trigger can occur in a periodic interrupt.  This
// approach has the advantage of being simple.  However, it does
// not guarantee real-time.
//
// A better approach would be to use a hardware timer to trigger
// the ADC0 conversion independently from software and generate
// an interrupt when the conversion is finished.  Then, the
// software can transfer the conversion result to memory and
// process it after all measurements are complete.

// This initialization function sets up the ADC according to the
// following parameters.  Any parameters not explicitly listed
// below are not modified:
// Max sample rate: <=125,000 samples/second
// Sequencer 0 priority: 1st (highest)
// Sequencer 1 priority: 2nd
// Sequencer 2 priority: 3rd
// Sequencer 3 priority: 4th (lowest)
// SS3 triggering event: software trigger
// SS3 1st sample source: Ain9 (PE4)
// SS3 interrupts: enabled but not promoted to controller
void ADC0_InitSWTriggerSeq3_Ch9(void);

// This initialization function sets up the ADC according to the
// following parameters.  Any parameters not explicitly listed
// below are not modified:
// Max sample rate: <=125,000 samples/second
// Sequencer 0 priority: 1st (highest)
// Sequencer 1 priority: 2nd
// Sequencer 2 priority: 3rd
// Sequencer 3 priority: 4th (lowest)
// SS3 triggering event: software trigger
// SS3 1st sample source: programmable using variable 'channelNum' [0:7]
// SS3 interrupts: enabled but not promoted to controller
void ADC0_InitSWTriggerSeq3(uint32_t channelNum);

// This initialization function sets up the ADC according to the
// following parameters.  Any parameters not explicitly listed
// below are not modified:
// Max sample rate: <=125,000 samples/second
// Sequencer 0 priority: 1st (highest)
// Sequencer 1 priority: 2nd
// Sequencer 2 priority: 3rd
// Sequencer 3 priority: 4th (lowest)
// SS3 triggering event: always trigger
// SS3 1st sample source: programmable using variable 'channelNum' [0:11]
// SS3 interrupts: enabled but not promoted to controller
void ADC0_InitAllTriggerSeq3(uint32_t channelNum);

// This initialization function sets up the ADC according to the
// following parameters.  Any parameters not explicitly listed
// below are not modified:
// Max sample rate: <=125,000 samples/second
// Sequencer 0 priority: 1st (highest)
// Sequencer 1 priority: 2nd
// Sequencer 2 priority: 3rd
// Sequencer 3 priority: 4th (lowest)
// SS3 triggering event: always trigger
// SS3 1st sample source: programmable using variable 'channelNum' [0:11]
// SS3 interrupts: enabled but not promoted to controller
void ADC0_InitAllTriggerSeq3(uint32_t channelNum);
// PD0 is AIN7
void ADC0_InitSWTriggerSeq3_Ch7(void);

//------------ADC0_InSeq3------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
uint32_t ADC0_InSeq3(void);

// Initializes ADC8 and ADC9 sampling
// 125k max sampling
// SS2 triggering event: software trigger, busy-wait sampling
// SS2 1st sample source: Ain9 (PE4)
// SS2 2nd sample source: Ain8 (PE5)
// SS2 interrupts: enabled after 2nd sample but not promoted to controller
void ADC_Init89(void);

//------------ADC_In89------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: two 12-bit result of ADC conversions
// Samples ADC8 and ADC9 
// 125k max sampling
// software trigger, busy-wait sampling
// data returned by reference
// data[0] is ADC8 (PE5) 0 to 4095
// data[1] is ADC9 (PE4) 0 to 4095
void ADC_In89(uint32_t data[2]);



// SS2 triggering event: software trigger, busy-wait sampling
// SS2 1st sample source: AIN6 (PD1)
// SS2 2nd sample source: AIN5 (PD2)
// SS2 3st sample source: AIN4 (PD3)
// SS2 interrupts: enabled after 3rd sample but not promoted to controller
// 'task' is a pointer to a user function called in SS2 interrupt
// the user function has three parameters that get the x-, y-, and
// z-values from the accelerometer passed by value
void ADC0_InitSWTriggerSeq2(void(*task)(uint32_t x, uint32_t y, uint32_t z));

//------------ADC0_InSeq2------------
// Busy-wait Analog to digital conversion initiates SS2 and
// calls the user function provided in the initialization with
// the results of the conversion in the parameters.
// UserTask2(AIN6, AIN5, AIN4);
// Input: none
// Output: none
// Samples ADC6, ADC5, and ADC4
// 125k max sampling
// software trigger, busy-wait sampling
void ADC0_InSeq2(void);

// This initialization function sets up the ADC according to the
// following parameters.  Any parameters not explicitly listed
// below are not modified:
// Max sample rate: <=125,000 samples/second
// Sequencer 0 priority: 1st (highest)
// Sequencer 1 priority: 2nd
// Sequencer 2 priority: 3rd
// Sequencer 3 priority: 4th (lowest)
// SS3 triggering event: software trigger
// SS3 1st sample source: internal temperature
// SS3 interrupts: enabled but not promoted to controller
void ADC0_InitSWTriggerSeq3_InternalTemperature(void);

//------------ADC0_InSeq3_InternalTemperature------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
uint32_t ADC0_InSeq3_InternalTemperature(void);

// Initializes ADC0(PE3), ADC1(PE2), ADC2(PE1) and ADC3(PE0) sampling
// 125k max sampling
// SS2 triggering event: software trigger, busy-wait sampling
// SS2 1st sample source: Ain0 (PE3)
// SS2 2nd sample source: Ain1 (PE2)
// SS2 3rd sample source: Ain2 (PE1)
// SS2 4th sample source: Ain3 (PE0)
// SS2 interrupts: enabled after 4th sample but not promoted to controller
void ADC_Init3210(void);

//------------ADC_In3210------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: four 12-bit result of ADC conversions
// Samples ADC0(PE3), ADC1(PE2), ADC2(PE1) and ADC3(PE0) 
// 125k max sampling
// software trigger, busy-wait sampling
// data returned by reference
// data[0] is ADC3 (PE0) 0 to 4095
// data[1] is ADC2 (PE1) 0 to 4095
// data[2] is ADC1 (PE2) 0 to 4095
// data[3] is ADC0 (PE3) 0 to 4095
void ADC_In3210(uint32_t data[4]);

/* Calibration data
distance measured from front of the sensor to the wall                
d(cm) 1/d    bL     al     aR   bR  adcSample d (0.01cm)  error
10    0.100  2813  2830  2820  2830  2823.25  1006        0.06
15    0.067  1935  1976  1986  1978  1968.75  1482       -0.18
20    0.050  1520  1500  1520  1550  1522.5   1966       -0.34
30    0.033  1040  1096  1028   933  1024.25  3099        0.99
  
      adcSample = 26813/d+159      2681300    
      d = 26813/(adcSample-159)      -159    
*/



