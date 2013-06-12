
#include "lpc24xx.h"

#define IOB1_DATA0_PIN_PORT0   5
#define IOB1_DATA1_PIN_PORT1  10
#define IOB1_DATA2_PIN_PORT0  13
#define IOB1_DATA3_PIN_PORT0  14
#define IOB1_DATA4_PIN_PORT0  19
#define IOB1_DATA5_PIN_PORT0  20
#define IOB1_DATA6_PIN_PORT0  21
#define IOB1_DATA7_PIN_PORT0  22

#define IOB2_DATA0_PIN_PORT0  29
#define IOB2_DATA1_PIN_PORT0  30
#define IOB2_DATA2_PIN_PORT0   4
#define IOB2_DATA3_PIN_PORT1  16
#define IOB2_DATA4_PIN_PORT1  17
#define IOB2_DATA5_PIN_PORT1  18
#define IOB2_DATA6_PIN_PORT1  19
#define IOB2_DATA7_PIN_PORT1  20

#define IOB3_DATA0_PIN_PORT1   0
#define IOB3_DATA1_PIN_PORT1   1
#define IOB3_DATA2_PIN_PORT1   2
#define IOB3_DATA3_PIN_PORT1   3
#define IOB3_DATA4_PIN_PORT1   4
#define IOB3_DATA5_PIN_PORT1   5
#define IOB3_DATA6_PIN_PORT1   6
#define IOB3_DATA7_PIN_PORT1   7

#define IOB4_DATA0_PIN_PORT1  15
#define IOB4_DATA1_PIN_PORT1  14
#define IOB4_DATA2_PIN_PORT1  13
#define IOB4_DATA3_PIN_PORT1  12
#define IOB4_DATA4_PIN_PORT1  11
#define IOB4_DATA5_PIN_PORT0  18
#define IOB4_DATA6_PIN_PORT0  16
#define IOB4_DATA7_PIN_PORT0  15

#define IOB5_DATA0_PIN_PORT2  11
#define IOB5_DATA1_PIN_PORT2  12
#define IOB5_DATA2_PIN_PORT1   8
#define IOB5_DATA3_PIN_PORT1   9
#define IOB5_DATA4_PIN_PORT0  17

#define PSI_IOB1_TR_PIN_PORT2  3
#define PSI_IOB2_TR_PIN_PORT2  4
#define PSI_IOB3_TR_PIN_PORT2  5
#define PSI_IOB4_TR_PIN_PORT2  8
#define PSI_IOB5_TR_PIN_PORT2  9

#define PSI_READY_PIN_PORT2   13

#define PSI_PINSEL0 ( (3<<10)|(3<<26)|(3<<28)                                    )
#define PSI_PINSEL1 ( (3<< 6)|(3<< 8)|(3<<10)|(3<<12)|(3<<26)|(3<<28)            )
#define PSI_PINSEL2 ( (3<<16)|(3<<20)|(3<<22)|(3<<24)|(3<<26)|(3<<28)|(3<<30)    )
#define PSI_PINSEL4 ( (3<< 6)|(3<< 8)|(3<<10)|(3<<16)|(3<<18)|(3<<24)|(3<<26)    )



//Datenleitungen (10 Leitungen)
#define PSI_DATA00_PIN_PORT0  IOB1_DATA0_PIN_PORT0
#define PSI_DATA01_PIN_PORT1  IOB1_DATA1_PIN_PORT1
#define PSI_DATA02_PIN_PORT0  IOB1_DATA2_PIN_PORT0
#define PSI_DATA03_PIN_PORT0  IOB1_DATA3_PIN_PORT0
#define PSI_DATA04_PIN_PORT0  IOB1_DATA4_PIN_PORT0
#define PSI_DATA05_PIN_PORT0  IOB1_DATA5_PIN_PORT0
#define PSI_DATA06_PIN_PORT0  IOB1_DATA6_PIN_PORT0
#define PSI_DATA07_PIN_PORT0  IOB1_DATA7_PIN_PORT0
#define PSI_DATA08_PIN_PORT0  IOB2_DATA0_PIN_PORT0
#define PSI_DATA09_PIN_PORT0  IOB2_DATA1_PIN_PORT0



#define PSI_DATA_PINS_PORT0   (  (1<<PSI_DATA09_PIN_PORT0)|(1<<PSI_DATA08_PIN_PORT0)|(1<<PSI_DATA07_PIN_PORT0)|(1<<PSI_DATA06_PIN_PORT0)|(1<<PSI_DATA05_PIN_PORT0)|(1<<PSI_DATA04_PIN_PORT0)|(1<<PSI_DATA03_PIN_PORT0)|(1<<PSI_DATA02_PIN_PORT0)|(1<<PSI_DATA00_PIN_PORT0)  )
#define PSI_DATA_PINS_PORT1   (  (1<<PSI_DATA01_PIN_PORT1)  )


//Adressleitungen
#define PSI_ADR0_PIN_PORT1    IOB4_DATA0_PIN_PORT1
#define PSI_ADR1_PIN_PORT1    IOB4_DATA1_PIN_PORT1
#define PSI_ADR2_PIN_PORT1    IOB4_DATA2_PIN_PORT1

#define PSI_ADR_PINS_PORT1    (  (1<<PSI_ADR0_PIN_PORT1)|(1<<PSI_ADR1_PIN_PORT1)|(1<<PSI_ADR2_PIN_PORT1)  )

#define PSI_IOB3_UNUSED       ( (1<<IOB3_DATA2_PIN_PORT1) |  (1<<IOB3_DATA3_PIN_PORT1) |  (1<<IOB3_DATA4_PIN_PORT1) |  (1<<IOB3_DATA5_PIN_PORT1) |  (1<<IOB3_DATA6_PIN_PORT1) |  (1<<IOB3_DATA7_PIN_PORT1) )

//Steuerleitungen
#define PSI_RW_PIN_PORT1      IOB4_DATA4_PIN_PORT1
#define PSI_ACK_PIN_PORT1     IOB3_DATA0_PIN_PORT1
#define PSI_REQ_PIN_PORT1     IOB4_DATA3_PIN_PORT1
#define PSI_RESULT_PIN_PORT1  IOB3_DATA1_PIN_PORT1

#define PSI_CNTRL_PINS_PORT1  ( (1<<PSI_RW_PIN_PORT1)|(1<<PSI_REQ_PIN_PORT1) )
#define PSI_CNTRL_PINS_PORT2  ( (1<<PSI_READY_PIN_PORT2)|(1<<PSI_IOB1_TR_PIN_PORT2)|(1<<PSI_IOB2_TR_PIN_PORT2)|(1<<PSI_IOB3_TR_PIN_PORT2)|(1<<PSI_IOB4_TR_PIN_PORT2)|(1<<PSI_IOB5_TR_PIN_PORT2) )
#define PSI_STATUS_PINS_PORT1 ( (1<<PSI_ACK_PIN_PORT1)|(1<<PSI_RESULT_PIN_PORT1)| PSI_IOB3_UNUSED )

//Ready
#define PSI_CLR_RDY_PIN       FIO2CLR = (1<<PSI_READY_PIN_PORT2)
#define PSI_SET_RDY_PIN       FIO2SET = (1<<PSI_READY_PIN_PORT2)


//Steuerung der Datenrichtungen der IOBs
#define PSI_CLR_IOB1_TR_PIN   FIO2CLR = (1<<PSI_IOB1_TR_PIN_PORT2)
#define PSI_SET_IOB1_TR_PIN   FIO2SET = (1<<PSI_IOB1_TR_PIN_PORT2)

#define PSI_CLR_IOB2_TR_PIN   FIO2CLR = (1<<PSI_IOB2_TR_PIN_PORT2)
#define PSI_SET_IOB2_TR_PIN   FIO2SET = (1<<PSI_IOB2_TR_PIN_PORT2)

#define PSI_CLR_IOB3_TR_PIN   FIO2CLR = (1<<PSI_IOB3_TR_PIN_PORT2)
#define PSI_SET_IOB3_TR_PIN   FIO2SET = (1<<PSI_IOB3_TR_PIN_PORT2)

#define PSI_CLR_IOB4_TR_PIN   FIO2CLR = (1<<PSI_IOB4_TR_PIN_PORT2)
#define PSI_SET_IOB4_TR_PIN   FIO2SET = (1<<PSI_IOB4_TR_PIN_PORT2)

#define PSI_CLR_IOB5_TR_PIN   FIO2CLR = (1<<PSI_IOB5_TR_PIN_PORT2)
#define PSI_SET_IOB5_TR_PIN   FIO2SET = (1<<PSI_IOB5_TR_PIN_PORT2)


//Abfragen von Statusleitungen
#define PSI_WAIT_ACK_CLR      while( (FIO1PIN & (1<<PSI_ACK_PIN_PORT1)) != 0){}
#define PSI_WAIT_ACK_SET      while( (FIO1PIN & (1<<PSI_ACK_PIN_PORT1)) == 0){}

#define PSI_WAIT_RESULT_SET   while( (FIO1PIN & (1<<PSI_RESULT_PIN_PORT1)) == 0){}

//Setzen der Steuerleitungen
#define PSI_CLR_REQ           FIO1CLR = (1<<PSI_REQ_PIN_PORT1)
#define PSI_SET_REQ           FIO1SET = (1<<PSI_REQ_PIN_PORT1)

#define PSI_ENABLE_READ       FIO0DIR &= ~PSI_DATA_PINS_PORT0;  FIO1DIR &= ~PSI_DATA_PINS_PORT1; PSI_SET_IOB1_TR_PIN; PSI_SET_IOB2_TR_PIN; FIO1SET = (1<<PSI_RW_PIN_PORT1);
#define PSI_ENABLE_WRITE      FIO1CLR = (1<<PSI_RW_PIN_PORT1);  PSI_CLR_IOB1_TR_PIN; PSI_CLR_IOB2_TR_PIN; FIO0DIR |=  PSI_DATA_PINS_PORT0;  FIO1DIR |=  PSI_DATA_PINS_PORT1;


//Daten Ein- und Ausgabe
#define PSI_WRITE_DATA( d )   FIO0MASK = ~PSI_DATA_PINS_PORT0;                                                                  \
                              FIO0PIN  = (d & (3<<8))<<(29-8) | (d & (0xf<<4))<<(19-4) | (d & 0xc)<<11 | (d & 1)<<5;            \
                              FIO0MASK = 0;                                                                                     \
                              FIO1MASK = ~PSI_DATA_PINS_PORT1;                                                                  \
                              FIO1PIN  = (d & 2)<<9;                                                                            \
                              FIO1MASK = 0;


#define PSI_READ_DATA( d )    {int x = FIO0PIN; d = ( ( x >> (29-8) ) & (0x3<<8) ) | ( ( x >> (19-4) ) & (0xf<<4) ) | ( ( x >> (13-2) ) & (0x3<<2) ) | ( ( x >> (5-0) ) & (0x1<<0) ) | ( ( FIO1PIN >> (10-1) ) & (0x1<<1) );}


#define PSI_INIT              PINSEL0 = PINSEL0 & ~PSI_PINSEL0;PINSEL1 = PINSEL1 & ~PSI_PINSEL1;PINSEL2 = PINSEL2 & ~PSI_PINSEL2;PINSEL4 = PINSEL4 & ~PSI_PINSEL4;    \
                              FIO0DIR &= ~PSI_DATA_PINS_PORT0;                                                                                                        \
                              FIO1DIR &= ~PSI_STATUS_PINS_PORT1;                                                                                                      \
                              /*FIO2DIR &= ~PSI_STATUS_PINS_PORT2; */                                                                                                     \
                              FIO1DIR |= PSI_CNTRL_PINS_PORT1 | PSI_ADR_PINS_PORT1;                                                                                   \
                              FIO2DIR |= PSI_CNTRL_PINS_PORT2;                                                                                                        \
                              PSI_CLR_IOB1_TR_PIN;                                                                                                                    \
                              PSI_CLR_IOB2_TR_PIN;                                                                                                                    \
                              PSI_SET_IOB3_TR_PIN;                                                                                                                    \
                              PSI_CLR_IOB4_TR_PIN;                                                                                                                    \
                              PSI_CLR_IOB5_TR_PIN;                                                                                                                    \
                              PSI_SET_RDY_PIN;
