#include "spi.h"
#include "ConfigStick.h"
#include "malloc.h"
#include "mp3.h"
#include "outline.h"
#define ARM

#define CPUFREQ 72000000



int main( void ){

    BaseStickConfig();
    sspInit();

	printf("main gestartet!\n");		

	//int buffersize = MAINBUF_SIZE;  //1940
	



	//fillBuffer();
	initMp3Module();	
	decodeMp3();
	fillBuffer();	
	decodeMp3();	
	
	/*
	int i;
	for(i = 0; i < 3; i++){
		decodeMp3();
		fillBuffer();
	}*/
    

	mp3Cleanup();

	
	//char *data = spiReadBytes(buffersize, 0, SPI_INT);
		
	//menue();

    //initMp3Module(data);

    //dac();



    while(1){}
}//main
