// 0.Documentation Section 
// Oscilator, main.c

// Runs TM4C123 LaunchPad
// Input From: PF0, PF4
// Output to:	 PF2, PF3
//
// 
// Author: Sayali Naik
// Date: 9/28/2016

// 1. Pre-processor Directives Section
// Constant declarations to access port registers using 
// symbolic names instead of addresses

#define GPIO_PORTF_DATA_R       (*((volatile unsigned long *)0x400253FC))
#define GPIO_PORTF_DIR_R        (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_AFSEL_R      (*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_PUR_R        (*((volatile unsigned long *)0x40025510))
#define GPIO_PORTF_DEN_R        (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_LOCK_R       (*((volatile unsigned long *)0x40025520))
#define GPIO_PORTF_CR_R         (*((volatile unsigned long *)0x40025524))
#define GPIO_PORTF_AMSEL_R      (*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R       (*((volatile unsigned long *)0x4002552C))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))




// 2. Declarations Section
//   Global Variables

int delay;

//   Function Prototypes 

void PortF_Init(void);

void systick_Init(void);
void Delay(unsigned long frequency);


// 3. Subroutines Section
// MAIN: Mandatory for a C Program to be executable


int main(void){
	PortF_Init();
	
 
	while(1){
	
switch(GPIO_PORTF_DATA_R&0x11)
			
		{
			case 0x00:												  //both switches are pressed
				GPIO_PORTF_DATA_R &=~ 0x0C;				//no LEDs are glowing and no sound
				break;
				
		 
			case 0x01:												  //press switch1
				GPIO_PORTF_DATA_R ^= 0x04;				//blue LED is pressed and produce frequency of 262Hz
				Delay(262);
				break;
			
		
			case 0x10:													//press switch2
				GPIO_PORTF_DATA_R ^= 0x08;				//green LED is pressed and produce frequency of 262Hz
				Delay(392);
				break;
				

			case 0x11:
				GPIO_PORTF_DATA_R &=~ 0x0C;				//no switch is pressed
				break;
		}	
		
		}

	}

	
	
	// Subroutine to initialize ports for input and output

	
	void PortF_Init()
		
	{
	SYSCTL_RCGC2_R |= 0x20;									//Initialize port F
	delay=SYSCTL_RCGC2_R;
	GPIO_PORTF_DIR_R |=0X0C;								// PF0 and PF4 are outputs on PortF and PF2,PF3 are inputs
	GPIO_PORTF_AMSEL_R &= ~0x1D;						//	disable analog on PortF
	GPIO_PORTF_LOCK_R |= 0x4C4F434B;
	GPIO_PORTF_PCTL_R &= ~0x000FFF0F; 			//  PCTL GPIO on PF4 and PF0
	GPIO_PORTF_CR_R |= 0x11;
	GPIO_PORTF_AFSEL_R &= ~0xFF;						// disable alternate function on PortF
	GPIO_PORTF_PUR_R |= 0x11;								// enable pull-up on PF0 and PF4
	GPIO_PORTF_DEN_R |= 0x1D;								// enable digital pins PF0,PF2,PF3,PF4
	}
	
// MODIFY and use the following delay function to create delay periods
// We will make a precise estimate later: 
//   For now we assume it takes 0.1 sec to count down
//   from 145448 down to zero
// Inputs: Modify as needed
// Outputs: None
// Notes: ...

void Delay(unsigned long frequency){unsigned long volatile time;
  unsigned long volatile x;
	
	x=(1454480/frequency);										//for 0.1 sec  counnt from 145448
																						//thus for 262 we count from (145448/(262*0.1))				
time = x/2 ;																//for each half cycle
	
  while(time){															//till time=0,repeat this loop
		time--;
  }
}




