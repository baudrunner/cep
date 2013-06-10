#include "mp3.h"



unsigned char inputBuffer[READBUF_SIZE];
unsigned char *inBufPtr;
int spiDataPosition = 0;
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


int initMp3Module(){

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
	
	spiDataPosition = 0;	
	bytesLeft = 0;	
	return 0;
}




int decode(int16_t *outBuf){
	

	/* somewhat arbitrary trigger to refill buffer - should always be enough for a full frame */
	if (bytesLeft < 2*MAINBUF_SIZE && !eofReached) {
		nRead =fillBuffer(inputBuffer, inBufPtr, READBUF_SIZE, bytesLeft, spiDataPosition);
		spiDataPosition += nRead;
		//nRead = FillReadBuffer(readBuf, readPtr, READBUF_SIZE, bytesLeft, infile);
		bytesLeft += nRead;
		inBufPtr = inputBuffer;
		if (nRead == 0)
			eofReached = 1;
	}

	/* find start of next MP3 frame - assume EOF if no sync found */
	offset = MP3FindSyncWord(inBufPtr, bytesLeft);
	if (offset < 0) {
		outOfData = 1;
	}
	inBufPtr += offset;
	bytesLeft -= offset;


	/* decode one MP3 frame - if offset < 0 then bytesLeft was less than a full frame */
		err = MP3Decode(mp3Dec, &inBufPtr, &bytesLeft, outBuf, 0);
		//printf("mp3DecResu= %d\n",err);	
		nFrames++;
		
	if (err) {
		/* error occurred */
		switch (err) {
		case ERR_MP3_INDATA_UNDERFLOW:
			outOfData = 1;
			return ERR_MP3_INDATA_UNDERFLOW;
		case ERR_MP3_MAINDATA_UNDERFLOW:
			/* do nothing - next call to decode will provide more mainData */
			return ERR_MP3_MAINDATA_UNDERFLOW;
		case ERR_MP3_FREE_BITRATE_SYNC:
		default:
			outOfData = 1;
			return ERR_MP3_FREE_BITRATE_SYNC;
		}
	} else {
		/* no error */
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
	/*
	int j = 0;
	for(j = 0; j < 10; j++){
		printf("outBuf[%d]= %d\n",j,outBuf[j]);
	}*/

}

int fillBuffer(unsigned char *inputBuffer, unsigned char *inBufPtr, int bufSize, int bytesLeft, int readAddr){

	//printf("fillBuffer(*inputBuffer= %p, *inBufPtr= %p, bufSize= %d, bytesLeft= %d, readAddr= %d\n",inputBuffer,inBufPtr,bufSize,bytesLeft,readAddr);
	//printf("fillBuffer() : lese %d neue Bytes vom SPI\n",bufSize - bytesLeft);
	


	/*
	int i;	
 	printf("10 Bytes ab inputBuffer:\n");
	for( i = 0; i < 10; i++){
		printf("inputBuffer[%d]= 0x%02X\n",i,inputBuffer[i]);
		 
	}	

	printf("10 Bytes ab inBufPtr:\n");
	for( i = 0; i < 10; i++){
		printf("inBufPtr[%d]= 0x%02X\n",i,inBufPtr[i]);
		 
	}	*/


	memmove(inputBuffer, inBufPtr, bytesLeft);		
	spiReadBytes(inputBuffer + bytesLeft, bufSize - bytesLeft, readAddr, SPI_INT);	
	inBufPtr = inputBuffer;


	/* zero-pad to avoid finding false sync word after last frame (from old data in readBuf) */
	//if (nRead < bufSize - bytesLeft){
	//	memset(readBuf + bytesLeft + nRead, 0, bufSize - bytesLeft - nRead);	
	//}

	//printf("fillBuffer(): inputBuffer= %p\ninBufPtr= %p\n",inputBuffer,inBufPtr);

	/*
	printf("10 Bytes ab inputBuffer [nach kopieren]:\n");
	for( i = 0; i < 10; i++){
		printf("inputBuffer[%d]= 0x%02X\n",i,inputBuffer[i]);
		 
	}*/

	return bufSize - bytesLeft;
}


void mp3Cleanup(){

	printf("MP3FreeDecoder()...\n");
	MP3FreeDecoder(mp3Dec);	

}

