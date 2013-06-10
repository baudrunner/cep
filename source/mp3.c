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
	
	inBuf = &inputBuffer[0];	
	printf("inBuf= %p\ninputBuffer= %p\n",inBuf,inputBuffer);		

	if(mp3Dec == NULL){
		printf("ERROR Initialisierung des mp3 Decoder fehlgeschlagen! mp3Dec= %d\n",mp3Dec);
	}else{
		printf("mp3 Decoder initialisiert! mp3Dec= %d\n",mp3Dec);
	}
	
	//int nextSyncWord = MP3FindSyncWord((unsigned char*)inBuf, MAINBUF_SIZE); 

    //printf("nextSyncWord = %d\n",nextSyncWord);	


	//void MP3GetLastFrameInfo(HMP3Decoder hMP3Decoder,MP3FrameInfo *mp3FrameInfo);

	bytesLeft = 0;
	spiReadBytes(inBuf, MAINBUF_SIZE, spiDataPosition, SPI_INT);  //lese anfang der Datei ein um laenge der Id-Tags zu finden 

    int nextSyncWord = 0;
	spiDataPosition = 0;	
	nextSyncWord = MP3FindSyncWord((unsigned char*)inBuf, MAINBUF_SIZE); 
	printf("nextSyncWord = %d\n",nextSyncWord);	


	
	
	if(nextSyncWord > 0){
		printf("initMp3Module(): spiDataPosition= %d  nextSyncWord= %d\n",spiDataPosition,nextSyncWord);	
 		printf("lese Daten neu ab erstem Syncwort ein -> SPI@%d\n",spiDataPosition + nextSyncWord);		
		spiReadBytes(inBuf, MAINBUF_SIZE, spiDataPosition + nextSyncWord, SPI_INT);	//daten nach iD Tags an anfang des puffers schreiben
	}

	bytesLeft = MAINBUF_SIZE;	
	//unsigned char *pInput;
	//inBuf = (unsigned char *)inBuf + nextSyncWord;

	
	//inBuf = &inputBuffer[0];	
	
	
	return 0;

}

void decodeMp3(void){
	
	printf("decodeMp3(): inBuf= %p\ninputBuffer= %p\n",inBuf,inputBuffer);	
	int nextSyncWord = 0;
	nextSyncWord = MP3FindSyncWord((unsigned char*)inBuf, MAINBUF_SIZE); 
	printf("decodeMp3() nextSyncWord = %d\n",nextSyncWord);	


    MP3FrameInfo mp3FrameInfo;
	bytesLeft = MAINBUF_SIZE;
	printf("decoding %d Bytes\n",bytesLeft);
	int mp3DecodeResu = MP3Decode(mp3Dec, &inBuf, &bytesLeft, outBuf[1], 0);
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
	printf("decodeMp3(): inBuf= %p\ninputBuffer= %p\n",inBuf,inputBuffer);	
	printf("spiDataPosition= %d\n",spiDataPosition);
	

	if(bytesLeft > 0){
		printf("moving %d Bytes from Address %p  to startAdress of inBuf\n",bytesLeft,inBuf + (MAINBUF_SIZE -bytesLeft));	
		memmove(inBuf, (unsigned char*)inBuf + (MAINBUF_SIZE -bytesLeft), bytesLeft);
	}

	

	printf("read %d Bytes from spiDataPosition= %d and fill Buffer @Address= %p  with it  ->bytesLeft= %d\n",(MAINBUF_SIZE -bytesLeft),spiDataPosition,inBuf,bytesLeft);	
	
	int nBytesToRead = (MAINBUF_SIZE - bytesLeft);
	spiReadBytes(inBuf, nBytesToRead, spiDataPosition, SPI_INT);
	spiDataPosition += nBytesToRead;

	printf("spiDataPosition= %d\n",spiDataPosition);
}


void mp3Cleanup(){

	printf("MP3FreeDecoder()...\n");
	MP3FreeDecoder(mp3Dec);	

}

