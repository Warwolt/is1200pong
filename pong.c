/*
********************************************************************************
* name   :  pong.c
* author :  Rasmus Kallqvist / Michel Bitar, 2017
* brief  :  Contains all game logic for pong game.
********************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "pong.h"

/* Local variables -----------------------------------------------------------*/
static int 		update_counter; /* Tracks 30 updates per second */
static struct  	actor ball;
static struct  	actor left_racket;
static struct  	actor right_racket;
static int	   	pl1_score; 	/* Player 1 score tracker */	
static int     	pl2_score; 	/* Player 2 score tracker */
static enum 	game_state current_state = round_begin;
static enum 	game_state next_state;

/* Function definitions ------------------------------------------------------*/
/* Brief  : Set up pong game and initialize file local variables.
   Author : Michel Bitar */
void pong_setup(void)
{
	/* Settings */
	uint32_t edge_offset = 34; // distance from screen edge

	/* Intialize game structs */
	left_racket.w = 3;
	left_racket.h = 9;
	left_racket.x = edge_offset;
	left_racket.y = 16 - 1 - (left_racket.h / 2);

	right_racket.w = 3;
	right_racket.h = 9;
	right_racket.x = 128 - 1 - right_racket.w - edge_offset;
	right_racket.y = 16 - 1 - (right_racket.h / 2);

	ball.w = 2;
	ball.h = 2;
	ball.x = 64-1;
	ball.y = 16-1;
	ball.dx = 1;
	ball.dy = -1;
}

/* Brief  : Carries out one iteration of the pong game state with the sequence;
 * 			draw game state, read inputs, and update game state.
 * Author : Michel Bitar and Rasmus Kallqvist */
void pong_work(void)
{
	static char c; // Temporary character storage 
	uint16_t analog_values[2];

	/* Draw step */
	display_cls(); /* Clear screen */

	/* Draw actors */
	display_draw_actor(&left_racket);
	display_draw_actor(&right_racket);
	display_draw_actor(&ball);
	/* Draw playing field */
	display_draw_dotline(LEFT_EDGE - 1);
	display_draw_dotline(RIGHT_EDGE - 1);
	/* Draw scores */
	c = 0x30 + pl1_score;
	display_print("pl1", 0, 0);
	display_print(&c, 8, 10);
	c  = 0x30 + pl2_score;
	display_print("pl2", 128-24, 0);
	display_print(&c, 128-16, 10);
 
 	display_update(); /* Update screen */

	/* Input step */
	analog_values[0] = 1023 - input_get_analog(0);	/* Player 1 */
	analog_values[1] = 1023 - input_get_analog(0); 	/* Player 2 */

	/* Update step */
	pong_update_step(analog_values);
	current_state = next_state;
}


/* Brief  : Carries out the update step of one pong game iteration.
 * Author : Michel Bitar and Rasmus Kallqvist */
void pong_update_step(uint16_t* analog_values)
{
	uint8_t point_scored = 0;
	static int updates_waited;

	/* Track updates */
	update_counter++;

  	/* Move rackets */
    left_racket.y = analog_values[0] * (32 - left_racket.h) / 1024;
	right_racket.y = analog_values[1] * (32 - right_racket.h) / 1024;

	/* Update ball */
	if(current_state == round_begin)
	{
		updates_waited++; // pause ball when round begins
	}
	if(current_state == round_playing)
	{
		pong_update_ball();		
	}

    /* Check if score */
    if(ball.x > RIGHT_EDGE)
    {
    	pl1_score++;
    	ball.x = PLAYINGFIELD_MIDDLE;
    	ball.dx = -ball.dx;
    	if(pl1_score == 5)
    	{ 
    		pl1_score = 0;
    		point_scored = 1;
    	}
    }
    if(ball.x < LEFT_EDGE)
    {
    	pl2_score++;
    	ball.x = PLAYINGFIELD_MIDDLE;
    	ball.dx = -ball.dx;
    	if(pl2_score == 5)
    	{ 
    		pl2_score = 0;
    		point_scored = 1;
    	}
    }	

    /* Update states */
    if(point_scored)
    {
    	next_state = round_begin;
    }
    if(updates_waited == 2)
    {
    	next_state = round_playing;
    }
}

/* Brief  : Ball part of game state update step
 * Author : Michel Bitar and Rasmus Kallqvist */
void pong_update_ball(void)
{
	/* Check ball collisons */
	// collide with roof and floor 
	if(ball.x+ball.w >= 127 | ball.x <= 1)
		ball.dx = -ball.dx;
	if(ball.y+ball.h >= 31 | ball.y <= 0)
		ball.dy = -ball.dy;
	// collide with rackets 
	if(actor_collision(&ball, &right_racket)
		 || actor_collision(&ball, &left_racket))
		ball.dx = -ball.dx;
	
	/* Move ball */
	ball.x += ball.dx;
    ball.y += ball.dy;
}


/* Brief  : Checks for actor collision using Axis Aligned Bounding Box (AABB)
 * Author : Michel Bitar */
int actor_collision(struct actor *a, struct actor *b)
{
	if ( (a->x + a->dx) < (b->x + b->w) && (a->x + a->w + a->dx) > (b->x) &&
		 (a->y + a->dy) < (b->y + b->h) && (a->y + a->h + a->dy) > (b->y) )
		return 1;
	else
		return 0;

}