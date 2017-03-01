/*
********************************************************************************
* name	 : 	demo.c
* author : 	Rasmus Kallqvist, Michel Bitar (c) 2017
* brief  :  Contains some demos for the project and peripherals, primarily used
*			to try out stuff like the display and input controls.
********************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "demo.h"

/* Function definitions ------------------------------------------------------*/

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


/* Stuff for demo */
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


/* Brief  : Demo of display.c, developer testing of functions
 * Author : Michel Bitar */
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


/* Brief  : Demo that tries out spacing updates out by counting number of calls
 * Author : Rasmus Kallqvist */
void demo_update_counting(void)
{
	static char sec = 0x37; 
	static updates;

	/* Track updates */
	updates++;	

	/* Display seconds */
	display_cls();
	display_print("hello updates", 0, 0);
	display_print(&sec, 0, 8);
	display_update();

	/* Update secodns */
	if(updates % 30 == 0)
		sec++;
	if(sec==0x3A)
		sec = 0x30;
}

/* Brief  : Demo of input.c function, tests that get button function works
 * Author : Rasmus Kallqvist */
void demo_get_btn(void)
{
	// starting test with only button 3, add 2:0 later
	static uint8_t led_states;
	static uint8_t prev_button_state; 
	uint8_t button_state;

	/* Read buttons */
	button_state = input_get_btn(3); 
	
	/* If pushed, light leds */
	if(button_state && !prev_button_state)
		led_states = ~led_states;

	/* Update LEDs */
	led_write(led_states);

	/* Remember button states */
	prev_button_state = button_state;

}
