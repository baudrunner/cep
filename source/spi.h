#ifndef _CE_SPI_H_
#define _CE_SPI_H_
#include <stdio.h>


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

#define CHUNKSIZE 256

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



int  spiFlashMemReadStatusRegister(int spiDescriptor);
void spiFlashMemWaitReady(int spiDescriptor);
void spiFlashMemSetWriteEnable(int spiDescriptor);
void spiFlashMemGlobalUnprotect(int spiDescriptor);
void spiFlashMemErase(int destAddr, int blockSizeDef, int spiDescriptor);
void spiFlashMemWrite(int dstAddr, char *data, int count, int spiDescriptor);
int  spiFlashMemRead(int srcAddr, char *destAddr, int count, int spiDescriptor);
void eraseChip(int spiDescriptor);
void spiReadBytes(unsigned char* dstBuf,int cnt, int srcAddr, int spiDesc);
void readSpecs();
void menue();
int  testSpiMem();
void sspInit();

#endif //_CE_SPI_H_
