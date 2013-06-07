#include "spi.h"
#include "ConfigStick.h"
#include "malloc.h"
#include "mp3.h"

#define ARM

#define CPUFREQ 72000000

int main( void ){

	printf("main gestartet!\n");	
    BaseStickConfig();
    sspInit();

	int buffersize = 4096;
	int chunksize = 256;
	char *data = malloc(buffersize);
	
	int readBytes = 0;
	int i = 0;
	for(i = 0; i < buffersize; i += chunksize){
		spiFlashMemRead(readBytes , data, chunksize, SPI_EXT);
 	    readBytes += chunksize;
	}
	
	//menue();
    initMp3Module(data);
    while(1){};
}//main
