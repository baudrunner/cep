#include "isrRoutine.h"


void isrRoutine(void){
	FIO1PIN = ( FIO1PIN ^ (1<<LED3BIT) ); //UNDERFLOW_LED3 TOGGLE

	if(currentIsrBuffer->sampleCnt <= 0){
 	   FIO1PIN = ( FIO1PIN | (1<<LED2BIT) ); //UNDERFLOW_LED2 ON
    }else{   
 	    FIO1PIN = ( FIO1PIN & ~(1<<LED2BIT) ); //UNDERFLOW_LED2 OFF
		//PWM1_MR1 = ((uint16_t)currentIsrBuffer->data[isrBufferIdx] / 40);  // viel zu langsam
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

