/*
********************************************************************************
* name	 : 	main.c
* author : 	Rasmus Kallqvist, Michel Bitar (c) 2017
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
		demo_ball_struct();
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
	static int8_t x = 64-1;
	static int8_t y = 16-1;
	static int8_t dx = 2;
	static int8_t dy = -1;

	/* Draw step */
	display_cls();
	display_draw_rect(126,0,127,31); // draw left right
	display_draw_rect(0,0,1,31);	 // draw left line
	display_draw_rect(x, y, x+width, y+height); // draw ball
	display_update();
	quicksleep(100000);

	/* Update step */
	x += dx;
	y += dy;
	if(x+width >= 127 | x <= 1)
		dx = -dx;
	if(y+height >= 31 | y <= 0)
		dy = -dy;
}


/* Brief  : Test struct for pong ball
 * Author : Michel Bitar */
struct ball
{
	int x;
	int y;
	int w;
	int h;
	int dx;
	int dy;

}	ball;

struct ball b1 =
{
	.x = 64-1,
	.y = 16-1,
	.w = 4,
	.h = 4,
	.dx = 1,
	.dy = -1,
};

struct ball b2 =
{
	.x = 50-1,
	.y = 9-1,
	.w = 4,
	.h = 4,
	.dx = -1,
	.dy = 1,
};

void demo_ball_struct(void)
{
	/* Draw step */
	display_cls();
	display_draw_rect(b1.x, b1.y, b1.x+b1.w, b1.y+b1.h);
	display_draw_rect(b2.x, b2.y, b2.x+b2.w, b2.y+b2.h);
	display_update();

/* Update step ball1 */
	if(b1.x+b1.w >= 127 | b1.x <= 1)
		b1.dx = -b1.dx;
	if(b1.y+b1.h >= 31 | b1.y <= 0)
		b1.dy = -b1.dy;

		b1.x += b1.dx;
		b1.y += b1.dy;

/* Update step ball2 */
		if(b2.x+b2.w >= 127 | b2.x <= 1)
				b2.dx = -b2.dx;
			if(b2.y+b2.h >= 31 | b2.y <= 0)
				b2.dy = -b2.dy;

				b2.x += b2.dx;
				b2.y += b2.dy;

/* Sleep step */
	quicksleep(100000);

	return;
}


/* Brief  : Demo of display.c, developer testing of functions
 * Author : Michel Bitar */
void demo_moving_ball(void)
{
	//Ball1
	int8_t width = 4;
	int8_t height = 4;
	static int8_t x = 64-1;
	static int8_t y = 16-1;
	static int8_t dx = 1;
	static int8_t dy = -1;

	//Ball2
	int8_t awidth = 4;
	int8_t aheight = 4;
	static int8_t ax = 30-1;
	static int8_t ay = 18-1;
	static float adx = 2.8;
	static int8_t ady = -1;


	/* Draw step */
	display_cls();
	display_draw_rect(126,0,127,31); // draw left right
	display_draw_rect(0,0,1,31);	 // draw left line
	display_draw_rect(x, y, x+width, y+height); // draw ball 1
	display_draw_rect(ax, ay, ax+awidth, ay+aheight); // draw ball2
	display_update();
	quicksleep(100000);

	/* Update step */

	if(x+width >= 127 | x <= 1)
		dx = -dx;
	if(y+height >= 31 | y <= 0)
		dy = -dy;

		x += dx;
		y += dy;


		if(ax+awidth >= 127 | ax <= 1)
			adx = -adx;
		if(ay+aheight >= 31 | ay <= 0)
			ady = -ady;

			ax += adx;
			ay += ady;
}
