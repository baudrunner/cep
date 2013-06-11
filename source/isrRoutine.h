#ifndef _CE_ISRROUTINE_H_
#define _CE_ISRROUTINE_H_

#include "lpc24xx.h"
#include "constants.h"


extern int count;

extern int isrBufferIdx;

extern int a_fixpoint;

extern int b_fixpoint;
extern int msRuntime;
extern int running;
extern int timerRunning;
extern int printCount;
/**/

extern struct buffer *IsrBuffer1;
extern struct buffer *IsrBuffer2;
extern struct buffer *currentIsrBuffer;

extern int dacChannel;

void isrRoutine(void);

#endif //_CE_ISRROUTINE_H_
