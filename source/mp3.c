#include "mp3.h"
unsigned char inputBuffer[READBUF_SIZE];
unsigned char *inBufPtr;
int spiDataPosition;
int bytesLeft;
HMP3Decoder mp3Dec;

int bytesLeft = 0;
int outOfData = 0;
int eofReached = 0;
int nRead = 0;
int nFrames = 0;
int offset = 0;
int err = 0;
MP3FrameInfo mp3FrameInfo;	

int initMp3Module(int startAddr){

	printf("initialisiere mp3 Decoder...\n");
	mp3Dec = MP3InitDecoder();
	
	inBufPtr = inputBuffer;	
	//inputBuffer = inputBuffer;	
	printf("inBufPtr= %p\ninputBuffer= %p\n",inBufPtr,inputBuffer);		

	if(mp3Dec == NULL){
		printf("ERROR Initialisierung des mp3 Decoder fehlgeschlagen! mp3Dec= %p\n",mp3Dec);
	}else{
		printf("mp3 Decoder initialisiert! mp3Dec= %p\n",mp3Dec);
	}
	
	spiDataPosition = startAddr;	
	bytesLeft = 0;	
	return 0;
}


int decode(int16_t *outBuf){

	if (bytesLeft < MAINBUF_SIZE) {
		nRead =fillBuffer(inputBuffer, inBufPtr, READBUF_SIZE, bytesLeft, spiDataPosition);
		spiDataPosition += nRead;
		bytesLeft += nRead;
		inBufPtr = inputBuffer;
	}

	offset = MP3FindSyncWord(inBufPtr, bytesLeft);
	if (offset < 0) { // mp3 vermutl. zuende...
		outOfData = 1;
	}
	inBufPtr += offset;
	bytesLeft -= offset;

	err = MP3Decode(mp3Dec, &inBufPtr, &bytesLeft, outBuf, 0);
	nFrames++;
		
	if (err) {
		switch (err) {
		case ERR_MP3_INDATA_UNDERFLOW:
			outOfData = 1;
			return ERR_MP3_INDATA_UNDERFLOW;
		case ERR_MP3_MAINDATA_UNDERFLOW:
			return ERR_MP3_MAINDATA_UNDERFLOW;
		case ERR_MP3_FREE_BITRATE_SYNC:
		default:
			outOfData = 1;
			return ERR_MP3_FREE_BITRATE_SYNC;
		}
	} else {

		MP3GetLastFrameInfo(mp3Dec, &mp3FrameInfo);
			
		/*
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
		*/	
	}

	return mp3FrameInfo.outputSamps;
}

int fillBuffer(unsigned char *inputBuffer, unsigned char *inBufPtr, int bufSize, int bytesLeft, int readAddr){

	memmove(inputBuffer, inBufPtr, bytesLeft);		
	spiReadBytes(inputBuffer + bytesLeft, bufSize - bytesLeft, readAddr, SPI_INT);	
	inBufPtr = inputBuffer;

	return bufSize - bytesLeft;
}


void mp3Cleanup(){

	printf("mp3Cleanup() leere inputBuffer...\n");
	memset(inputBuffer, 0, READBUF_SIZE);
	printf("MP3FreeDecoder()...\n");	
	MP3FreeDecoder(mp3Dec);	

}

