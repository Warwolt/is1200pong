/*
********************************************************************************
* name	 : 	main.c
* author : 	Rasmus Kallqvist, 2017-02-21
* brief  : 	Header file for main.c
********************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "display.h"

/* Function prototypes -------------------------------------------------------*/
void user_isr(void);
void init_mcu(void);
void led_write(uint8_t write_data);