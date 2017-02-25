/*
********************************************************************************
* name	 : 	input.h
* author : 	Rasmus Kallqvist, 2017-02-21
* brief  : 	Header file for main.c
********************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include <pic32mx.h>	/* Declarations of hardware-specific addresses etc */
#include <stdint.h>		/* Containts uint32_t and the like */

/* Function prototypes -------------------------------------------------------*/
uint16_t input_get_analog(uint8_t pin);
void init_adc(void);
