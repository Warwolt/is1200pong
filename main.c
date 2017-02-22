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
	led_write(0x1); // signal bootup
	init_display();
	clear_display();
	led_write(0x0); // bootup done

	/* draw rectangle as a test */
	int8_t xoffs = 64-1;
	int8_t yoffs = 16-1;
	int8_t width = 4;
	int8_t height = 4;
	int8_t dx = 2;
	int8_t  dy = -1;	
	while(1)
	{
		/* Draw step */
		display_clear_screen();
		// draw lines
		display_draw_rect(126,0,127,31); 
		display_draw_rect(0,0,1,31);	 
		// draw ball
		display_draw_rect(xoffs, yoffs, xoffs+width, yoffs+height);
		// render 
		update_display();	
		quicksleep(100000);

		/* Update step */
		// change position
		xoffs += dx;
		yoffs += dy;
		// bounce against sides of screen
		if(xoffs+width >= 127 | xoffs <= 1)
			dx = -dx;
		if(yoffs+height >= 31 | yoffs <= 0)
			dy = -dy;		
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

	/* Enable LED1 through LED8 */
	TRISECLR = 0x00FF;

	/* Configure switches and buttonpins as inputs */ 
	TRISDSET = 0x0FE0; // Switches 4:1, Buttons 4:2
	TRISFSET = 0x0001; // Button 1	
	
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
	SPI2STATCLR = 0x40; 	/* SPI2STAT bit SPIROV = 0; */
    SPI2CONSET = 0x40; 		/* SPI2CON bit CKP = 1; */
	SPI2CONSET = 0x20; 		/* SPI2CON bit MSTEN = 1; */
	SPI2CONSET = 0x8000;	/* SPI2CON bit ON = 1; */	
}

/* Dummy function for handling interrupts */
// todo: implement something useful here. 
void user_isr(void)
{
	return;
}

/* Turn LED7 to LED0 on or off, bits in write_data specifies LED states */
void led_write(uint8_t write_data)
{
  /* Pointer to PORTE register */
  volatile uint32_t *porte = (volatile uint32_t*) 0xbf886110;

  /* Modify state of LEDs */
  *(porte+1) = ~(write_data);  // write zeros using PORTECLR 
  *(porte+2) =  (write_data);  // write ones using PORTESET 
  return;
}