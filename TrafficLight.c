// TrafficLight.c
// Runs on LM4F120 or TM4C123
// Index implementation of a Moore finite state machine to operate
// a traffic light.
// Your Name:Sayali Naik and Aishwarya Sawant Bhosle
// created: 10 October,2016

// Enter description of your program here
// Inputs: PA5,PA4,PA3,PA2
// Outputs:PB7,PB6,PB5,PB4,PB3,PB2,PB1,PB0
// Walk Request on Warner pressed, detector connected to PA5 (1=pedestrian present)
// Walk Request on W1 pressed, detector connected to PA4 (1=pedestrian present)
// Warner Street car detector connected to PA3 (1=car present)
// W1 street car detector connected to PA2 (1=car present)

// WalkWarner connected to PB7
// WalkW1 connected to PB6
// Warner Street red light connected to PB5
// Warner Street yellow light connected to PB4
// Warner Street green light connected to PB3
// W1 Street red light connected to PB2
// W1 Street yellow light connected to PB1
// W1 Street green light connected to PB0


// Define ports addresses here

#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))

#define GPIO_PORTB_DATA_R       (*((volatile unsigned long *)0x400053FC))
#define GPIO_PORTB_DIR_R        (*((volatile unsigned long *)0x40005400))
#define GPIO_PORTB_DEN_R        (*((volatile unsigned long *)0x4000551C))
#define GPIO_PORTB_AMSEL_R      (*((volatile unsigned long *)0x40005528))
#define GPIO_PORTB_PCTL_R       (*((volatile unsigned long *)0x4000552C))
#define GPIO_PORTB_AFSEL_R      (*((volatile unsigned long *)0x40005420))

	
#define GPIO_PORTA_DATA_R       (*((volatile unsigned long *)0x400043FC))   
#define GPIO_PORTA_DIR_R        (*((volatile unsigned long *)0x40004400)) 
#define	GPIO_PORTA_DEN_R        (*((volatile unsigned long *)0x4000451C)) 
#define GPIO_PORTA_AFSEL_R      (*((volatile unsigned long *)0x40004420))	
#define GPIO_PORTA_AMSEL_R     	(*((volatile unsigned long *)0x40004528))
#define GPIO_PORTA_PCTL_R       (*((volatile unsigned long *)0x4000452C))

// add any needed port definitions 

#define SYSCTL_RCGC2_GPIOA      (*((volatile unsigned long *)0x00000001))   // port A Clock Gating Control
#define SYSCTL_RCGC2_GPIOB     	(*((volatile unsigned long *) 0x00000002 )) // port B Clock Gating Control


#define NVIC_ST_CTRL_R		(*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R	(*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R	(*((volatile unsigned long *)0xE000E018))


	

// Functions Prototypes for: SysTick_Init

void SysTick_Init(void);
void SysTick_Wait(unsigned long delay);
void SysTick_Wait10ms(unsigned long DELAY);


	// Linked data structure
	// define your structure here

struct State {
  unsigned long Out;            												// 8-bit output
  unsigned long Time;          												  // 10 ms
  const struct State *Next[8];};												// depends on 3-bit input

typedef const struct State SType;
					
	//define your states here e.g. #define stateName 0, etc.
	
#define goWarner &FSM[0]
#define waitWarner &FSM[1]
#define goW1 &FSM[2]
#define waitW1 &FSM[3]
#define Walk &FSM[4]

	
	
	//Declare your states here 

	SType FSM[5]={
 {0x0C,700,{goWarner,waitWarner	,goWarner	,waitWarner	,waitWarner	,waitWarner	,waitWarner	,waitWarner}},
 {0x14,200,{goW1		,goW1				,goW1			,goW1				,Walk				,Walk				,Walk				,goW1}},
 {0x21,700,{waitW1	,goW1				,waitW1		,waitW1			,waitW1			,waitW1			,waitW1			,waitW1}},
 {0x22,200,{goWarner,goWarner		,goWarner	,goWarner		,Walk				,Walk				,Walk				,Walk}},
 {0xE4,500,{goWarner,goW1				,goWarner	,goWarner		,Walk				,goW1				,goWarner		,goWarner}}};
	
	//main program
 
int main(void){ 
	
volatile unsigned long delay;
volatile unsigned long Input,Warner_SW,W1_SW,BothWalk_SW;
	
  
	SType *Ptr; 																				// state pointer
  
	
	// Initialize Ports and SysTick here
  SYSCTL_RCGC2_R |=0x0000001;
	delay = SYSCTL_RCGC2_R;															// activate port A and port B	
	SYSCTL_RCGC2_R |=0x0000002;
	delay = SYSCTL_RCGC2_R;														
	
	
  SysTick_Init();           													// initialize SysTick timer
	
  GPIO_PORTA_DIR_R &= ~0x3C;;   											// make PA5-2 in
  GPIO_PORTA_DEN_R |= 0x3C;														// enable digital I/O on PA5-2
  GPIO_PORTA_AFSEL_R &= ~0x3C;												// disable alternate function on PA5-PA2
	GPIO_PORTA_AMSEL_R &= ~0x3C;												// disable analog function on PA5-PA2
	GPIO_PORTA_PCTL_R  &= ~0x00FFFF00;									// use PA5-PA2 as GPIOs
	
	
	
	GPIO_PORTB_DIR_R |= 0xFF; 													// make PB7-0 out
  GPIO_PORTB_DEN_R |= 0xFF; 													// enable digital I/O on PB7-0
	GPIO_PORTB_AFSEL_R &= ~0xFF;												// disable alternate function on PB7-PB0
	GPIO_PORTB_AMSEL_R &= ~0xFF;												// disable analog function on PB7-PB0
	GPIO_PORTB_PCTL_R  &= ~0xFFFFFFFF;									// use PB7-PB0 as GPIOs
	
	
	//main code
	
	
	Ptr = goWarner;
	  
  while(1){
  GPIO_PORTB_DATA_R = Ptr->Out;
    SysTick_Wait10ms(Ptr->Time);
		
		W1_SW = GPIO_PORTA_DATA_R & 0x20;									//Walk button on W1 from input PA5
		Warner_SW = GPIO_PORTA_DATA_R & 0x10;							//Walk button on Warner from input PA4
								
		
		if ((Warner_SW || W1_SW )==1)											//if any walk pressed,walk on both streets enabled
			BothWalk_SW = 1;
		else																							//else walk signal is off
			BothWalk_SW = 0;
			
		
    Input = (GPIO_PORTA_DATA_R&0x0C) >> 2;						//Read input on PA2 and PA3 and shift right by 2 bits
		BothWalk_SW = BothWalk_SW << 2;										//Shift left by 2 bits 
		
		Input = Input | BothWalk_SW;											//Input and BothWalk_SW merged to form 3 bit input
		
    Ptr = Ptr->Next[Input];														//Pointer allows to move from one state to another. 
		
  }
}

void SysTick_Init()
	{
	NVIC_ST_CTRL_R |= 0; 																// 1) disable SysTick during setup
	NVIC_ST_RELOAD_R = 0x00FFFFFF; 											// 2) maximum reload value
	NVIC_ST_CURRENT_R = 0; 															// 3) any write to CURRENT clears it
	NVIC_ST_CTRL_R = 0x00000005; 												// 4) enable SysTick with core clock
}
	
void SysTick_Wait(unsigned long delay){
NVIC_ST_RELOAD_R = delay-1; 													// number of counts
NVIC_ST_CURRENT_R = 0; 																// any value written to CURRENT clears
while((NVIC_ST_CTRL_R&0x00010000)==0){ 								// wait for flag
}
}


void SysTick_Wait10ms(unsigned long DELAY){
unsigned long i;																			// delay will run for 10*Time 
for(i=0; i<DELAY; i++){
SysTick_Wait(160000); 																// Frequency of oscillator is 16MHz
}																											
}
	
