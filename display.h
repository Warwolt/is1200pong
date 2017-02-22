/*
********************************************************************************
* name	 : 	display.h
* author : 	Rasmus Kallqvist, 2017-02-21
*		   	Original code by Fredrik Lundeval and Axel Isaksson, 2015
* brief  : 	Headerfile for display.c 
********************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of hardware-specific addresses etc */
#include <math.h>     /* Trigonometric functions */


/* Defines -------------------------------------------------------------------*/
/* Macros for display control pins */
#define DISPLAY_CHANGE_TO_COMMAND_MODE 	(PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE 	(PORTFSET = 0x10)
#define DISPLAY_ACTIVATE_RESET 			(PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET 			(PORTGSET = 0x200)
#define DISPLAY_ACTIVATE_VDD 			(PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT 			(PORTFCLR = 0x20)
#define DISPLAY_TURN_OFF_VDD 			(PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT 			(PORTFSET = 0x20)
/* Display controller commands */
#define CMD_DISPLAY_OFF 				(uint8_t)0xAE
#define CMD_DISPLAY_ON					(uint8_t)0xAF
#define CMD_SET_CHARGE_PUMP				(uint8_t)0x8D
#define CMD_ENABLE_CHARGE_PUMP			(uint8_t)0x14
#define CMD_SET_PRECHARGE_PERIOD		(uint8_t)0xD9
#define CMD_CHARGE_PHASE1(x)			(uint8_t)(x)
#define CMD_CHARGE_PHASE2(x)			(uint8_t)(x << 4)
#define	CMD_SET_SEGMENT_REMAP			(uint8_t)0xA1
#define CMD_SET_COM_SCAN_REMAP			(uint8_t)0xC8
#define CMD_SET_COM_PIN_CONFIG			(uint8_t)0xDA
#define CMD_SEQ_COM_LEFTRIGHT_REMAP		(uint8_t)0x20
#define	CMD_SET_PAGE_ADDRESS			(uint8_t)0x22
/* Math */
#define PI 								3.14159	

/* Function prototypes -------------------------------------------------------*/
/* Hardware abstractions */
void display_set_pixel(uint8_t x, uint8_t y);
void display_draw_rect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void display_draw_cos(uint32_t period, uint32_t phase);
void display_clear_screen(void);
/* Device drivers */
void init_display(void);
void clear_display(void);
void update_display(void);
/* Helper functions */
void quicksleep(int cyc);
uint8_t spi_send_recv(uint8_t data);