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
#include "structs.h"	/* Contains definitions for actor struct */

/* Defines -------------------------------------------------------------------*/
/* Timer */
#define   	CLOCKFREQ       80000000    	// 80 MHz
#define   	TIMEOUTPERIOD   0.00001  		// 10 us
#define   	TMR2PRESCALER   16 			    // (80 MHz / 16) = 5 MHz
#define   	TMR2PERIOD      ( (CLOCKFREQ / TMR2PRESCALER) * TIMEOUTPERIOD ) - 1
/* Pong game */
#define		MATCH_SCORE			5
#define 	PLAYINGFIELD_W		64
#define 	LEFT_EDGE			(DP_WIDTH - PLAYINGFIELD_W) / 2
#define 	RIGHT_EDGE			DP_WIDTH - PLAYINGFIELD_W / 2
#define		PLAYINGFIELD_MIDDLE DP_WIDTH / 2 - 1


/* Function prototypes -------------------------------------------------------*/
/* Pong game */
void pong_setup(void);
void pong_work(void); // note: maybe rename to pong_2p_mode?	
/* Init and interrupts */
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

