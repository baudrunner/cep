#include "audioOut.h"
#include "interrupt.h"

void __attribute__ ((interrupt("IRQ"))) isr_timer(void){

    TIMER2_IR = 0x01;   // INT-Acknowledge resp. switch off MR0-IRQ ( MatchRegister0 InterruptReQuest ) -> UM Chapter 6-6.1, Table 547  

	isrRoutine();

    VICVectAddr = 0;       // mark end of ISR  resp. trigger update of VIC priority logic -> UM Chapter 3-3.11, Table 113 
}



