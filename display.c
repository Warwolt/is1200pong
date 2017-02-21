/*
********************************************************************************
* name	 : 	display.c
* author : 	Rasmus Kallqvist, 2017-02-21
*		   	Original code by Fredrik Lundeval and Axel Isaksson, 2015
* brief  : 	Contains driver for interfacing with and initialize the chipkit 
*			uno32 basic i/o shield oled display. For more information about the 
* 			display, see reference manual for SSD1306 display controller and
*			reference document for the chipkit i/o shield.
********************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "display.h"

/* Function definitions ------------------------------------------------------*/
/* Name	  : display initiation
 * Brief  : performs low level initiation of the i/o shield OLED-display with
 *			a sequence of commands sent to display controller via spi.
 * Author : original code by Fredrik Lundeval / Axel Isaksson (note: which one?)
 *			display command macros by Rasmus Kallqvist */
void init_display(void) 
{
  /* Apply power to display controller (VDD) */
  DISPLAY_CHANGE_TO_COMMAND_MODE;
  quicksleep(10);
  DISPLAY_ACTIVATE_VDD;
  quicksleep(1000000);
  
  /* Turn off display */
  spi_send_recv(CMD_DISPLAY_OFF);
  DISPLAY_ACTIVATE_RESET;     
  quicksleep(10);             
  DISPLAY_DO_NOT_RESET;       
  quicksleep(10);              
  
  /* Enable 7.5 V to VBAT */
  spi_send_recv(CMD_SET_CHARGE_PUMP);        
  spi_send_recv(CMD_ENABLE_CHARGE_PUMP);     
  spi_send_recv(CMD_SET_PRECHARGE_PERIOD);   
  spi_send_recv(CMD_CHARGE_PHASE1(1) | CMD_CHARGE_PHASE2(15)); 
  
  /* Apply power display (VBAT) */
  DISPLAY_ACTIVATE_VBAT;
  quicksleep(10000000);   
  
  /* Set COM output and scan direction */
  spi_send_recv(CMD_SET_SEGMENT_REMAP);        
  spi_send_recv(CMD_SET_COM_SCAN_REMAP);       
  
  /* Set COM pins */
  spi_send_recv(CMD_SET_COM_PIN_CONFIG);       
  spi_send_recv(CMD_SEQ_COM_LEFTRIGHT_REMAP);  
 
  /* Turn on display */  
  spi_send_recv(CMD_DISPLAY_ON);    
}


/* Name	  : quicksleep
 * Brief  : a simple function to create a small delay. Very inefficient use of 
 *			computing resources, but very handy in some special cases.
 * Author : Fredrik Lundeval / Axel Isaksson (note: which one?) */
// ! todo: check if making this function volatile makes things any different.
// ! in particular, examine if the execution time differs.
// ! preferably check by toggling a pin and measuring with oscilloscope. 
void quicksleep(int cyc) 
{
	int i;
	for(i = cyc; i > 0; i--);
}


/* Name	  : spi send receive
 * Brief  : send/receive one byte over spi connected to the i/o shield display
 * Author : Fredrik Lundeval / Axel Isaksson (note: which one?) */
uint8_t spi_send_recv(uint8_t data) 
{
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while(!(SPI2STAT & 1));
	return SPI2BUF;
}