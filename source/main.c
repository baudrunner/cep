#include "spi.h"
#include "ConfigStick.h"
#include "malloc.h"


int main( void ){


	printf("main gestartet!\n");	
    BaseStickConfig();
    sspInit();

	menue();
    //initMp3Module(data);
    while(1){};
}//main
