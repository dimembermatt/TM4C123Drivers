// -------------------------------------------------------------------
// File name:     esp8266.c
// Description:   This starter code is used to bridge the TM4C123 board
//                and the Blynk Application via the ESP8266. 
//
// Authors:       Mark McDermott
// Converted to EE445L style Jonathan Valvano
// Date:          May 21, 2018
// Last update: Sept 20, 2018
//
// TM4C123       ESP8266-ESP01 (2 by 4 header)
// PE5 (U5TX) to Pin 1 (Rx)
// PE4 (U5RX) to Pin 5 (TX)
// PE3 output debugging
// PE2 nc
// PE1 output    Pin 7 Reset
// PE0 input     Pin 3 Rdy IO2
//               Pin 2 IO0, 10k pullup to 3.3V  
//               Pin 8 Vcc, 3.3V (separate supply from LaunchPad) 
// Gnd           Pin 4 Gnd  
// Place a 4.7uF tantalum and 0.1 ceramic next to ESP8266 3.3V power pin
// Use LM2937-3.3 and two 4.7 uF capacitors to convert USB +5V to 3.3V for the ESP8266
// http://www.ti.com/lit/ds/symlink/lm2937-3.3.pdf
#include <stdint.h>
#include <string.h>
#include "tm4c123gh6pm.h"
#include "UART.h"
#include "esp8266.h"
// the following two lines connect you to the internet
char    ssid[32]        = "Amped_B1912_E5F4";
char    pass[32]        = "8746bedc";
// create your own Blynk server app and edit this next line with your authentication code
char    auth[64]        = "2izrcNTsQZYsQRy6RZ4tiTqQCMrJKf7Y";

#define UART_FR_RXFF            0x00000040  // UART Receive FIFO Full
#define UART_FR_TXFF            0x00000020  // UART Transmit FIFO Full
#define UART_FR_RXFE            0x00000010  // UART Receive FIFO Empty
#define UART_LCRH_WLEN_8        0x00000060  // 8 bit word length
#define UART_LCRH_FEN           0x00000010  // UART Enable FIFOs
#define UART_CTL_UARTEN         0x00000001  // UART Enable
#define UART_IFLS_RX1_8         0x00000000  // RX FIFO >= 1/8 full
#define UART_IFLS_TX1_8         0x00000000  // TX FIFO <= 1/8 full
#define UART_IM_RTIM            0x00000040  // UART Receive Time-Out Interrupt
                                            // Mask
#define UART_IM_TXIM            0x00000020  // UART Transmit Interrupt Mask
#define UART_IM_RXIM            0x00000010  // UART Receive Interrupt Mask
#define UART_RIS_RTRIS          0x00000040  // UART Receive Time-Out Raw
                                            // Interrupt Status
#define UART_RIS_TXRIS          0x00000020  // UART Transmit Raw Interrupt
                                            // Status
#define UART_RIS_RXRIS          0x00000010  // UART Receive Raw Interrupt
                                            // Status
#define UART_ICR_RTIC           0x00000040  // Receive Time-Out Interrupt Clear
#define UART_ICR_TXIC           0x00000020  // Transmit Interrupt Clear
#define UART_ICR_RXIC           0x00000010  // Receive Interrupt Clear


void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

#define FIFOSUCCESS 1         // return value on success
#define FIFOFAIL    0         // return value on failure
                              // create index implementation FIFO (see FIFO.h)
// Two-index implementation of the receive FIFO
// can hold 0 to RX5FIFOSIZE messages
#define RX5FIFOSIZE 8    // must be a power of 2
#define MESSAGESIZE 64
uint32_t volatile Rx5PutI; // put next message
uint32_t volatile Rx5PutJ; // put next character 0 to MESSAGESIZE-1
uint32_t volatile Rx5GetI; // get next message
char Rx5Fifo[RX5FIFOSIZE][MESSAGESIZE]; // each message can have up to MESSAGESIZE-1 characters
char RxMessage[MESSAGESIZE];
// initialize index FIFO
void Rx5Fifo_Init(void){ long sr;
  sr = StartCritical(); // make atomic
  Rx5PutI = Rx5GetI = Rx5PutJ = 0;  // Empty
  EndCritical(sr);
}
// add element to end of index FIFO
// return TXFIFOSUCCESS if successful
int Rx5Fifo_Put(char data){
  if((Rx5PutI-Rx5GetI) & ~(RX5FIFOSIZE-1)){
    return(FIFOFAIL); // Failed, fifo full
  }
  Rx5Fifo[Rx5PutI&(RX5FIFOSIZE-1)][Rx5PutJ] = data; // put
  if(data != '\n'){
    if(Rx5PutJ == (MESSAGESIZE-3)){
      // you should never get here!
      Rx5Fifo[Rx5PutI&(RX5FIFOSIZE-1)][MESSAGESIZE-2] = ',';
      Rx5Fifo[Rx5PutI&(RX5FIFOSIZE-1)][MESSAGESIZE-1] = '\n'; // message overflow, force a '\n'
      Rx5PutI++;   // end of message
      Rx5PutJ = 0; // get ready for new message
      return(FIFOSUCCESS); // bad situation, message buffer overflow
    }
    Rx5PutJ++;  // Success, update
    return(FIFOSUCCESS);
  }
  Rx5Fifo[Rx5PutI&(RX5FIFOSIZE-1)][Rx5PutJ] = ',';
  Rx5Fifo[Rx5PutI&(RX5FIFOSIZE-1)][Rx5PutJ+1] = '\n';
  Rx5PutI++;   // end of message
  Rx5PutJ = 0; // get ready for new message
  return(FIFOSUCCESS);
}
// remove a message from front of index FIFO
// datapt points to an empty data buffer of MESSAGESIZE character
// if successful a message is copied from the FIFO into data buffer
// return FIFOSUCCESS if successful
// return FIFOFAIL if the FIFO is empty (no messages)
int ESP8266_GetMessage(char *datapt){char data; int j;
  if(Rx5PutI == Rx5GetI ){
    return(FIFOFAIL); // Empty if PutI=GetI
  }
  j = 0;
  do{
    data = Rx5Fifo[Rx5GetI&(RX5FIFOSIZE-1)][j];
    datapt[j] = data;
    j++;
  }while((j<MESSAGESIZE)&&(data != '\n'));
  Rx5GetI++;  // Success, update
  return(FIFOSUCCESS);
}
// number of messages in index FIFO
// 0 to RX5FIFOSIZE-1
uint32_t Rx5Fifo_Size(void){
 return ((uint32_t)(Rx5PutI-Rx5GetI));
}  

// Two-index implementation of the transmit FIFO
// can hold 0 to TX5FIFOSIZE elements
#define TX5FIFOSIZE 256    // must be a power of 2
uint32_t volatile Tx5PutI; // put next
uint32_t volatile Tx5GetI; // get next
char static Tx5Fifo[TX5FIFOSIZE];

// initialize index FIFO
void Tx5Fifo_Init(void){ long sr;
  sr = StartCritical(); // make atomic
  Tx5PutI = Tx5GetI = 0;  // Empty
  EndCritical(sr);
}
// add element to end of index FIFO
// return TXFIFOSUCCESS if successful
int Tx5Fifo_Put(char data){
  if((Tx5PutI-Tx5GetI) & ~(TX5FIFOSIZE-1)){
    return(FIFOFAIL); // Failed, fifo full
  }
  Tx5Fifo[Tx5PutI&(TX5FIFOSIZE-1)] = data; // put
  Tx5PutI++;  // Success, update
  return(FIFOSUCCESS);
}
// remove element from front of index FIFO
// return TXFIFOSUCCESS if successful
int Tx5Fifo_Get(char *datapt){
  if(Tx5PutI == Tx5GetI ){
    return(FIFOFAIL); // Empty if TxPutI=TxGetI
  }
  *datapt = Tx5Fifo[Tx5GetI&(TX5FIFOSIZE-1)];
  Tx5GetI++;  // Success, update
  return(FIFOSUCCESS);
}
// number of elements in index FIFO
// 0 to TX5FIFOSIZE-1
unsigned long Tx5Fifo_Size(void){
 return ((unsigned long)(Tx5PutI-Tx5GetI));
}

// Initialize UART5, interrupt driven
// Baud rate is 5,000,000/(ibrd+fbrd/64)
// ESP8266 needs 9600 bits/sec
// PE5=U5Tx,PE4=U5Rx UART5, bottom board
// inputs: ibrd,fbrd
//         priority 0 to 5 allowed
// outputs: none
void UART5_Init(uint32_t priority){
  if(priority>7){
    priority = 7;
  } 
  SYSCTL_RCGCUART_R |= 0x20; // activate UART5
  SYSCTL_RCGCGPIO_R |= 0x10; // activate port E
  Rx5Fifo_Init();                       // initialize empty FIFOs
  Tx5Fifo_Init();
  UART5_CTL_R &= ~UART_CTL_UARTEN;      // disable UART
         
  UART5_IBRD_R = 520;                   // IBRD = int(80,000,000 / (16 * 9600)) = int(520.833)
  UART5_FBRD_R = 53;                    // FBRD = round(0.833 * 64) = 53
                                        // 8 bit word length (no parity bits, one stop bit, FIFOs)
  UART5_LCRH_R = (UART_LCRH_WLEN_8|UART_LCRH_FEN);
  UART5_IFLS_R &= ~0x3F;                // clear TX and RX interrupt FIFO level fields
                                        // configure interrupt for TX FIFO <= 1/8 full
                                        // configure interrupt for RX FIFO >= 1/8 full
  UART5_IFLS_R += (UART_IFLS_TX1_8|UART_IFLS_RX1_8);
                                        // enable TX and RX FIFO interrupts and RX time-out interrupt
  UART5_IM_R |= (UART_IM_RXIM|UART_IM_TXIM|UART_IM_RTIM);
  UART5_CTL_R |= UART_CTL_UARTEN;       // enable UART
  GPIO_PORTE_AFSEL_R |= 0x30;           // enable alt funct on PE5-4
  GPIO_PORTE_DEN_R |= 0x30;             // enable digital I/O on PE5-4
                                        // configure PE5-4 as UART
  GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R&0xFF00FFFF)+0x00110000;
  GPIO_PORTE_AMSEL_R &= ~0x30;          // disable analog functionality on PE
                                        // set priority
  NVIC_PRI15_R = (NVIC_PRI15_R&0xFFFF00FF)|(priority<<13); // bits 13-15
  //interrupt 61, n=15, Bits 15:13 Interrupt [4n+1]
  NVIC_EN1_R = 0x01<<(61-32);           // enable interrupt 61 in NVIC
}

// copy from hardware RX FIFO to software RX FIFO
// stop when hardware RX FIFO is empty or software RX FIFO is full
void static copyHardwareToSoftware5(void){
  char letter;
  while(((UART5_FR_R&UART_FR_RXFE) == 0) && (Rx5Fifo_Size() < (RX5FIFOSIZE - 1))){
    letter = UART5_DR_R;
    Rx5Fifo_Put(letter);
  }
}
// copy from software TX FIFO to hardware TX FIFO
// stop when software TX FIFO is empty or hardware TX FIFO is full
void static copySoftwareToHardware5(void){
  char letter;
  while(((UART5_FR_R&UART_FR_TXFF) == 0) && (Tx5Fifo_Size() > 0)){
    Tx5Fifo_Get(&letter);
    UART5_DR_R = letter;
  }
}

// Are there any received messages to be input?
// return the number of characters available to be read
uint32_t ESP8266_AvailableInput(void){
  return Rx5Fifo_Size();
}
// output ASCII character to UART
// spin if TxFifo is full
void ESP8266_OutChar(char data){
  while(Tx5Fifo_Put(data) == FIFOFAIL){};
  UART5_IM_R &= ~UART_IM_TXIM;          // disable TX FIFO interrupt
  copySoftwareToHardware5();
  UART5_IM_R |= UART_IM_TXIM;           // enable TX FIFO interrupt
}
// at least one of three things has happened:
// hardware TX FIFO goes from 3 to 2 or less items
// hardware RX FIFO goes from 1 to 2 or more items
// UART receiver has timed out
void UART5_Handler(void){
  if(UART5_RIS_R&UART_RIS_TXRIS){       // hardware TX FIFO <= 2 items
    UART5_ICR_R = UART_ICR_TXIC;        // acknowledge TX FIFO
    // copy from software TX FIFO to hardware TX FIFO
    copySoftwareToHardware5();
    if(Tx5Fifo_Size() == 0){             // software TX FIFO is empty
      UART5_IM_R &= ~UART_IM_TXIM;      // disable TX FIFO interrupt
    }
  }
  if(UART5_RIS_R&UART_RIS_RXRIS){       // hardware RX FIFO >= 2 items
    UART5_ICR_R = UART_ICR_RXIC;        // acknowledge RX FIFO
    // copy from hardware RX FIFO to software RX FIFO
    copyHardwareToSoftware5();
  }
  if(UART5_RIS_R&UART_RIS_RTRIS){       // receiver timed out
    UART5_ICR_R = UART_ICR_RTIC;        // acknowledge receiver time out
    // copy from hardware RX FIFO to software RX FIFO
    copyHardwareToSoftware5();
  }
}

//------------ESP8266_OutString------------
// Output String (NULL termination)
// Input: pointer to a NULL-terminated string to be transferred
// Output: none
void ESP8266_OutString(char *pt){
  while(*pt){
    ESP8266_OutChar(*pt);
    pt++;
  }
}

//-----------------------ESP8266_OutUDec-----------------------
// Output a 32-bit number in unsigned decimal format
// Input: 32-bit number to be transferred
// Output: none
// Variable format 1-10 digits with no space before or after
void ESP8266_OutUDec(uint32_t n){
// This function uses recursion to convert decimal number
//   of unspecified length as an ASCII string
  if(n >= 10){
    ESP8266_OutUDec(n/10);
    n = n%10;
  }
  ESP8266_OutChar(n+'0'); /* n is between 0 and 9 */
}


// DelayMs
//  - busy wait n milliseconds
// Input: time to wait in msec
// Outputs: none
void DelayMs(uint32_t n){
  volatile uint32_t time;
  while(n){
    time = 6665;  // 1msec, tuned at 80 MHz
    while(time){
      time--;
    }
    n--;
  }
}

#define PE0       (*((volatile uint32_t *)0x40024004))    // RDY from ESP 8266
#define PE1       (*((volatile uint32_t *)0x40024008))    // RST_B to ESP8266
#define PE3       (*((volatile uint32_t *)0x40024020))    // LED to indicate that the 8266 connected
  
#define RDY   PE0
#define RDY1  0x01
#define RST   PE1
#define RST1  0x02
#define LOW   0x00

#define BIT0  0x01
#define BIT1  0x02
#define BIT2  0x04
#define BIT3  0x08   

// Initialize PE5,4,3,1,0 for interface to ESP8266
// Uses interrupt driven UART5 on PE5,4
// Uses simple GPIO output on PE3,1,0
void ESP8266_Init(void){
  SYSCTL_RCGCGPIO_R |= 0x10; // activate port E
  while((SYSCTL_PRGPIO_R&0x10)==0){};
  GPIO_PORTE_DIR_R |= 0x0A;       // output digital I/O on PE3,1
  GPIO_PORTE_DIR_R &= ~0x01;      // input digital I/O on PE0
  GPIO_PORTE_AFSEL_R &= ~0x0B;    // disable alt funct on PE3,1,0
  GPIO_PORTE_DEN_R |= 0x0B;       // enable digital I/O on PE3,1,0
  GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R&0xFFFF0F00);
  GPIO_PORTE_AMSEL_R &= ~0x0B;    // disable analog functionality on PE3,1,0
  UART5_Init(2);                  // Enable ESP8266 Serial Port 
  EnableInterrupts();
}
// Initialize ESP8266 interface and reset the
void ESP8266_Reset(void) {
  RST = LOW;      // Reset the 8266
  DelayMs(5000);  // Wait for 8266 to reset
  RST = RST1;     // Enable the 8266
  DelayMs(5000);  // Wait before setting up 8266
  PE3 = LOW;             // Turn off 
}

// ----------------------------------------------------------------------
// This routine sets up the Wifi connection between the TM4C and the
// hotspot. Enable the DEBUG flags in esp8266.h if you want to watch the transactions.
void ESP8266_SetupWiFi(void) { 
  while (!RDY) {      // Wait for ESP8266 indicate it is ready for programming data
    DelayMs(1000);
  }
  ESP8266_OutString(auth);    // Send authorization code
  ESP8266_OutChar(',');
  ESP8266_OutString(ssid);
  ESP8266_OutChar(',');
  ESP8266_OutString(pass); 
  ESP8266_OutChar(',');       // Extra comma needed for 8266 parser code
  ESP8266_OutChar('\n');      // Send NL to indicate EOT   

  //
  // This while loop receives debug info from the 8266 and optionally 
  // sends it out the debug port. The loop exits once the RDY signal
  // is deasserted and the serial port has no more character to xmit
  // 

  while(RDY){   // pause while RDY=1

    DelayMs(500);
  }
  while(ESP8266_GetMessage(RxMessage)){
  }

  Rx5Fifo_Init(); // flush buffer

  PE3 = BIT3;
}


