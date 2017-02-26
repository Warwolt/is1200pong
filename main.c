/*
********************************************************************************
* name	 : 	main.c
* author : 	Rasmus Kallqvist, Michel Bitar (c) 2017
*		   	Original code by Fredrik Lundeval and Axel Isaksson, 2015
* brief  : 	Main function for pong game on chipkit uno32 microcontroller
********************************************************************************
*/
// todo: maybe implement better tim 2 period to get good 30 hz timer

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Local variablrs -----------------------------------------------------------*/
static char test_char = 0x37; 	// (this should be removed when done with test)
static uint32_t timer_counter;	// tracks timer value 
static uint8_t  timeout_flag;	// signals 1/30th second has elapsed
static uint8_t  update_counter; // tracks 30 updates per second

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
		/* Wait for timeout */
		while(!timeout_flag);
		timeout_flag = 0; // rest timeout flag
		update_counter++;
		if(update_counter > 30)
			update_counter = 0;
			
		/* Draw step */
		display_print("30 ups test", 0);
		display_print(&test_char, 1); // timer test 
		display_show_text();

		/* Update step */
		if(update_counter == 30)
		{
			/* Once every 30 updates = 1 second */
			test_char += 1;
  			if(test_char == 0x3A)
  				test_char = 0x30;
		}
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

/* Init timer 2 with timeout interrupts */
void init_tim(void)
{
	/* Configure Timer 2 */
	T2CON     = 0x0;        	// reset control register 
	T2CONSET  = (0x4 << 4);     // set prescaler to 1:16
	PR2       = TMR2PERIOD; 	// set period length
	TMR2      = 0;          	// reset timer value 
	T2CONSET  = (0x1 << 15);    // start the timer 

	/* Enable Timer 2 interrupts */
	IPC(2)    = 0x4;        	// set interrupt priority to 4
	IECSET(0) = 0x1<<8;      	// timer 2 interrupt enable 
	IFSCLR(0) = 0x1<<8;      	// clear timer 2 interrupt flag
}


/* Dummy function for handling interrupts */
void user_isr(void)
{
 	/* Timer 2 timeout */
  	if(IFS(0) & 0x1<<8) // check interrupt flag  
  	{
  		timer_counter++;
  		if(timer_counter == 3225) // value has been trimmed manually for 30 ups 
  		{
  			timeout_flag = 0x1;		// set timeout flag
  			timer_counter = 0x0; 	// reset timer counter
  		}
  		
  		IFSCLR(0) = 0x01<<8; // reset interrupt flag      
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
