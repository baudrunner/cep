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

	printf("###############################      main gestartet!\n");		
	printf("###############################      MACH_REGISTER_COUNT = %d\n",MACH_REGISTER_COUNT);		

	//int buffersize = MAINBUF_SIZE;  //1940
	
	outBuf1 = malloc(sizeof(struct buffer));
	outBuf2 = malloc(sizeof(struct buffer));
	
	menue();

	outBuf1->sampleCnt = 0;
	outBuf1->data = outputBuffer1;         
    outBuf1->buffersize = READBUF_SIZE;  	
	
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
		
		//printf("warte bis buffer %p leer ist...\n",currentBuffer);	
 		FIO1PIN = ( FIO1PIN | (1<<LED1BIT) ); //WAITING_LED1 ON	
 		while( currentBuffer->sampleCnt > 0){};	
 		FIO1PIN = ( FIO1PIN & ~(1<<LED1BIT) ); //WAITING_LED1 OFF	

		i++;
	}


	//fillBuffer();
	//initMp3Module();	
	//decodeMp3();
	//fillBuffer();	
	//decodeMp3();	
	
	/*
	int i;
	for(i = 0; i < 3; i++){
		decodeMp3();
		fillBuffer();
	}*/
    

	mp3Cleanup();

	
	//char *data = spiReadBytes(buffersize, 0, SPI_INT);
		


    //initMp3Module(data);

    //dac();



    while(1){}
}//main
