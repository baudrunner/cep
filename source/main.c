#include "spi.h"
#include "ConfigStick.h"
#include "malloc.h"
#include "mp3.h"
#include "audioOut.h"
#include "constants.h"
//#include "lpc24xx.h"
#define ARM

#define CPUFREQ 72000000

int16_t outBuf[OUTSIZE];
int16_t outputBuffer1[OUTSIZE];
int16_t outputBuffer2[OUTSIZE];

struct buffer *outBuf1;
struct buffer *outBuf2;
struct buffer *currentBuffer;

int main( void ){

    BaseStickConfig();
	sspInit();
 	menue();

	printf("###############################      main gestartet!\n");		

	outBuf1 = malloc(sizeof(struct buffer));
	outBuf1->sampleCnt = 0;
	outBuf1->data = outputBuffer1;         
    outBuf1->buffersize = READBUF_SIZE;  	
	outBuf2 = malloc(sizeof(struct buffer));
	outBuf2->sampleCnt = 0;
	outBuf2->data = outputBuffer2;         
    outBuf2->buffersize = READBUF_SIZE; 
	
	currentBuffer = outBuf1;
    

	initMp3Module();
	pwmInit();
	dacInit(outBuf1, outBuf2);
	initTimer();	
	
	
	int i = 0;
	while(i < 89000){
		//printf("Druchlauf %d\n",i);
		currentBuffer->sampleCnt = decode(currentBuffer->data);
		
		if(currentBuffer == outBuf1){	// Wenn Buffergröße erreicht ist, dann aktuellen Buffer aendern
			currentBuffer = outBuf2;
		}else{
			currentBuffer = outBuf1;
		}
 		FIO1PIN = ( FIO1PIN | (1<<LED1BIT) ); //WAITING_LED1 ON	
 		while( currentBuffer->sampleCnt > 0){


		if((FIO1PIN & 1 << 8) == 0){
			printf("sinnvolles\n");
		}};
 		if((FIO1PIN & 1 << 9) == 0){
			printf("blalalal\n");
		}};

		if((FIO1PIN & 1 << 10) == 0){
			printf("blalalal\n");
		}};

		if((FIO1PIN & 1 << 11) == 0){
			printf("blalalal\n");
		}};
	


 		FIO1PIN = ( FIO1PIN & ~(1<<LED1BIT) ); //WAITING_LED1 OFF	


/*
case 
		S0:  	Abspielen der Daten ab Flashadresse 0x000000 über PWM.1 und PWM.2, linker Kanal zusätzlich über DAC.
		S1:  	Abspielen der Daten ab Flashadresse 0x000000 über PWM.1 und PWM.2, rechter Kanal zusätzlich über DAC.
		S2:  	Abspielen der Daten ab Flashadresse 0x200000 über PWM.1 und PWM.2, linker Kanal zusätzlich über DAC.
		S3:  	Abspielen der Daten ab Flashadresse 0x200000 über PWM.1 und PWM.2, rechter Kanal zusätzlich über DAC.
*/


		i++;
	}
	mp3Cleanup();
    while(1){}
}//main
