#include "mp3.h"

int16_t outBuf[NUMOFCHAN][OUTSIZE];
unsigned char inputBuffer[MAINBUF_SIZE];
unsigned char *inBuf = &inputBuffer[0];
int spiDataPosition = 0;
int bytesLeft;
HMP3Decoder mp3Dec;

int initMp3Module(void){

	printf("initialisiere mp3 Decoder...\n");
	mp3Dec = MP3InitDecoder();

	if(mp3Dec == NULL){
		printf("ERROR Initialisierung des mp3 Decoder fehlgeschlagen!\n");
	}else{
		printf("mp3 Decoder initialisiert!\n");
	}
	
	//int nextSyncWord = MP3FindSyncWord((unsigned char*)inBuf, MAINBUF_SIZE); 

    //printf("nextSyncWord = %d\n",nextSyncWord);	


	//void MP3GetLastFrameInfo(HMP3Decoder hMP3Decoder,MP3FrameInfo *mp3FrameInfo);

	spiDataPosition	= 0;

	bytesLeft = 0;
	//unsigned char *pInput;
	//inBuf = (unsigned char *)inBuf + nextSyncWord;

	//inBuf = &inputBuffer[0] + nextSyncWord;
	inBuf = &inputBuffer[0];	
	printf("inBuf= %p\ninputBuffer= %p\n",inBuf,inputBuffer);
	


/*
	MP3FrameInfo mp3FrameInfo;

	int mp3DecodeResu = MP3Decode(mp3Dec, &pInput, &bytesLeft, outBuf[1], 0);
	printf("mp3DecodeResu= %d\n",mp3DecodeResu);

	MP3GetLastFrameInfo(mp3Dec, &mp3FrameInfo);

	printf("#######  MP3 HEADER  ########\n");	
	printf("# bitrate =      %10d #\n", mp3FrameInfo.bitrate);
	printf("# nChans =       %10d #\n", mp3FrameInfo.nChans);
	printf("# samprate =     %10d #\n", mp3FrameInfo.samprate);
	printf("# bitsPerSample =%10d #\n", mp3FrameInfo.bitsPerSample);
	printf("# outputSamps =  %10d #\n", mp3FrameInfo.outputSamps);
	printf("# layer =        %10d #\n", mp3FrameInfo.layer);	
	printf("# version =      %10d #\n", mp3FrameInfo.version);	
	printf("#############################\n");		

	printf("MP3FreeDecoder()...\n");
	MP3FreeDecoder(mp3Dec);	

*/


	//printf("adresse von : outBuf %p\n", outBuf[1]);
	//printf("Wert von : outBuf %d\n", outBuf[1]);

	/*
	PCONP |= (1<<6); // Enable PWM1
	PCLKSEL0 |= (1<<12); // Clock == CCLK (48 MHz)
	PINSEL4 |= (1<<0) | (1<<2) | (1<<4); // P2.0 ist PWM1.1, P2.1 ist PWM1.2, P2.2 ist PWM1.3
	PWM1_PR = 0; // Prescaler, PWM-Takt = 1 Hz
	PWM1_MCR = (1<<0) | (1<<1); // Interrupt on PWMMR0, Reset on PWMMR0
	PWM1_PCR = (1<<2) | (1<<3) | // PWM1.2 double edge, PWM1.3 double edge,
	(1<<9) | (1<<10) | (1<<11); // PWM1.1 PWM1.2 PWM1.3 enabled
	PWM1_MR0 = 1088; // PWM-Period
	
	PWM1_MR1 = 100; // PWM-Match 1
	PWM1_MR2 = 2; // PWM-Match 2
	
	PWM1_LER = (1<<0) | (1<<1) | // Latch Enable
	(1<<2) | (1<<3);
	PWM1_TCR = (1<<0) | (1<<3); // Counter Enable, PWM Enable
	

*/


	return 0;

}

void decodeMp3(void){
	
	MP3FrameInfo mp3FrameInfo;

    int nextSyncWord = 0;

	if(bytesLeft == 0){
		printf("%d bytesLeft --> neues Syncword finden!\n",bytesLeft);
		nextSyncWord = MP3FindSyncWord((unsigned char*)inBuf, MAINBUF_SIZE); 
 		printf("nextSyncWord = %d\n",nextSyncWord);	
		bytesLeft = MAINBUF_SIZE - nextSyncWord;	
	}
    
	
	printf("decoding %d Bytes\n",bytesLeft);
	int mp3DecodeResu = MP3Decode(mp3Dec, (&inBuf + nextSyncWord), &bytesLeft, outBuf[1], 0);
	printf("mp3DecodeResu= %d\n",mp3DecodeResu);
	

	MP3GetLastFrameInfo(mp3Dec, &mp3FrameInfo);

	printf("#######  MP3 HEADER  ########\n");	
	printf("# bitrate =      %10d #\n", mp3FrameInfo.bitrate);
	printf("# nChans =       %10d #\n", mp3FrameInfo.nChans);
	printf("# samprate =     %10d #\n", mp3FrameInfo.samprate);
	printf("# bitsPerSample =%10d #\n", mp3FrameInfo.bitsPerSample);
	printf("# outputSamps =  %10d #\n", mp3FrameInfo.outputSamps);
	printf("# layer =        %10d #\n", mp3FrameInfo.layer);	
	printf("# version =      %10d #\n", mp3FrameInfo.version);	
	printf("#############################\n");		

	printf("bytesLeft after decode: %d\n",bytesLeft);
}

void fillBuffer(void){

	//bytesLeft = MAINBUF_SIZE;
	
	//unsigned char *pInput;
	//pInput = (unsigned char *)buffer + nextSyncWord;

	
	
	printf("moving %d Bytes from Address %p  to startAdress of inBuf\n",bytesLeft,inBuf + (MAINBUF_SIZE -bytesLeft));
	memmove(inBuf, (unsigned char*)inBuf + (MAINBUF_SIZE -bytesLeft), bytesLeft);

	printf("read %d Bytes from spiDataPosition= %d and fillBuffer @Address= %p  with it  ->bytesLeft= %d\n",(MAINBUF_SIZE -bytesLeft),spiDataPosition,inBuf,bytesLeft);	
	
	int nBytesToRead = (MAINBUF_SIZE - bytesLeft);
	spiReadBytes(inBuf, nBytesToRead, spiDataPosition, SPI_INT);
	spiDataPosition += nBytesToRead;

}


void mp3Cleanup(){

	printf("MP3FreeDecoder()...\n");
	MP3FreeDecoder(mp3Dec);	

}

