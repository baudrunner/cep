/* Compiler:  	GNU
 *
 * Date:      	5. 7. 2008
 * Author:    	Alfred Lohmann
 *
 *          	HAW-Hamburg
 *              Labor für technische Informatik
 *            	Berliner Tor  7
 *            	D-20099 Hamburg
 ********************************************************************
 * Description:
 *
 * - main program
 ********************************************************************
 * History:
 *
 *     Initial revision 
 ********************************************************************/
#include <stdio.h>

#include "lpc24xx.h"
#include "config.h"
#include "ConfigStick.h"
#include "portlcd.h"
#include "fio.h"


#define ARM
#include "mp3dec.h"

#define CPUFREQ 72000000


int main (void)
{
    //-- initialize the LPC-Stick, enable UART0, UART2 and CAN
    //--
    BaseStickConfig();
    
    printf("Hallo TI_Labor\n");
    
#ifdef LCD_SUPPORT     
    LCD_puts ( "Hallo TI_Labor" );
#endif
  
    //-- your code goes here 
    //--
    while (1)
    {

    }
}

/************************************** EOF *********************************/
