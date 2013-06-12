#ifndef _CE_CONSTANTS_H_
#define _CE_CONSTANTS_H_

#include <inttypes.h>

#define LED0BIT 0
#define LED1BIT 1
#define LED2BIT 2
#define LED3BIT 3


#define EMPTY 12345
#define FULL 54321
#define BUFFERSIZE 1024
#define FREQ_ABTAST 44090                //Frequenz, mit der die Ausgabe erfolgt 44.117,64706 hz
#define SYS_CLK 72000000
#define MACH_REGISTER_COUNT ((SYS_CLK + FREQ_ABTAST / 2) / FREQ_ABTAST)


#define N_STUETZ 256                     //Anzahl der Stuetzstellen
#define N_PRINTS_PER_SECOND 2
#define PRINT_COUNTER (FREQ_ABTAST / N_PRINTS_PER_SECOND)

#define FREQUENZ_S0 400
#define FREQUENZ_S1 5000
#define B_FIXPOINT_1V 620
#define B_FIXPOINT_05V 310
#define AMPL     (1<<15)                 //Amplitude der Stuetzstellenwerte
#define N_NACHKOMMA 16                   //Anzahl der binaeren Nachkommastellen bei der Zeitberechnung

#define S0_BIT (1 << 8)
#define S1_BIT (1 << 9)
#define S2_BIT (1 << 10)
#define S3_BIT (1 << 11)

#define DAC_CHANNEL_RIGHT 0x1
#define DAC_CHANNEL_LEFT  0x2


struct buffer{
    int sampleCnt;
    int16_t *data;         
    int buffersize;      
};


#endif //_CE_CONSTANTS_H_
