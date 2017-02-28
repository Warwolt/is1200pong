/*
********************************************************************************
* name   :  pong.h
* author :  Rasmus Kallqvist / Michel Bitar, 2017
* brief  :  Header for pong.c
********************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include <pic32mx.h>	/* Declarations of hardware-specific addresses etc */
#include <stdint.h>  	/* Declarations of uint_32 and the like */
#include "display.h"  	/* OLED display device drivers and draw functions */
#include "input.h"		/* Read potentiometer and buttons values */
#include "structs.h"	/* Contains definitions for actor struct */

/* Defines -------------------------------------------------------------------*/
#define		MATCH_SCORE			5
#define 	PLAYINGFIELD_W		64
#define 	LEFT_EDGE			(DISPLAY_WIDTH - PLAYINGFIELD_W) / 2
#define 	RIGHT_EDGE			DISPLAY_WIDTH - PLAYINGFIELD_W / 2
#define		PLAYINGFIELD_MIDDLE DISPLAY_WIDTH / 2 - 1

/* Enums ---------------------------------------------------------------------*/
enum game_state {match_begin, round_begin, round_playing, match_end};
enum player	    {no_player, player_1, player_2};

/* Function prototypes -------------------------------------------------------*/
/* Pong game */
void pong_setup(void);
void pong_work(void); // note: maybe rename to pong_2p_mode?	
void pong_draw_step(void);
enum game_state pong_update_step(uint16_t* analog_values,
					  enum	game_state current_state);
enum player pong_update_ball(void);
int actor_collision(struct actor *a, struct actor *b);

