/*
********************************************************************************
* name   :  menu.c
* author :  Rasmus Kallqvist, 2017
* brief  :  Contains menu system for pong game
********************************************************************************
*/

/* Include -------------------------------------------------------------------*/
#include "menu.h"

// todo move this to header
/* Declarations --------------------------------------------------------------*/
enum menu_state {item_1, item_2, item_3, num_items};


/* Function definitions ------------------------------------------------------*/
/* Brief  : Testing a menu state machine system 
 * Author : Rasmus Kallqvist 	*/ 
void menu_test(void)
{
	static enum menu_state current_state;
	enum menu_state next_state; 
	uint16_t analog_input;
	uint16_t item_select;

	display_cls();
	// display menu items 
	display_print("item 1", 32, 0);
	display_print("item 2", 32, 8);
	display_print("item 3", 32, 16);
	// show cursor
	display_print(">", 24, current_state * 8);
	display_update();

	// get analog inputs
	analog_input = input_get_analog(0);

	// update menu 
	item_select = analog_input * num_items / 1024;
	next_state = item_select;

	// update state machine 
	current_state = next_state;
}