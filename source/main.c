#include "spi.h"
#include "ConfigStick.h"
#include "malloc.h"
#include "mp3.h"
#include "audioOut.h"
#include "constants.h"
//#include "lpc24xx.h"
#include "psi.h"
#define ARM

#define CPUFREQ 72000000

//int16_t outBuf[OUTSIZE];
int16_t outputBuffer1[OUTSIZE];
int16_t outputBuffer2[OUTSIZE];

struct buffer *outBuf1;
struct buffer *outBuf2;
struct buffer *currentBuffer;
void restart(int startAddr);
int dacChannel;

int main( void ){

    BaseStickConfig();
	sspInit();
	printf("###############################      main gestartet!\n");		

	printf("MACH_REGISTER_COUNT Wert : %d\n", MACH_REGISTER_COUNT);
 	menue();	

	outBuf1 = malloc(sizeof(struct buffer));
	outBuf1->sampleCnt = 0;
	outBuf1->data = outputBuffer1;         
    outBuf1->buffersize = READBUF_SIZE;  	
	outBuf2 = malloc(sizeof(struct buffer));
	outBuf2->sampleCnt = 0;
	outBuf2->data = outputBuffer2;         
    outBuf2->buffersize = READBUF_SIZE; 
	
	currentBuffer = outBuf1;
    

	initMp3Module(0x000000);
	pwmInit();
	dacInit(outBuf1, outBuf2);
	//initTimer();	
	

	int fio1PinState;
	dacChannel = DAC_CHANNEL_LEFT;
	
	int running = 1;
	while(running){
		
		
		
 		fio1PinState = FIO1PIN;
 		
		if( (fio1PinState & S0_BIT) == 0){
 			//int psiInit( void );	
			dacChannel = DAC_CHANNEL_LEFT;
			restart(0x000000);
		}/*
		else if( (fio1PinState & S1_BIT) == 0){
			dacChannel = DAC_CHANNEL_RIGHT;
			restart(0x000000);	
		}else if( (fio1PinState & S2_BIT) == 0){
			dacChannel = DAC_CHANNEL_LEFT;
			restart(0x200000);	
		}else if( (fio1PinState & S3_BIT) == 0){
			dacChannel = DAC_CHANNEL_RIGHT;
			restart(0x200000);	
		}	
		
		currentBuffer->sampleCnt = decode(currentBuffer->data);
		
		if( currentBuffer->sampleCnt <= 0){ running = 0; } //mp3 scheint zu Ende
		
		if(currentBuffer == outBuf1){	// Wenn Buffergröße erreicht ist, dann aktuellen Buffer aendern
			currentBuffer = outBuf2;
		}else{
			currentBuffer = outBuf1;
		}

 	 	FIO1PIN = ( fio1PinState | (1<<LED1BIT)) ; //WAITING_LED1 ON	
		while( currentBuffer->sampleCnt > 0){};	
 		FIO1PIN = ( fio1PinState & ~(1<<LED1BIT) ); //WAITING_LED1 OFF		
		*/
	}
	mp3Cleanup();
    while(1){}
}//main


void restart(int startAddr){
	printf("cleanup(): ### leere output-Buffer...\n");
    
	outBuf1->sampleCnt = 0;	
	outBuf2->sampleCnt = 0;

	memset(outputBuffer1, 0, OUTSIZE);
    memset(outputBuffer2, 0, OUTSIZE); 

	mp3Cleanup();
	initMp3Module(startAddr);

	pwmInit();
	dacInit(outBuf1, outBuf2);
	//initTimer();		

	currentBuffer = outBuf1;
}
