/*
********************************************************************************
* name	 : 	main.c
* author : 	Rasmus Kallqvist, 2017-02-21
* brief  : 	Header file for main.c
********************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "display.h"  	/* OLED display device drivers and draw functions */
#include "input.h"		/* Read potentiometer and buttons values */
#include <pic32mx.h>	/* Declarations of hardware-specific addresses etc */


/* Function prototypes -------------------------------------------------------*/
void user_isr(void);
void init_mcu(void);
/* Peripherals */
void led_write(uint8_t write_data);
/* Demos */
void demo_bouncing_ball(void);
void demo_moving_ball(void);
void demo_cycling_cosine(void);
void demo_ball_struct(void);
