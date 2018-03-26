// ADC.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0 SS3 to be triggered by
// software and trigger a conversion, wait for it to finish,
// and return the result.
// Put your name here 
// Put the date here


#include "tm4c123gh6pm.h"

// This initialization function 
// Max sample rate: <=125,000 samples/second
// Sequencer 0 priority: 1st (highest)
// Sequencer 1 priority: 2nd
// Sequencer 2 priority: 3rd
// Sequencer 3 priority: 4th (lowest)
// SS3 triggering event: software trigger
// SS3 1st sample source: Ain1 (PE2)
// SS3 interrupts: flag set on completion but no interrupt requested
void ADC_Init(void)
{
volatile unsigned long delay;
SYSCTL_RCGCGPIO_R |= 0x10; 								// 1) activate clock for Port E
while ((SYSCTL_RCGCGPIO_R&0x10) == 0){};		
GPIO_PORTE_DIR_R &= ~0x04; 								// 2) make PE2 input
GPIO_PORTE_AFSEL_R |= 0x04; 							// 3) enable alternate fun on PE2
GPIO_PORTE_DEN_R &= ~0x04; 								// 4) disable digital I/O on PE2
GPIO_PORTE_AMSEL_R |= 0x04;					 			// 5) enable analog fun on PE2
SYSCTL_RCGCADC_R |= 0x01; 								// 6) activate ADC0
ADC0_PC_R = 0x01; 												// 7) configure for 125K
ADC0_SSPRI_R = 0x0123; 										// 8) Seq 3 is highest priority
ADC0_ACTSS_R &= ~0x0008; 									// 9) disable sample sequencer 3
ADC0_EMUX_R &= ~0x0F00; 									// 10) seq2 is software trigger
ADC0_SSMUX3_R = (ADC0_SSMUX3_R&0xFFFFFFF0)+1; // 11) Ain1 (PE2)									
ADC0_SSCTL3_R = 0x0006; 									// 12) no TS0 D0, yes IE0 END0
ADC0_IM_R &= ~0x0008; 										// 13) disable SS3 interrupts
ADC0_ACTSS_R |= 0x0008; 									// 14) enable sample sequencer 3

}
//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
unsigned long ADC_In(void)
 
{  unsigned long data;
  ADC0_PSSI_R = 0x0008;            			// 1) initiate SS2
  while((ADC0_RIS_R&0x08)==0){};   			// 2) wait for conversion done
  data = ADC0_SSFIFO3_R&0xFFF;   				// 3) read result
  ADC0_ISC_R = 0x0008;             			// 4) acknowledge completion
  return data;
}


