#include <stdio.h>
#include "lpc24xx.h"
#include "config.h"
#include "ConfigStick.h"
#include "portlcd.h"
#include "fio.h"
#include "malloc.h"
#include "outline.h"

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


int tableIdx = 0;
int buffIdx = 0;


void __attribute__ ((interrupt("IRQ"))) isr_ms(void){
 
	TIMER3_IR = 0x01;   // INT-Acknowledge resp. switch off MR0-IRQ ( MatchRegister0 InterruptReQuest ) -> UM Chapter 6-6.1, Table 547  
	msRuntime++;
	VICVectAddr = 0;       // mark end of ISR  resp. trigger update of VIC priority logic -> UM Chapter 3-3.11, Table 113 
}


void __attribute__ ((interrupt("IRQ"))) isr_timer(void){

    TIMER2_IR = 0x01;   // INT-Acknowledge resp. switch off MR0-IRQ ( MatchRegister0 InterruptReQuest ) -> UM Chapter 6-6.1, Table 547  

	if(isrbuffer->state == EMPTY){
 	   // GPIO1_IOPIN = ( GPIO1_IOPIN | (1<<LED2BIT) ); //UNDERFLOW_LED2 ON
    }else{   
 	    //GPIO1_IOPIN = ( GPIO1_IOPIN & ~(1<<LED2BIT) ); //UNDERFLOW_LED2 OFF

 	    DACR = (isrbuffer->data[isrBufferIdx] << 6); // Wert auf den DAC schreiben
		count++;
 	    isrBufferIdx++;
 		printCount++;

		if(isrBufferIdx == BUFFERSIZE){ 
			isrbuffer->state = EMPTY;	//den letzten Wert ausgegeben
			
			if(isrbuffer == buffer1){	// Wenn Buffergröße erreicht ist, dann aktuellen Buffer aendern
				isrbuffer = buffer2;
			}else{
				isrbuffer = buffer1;
			}
 		    isrBufferIdx = 0; // wieder am Anfang des neuen Buffers anfangen zu lesen
		}	
    }
    VICVectAddr = 0;       // mark end of ISR  resp. trigger update of VIC priority logic -> UM Chapter 3-3.11, Table 113 
}




void dacInit (void)
{
 
    PCLKSEL0 |= 0x01<<22; // clock für DAC setzten
    PINSEL1  |= 0x10<<20; // AOUT Pin wählen ( Was bringt : PINMODE1  = 0x01; )
	
	// Buffer für Wechselseitiges Lesen/Schreiben
	buffer1 = malloc(sizeof(struct buffer));
    buffer1->state = EMPTY;
    buffer1->data = malloc(sizeof(*buffer1->data) * BUFFERSIZE);
    buffer1->buffersize = BUFFERSIZE;

    buffer2 = malloc(sizeof(struct buffer));
    buffer2->state = EMPTY;
    buffer2->data = malloc(sizeof(*buffer2->data) * BUFFERSIZE);
    buffer2->buffersize = BUFFERSIZE;

	actualbuffer = buffer1;
	isrbuffer = buffer1;
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


    PCONP     |= (1<<23); // enable timer
	PCLKSEL1  |= (1<<12); // set clock to cclk (48 MHz)
	TIMER3_TCR =  0x02; // reset timer
	TIMER3_MR0 =  12000; // set match register 0 -->   1000Hz ( warum nicht 48000 ??? )
	TIMER3_PR  =  0; // set prescaler
	TIMER3_IR  =  0xff; // reset all interrrupts
	TIMER3_MCR =  3<<0; // interrupt on MR0, reset on MR0, do not stop timer
	TIMER3_TCR =  0x01; // start timer	

  	VICIntSelect      &=  ~(1<<27);          // Timer3 assigned to IRQ category and NOT FIQ   -> UM Chapter 7-3.6,  Table 108 (& 117)
	VICVectPriority27  =  15;                    // assign Prio 15 for Timer3                      -> UM Chapter 7-3.10, Table 112 (& 102, 117)
	VICVectAddr27      =  (int)( isr_ms );   // bind "timer isr" to Timer3 IRQ                -> UM Chapter 7-3.11, Table 113 (& 102, 117)
	VICIntEnable       =  (1<<27);           // enable interrupt for Timer3 IRQ               -> UM Chapter 7-3.4,  Table 106 (& 117)		

    timerRunning = 1;	
}
/************************************** EOF *********************************/
