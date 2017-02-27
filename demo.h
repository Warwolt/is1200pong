/*
********************************************************************************
* name	 : 	demo.h
* author : 	Rasmus Kallqvist, Michel Bitar (c) 2017
* brief  :  header for demo.c
********************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Function declarations -----------------------------------------------------*/
void demo_bouncing_ball(void);
void demo_ball_struct(void);
void demo_moving_ball(void);
void demo_update_counting(void);

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
