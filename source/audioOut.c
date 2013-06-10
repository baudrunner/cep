#include <stdio.h>
#include "lpc24xx.h"
#include "config.h"
#include "ConfigStick.h"
#include "portlcd.h"
#include "fio.h"
#include "malloc.h"
#include "interrupt.h"
#include "audioOut.h"


char stringBufferForLcdPrint[33]; //16 stellen pro LCD-Zeile + term. null

int *tabelle[N_STUETZ+1];
//var theFunction = selectFunction(); //Funktion festlegen (Sinus oder Dreieck)

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

void isrRoutine(){
	FIO1PIN = ( FIO1PIN ^ (1<<LED3BIT) ); //UNDERFLOW_LED3 TOGGLE

	if(currentIsrBuffer->sampleCnt <= 0){
 	   FIO1PIN = ( FIO1PIN | (1<<LED2BIT) ); //UNDERFLOW_LED2 ON
    }else{   
 	    FIO1PIN = ( FIO1PIN & ~(1<<LED2BIT) ); //UNDERFLOW_LED2 OFF
		//PWM1_MR1 = ((uint16_t)currentIsrBuffer->data[isrBufferIdx] / 40);
 		PWM1_MR1 = ((((int)currentIsrBuffer->data[isrBufferIdx] + (1 << 15) )  * 1633) >> 16);
				
		PWM1_LER = (1<<0) | (1<<1) |              // Latch Enable
                   (1<<2) | (1<<3);	
		DACR = (  ((a_fixpoint + (b_fixpoint * currentIsrBuffer->data[isrBufferIdx]))>>N_NACHKOMMA)      << 6); // Wert auf den DAC schreiben
		count++;
 	    isrBufferIdx += 2;
 		printCount++;

		if(currentIsrBuffer->sampleCnt <= isrBufferIdx){ 
			currentIsrBuffer->sampleCnt = 0;
			if(currentIsrBuffer == IsrBuffer1){	// Wenn Buffergröße erreicht ist, dann aktuellen Buffer aendern
				currentIsrBuffer = IsrBuffer2;
			}else{
				currentIsrBuffer = IsrBuffer1;
			}
 		    isrBufferIdx = 0; // wieder am Anfang des neuen Buffers anfangen zu lesen
		}	
    }
}

void pwmInit(){

	PCONP |= (1<<6);                          // Enable PWM1
    PCLKSEL0 |= (1<<12);                      // Clock == CCLK (48 MHz)
    PINSEL4 |= (1<<0) | (1<<2) | (1<<4);      // P2.0 istPWM1.1, P2.1 istPWM1.2, P2.2 istPWM1.3
    PWM1_PR =  0;               // Prescaler, PWM-Takt= 1 Hz
    PWM1_MCR = (1<<0) | (1<<1);               // Interrupt on PWMMR0, Reset on PWMMR0
    PWM1_PCR = //(1<<2) | (1<<3) |            // PWM1.2 double edge, PWM1.3 double edge,
               (1<<9);// | (1<<10) | (1<<11); // PWM1.1 PWM1.2 PWM1.3 enabled
    PWM1_MR0 = 1633;                             // PWM-Period
    PWM1_MR1 = 816;                             // PWM-Match 1
    PWM1_LER = (1<<0) | (1<<1) |              // Latch Enable
               (1<<2) | (1<<3);
    PWM1_TCR = (1<<0) | (1<<3);               // Counter Enable, PWM Enable

}

void dacInit (struct buffer *isrBuf1, struct buffer *isrBuf2)
{
 
    PCLKSEL0 |= 0x01<<22; // clock für DAC setzten
    PINSEL1  |= 0x10<<20; // AOUT Pin wählen ( Was bringt : PINMODE1  = 0x01; )
	
/*
	
	// Buffer für Wechselseitiges Lesen/Schreiben
	buffer1 = malloc(sizeof(struct buffer));
    buffer1->state = EMPTY;
    buffer1->data = malloc(sizeof(*buffer1->data) * BUFFERSIZE);
    buffer1->buffersize = BUFFERSIZE;
	
    buffer2 = malloc(sizeof(struct buffer));
    buffer2->state = EMPTY;
    buffer2->data = malloc(sizeof(*buffer2->data) * BUFFERSIZE);
    buffer2->buffersize = BUFFERSIZE; 
*/

	//actualbuffer = buffer1;

	IsrBuffer1 = isrBuf1;
	IsrBuffer2 = isrBuf2;	

	currentIsrBuffer = IsrBuffer1;		
}

/*
void dacOut(){

	while(1){

			//WAIT_LED ON
		    // GPIO1_IOPIN = ( GPIO1_IOPIN | (1<<LED1BIT) ); //aktiviere LED1 (IDLE)	
	
	        while(actualbuffer->state != EMPTY){   //solange puffer noch voll
	        }
	        
			//WAIT_LED OFF
	 	    GPIO1_IOPIN = ( GPIO1_IOPIN & ~(1<<LED1BIT) ); //deaktiviere LED1 (IDLE)
			
	        //FILL actualbuffer
			while(buffIdx < BUFFERSIZE){
				
				if( tableIdx >= (N_STUETZ << N_NACHKOMMA) ){
					tableIdx = tableIdx - (N_STUETZ << N_NACHKOMMA) ;
				}
	
				//							       A      +       B     *                 Nstuetz         <RUNDEN>
				actualbuffer->data[buffIdx] =   ( a_fixpoint + (b_fixpoint *   table_wave[4]))>>N_NACHKOMMA ;
				tableIdx += offset;		
				buffIdx++;
			}
	
			buffIdx = 0;
	       
	        actualbuffer->state = FULL;
			if(timerRunning == 0){
				initTimer();	
			}
	 	    	
	        // Buffer Wechseln
	        if(actualbuffer == buffer1){
	            actualbuffer = buffer2;
	 		    GPIO1_IOPIN = ((GPIO1_IOPIN | (1<<LED4BIT)) & ~(1<<LED3BIT)); //Activate LED4 & deactivate LED3		
	        }else{
				actualbuffer = buffer1;
	 		    GPIO1_IOPIN = ((GPIO1_IOPIN | (1<<LED3BIT)) & ~(1<<LED4BIT)); //Activate LED3 & deactivate LED4			
	        }  
	}
}
*/

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

    timerRunning = 1;	
}

/************************************** EOF *********************************/
