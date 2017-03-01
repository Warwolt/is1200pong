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
static struct  	actor g_ball;
static struct  	actor g_left_racket;
static struct  	actor g_right_racket;
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
	g_left_racket.w = 3;
	g_left_racket.h = 9; 
	g_left_racket.x = edge_offset;
	g_left_racket.y = 16 - 1 - (g_left_racket.h / 2);

	g_right_racket.w = g_left_racket.w;
	g_right_racket.h = g_left_racket.h;
	g_right_racket.x = 128 - 1 - g_right_racket.w - edge_offset;
	g_right_racket.y = 16 - 1 - (g_right_racket.h / 2);

	g_ball.w = 2;
	g_ball.h = 2;
	g_ball.x = 64-1;
	g_ball.y = 16-1;
	g_ball.dx = 1;
	g_ball.dy = -1;
}

/* Brief  : Draws a pause splash screen displayed with the game is puased 
 * Author : Rasmus Kallqvist */
void pong_pause(void)
{
	/* Draw splash over paused game state */
	display_draw_rectfill(18,10, (18+88+2),(10+10+3), 0);
	display_draw_rect    (18,10, (18+88+2),(10+10+3), 1);
	display_print("game paused", 19, 12);
	display_update();
}

/* Brief  : Carries out one iteration of the pong game state with the sequence;
 * 			draw game state, read inputs, and update game state.
 * Author : Michel Bitar and Rasmus Kallqvist */
void pong_work(void)
{
	static enum	game_state current_state = match_begin;
	static int updates_waited;
	enum game_state next_state;
	uint16_t analog_values[2];
	uint32_t c; // ascii values

	/* Draw step */
	display_cls();
	switch(current_state)
	{
		/* Draw match begin message */
		case(match_begin) :
			updates_waited++;
			/* "Get ready" for 2 seconds */
			if(updates_waited <= 60)
			{
				display_print("Get ready", 28, 12);
			}
			/* "Playing to N" for 2 seconds */
			if(updates_waited > 60)
			{
				display_print("Playing to ", 16, 12);
				c = 0x30 + MATCH_SCORE;
				display_print((char*)&c, 104, 12);
			}
			/* Reset counter after 4 seconds */
			if(updates_waited == 120)
				updates_waited = 0;
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
			display_draw_actor(&g_left_racket);
			display_draw_actor(&g_right_racket);
			display_draw_actor(&g_ball);
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
	analog_values[0] = 1023 - input_get_analog(1);	/* Player 1 */
	analog_values[1] = 1023 - input_get_analog(2); 	/* Player 2 */

	/* Update step */
	next_state = pong_update_step(analog_values, current_state);

	/* Update state machine */
	current_state = next_state;
}


/* Brief  : Carries out the update step of one pong game iteration.
 * Author : Michel Bitar and Rasmus Kallqvist 
 * Note   : Next state defaults to the current state. */
enum game_state pong_update_step(uint16_t* analog_values,
					  enum	game_state current_state)
{
	enum game_state next_state = current_state; 
	enum player scoring_player;
	static int updates_waited;

  	/* Update racket positions */
    g_left_racket.y = analog_values[0] * (32 - g_left_racket.h) / 1024;
	g_right_racket.y = analog_values[1] * (32 - g_right_racket.h) / 1024;

	/* Update game state */
	switch(current_state)
	{
		/* Start of match */
		case(match_begin):
			updates_waited++;
			/* Start first round after 4 seconds */
			if(updates_waited == 120) 
			{
				updates_waited = 0; // reset counter 
				next_state = round_begin;
			}
			break;

		/* Start of a round */
		case(round_begin):
			updates_waited++;
			/* Pause 0.5 second inbetween rounds */
			if(updates_waited == 15)
			{
				updates_waited = 0; // reset counter
				g_ball.dx = g_ball.dx / g_ball.dx; // reset speed, keep sign
				next_state = round_playing;
			}
			break;

		/* Round is going on */
		case(round_playing):			
			/* Move ball */
			scoring_player = pong_update_ball();

			/* Check if player 1 scored */
			if(scoring_player==player_1)
			{
				g_pl1_score++;
				if(g_pl1_score==MATCH_SCORE)
				{
					/* Player 2 won match */
					g_winning_player = player_1;
					/* Reset scores */
					g_pl1_score = 0;
					g_pl2_score = 0;
					next_state = match_end;
				}
				else
					/* Scored point for round */
					next_state = round_begin; 			
			}

			/* Check if player 2 scored */
			if(scoring_player==player_2)
			{
				g_pl2_score++;
				if(g_pl2_score==MATCH_SCORE)
				{
					/* Player 2 won match */
					g_winning_player = player_2;
					/* Reset scores */
					g_pl1_score = 0;
					g_pl2_score = 0;
					next_state = match_end;
				}
				else
					/* Scored point for round */
					next_state = round_begin; 			
			}
			break;

		/* End of match */
		case(match_end):
			updates_waited++;
			/* Display victory message for 2.5 seconds */
			if(updates_waited == 75) 
			{
				updates_waited = 0; // reset
				next_state = match_begin;
			}
			break;	
		}

	return next_state;
}


/* Brief  : Ball part of game state update step.
 *			Returns which player scored.
 * Author : Michel Bitar and Rasmus Kallqvist */
enum player pong_update_ball(void)
{
	enum player scoring_player;

	/* Collide ball with roof and floor */
	if(g_ball.x+g_ball.w >= 127 | g_ball.x <= 1)
		g_ball.dx = -g_ball.dx;
	if(g_ball.y+g_ball.h >= 31 | g_ball.y <= 0)
		g_ball.dy = -g_ball.dy;
	
	/* Collide ball with rackets */
	if(actor_collision(&g_ball, &g_right_racket)
		 || actor_collision(&g_ball, &g_left_racket))
	{
		/* Bounce back ball and increase speed */
		g_ball.dx = -g_ball.dx * BALL_SPEEDUP;
		/* Cap ball speed to a max value */
		if(ABS(g_ball.dx) >= BALL_MAXSPEED)
		{
			g_ball.dx = SGN(g_ball.dx) * BALL_MAXSPEED;
		}		
	}

	// developer test 
	if(ABS(g_ball.dx) == BALL_MAXSPEED)
		led_write(0xFF);

	/* Move ball */
	g_ball.x += g_ball.dx;
    g_ball.y += g_ball.dy;

    /* Check if scored */
    if(g_ball.x > RIGHT_EDGE)
    {
    	g_ball.x = PLAYINGFIELD_MIDDLE;
    	g_ball.dx = -g_ball.dx;
    	scoring_player = player_1;
    }
    else if(g_ball.x < LEFT_EDGE)
    {
    	g_ball.x = PLAYINGFIELD_MIDDLE;
    	g_ball.dx = -g_ball.dx;
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
