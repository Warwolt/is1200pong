/*
********************************************************************************
* name	 : 	main.c
* author : 	Rasmus Kallqvist, Michel Bitar (c) 2017
*		   	Original code by Fredrik Lundeval and Axel Isaksson, 2015
* brief  : 	Main function for pong game on chipkit uno32 microcontroller
********************************************************************************
*/

// todo: maybe implement better tim 2 period to get good 30 hz timer

static char test_char = 0x30;
static int timer_counter;

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Function definitions ------------------------------------------------------*/
/* Main */
int main(void)
{
	/* Low level initialization */
	init_mcu();

	/* Initialization */
	led_write(0x3); // signal bootup
	init_tim();
	init_adc();
	init_display();
	enable_interrupt();
	led_write(0x1); // bootup done

	/* Run demos */
	while(1)
	{
		display_print(0,"hello timer test!");
		display_print(1, &test_char);
		display_show_text();
	}

	return 0;
}

/* Low level initialization of microcontroller */
void init_mcu(void)
{
	/* Set up peripheral bus clock */
    /* OSCCONbits.PBDIV = 1; */
    OSCCONCLR = 0x100000; /* clear PBDIV bit 1 */
	OSCCONSET = 0x080000; /* set PBDIV bit 0 */

	/* Enable LED1 through LED8 */
	TRISECLR = 0x00FF;

	/* Configure switches and buttonpins as inputs */
	TRISDSET = 0x0FE0; // Switches 4:1, Buttons 4:2
	TRISFSET = 0x0001; // Button 1

	/* Output pins for display signals */
	PORTF = 0xFFFF;
	PORTG = (1 << 9);
	ODCF = 0x0;
	ODCG = 0x0;
	TRISFCLR = 0x70;
	TRISGCLR = 0x200;

	/* Set up SPI as master */
	SPI2CON = 0;
	SPI2BRG = 4;
	SPI2STATCLR = 0x40; 	/* SPI2STAT bit SPIROV = 0; */
  	SPI2CONSET = 0x40; 		/* SPI2CON bit CKP = 1; */
	SPI2CONSET = 0x20; 		/* SPI2CON bit MSTEN = 1; */
	SPI2CONSET = 0x8000;	/* SPI2CON bit ON = 1; */
}

/* Init timer 2 30Hz interrupts */
void init_tim(void)
{
	/* Configure Timer 2 */
	T2CON     = 0x0;        // reset control register 
	T2CONSET  = 0x70;       // set prescaler to 1:256
	PR2       = TMR2PERIOD; // set period length
	TMR2      = 0;          // reset timer value 
	T2CONSET  = 0x8000;     // start the timer 

	/* Enable Timer 2 interrupts */
	IPC(2)    = 0x4;        // set interrupt priority to 4
	IECSET(0) = 0x100;      // timer 2 interrupt enable 
	IFSCLR(0) = 0x100;      // clear timer 2 interrupt flag
}


/* Dummy function for handling interrupts */
void user_isr(void)
{
 	/* Timer 2 timeout */
  	if(IFS(0) & 0x100) // check interrupt flag  
  	{
  		timer_counter++;
  		if(timer_counter == 30)
  		{
  			test_char += 1;
  			if(test_char == 0x3A)
  				test_char = 0x30;
  			timer_counter = 0; // reset timer counter
  		}
  		
  		IFSCLR(0) = 0x0100; // reset interrupt flag      
  	}		
 
}

/* Turn LED7 to LED0 on or off, bits in write_data specifies LED states */
void led_write(uint8_t write_data)
{
  /* Modify state of LEDs */
  PORTECLR = ~(write_data);	// write zeros of write_data
  PORTESET = (write_data);	// write ones  of write_data
  return;
}
