/*
********************************************************************************
* name	 : 	input.h
* author : 	Rasmus Kallqvist, 2017-02-21
*			Some original code by Axel Isaksson, 2015
* brief  : 	Header file for main.c
*
* 	Pin nr		PORT bit nr 
* 	------		-----------
* 	A0			B2
* 	A1			B4
* 	A2			B8
* 	A3			B10
* 	A4			B12
* 	A5			B14
*
********************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "input.h"

/* Local variables -----------------------------------------------------------*/
static uint8_t analog_pin_nr[6] = {2, 4, 8, 10, 12, 14};


/* Function definitions ------------------------------------------------------*/
/* 	Brief	: Get sampled analog input values 
	Author	: Rasmus Kallqvist / Axel Isaksson */
uint16_t input_get_analog(uint8_t pin)
{
	/* Select potentiometer in MUX A */
	AD1CHSCLR = 0xF << 16; // clear bit <19:16>
	AD1CHSSET = analog_pin_nr[pin] << 16; // select A0 in MUX A

	/* Start sampling, wait until conversion is done */
	AD1CON1 |= (0x1 << 1);
	while(!(AD1CON1 & (0x1 << 1)));
	while(!(AD1CON1 & 0x1));

	/* Return sampled value found in buffer */
	return ADC1BUF0;
}


/* 	Brief	: Initialize the ADC peripheral
	Author	: Original code by Axel Isaksson, edited by Rasmus Kallqvist */
void init_adc(void)
{
	/* Set up LED pins as outputs */
	TRISECLR = 0xFF;
	
	/* Enable analog input A0, Port B pin 2 */
	AD1PCFGCLR = ~(0x1 << 2);
	TRISBSET   =  (0x1 << 2);

	/* Enable analog input A1, Port B pin 4 */
	AD1PCFGCLR = ~(0x1 << 4);
	TRISBSET   =  (0x1 << 4);
	
	/* Data format in uint32, 0 - 1023
	Manual sampling, auto conversion when sampling is done
	FORM = 0x4; SSRC = 0x7; CLRASAM = 0x0; ASAM = 0x0; */
	AD1CON1		= (0x4 << 8) | (0x7 << 5);
	AD1CON2 	= 0x0;
	AD1CON3SET 	= 0x1 << 15;

	/* Turn on ADC */
	AD1CON1SET = (0x1 << 15);
}