
#ifndef _CE_OUTLINE_H_
#define _CE_OUTLINE_H_


void dac (void);

#define LED1BIT 7
#define LED2BIT 6
#define LED3BIT 5
#define LED4BIT 4

#define EMPTY 12345
#define FULL 54321
#define BUFFERSIZE 1024
#define FREQ_ABTAST 44118                //Frequenz, mit der die Ausgabe erfolgt 44.117,64706 hz
#define MACH_REGISTER_COUNT (SYS_CLK + FREQ_ABTAST / 2) / FREQ_ABTAST -1 

#define SYS_CLK 48000000
#define N_STUETZ 256                     //Anzahl der Stuetzstellen
#define N_PRINTS_PER_SECOND 2
#define PRINT_COUNTER (FREQ_ABTAST / N_PRINTS_PER_SECOND)

#define FREQUENZ_S0 400
#define FREQUENZ_S1 5000
#define B_FIXPOINT_1V 620
#define B_FIXPOINT_05V 310
#define AMPL     (1<<15)                 //Amplitude der Stuetzstellenwerte
#define N_NACHKOMMA 16                   //Anzahl der binaeren Nachkommastellen bei der Zeitberechnung

void initTimer();
void dacInit();

struct buffer{
    volatile int state;
    volatile int *data;         
    volatile int buffersize;      
};
volatile struct buffer *buffer1;
volatile struct buffer *buffer2;
volatile struct buffer *actualbuffer;
volatile struct buffer *isrbuffer;


#endif //_CE_OUTLINE_H_
