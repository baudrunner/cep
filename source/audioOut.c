#include <stdio.h>
#include "lpc24xx.h"
#include "config.h"
#include "ConfigStick.h"
#include "portlcd.h"
#include "fio.h"
#include "malloc.h"
#include "isrRoutine.h"
#include "audioOut.h"

char stringBufferForLcdPrint[33]; //16 stellen pro LCD-Zeile + term. null
int *tabelle[N_STUETZ+1];

int count = 0;
int msRuntime = 0;
int running = 0;
int timerRunning = 0;
int printCount = 0;
int isrBufferIdx = 0;
int freq_signal = 440;
int a_fixpoint = 30504029;
int b_fixpoint = 621;

struct buffer *IsrBuffer1;
struct buffer *IsrBuffer2;
struct buffer *currentIsrBuffer;

int tableIdx = 0;
int buffIdx = 0;

void __attribute__ ((interrupt("IRQ"))) isr_timer(void){

    TIMER2_IR = 0x01;   // INT-Acknowledge resp. switch off MR0-IRQ ( MatchRegister0 InterruptReQuest ) -> UM Chapter 6-6.1, Table 547  
	isrRoutine();
    VICVectAddr = 0;       // mark end of ISR  resp. trigger update of VIC priority logic -> UM Chapter 3-3.11, Table 113 
}

void pwmInit(){

	PCONP |= (1<<6);                          // Enable PWM1
    PCLKSEL0 |= (1<<12);                      // Clock == CCLK (48 MHz)
    PINSEL4 |= (1<<0) | (1<<2) | (1<<4);      // P2.0 istPWM1.1, P2.1 istPWM1.2, P2.2 istPWM1.3
    PWM1_PR =  0;               // Prescaler, PWM-Takt= 1 Hz
    PWM1_MCR = (1<<0) | (1<<1);               // Interrupt on PWMMR0, Reset on PWMMR0
    PWM1_PCR = //(1<<2) | (1<<3) |            // PWM1.2 double edge, PWM1.3 double edge,
               (1<<9) | (1<<10); // PWM1.1 PWM1.2 PWM1.3 enabled
    PWM1_MR0 = MACH_REGISTER_COUNT;                             // PWM-Period
    PWM1_LER = (1<<0) | (1<<1) |              // Latch Enable
               (1<<2) | (1<<3);
    PWM1_TCR = (1<<0) | (1<<3);               // Counter Enable, PWM Enable

}

void dacInit (struct buffer *isrBuf1, struct buffer *isrBuf2)
{
 
    PCLKSEL0 |= 0x01<<22; // clock für DAC setzten
    PINSEL1  |= 0x10<<20; // AOUT Pin wählen ( Was bringt : PINMODE1  = 0x01; )
	
	IsrBuffer1 = isrBuf1;
	IsrBuffer2 = isrBuf2;	

	currentIsrBuffer = IsrBuffer1;		
}

void initTimer(){
    PCONP    |= (1<<22); // enable timer
	PCLKSEL1 |= (1<<12); // set clock to cclk (48 MHz)
	TIMER2_TCR = 0x02; // reset timer
	TIMER2_MR0 = MACH_REGISTER_COUNT; // set match register 0 -->   44.117,64706 hz
	TIMER2_PR  =  0; // set prescaler
	TIMER2_IR  =  0xff; // reset all interrrupts
	TIMER2_MCR = 3<<0; // interrupt on MR0, reset on MR0, do not stop timer
	TIMER2_TCR = 0x01; // start timer

 	VICIntSelect      &=  ~(1<<26);          // Timer2 assigned to IRQ category and NOT FIQ   -> UM Chapter 7-3.6,  Table 108 (& 117)
	VICVectPriority26  =  1;                    // assign Prio 5 for Timer2                      -> UM Chapter 7-3.10, Table 112 (& 102, 117)
	VICVectAddr26      =  (int)( isr_timer );   // bind "timer isr" to Timer2 IRQ                -> UM Chapter 7-3.11, Table 113 (& 102, 117)
	VICIntEnable       =  (1<<26);           // enable interrupt for Timer2 IRQ               -> UM Chapter 7-3.4,  Table 106 (& 117)	
}

/************************************** EOF *********************************/
