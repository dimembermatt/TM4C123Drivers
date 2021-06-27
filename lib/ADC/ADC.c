#include "ADC.h"

#include <stdint.h>
#include <inc/tm4c123gh6pm.h>


void ADC_Init(void){
    SYSCTL_RCGCGPIO_R |= 0x08;      // 1) activate clock for Port D 
  while((SYSCTL_PRGPIO_R&0x08) == 0){};
  GPIO_PORTD_DIR_R &= ~0x04;      // 2) make PD2 input
  GPIO_PORTD_AFSEL_R |= 0x04;     // 3) enable alternate fun on PD2
  GPIO_PORTD_DEN_R &= ~0x04;      // 4) disable digital I/O on PD2
  GPIO_PORTD_AMSEL_R |= 0x04;     // 5) enable analog fun on PD2
	volatile unsigned long delay;
  SYSCTL_RCGCADC_R |= 0x01;       // 6) activate ADC0 
  delay = SYSCTL_RCGCADC_R;       // extra time to stabilize
  delay = SYSCTL_RCGCADC_R;       // extra time to stabilize
  delay = SYSCTL_RCGCADC_R;       // extra time to stabilize
  delay = SYSCTL_RCGCADC_R;
  ADC0_PC_R = 0x01;               // 7) configure for 125K 
  ADC0_SSPRI_R = 0x0123;          // 8) Seq 3 is highest priority
  ADC0_ACTSS_R &= ~0x0008;        // 9) disable sample sequencer 3
  ADC0_EMUX_R &= ~0xF000;         // 10) seq3 is software trigger
 // ADC0_SSMUX3_R = (ADC0_SSMUX3_R&0xFFFFFFF0)+5;  // 11) Ain5 (PD2)
  ADC0_SSCTL3_R = 0x0006;         // 12) no TS0 D0, yes IE0 END0
  ADC0_IM_R &= ~0x0008;           // 13) disable SS3 interrupts
  ADC0_ACTSS_R |= 0x0008; 
  ADC0 = 1;
}


Error_t AnalogPort_Init(AnalogPort_t port){
    if(ADC0 == 0) return ERROR;

    switch(port){

        case(PE3):
            SYSCTL_RCGCGPIO_R |= 0x10;      // 1) activate clock for Port E
            while((SYSCTL_PRGPIO_R&0x10) == 0){}; //wait for clock to stabilize
            GPIO_PORTE_DIR_R &= ~0x08;      // 2) make pin input
            GPIO_PORTE_AFSEL_R |= 0x08;     // 3) enable alternate function 
            GPIO_PORTE_DEN_R &= ~0x08;      // 4) disable digital I/O 
            GPIO_PORTE_AMSEL_R |= 0x08;     // 5) enable analog for pin
            break;
        
        case(PE2):
            SYSCTL_RCGCGPIO_R |= 0x10;      // 1) activate clock for Port E 
            while((SYSCTL_PRGPIO_R&0x10) == 0){}; //wait for clock to stabilize
            GPIO_PORTE_DIR_R &= ~0x04;      // 2) make pin input
            GPIO_PORTE_AFSEL_R |= 0x04;     // 3) enable alternate function 
            GPIO_PORTE_DEN_R &= ~0x04;      // 4) disable digital I/O 
            GPIO_PORTE_AMSEL_R |= 0x04;     // 5) enable analog for pin
            break;

        case(PE1):
            SYSCTL_RCGCGPIO_R |= 0x10;      // 1) activate clock for Port E 
            while((SYSCTL_PRGPIO_R&0x10) == 0){}; //wait for clock to stabilize
            GPIO_PORTE_DIR_R &= ~0x02;      // 2) make pin input
            GPIO_PORTE_AFSEL_R |= 0x02;     // 3) enable alternate function 
            GPIO_PORTE_DEN_R &= ~0x02;      // 4) disable digital I/O 
            GPIO_PORTE_AMSEL_R |= 0x02;     // 5) enable analog for pin
            break;

        case(PE0):
            SYSCTL_RCGCGPIO_R |= 0x10;      // 1) activate clock for Port E 
            while((SYSCTL_PRGPIO_R&0x10) == 0){}; //wait for clock to stabilize
            GPIO_PORTE_DIR_R &= ~0x01;      // 2) make pin input
            GPIO_PORTE_AFSEL_R |= 0x01;     // 3) enable alternate function 
            GPIO_PORTE_DEN_R &= ~0x01;      // 4) disable digital I/O 
            GPIO_PORTE_AMSEL_R |= 0x01;     // 5) enable analog for pin
            break;

        case(PD3):
             SYSCTL_RCGCGPIO_R |= 0x08;      // 1) activate clock for Port D 
            while((SYSCTL_PRGPIO_R&0x08) == 0){}; //wait for clock to stabilize
            GPIO_PORTD_DIR_R &= ~0x08;      // 2) make pin input
            GPIO_PORTD_AFSEL_R |= 0x08;     // 3) enable alternate function 
            GPIO_PORTD_DEN_R &= ~0x08;      // 4) disable digital I/O 
            GPIO_PORTD_AMSEL_R |= 0x08;     // 5) enable analog for pin
            break;

        case(PD2):
            SYSCTL_RCGCGPIO_R |= 0x08;      // 1) activate clock for Port D 
            while((SYSCTL_PRGPIO_R&0x08) == 0){}; //wait for clock to stabilize
            GPIO_PORTD_DIR_R &= ~0x04;      // 2) make pin input
            GPIO_PORTD_AFSEL_R |= 0x04;     // 3) enable alternate function 
            GPIO_PORTD_DEN_R &= ~0x04;      // 4) disable digital I/O 
            GPIO_PORTD_AMSEL_R |= 0x04;     // 5) enable analog for pin
            break;

        case(PD1):
            SYSCTL_RCGCGPIO_R |= 0x08;      // 1) activate clock for Port D 
            while((SYSCTL_PRGPIO_R&0x08) == 0){}; //wait for clock to stabilize
            GPIO_PORTD_DIR_R &= ~0x02;      // 2) make pin input
            GPIO_PORTD_AFSEL_R |= 0x02;     // 3) enable alternate function 
            GPIO_PORTD_DEN_R &= ~0x02;      // 4) disable digital I/O
            GPIO_PORTD_AMSEL_R |= 0x02;     // 5) enable analog for pin
            break;

        case(PD0):
            SYSCTL_RCGCGPIO_R |= 0x08;      // 1) activate clock for Port D 
            while((SYSCTL_PRGPIO_R&0x08) == 0){}; //wait for clock to stabilize
            GPIO_PORTD_DIR_R &= ~0x01;      // 2) make pin input
            GPIO_PORTD_AFSEL_R |= 0x01;     // 3) enable alternate function 
            GPIO_PORTD_DEN_R &= ~0x01;      // 4) disable digital I/O 
            GPIO_PORTD_AMSEL_R |= 0x01;     // 5) enable analog for pin
            break;

        case(PE5):
            SYSCTL_RCGCGPIO_R |= 0x10;      // 1) activate clock for Port E 
            while((SYSCTL_PRGPIO_R&0x10) == 0){}; //wait for clock to stabilize
            GPIO_PORTE_DIR_R &= ~0x20;      // 2) make pin input
            GPIO_PORTE_AFSEL_R |= 0x20;     // 3) enable alternate function 
            GPIO_PORTE_DEN_R &= ~0x20;      // 4) disable digital I/O 
            GPIO_PORTE_AMSEL_R |= 0x20;     // 5) enable analog for pin
            break;
    }

    ports[port] = INIT;
    return NO_ERROR;
}

Error_t ReadAnalogPort(AnalogPort_t port, uint16_t* data){
    //Error if ADC0 not initialized
    if(ADC0 == 0) return ERROR;

    //Error if port not initialized
    if(ports[port] == UNINIT) return ERROR;

    ADC0_SSMUX3_R = (ADC0_SSMUX3_R&0xFFFFFFF0)+port; //set the ADC0 channel to the desired port
    ADC0_PSSI_R = 0x0008; //start ADC
	while((ADC0_RIS_R & 0x08) == 0) {}; //Busy wait. If it's ready flag becomes 0
	*data = ADC0_SSFIFO3_R&0xFFF; //read Data
	ADC0_ISC_R = 0x0008; //clear Flag

	return NO_ERROR;
}

