/*
********************************************************************************
* name	 : 	main.c
* author : 	Rasmus Kallqvist, 2017-02-21
*		   	Original code by Fredrik Lundeval and Axel Isaksson, 2015
* brief  : 	Main function for pong game on chipkit uno32 microcontroller
********************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Function definitions ------------------------------------------------------*/
/* Main */
int main(void) 
{
	/* Initialization */
	init_mcu();
	init_display();
	clear_display();

	/* draw a cosine, just for fun */
	while(1)
	{
		uint32_t i = 0;
		for(i = 0; i < 128; i++)
		{
			display_clear_screen();
			display_draw_cos(128-i, 0);	
			update_display();
			quicksleep(100000);
		}
	}
	return 0;
}

/* Low level initialization of microcontroller */
void init_mcu(void)
{
	/* Set up peripheral bus clock */
    /* OSCCONbits.PBDIV = 1; */
    OSCCONCLR = 0x100000; /* clear PBDIV bit 1 */
	OSCCONSET = 0x080000; /* set PBDIV bit 0 */
	
	/* Output pins for display signals */
	PORTF = 0xFFFF;			
	PORTG = (1 << 9);		
	ODCF = 0x0;				
	ODCG = 0x0;				
	TRISFCLR = 0x70;		
	TRISGCLR = 0x200;		
	
	/* Set up SPI as master */
	SPI2CON = 0;
	SPI2BRG = 4;
	/* SPI2STAT bit SPIROV = 0; */
	SPI2STATCLR = 0x40;
	/* SPI2CON bit CKP = 1; */
    SPI2CONSET = 0x40;
	/* SPI2CON bit MSTEN = 1; */
	SPI2CONSET = 0x20;
	/* SPI2CON bit ON = 1; */
	SPI2CONSET = 0x8000;	
}

/* Dummy function for handling interrupts */
// todo: implement something useful here. 
void user_isr(void)
{
	return;
}