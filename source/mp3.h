
#ifndef _CE_MP3_H_
#define _CE_MP3_H_

#include <stdio.h>
#define ARM
#include "mp3dec.h"
#include <inttypes.h>
#include <string.h> //memmove
#include "spi.h"


#define OUTSIZE (MAX_NGRAN*MAX_NCHAN*MAX_NSAMP)
#define NUMOFCHAN 2
#define READBUF_SIZE (2*MAINBUF_SIZE)

int initMp3Module(int startAddr);
int decode(int16_t *outBuf);
int fillBuffer(unsigned char *inputBuffer, unsigned char *inBufPtr, int bufSize, int bytesLeft, int readAddr);
void mp3Cleanup(void);





#endif //_CE_MP3_H_

