/*
********************************************************************************
* name	 : 	main.c
* author : 	Rasmus Kallqvist, 2017-02-21
*		   	Original code by Fredrik Lundeval and Axel Isaksson, 2015
* brief  : 	Main function for pong game on chipkit uno32 microcontroller
********************************************************************************
*/

/* 	TODO:
 * [ ]	Check that border clipping on display_draw_rect works
 * [ ]	Check that LED works using the pic32mx.h address defines 
 * [ ] 	Configure hardware for analog inputs 
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Function definitions ------------------------------------------------------*/
/* Main */
int main(void) 
{
	/* Initialization */
	init_mcu();

	led_write(0x3); // signal bootup
	init_adc();
	init_display();
	led_write(0x1); // bootup done

	/* Run demos */
	while(1)
	{
		demo_bouncing_ball();
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
  /* Modify state of LEDs */
  PORTECLR = ~(write_data);	// write zeros of write_data
  PORTESET = (write_data);	// write ones  of write_data
  return;
}

/* Brief  : Demo of display.c functions, bounces a rectangle ball against the
 * 			sides of the screen and draws to lines at each side.
 * Author : Rasmus Kallqvist */
void demo_bouncing_ball(void)
{
	int8_t width = 4;
	int8_t height = 4;
	static int8_t xoffs = 64-1;
	static int8_t yoffs = 16-1;
	static int8_t dx = 2;
	static int8_t dy = -1;	

	/* Draw step */
	display_cls();
	display_draw_rect(126,0,127,31); // draw left right
	display_draw_rect(0,0,1,31);	 // draw left line
	display_draw_rect(xoffs, yoffs, xoffs+width, yoffs+height); // draw ball
	display_update();	
	quicksleep(100000);

	/* Update step */
	xoffs += dx;
	yoffs += dy;
	if(xoffs+width >= 127 | xoffs <= 1)
		dx = -dx;
	if(yoffs+height >= 31 | yoffs <= 0)
		dy = -dy;		
}