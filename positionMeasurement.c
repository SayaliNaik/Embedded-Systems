
// Lab6.c
// this lab creates a position measurment software that outputs the result on LCD
// The software samples analog input signal and use calibration to output the position
// Runs on LM4F120 or TM4C123
// Use the SysTick timer to request interrupts at a particular period.
// Sayali Naik,Aishwarya Sawant Bhosle
// 3rd december

// 


#include "Nokia5110.h"
#include "pll.h"
#include "ADC.h"
#include "tm4c123gh6pm.h"

void SysTick_Init(void); 
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode


#define PF1       (*((volatile unsigned long *)0x40025008))
#define PF2       (*((volatile unsigned long *)0x40025010))
#define PF3       (*((volatile unsigned long *)0x40025020))
	

void PortF_Init(void){unsigned long volatile delay;
	SYSCTL_RCGC2_R |= 0x20;									//Initialize port F
	delay=SYSCTL_RCGC2_R;
	GPIO_PORTF_DIR_R |=0X04;								// PF2 is output
	GPIO_PORTF_AMSEL_R &= ~0x04;						//	disable analog on PortF
	GPIO_PORTF_PCTL_R &= ~0x00000F00; 			//  PCTL GPIO on PF2
	GPIO_PORTF_CR_R |= 0x11;
	GPIO_PORTF_AFSEL_R &= ~0xFF;						// disable alternate function on PortF
	GPIO_PORTF_DEN_R |= 0x04;								// enable digital pins ,PF2

}




unsigned long Data;      // 12-bit ADC
unsigned long Position;  // 32-bit fixed-point 0.001 cm
unsigned long ADCMail;
unsigned long ADCStatus;
unsigned long fixedConv=0;
unsigned long x;


//int main2(void){      // single step this program and look at Data
//  PLL_Init();         // Bus clock is 80 MHz 
//  ADC_Init();         // turn on ADC, set channel to 1
//  while(1){                
//    Data = ADC_In();  // sample 12-bit channel 1
//  }
//}

//int main(void){
//  PLL_Init();         // Bus clock is 80 MHz 
//  ADC_Init();         // turn on ADC, set channel to 1
//  PortF_Init();				//initializes port F for debugging
//  while(1){           // use scope to measure execution time for ADC_In and LCD_OutDec           
//    PF2 = 0x04;       // Profile
//    Data = ADC_In();  // sample 12-bit channel 1
//    PF2 = 0x00;       // Profile

//  }
//}


unsigned long Convert(unsigned long input)
	{
fixedConv =(( 241 * input)-42061)/512;	
 return fixedConv ;
}



int main(void){ 
	
	unsigned long a=0,b=0,c=0,d;
	
	DisableInterrupts();
	
  PLL_Init();        // Bus clock is 80 MHz 
  PortF_Init();
  ADC_Init();
	Nokia5110_Init();
	SysTick_Init();
	Nokia5110_Clear();
	// turn on ADC, set channel to 1
	EnableInterrupts();	
  while(1){  
		//Nokia5110_OutString("Sayali Ash 544");
		
		if(ADCStatus == 1){											//mailbox flag status
			Nokia5110_Clear(); 
			x = ADCMail;													//read the sample data and store in x
			 ADCStatus=0;													//set the ADCStatus to indicate the mailbox is now empty
			 Position=Convert(x);									//The sample data to a fixed point number
			
			if(x<=0x03)
		{
		Nokia5110_OutUDec(0);
		}
			
		else{
			c=1000;
			b=Position;
			
			if(b>=1000)
			{
				d=1;
				a=b-c;									//For the result to appear in decimal points
													//"d" represents the part before decimal
			}										//"a" reprents the part after decimal
		
				else 
			{
				d=0;
				a=b;
			}
	
			
	Nokia5110_OutUDec(d);
	Nokia5110_OutChar('.');
	Nokia5110_OutUDec(a);
		}
		}
	}
	}

void SysTick_Init(){
  NVIC_ST_CTRL_R = 0;         			// disable SysTick during setup
  NVIC_ST_RELOAD_R = 2000000-1;			// reload value
  NVIC_ST_CURRENT_R = 0;     				// any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x80000000; // priority 2          
  NVIC_ST_CTRL_R = 0x07; 						// enable SysTick with core clock and interrupts
																		// enable interrupts after all initialization is finished
}
 
void SysTick_Handler(void){
 GPIO_PORTF_DATA_R ^= 0x04;       // toggle PF2
ADCMail=ADC_In();									//same the sample data into ADCMail
ADCStatus=1;											//ADCStatus will indicate new data is available
 GPIO_PORTF_DATA_R ^= 0x04;				//toggle the output
	
}

