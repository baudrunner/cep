
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

int initMp3Module(void);
void fillBuffer(void);
void mp3Cleanup(void);
void decodeMp3(void);




#endif //_CE_MP3_H_

