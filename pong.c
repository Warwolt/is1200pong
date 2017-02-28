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
static int	   	g_pl1_score; 	/* Player 1 score tracker */
static int     	g_pl2_score; 	/* Player 2 score tracker */
static enum 	player 	   g_winning_player;

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
	static enum	game_state current_state = match_begin; //match_begin
	enum game_state next_state;
	uint16_t analog_values[2];
	int32_t c; // Temporary character storage

	/* Draw step */
	display_cls();
	switch(current_state)
	{
		/* Draw match begin message */
		case(match_begin) :
			display_print("Get ready", 32, 12);
			break;

		/* Draw player won message */
		case(match_end) : 
			if(g_winning_player == player_1)
				display_print("Player 1", 32, 8);
				display_print("wins!", 48, 16);
			if(g_winning_player == player_2)
				display_print("Player 2", 32, 8);
				display_print("wins!", 48, 16);
			break;

		/* Draw pong round */
		default : 
			/* Draw actors */
			display_draw_actor(&left_racket);
			display_draw_actor(&right_racket);
			display_draw_actor(&ball);
			/* Draw playing field */
			display_draw_dotline(LEFT_EDGE - 1, 3);
			display_draw_dotline(RIGHT_EDGE - 1, 3);
			/* Draw scores */
			c = 0x30 + g_pl1_score;
			display_print("pl1", 0, 0);
			display_print((char*)&c, 8, 10);
			c  = 0x30 + g_pl2_score;
			display_print("pl2", 128-24, 0);
			display_print((char*)&c, 128-16, 10);
			break;
	}
 	display_update();

	/* Input step */
	analog_values[0] = 1023 - input_get_analog(0);	/* Player 1 */
	analog_values[1] = 1023 - input_get_analog(0); 	/* Player 2 */

	/* Update step */
	next_state = pong_update_step(analog_values, current_state);

	/* Update state machine */
	current_state = next_state;
}

/* Brief  : Carries out the update step of one pong game iteration.
 * 			States are changed if points are scored or time has elapsed.
 * Author : Michel Bitar and Rasmus Kallqvist */
enum game_state pong_update_step(uint16_t* analog_values,
					  enum	game_state current_state)
{
	static int updates_waited = 0;
	enum game_state next_state;
	enum player scoring_player;
	uint8_t scored_point = 0;
	uint8_t match_point = 0;
	uint8_t done_waiting = 0;

  	/* Move rackets */
    left_racket.y = analog_values[0] * (32 - left_racket.h) / 1024;
	right_racket.y = analog_values[1] * (32 - right_racket.h) / 1024;

	switch(current_state)
	{
		/* Wait a little at start of match */
		case (match_begin) :
			updates_waited++;
			if(updates_waited == 45)
				done_waiting = 1;
			break;

		/* Wait a little at start of reach round */
		case (round_begin) :
			updates_waited++;
			if(updates_waited == 15)
				done_waiting = 1;
			break;

		/* Update ball when round is underway */
		case (round_playing) :
			scoring_player = pong_update_ball();
			break;

		/* Wait a bit little end of match */
		case (match_end) :
			updates_waited++;
			if(updates_waited == 60)
				done_waiting = 1;
			break;
	}

    /* Check if score */
	switch(scoring_player)
	{
		case(player_1)  :
	    	g_pl1_score++;

	    	scored_point = 1;
	    	if(g_pl1_score == MATCH_SCORE)
	    	{
	    		g_winning_player = player_1;
	    		match_point = 1;
	    	}
			break;

		case(player_2)  :
	    	g_pl2_score++;
	    	scored_point = 1;
	    	if(g_pl2_score == MATCH_SCORE)
	    	{
	    		g_winning_player = player_2;
	    		match_point = 1;
	    	}
			break;

		case(no_player) :
			break;
	}

    /* Update states */
    if(match_point)
    {
    	g_pl1_score = 0;
    	g_pl2_score = 0;
    	next_state = match_end;
    }
    else if(scored_point)
    {
    	updates_waited = 0;
    	next_state = round_begin;
    }
    else if(current_state == match_begin && done_waiting)
    {
    	updates_waited = 0;
    	done_waiting = 0;
    	next_state = round_begin;
    }
    else if(current_state == round_begin && done_waiting)
    {
    	updates_waited = 0;
    	done_waiting = 0;
    	next_state = round_playing;
    }
    else if(current_state == match_end && done_waiting)
    {
    	updates_waited = 0;
    	done_waiting = 0;
    	next_state = match_begin;
    }
    else /* Default */
    {
    	next_state = current_state;
    }

    return next_state;
}

/* Brief  : Ball part of game state update step.
 *			Returns which player scored.
 * Author : Michel Bitar and Rasmus Kallqvist */
enum player pong_update_ball(void)
{
	enum player scoring_player;

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

    /* Check if scored */
    if(ball.x > RIGHT_EDGE)
    {
    	ball.x = PLAYINGFIELD_MIDDLE;
    	ball.dx = -ball.dx;
    	scoring_player = player_1;
    }
    else if(ball.x < LEFT_EDGE)
    {
    	ball.x = PLAYINGFIELD_MIDDLE;
    	ball.dx = -ball.dx;
    	scoring_player = player_2;
    }
    else
    {
    	scoring_player = no_player;
    }

    return scoring_player;
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
