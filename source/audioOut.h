
#ifndef _CE_AUDIO_OUT_H_
#define _CE_AUDIO_OUT_H_
#include <inttypes.h>
#include "isrRoutine.h"

void dac (void);

void initTimer();
void dacInit(struct buffer *isrBuf1, struct buffer *isrBuf2);
void pwmInit();
void isrRoutine();
void __attribute__ ((interrupt("IRQ"))) isr_timer(void);

#endif //_CE_AUDIO_OUT_H_

