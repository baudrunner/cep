
#ifndef _CE_OUTLINE_H_
#define _CE_OUTLINE_H_
#include <inttypes.h>

void dac (void);

//#define LED1BIT 7
//#define LED2BIT 6
//#define LED3BIT 5
//#define LED4BIT 4


#define LED1BIT 0
#define LED2BIT 1
#define LED3BIT 2
#define LED4BIT 4


#define EMPTY 12345
#define FULL 54321
#define BUFFERSIZE 1024
#define FREQ_ABTAST 44118                //Frequenz, mit der die Ausgabe erfolgt 44.117,64706 hz
#define SYS_CLK 72000000
#define MACH_REGISTER_COUNT (SYS_CLK + FREQ_ABTAST / 2) / FREQ_ABTAST -1 


#define N_STUETZ 256                     //Anzahl der Stuetzstellen
#define N_PRINTS_PER_SECOND 2
#define PRINT_COUNTER (FREQ_ABTAST / N_PRINTS_PER_SECOND)

#define FREQUENZ_S0 400
#define FREQUENZ_S1 5000
#define B_FIXPOINT_1V 620
#define B_FIXPOINT_05V 310
#define AMPL     (1<<15)                 //Amplitude der Stuetzstellenwerte
#define N_NACHKOMMA 16                   //Anzahl der binaeren Nachkommastellen bei der Zeitberechnung


struct buffer{
    int sampleCnt;
    int16_t *data;         
    int buffersize;      
};

void initTimer();
void dacInit(struct buffer *isrBuf1, struct buffer *isrBuf2);
void pwmInit();

volatile struct buffer *buffer1;
volatile struct buffer *buffer2;
volatile struct buffer *actualbuffer;
volatile struct buffer *isrbuffer;


#endif //_CE_OUTLINE_H_
