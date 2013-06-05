// Autoren: Michel Rottleuthner und Christoph Bunke
///
#include <stdio.h>
#include "lpc24xx.h"
#include "config.h"
#include "ConfigStick.h"
#include "portlcd.h"
#include "fio.h"
#include "malloc.h"
#include <stdio.h>
#include <string.h> //memset

int  spiFlashMemReadStatusRegister(int spiDescriptor);
void spiFlashMemWaitReady(int spiDescriptor);
void spiFlashMemSetWriteEnable(int spiDescriptor);
void spiFlashMemGlobalUnprotect(int spiDescriptor);
void spiFlashMemErase(int destAddr, int blockSizeDef, int spiDescriptor);
void spiFlashMemWrite(int dstAddr, char *data, int count, int spiDescriptor);
int  spiFlashMemRead(int srcAddr, char *destAddr, int count, int spiDescriptor);
void eraseChip(int spiDescriptor);
void readSpecs();
void menue();
int  testSpiMem();
void sspInit();


// macro converts binary value (containing up to 8 bits resp. <=0xFF) to unsigned decimal value
// as substitute for missing 0b prefix for binary coded numeric literals.
// macro does NOT check for an invalid argument at all.
#define b(n) (                                               \
    (unsigned char)(                                         \
            ( ( (0x##n##ul) & 0x00000001 )  ?  0x01  :  0 )  \
        |   ( ( (0x##n##ul) & 0x00000010 )  ?  0x02  :  0 )  \
        |   ( ( (0x##n##ul) & 0x00000100 )  ?  0x04  :  0 )  \
        |   ( ( (0x##n##ul) & 0x00001000 )  ?  0x08  :  0 )  \
        |   ( ( (0x##n##ul) & 0x00010000 )  ?  0x10  :  0 )  \
        |   ( ( (0x##n##ul) & 0x00100000 )  ?  0x20  :  0 )  \
        |   ( ( (0x##n##ul) & 0x01000000 )  ?  0x40  :  0 )  \
        |   ( ( (0x##n##ul) & 0x10000000 )  ?  0x80  :  0 )  \
    )                                                        \
)

#define SPI_INT_DISABLE FIO0SET  = (1<<16);// deselect slave/flash ;  connected flash_CS is low active
#define SPI_EXT_DISABLE FIO1SET  = (1<< 7); //deselect external SPI Board  -> Set Port 1 pin 7 setzen 

#define SPI_INT_ENABLE	FIO0CLR  = (1<<16);  // select slave/flash ;  CS is low active
#define SPI_EXT_ENABLE  FIO1CLR  = (1<< 7);  //select external SPI Board  -> Set Port 1 pin 7 setzen 	

#define SPI_EXT 1
#define SPI_INT 2

#define ENABLE_SSEL( SPI ) if( (SPI) == SPI_INT ){ SPI_INT_ENABLE }else if( (SPI) == SPI_EXT ){ SPI_EXT_ENABLE }
#define DISABLE_SSEL( SPI ) if( (SPI) == SPI_INT ){ SPI_INT_DISABLE }else if( (SPI) == SPI_EXT ){ SPI_EXT_DISABLE }

#define CMD_WRITE_STATUS_REGISTER_BYTE1 0x01
#define CMD_READ_ARRAY					0x0B
#define CMD_READ_STATUS_REGISTER		0x05
#define CMD_SET_WRITE_ENABLE			0x06
#define CMD_BLOCK_ERASE_4K              0x20
#define CMD_BLOCK_ERASE_32K				0x52
#define CMD_BLOCK_ERASE_64K				0xD8
#define CMD_PAGE_PROGRAMM               0x02
#define CMD_READ_MANUFACTOR_ID          0x9F
#define CMD_CHIP_ERASE                  0x60


#define BLOCKSIZE_4K  1
#define BLOCKSIZE_32K 2
#define BLOCKSIZE_64K 3

#define DUMMY_BYTE 0xFF
#define RDY_BIT 0x01

#define BM_HIGH_HWORD 0xFFFF0000
#define BM_LOW_HWORD  0x0000FFFF
#define BM_BYTE 0xFF
#define BM_HIGH_BYTE 0xFF00
#define BM_LOW_BYTE 0x00FF
#define BM_HIGH_NIBBLE 0xF0
#define BM_LOW_NIBBLE  0x0F


// simplified access to SSP0 SR bits             -> see UM Chapter 20-6.4, Table 474
// Receive fifo Not Empty ;      SSP0-SR: RNE bit equals 0 if fifo empty
#define  SSP0_SR_RNE             ( (SSP0_SR & 0x04)  != 0 )
// Transmit fifo Not Empty ;     SSP0-SR: TFE bit equals 1 if fifo empty ;      fifo seems to have size of 12 bytes
#define  SSP0_SR_TNE             ( (SSP0_SR & 0x01)  != 1 )
//Transmit FIFO Not Full;     This bit is 0 if the Tx FIFO is full, 1 if not.
#define  SSP0_SR_TNF ( (SSP0_SR & 0x01) == 1 )

int throwAway; //hier dummy Bytes drauf schreiben


int main( void ){
    BaseStickConfig();    
    sspInit();
	menue();
    while(1){};
}//main



void sspInit(){

    // Synchronous Serial Port (SSP) -> see UM Chapter 20: LPC24XX SSP interface SSP0/1
    // SSP is able to do SPI and however, the SSP0 is connected via SPI to AT25DF641 flash memory in actual HW equipment.
    // LPC2468_GPIO_Port0[pin16] has to be connected to AT25DF641_nCS (as workaround, since Slave_SELect SSEL0 was problematic/too short).
    // hence, signaling of Slave_SELect/Chip_Select has to be done manually

    // configure GPIO Port0[pin16] for manual signaling of Slave_SELect/Chip_Select
    FIO0DIR |= (b(1)<<16);                            // GPIO Port0[pin16] has to be output
    FIO0SET  = (b(1)<<16);                            // deselect slave/flash ;  connected flash_CS is low active

    SCS |= 0x01; //(UM Table 38.) GPIOM  High speed GPIO is enabled
    PCLKSEL1 |= (0x01 << 2); //PCLK_GPIO = CCLK   (UM Table 57 u. 58)
	//PINSEL2 = 0x00000000;// GPIO for Port1
	FIO1DIR = 0xFF;// Port 1 Pin 0 - 7 auf Ausgang

    SPI_INT_DISABLE;
    SPI_EXT_DISABLE;

    // Power CONtrol for Peripherals register                                                                                   -> UM Chapter 4-3.4.9, Table 63
    PCONP |= (b(1)<21);                                   // turn SSP0 on
    // Peripheral CLocK SELection register 1                                                                                    -> UM Chapter 4-3.3.4, Table 57 & 58
    PCLKSEL1 = ( PCLKSEL1 & ~(b(11)<<10)) | (b(01)<<10);  // peripheral clock for SSP0 = 48MHz = Cpu CLocK
    // PIN function SELect register 0                                                                                           -> UM Chapter 9-5.5,   Table 130
    PINSEL0 = ( PINSEL0 & ~(b(11)<<30)) | (b(10)<<30);    // Spi ClocK ssp0 (SCK0)
    // PIN function SELect register 1                                                                                           -> UM Chapter 9-5.5,   Table 131
    PINSEL1 =    ( PINSEL1 & ~(b(111111)<<0))
               | (b(00)<<0)                               // ssp0 Slave SELection (SSEL) is done (manually) via related GPIO register over Port0[pin16]
               | (b(10)<<2)                               // MISO_ssp0  (over Port0[pin17])
               | (b(10)<<4);                              // MOSI_ssp0  (over Port0[pin18])
    
    // SSPn Control Register 0 -> UM Chapter 20-6.1, Table 471
    SSP0_CR0 =   (   0x7<<0)                              // 8 bits/frame ;  set DSS  (Data Size Select)
               | ( b(00)<<4)                              // SPI ;           set FRF  (FRame Format)
               | (  b(1)<<6)                              //                 set CPOL (Clock out POLarity)
               | (  b(1)<<7)                              //                 set CPHA (Clock out PHAse)
               | (     0<<8);                             //                 set SCR  (Serial Clock Rate)
    // SSPn Control Register 0 -> UM Chapter 20-6.2, Table 472
    SSP0_CR1 =   ( b(0)<<0 )                              // normal operation ;    set LBM  (Loop Back Mode)
               | ( b(1)<<1 )                              // enable SSP0 ;         set SSPE (Synchronous Serial Port controller Enable)
               | ( b(0)<<2 )                              // SSP0 ist bus master ; set MS   (Master/Slave mode)
               | ( b(0)<<3 );                             // not relevant              SOD  (Slave Output Disable)
    // SSPn Clock Prescale Register -> UM Chapter 20-6.5, Table 475
    SSP0_CPSR = 4;//20                                       // 48MHz/20 = 2400KHz - well suited for oscilloscope
}

void eraseChip(int spiDescriptor){
	printf("loesche gesamten Speicher...\n");
	spiFlashMemSetWriteEnable(spiDescriptor);
	ENABLE_SSEL(spiDescriptor);
	SSP0_DR = CMD_CHIP_ERASE;
	while( !SSP0_SR_RNE ){}  // warte bis Recieve-FIFO daten zum lesen hat	
    throwAway = SSP0_DR;	
    DISABLE_SSEL(spiDescriptor);	
	spiFlashMemWaitReady(spiDescriptor);
    printf("Speicher komplett geloescht!\n");		
}

int testSpiMem(){
	
	int buffersize = 256;
	int memsize = 0x800000; //1 Mb
	int addr =0;

	char *bufExt = malloc(buffersize); 
	char *bufInt = malloc(buffersize); 	

	int wrote = 0;
	for(wrote = 0; wrote < memsize; wrote += buffersize){
		spiFlashMemRead(addr, bufExt, buffersize, SPI_EXT);						
		spiFlashMemWrite(addr, bufExt, buffersize, SPI_INT);
		spiFlashMemRead(addr, bufInt, buffersize, SPI_INT);
 		if( memcmp(bufInt, bufExt, buffersize) != 0){
			printf("######  ERROR BEIM KOPIEREN -> Daten ab Adresse %d fehlerhaft\n",addr);
			return -1;
		}	
 	    
		addr += wrote;
	}
	
	free(bufExt);
	free(bufInt);

	printf("testSpiMem(): %d bytes kopiert und verifiziert!\n",memsize);
	return 0;
}


int spiFlashMemReadStatusRegister(int spiDescriptor){
	
	ENABLE_SSEL(spiDescriptor);
 	
    SSP0_DR = CMD_READ_STATUS_REGISTER; // sende CMD_READ_STATUS_REGISTER
    
	SSP0_DR = DUMMY_BYTE; //write DUMMY_BYTE
	SSP0_DR = DUMMY_BYTE; //write DUMMY_BYTE    

	int j = 0;
    int status = 0;	
	for(j = 0; j < 3; j++){
		while( !SSP0_SR_RNE ){}  // warte bis Recieve-FIFO daten zum lesen hat
		status = ( (status << 8) | SSP0_DR );
	}

	DISABLE_SSEL(spiDescriptor);

	return (status & BM_LOW_HWORD);
}


//Liest das Statusregisters des Flashspeichers solange aus, 
//bis das RDY/BSY-Bit gelöscht ist. Wird verwendet beim Löschen und Programmieren 
//um solange zu warten, bis der Speicherbaustein den jeweiligen Vorgang abgeschlossen hat.
void spiFlashMemWaitReady(int spiDescriptor){
	//printf("spiFlashMemWaitReady\n");
	ENABLE_SSEL(spiDescriptor);

    char statReg1;
	char statReg2;

    SSP0_DR = CMD_READ_STATUS_REGISTER; // sende CMD_READ_STATUS_REGISTER
    while( !SSP0_SR_RNE ){}  // warte bis Recieve-FIFO daten zum lesen hat
	throwAway = SSP0_DR; //dummy-byte wg. CMD_READ_STATUS_REGISTER-befehl
    
	do{
		SSP0_DR = DUMMY_BYTE; //write DUMMY_BYTE
		SSP0_DR = DUMMY_BYTE; //write DUMMY_BYTE; //write DUMMY_BYTE
		
		while( !SSP0_SR_RNE ){}  // warte bis Recieve-FIFO daten zum lesen hat
		statReg1 = SSP0_DR;
 		while( !SSP0_SR_RNE ){}  // warte bis Recieve-FIFO daten zum lesen hat	
		statReg2 = SSP0_DR;

	}while( (statReg2 & RDY_BIT) );

	DISABLE_SSEL(spiDescriptor);
}

void spiFlashMemSetWriteEnable(int spiDescriptor){
	//printf("spiFlashMemSetWriteEnable\n");
	ENABLE_SSEL(spiDescriptor);

    SSP0_DR = CMD_SET_WRITE_ENABLE;// sende CMD_SET_WRITE_ENABLE
    while( !SSP0_SR_RNE ){}  // warte bis Recieve-FIFO daten zum lesen hat
	throwAway = SSP0_DR; //dummy-byte wg. CMD_SET_WRITE_ENABLE-befehl

	DISABLE_SSEL(spiDescriptor);
}


void spiFlashMemGlobalUnprotect(int spiDescriptor){
    //printf("spiFlashMemGlobalUnprotect\n");
	ENABLE_SSEL(spiDescriptor);

	SSP0_DR = CMD_WRITE_STATUS_REGISTER_BYTE1;// sende CMD_WRITE_STATUS_REGISTER_BYTE1
	SSP0_DR = 0x00; //schreibe 0
	while( !SSP0_SR_RNE ){}  // warte bis Recieve-FIFO daten zum lesen hat
	throwAway = SSP0_DR; //dummy-byte wg. CMD_WRITE_STATUS_REGISTER_BYTE1-befehl
	while( !SSP0_SR_RNE ){}  // warte bis Recieve-FIFO daten zum lesen hat
	throwAway = SSP0_DR; //dummy-byte wg. CMD_WRITE_STATUS_REGISTER_BYTE1-befehl
    
	DISABLE_SSEL(spiDescriptor);
}


void spiFlashMemErase(int destAddr, int blockSizeDef, int spiDescriptor){
	//printf("spiFlashMemErase\n");
	spiFlashMemSetWriteEnable(spiDescriptor); //damit global-unprotect ausgefuehrt werden kann
	spiFlashMemGlobalUnprotect(spiDescriptor);
    spiFlashMemSetWriteEnable(spiDescriptor); //damit geloescht werden kann	

	ENABLE_SSEL(spiDescriptor);	

	if(blockSizeDef == BLOCKSIZE_4K ){
		SSP0_DR = CMD_BLOCK_ERASE_4K;// sende CMD_BLOCK_ERASE_4K
	}else if(blockSizeDef == BLOCKSIZE_32K){
		SSP0_DR = CMD_BLOCK_ERASE_32K;// sende CMD_BLOCK_ERASE_32K
	}else if(blockSizeDef == BLOCKSIZE_64K){
		SSP0_DR = CMD_BLOCK_ERASE_64K;// sende CMD_BLOCK_ERASE_64K
	}else{
		printf("ERROR: spiFlashMemErase -> ILLEGAL blockSizeDef!\n");
	}
	
	SSP0_DR = ((destAddr >> 16) & BM_BYTE); //schreibe HightByte
    SSP0_DR = ((destAddr >>  8) & BM_BYTE); //schreibe MiddleByte	
	SSP0_DR = ( destAddr & BM_BYTE ); //schreibe LowByte	

	int j;
	for(j = 0; j < 4; j++){
		while( !SSP0_SR_RNE ){}  // active waiting for receive fifo to fill resp. "NOT ( Receive fifo Not Empty )"
		throwAway = SSP0_DR;
	}

	DISABLE_SSEL(spiDescriptor);
	spiFlashMemWaitReady(spiDescriptor);
}

void spiFlashMemWrite(int dstAdrr, char *data, int count, int spiDescriptor){
	//printf("spiFlashMemWrite\n");
	spiFlashMemSetWriteEnable(spiDescriptor); //damit global-unprotect ausgefuehrt werden kann
	spiFlashMemGlobalUnprotect(spiDescriptor);
    spiFlashMemSetWriteEnable(spiDescriptor); //damit geschrieben werden kann	

	ENABLE_SSEL(spiDescriptor);
	
    SSP0_DR = CMD_PAGE_PROGRAMM;// sende CMD_PAGE_PROGRAMM	

	SSP0_DR = ((dstAdrr >> 16) & 0xFF); //write HightByte
    SSP0_DR = ((dstAdrr >>  8) & 0xFF); //write MiddleByte	
	SSP0_DR = ( dstAdrr & 0xFF ); //write LowByte	

	int j;
	for( j = 0; j < count; j++){
		while( !SSP0_SR_TNF ){}  // warte bis Transmitt-FIFO platz hat um daten aufzunehmen
		//printf("schreibe data[j] = 0x%02X \n",data[j]);
		SSP0_DR = data[j];
 		while( !SSP0_SR_RNE ){}  // warte bis Recieve-FIFO daten zum lesen hat
		throwAway = SSP0_DR; //lese Dummy
 	    //printf("0x%02X (DUMMY) weggeworfen...\n",throwAway);	
	}

	// lese 3 dummybytes fuer Adresse u. 1 fuer CMD
	for(j = 0; j < 4; j++){
		while( !SSP0_SR_RNE ){}  // warte bis Recieve-FIFO daten zum lesen hat
		throwAway = SSP0_DR;
	}

	DISABLE_SSEL(spiDescriptor);	
	spiFlashMemWaitReady(spiDescriptor);
}


int spiFlashMemRead(int srcAddr, char *destAddr, int count, int spiDescriptor){
	//printf("spiFlashMemRead\n");
    ENABLE_SSEL(spiDescriptor);	
    
    // send out command
    SSP0_DR = CMD_READ_ARRAY; // "CMD_READ_ARRAY"-command  -> Atmel AT25DF641 Table 5.1
    
	//printf("HightByte:  0x%02X\n",(srcAddr >> 16) & 0xFF ); //write HightByte
    //printf("MiddleByte: 0x%02X\n",(srcAddr >>  8) & 0xFF ); //write MiddleByte	
	//printf("LowByte:    0x%02X\n",(srcAddr & 0xFF ) ); //write LowByte	

	SSP0_DR = ((srcAddr >> 16) & 0xFF); //write HightByte
    SSP0_DR = ((srcAddr >>  8) & 0xFF); //write MiddleByte	
	SSP0_DR = ( srcAddr & 0xFF ); //write LowByte	
    SSP0_DR = 0; //write zero		

	int i = 0;
	int dummy = 0;
	for( i = 0; i < 5; i++){ //read 5 dummy-bytes (cmd, addr, 0)
		while( !SSP0_SR_RNE ){}  // active waiting for receive fifo to fill resp. "NOT ( Receive fifo Not Empty )"
		dummy = SSP0_DR;
		//printf("dummy[%d]:    0x%02X\n",i,dummy); //write LowByte	
	} 

	count--;
    SSP0_DR = 0; //write zero		
	//read <count> DataBytes
	int j;
	for(i = 0, j= count; j > 0; i++, j--){
		SSP0_DR = 0; //write zero		
 		while( !SSP0_SR_RNE ){}  // active waiting for receive fifo to fill resp. "NOT ( Receive fifo Not Empty )"
		destAddr[i] = SSP0_DR;
		//printf(" gelesen destAddr[%d]:    0x%02X\n",i,destAddr[i]);
	}

	while( !SSP0_SR_RNE ){}  // active waiting for receive fifo to fill resp. "NOT ( Receive fifo Not Empty )"
	destAddr[i] = SSP0_DR;	

    //printf("leaving spiFlashMemRead...\n" );
    DISABLE_SSEL(spiDescriptor);
	
	return 1;
}



void readSpecs(){

    unsigned int rxDat;  // Received DATa
    unsigned int id;     // ID of AT25DF641
    int i;               // just a temporary variable/index

	// read out ID of Atmel AT35DF641 flash memory	
    // send out command
    SSP0_DR = CMD_READ_MANUFACTOR_ID;                     // "Read Manufacturer and Device ID"-command  -> Atmel AT25DF641 Table 5.1
    while( !SSP0_SR_RNE ){}                               // active waiting for receive fifo to fill resp. "NOT ( Receive fifo Not Empty )"
    throwAway = SSP0_DR;                                  // 1 byte send, hence read 1 byte
    
    // receive answer
    //
    // send 4 dummy bytes, since 4 bytes have to be received for Manufacturer and Devive ID
    // receive 4 bytes
    for( i=0; i<4; i++ ){
        SSP0_DR = 0x00;
    }//for
    
	//
    // receive 4 bytes
    rxDat=0;
    for( i=0; i<4; i++ ){
        while( !SSP0_SR_RNE ){}                           // active waiting for receive fifo to fill resp. "NOT Receive fifo Not Empty"
        rxDat = (rxDat<<8) | SSP0_DR;
    }//for
    
    id = rxDat;                                           // store ID
    
    // print out
    printf( "0x1F480000 is expected\n" );                 //                                            -> Atmel AT25DF641 Chapter 11.2, Table 11-1 & 11-2
    printf( "0x%08X is coded AT25DF641 ID\n", id );       // Manufacturer and Device ID of Atmel AT35DF641 flash memory
    //
    i = id & 0xff;
    while ( 0 < i-- ){
        SSP0_DR = 0x00;                                   // send 1 byte to receive 1 byte
        while( !SSP0_SR_RNE ){}                           // active waiting for receive fifo to fill resp. "NOT Receive fifo Not Empty"
        rxDat = SSP0_DR;                                  // receive 1 byte
        printf( "%02X ", rxDat );
    }//while
    printf( "\n" );
    //
    // decode Manufacturer and Devive ID                                                                -> Atmel AT25DF641 Chapter 11.2, Table 11-1 & 11-2
    printf( "0x%02X JEDEC Assigned Code\n",                     ( (id>>24)&0xFF ) );
    printf( "0x%02X Family Code\n",                               ( (id>>21)&0x07 ) );
    printf( "0x%02X Density Code\n",                              ( (id>>16)&0x1F ) );
    printf( "0x%02X Sub Code\n",                                  ( (id>>13)&0x07 ) );
    printf( "0x%02X Product Version Code\n",                      ( (id>>8) &0x1F ) );
    printf( "0x%02X Extended Device Information String Length\n", (  id     &0xFF ) );
    // NEVER stop doing task

} 

void menue(void){

	char number;
	printf("\nEnter druecken um zu starten");
	while(1){
		number = getc(stdin);
		switch(number - '0'){
			case 0: {
						SPI_INT_ENABLE;	
						readSpecs();
						SPI_INT_DISABLE;	
						break;
			}
			case 1: {	
						int byteCnt = 0;
						int addr =0;
						printf("Adresse ab der gelesen wird eingeben:\n");
						scanf("%d", &addr);
						printf("Adresse :%d wurde eingelesen\n", addr);
						printf("Anzahl der einzulesenden Bytes angeben\n");	
						scanf("%d", &byteCnt);
						printf("Anzahl der Bytes die gelesen werden : %d\n", byteCnt);
	
						char *data = malloc(byteCnt); 
						
						int i;
					    spiFlashMemRead(addr, data, byteCnt, SPI_INT);						
					    printf("##### %d Bytes von externem SPI-Speicher gelesen #####\n",byteCnt);
						for(i = 0; i < byteCnt; i++){
							//unsigned char c = data[i];
							//printf("0x%02X ",c);
							    printf("%c",data[i]);	
						}
						break;
			}
			case 2: {
	
	
						int byteCnt = 0;
						int addr =0;
						printf("Adresse ab der gelesen wird eingeben:\n");
						scanf("%d", &addr);
						printf("Adresse :%d wurde eingelesen\n", addr);
						printf("Anzahl der einzulesenden Bytes angeben\n");	
						scanf("%d", &byteCnt);
						printf("Anzahl der Bytes die gelesen werden : %d\n", byteCnt);
	
						char *data = malloc(byteCnt); 
					    spiFlashMemRead(addr, data, byteCnt, SPI_EXT);						
	
						addr = 0;
						printf("Adresse auf die geschrieben werden soll:\n");
						scanf("%d", &addr);
						printf("Adresse :%d wurde eingelesen\n", addr);
	
						spiFlashMemErase(addr, BLOCKSIZE_4K, SPI_INT);
						spiFlashMemWrite(addr, data, byteCnt, SPI_INT);
						break;
			}
			case 3: {
						int byteCnt = 0;
						int addr =0;
						printf("Adresse auf die geschrieben werden soll:\n");
						scanf("%d", &addr);
						printf("Adresse :%d wurde eingelesen\n", addr);
						printf("Anzahl der zu schreibenden Bytes angeben\n");	
						scanf("%d", &byteCnt);
						printf("Anzahl der zu schreibenden Bytes: %d\n", byteCnt);
	
						char *data = malloc(byteCnt); 
						    memset(data, 'A', byteCnt);	
						
						spiFlashMemErase(addr, BLOCKSIZE_4K, SPI_INT);
						spiFlashMemWrite(addr, data, byteCnt, SPI_INT);
	
						break;
			}
			case 4:
					{
						eraseChip(SPI_INT);
						break;	
			}
			case 5: {
						testSpiMem();					
						break;	
			}
				
			case 7: {
						SPI_EXT_ENABLE;	
						readSpecs();
						SPI_EXT_DISABLE;	
						break;
			}
			case 8: {
						int byteCnt = 0;
						int addr =0;
						printf("Adresse ab der gelesen wird eingeben:\n");
						scanf("%d", &addr);
						printf("Adresse :%d wurde eingelesen\n", addr);
						printf("Anzahl der einzulesenden Bytes angeben\n");	
						scanf("%d", &byteCnt);
						printf("Anzahl der Bytes die gelesen werden : %d\n", byteCnt);
	
						char *data = malloc(byteCnt); 
						int i;
					    spiFlashMemRead(addr, data, byteCnt, SPI_EXT);						
					    printf("##### %d Bytes von externem SPI-Speicher gelesen #####\n",byteCnt);
						for(i = 0; i < byteCnt; i++){
							printf("%c",data[i]);
						}
						break;
			}			
			default :{	
						printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Ende>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><\n\n"); 		
				 		printf("                 ##############Menue##############				     \n");
						printf("0 = Baustein Info von Intern       || 1 = Lesen vom internen Speicher\n");
						printf("2 = Kopieren von Extern auf Intern || 3 = 'A' Schreiben auf Internen \n");
						printf("4 = Internen Baustein loeschen     || 5 = Speicherung Verifizieren   \n");
						printf("7 = Baustein Info von Extern       || 8 = Lesen vom externen Speicher\n");
						printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<Beginn<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
						break;
			}
	
		}
	}
}


