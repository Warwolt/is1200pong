/*
********************************************************************************
* name	 : 	main.c
* author : 	Rasmus Kallqvist, 2017-02-21
* brief  : 	Header file for main.c
********************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include <pic32mx.h>	/* Declarations of hardware-specific addresses etc */
#include <stdint.h>  	/* Declarations of uint_32 and the like */
#include "display.h"  	/* OLED display device drivers and draw functions */
#include "input.h"		/* Read potentiometer and buttons values */


/* Defines -------------------------------------------------------------------*/
/* Timer */
#define   CLOCKFREQ       80000000    	// 80 MHz
#define   TIMEOUTPERIOD   0.00001  		// 10 us
#define   TMR2PRESCALER   16 			// (80 MHz / 16) = 5 MHz
#define   TMR2PERIOD      ( (CLOCKFREQ / TMR2PRESCALER) * TIMEOUTPERIOD ) - 1


/* Function prototypes -------------------------------------------------------*/
void user_isr(void);
void init_mcu(void);
void init_tim(void);
/* Peripherals */
void led_write(uint8_t write_data);
/* Demos */
void demo_bouncing_ball(void);
void demo_moving_ball(void);
void demo_cycling_cosine(void);
void demo_ball_struct(void);

/* Structs -------------------------------------------------------------------*/
/* Brief  : Struct for all moving objects
 * Author : Michel Bitar */
struct actor
{
    int x;
    int y;
    int w;
    int h;
    int dx;
    int dy;
}	actor;
