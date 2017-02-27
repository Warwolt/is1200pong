/*
********************************************************************************
* name   :  display.c
* author :  Rasmus Kallqvist, 2017-02-21
*           Original code by Fredrik Lundeval and Axel Isaksson, 2015
* brief  :  Contains driver for interfacing with and initialize the chipkit
*           uno32 basic i/o shield oled display. For more information about the
*           display, see reference manual for SSD1306 display controller and
*           reference document for the chipkit i/o shield.
* note   :  In this file, "display" refers to the i/o shield hardware display,
*           and "screen" refers to the software representation of that display.
********************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "display.h"

/* Local variables -----------------------------------------------------------*/
static uint8_t screen_content[128][4];

/* Function definitions ------------------------------------------------------*/
/* Brief  : Sets a single pixel in the byte-representation of the oled display.
 *          The display is comprised of a sequence of 8 pixel high columns.
 *          A given y coordinate bust be divided up into two components; which
 *          byte to write to, and the bit offset within the given byte.
 * Author : Rasmus Kallqvist */
void display_set_pixel(uint8_t x, uint8_t y)
{
    uint8_t bit_offset = y % 8;                 /* Offset is the remainder  */
    uint8_t byte_select = (y - bit_offset) / 8; /* Select is floor function */

    /* Check if valid coordinates */
    if(x < 0 || x > 127 || y < 0 || y > 31)
        return;

    /* Copy to screen */
    screen_content[x][byte_select] |= 0x1 << bit_offset;

    return;
}


/* Brief  : Draws a rectangle with top left corner in (x0,y0) and lower
 *          right corner in (x1, y1). If either corner is out of the screen
 *          boundrary, only part of the rectangle will be drawn.
 * Author : Rasmus Kallqvist */
void display_draw_rect(int8_t x0, int8_t y0, int8_t x1, int8_t y1)
{
    uint8_t width = x1 - x0;
    uint8_t height = y1 - y0;
    uint8_t i, j;

    /* Check that width and height is non-negative */
    if(x1 < x0 || y1 < y0)
        return;

    /* Draw rectangle to screen */
    for(i = x0; (i < (x0+width)) && (i < DISPLAY_WIDTH); i++)
    {
        for(j = y0; (j < (y0+height)) && (j < DISPLAY_HEIGHT); j++)
        {
            display_set_pixel(i,j);
        }
    }
}


/* Brief  : Calls draw rectangle function to draw a rectangle actor struct.
*           Makes writing other stuff much easier on the syntax.
* Author  : Rasmus Kallqvist */
void display_draw_actor(struct actor *a)
{
    int x0 = (int) round(a->x);
    int y0 = (int) round(a->y);
    int x1 = (int) round(a->x + a->w);
    int y1 = (int) round(a->y + a->h);
    display_draw_rect(x0, y0, x1, y1);
}

/* Brief  : Draw right and left doted line
 * Author : Michel Bitar*/
void display_draw_dotline (int x0)
{
  int i;
	for (i = 0; i < 4; i++)
	{
		display_draw_rect(x0,0+(i*8),x0+1,(i*8)+4);
	}
}


/* Brief  : Draw a cosine wave with period and phase determined by arguments.
 *          Period is measured in pixels, the phase in degrees.
 * Author : Rasmus Kallqvist */
void display_draw_cos(uint32_t period, uint32_t phase)
{
    uint8_t x, y;
    double angle = 0.0;

    /* Wrap around negative phase */
    phase = (360 + phase) % 360;

    /* Draw cosine wave to screen */
    for(x = 0; x < 128; x++)
    {
        y = 16 - 16 * cos( (angle  + phase) * PI / 180);
        display_set_pixel(x, y);
        angle += 360/period;
    }
}


/* Brief  : Sets each byte in the screen to zero
 * Author : Rasmus Kallqvist */
void display_cls(void)
{
    uint8_t i, j;
    for(i = 0; i < 4; i++)
    {
        for(j = 0; j < 128; j++)
        {
            screen_content[j][i] = 0x00;
        }
    }
}


/* Driver functions */

/* Brief  : Performs low level initiation of the i/o shield OLED-display
 * Author : Original code by Fredrik Lundeval / Axel Isaksson / Diligent corp
 *          Display command macros and some comments by Rasmus Kallqvist */
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

    /* Enable 7.5 V to display */
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

    /* Clear out graphic RAM */
    display_cls();
    display_update();
}


/* Brief  : Clears the display by writing all zeroes to graphic ram
 * Author : Rasmus Kallqvist */
void clear_display(void)
{
    uint8_t cur_page;
    uint8_t cur_col;

    for(cur_page = 0; cur_page < 4; cur_page++)
    {
        DISPLAY_CHANGE_TO_COMMAND_MODE;
        quicksleep(10);

        /* Set page address */
        spi_send_recv(CMD_SET_PAGE_ADDRESS);
        spi_send_recv(cur_page);

        /* Set cursor at left column */
        spi_send_recv(0x00);
        spi_send_recv(0x10);

        DISPLAY_CHANGE_TO_DATA_MODE;
        quicksleep(10);

        /* Set each column in page to zero */
        for(cur_col = 0; cur_col < 128; cur_col++)
        {
          spi_send_recv(0x0);
        }
    }
}


/* Brief  : Writies the contents of screen_content to the display graphic ram.
 *          To change screen_content contents, use display_set_pixel().
 * Author : Rasmus Kallqvist */
void display_update(void)
{
    uint8_t cur_page;
    uint8_t cur_col;
    uint8_t data_byte;

    /* Display screen graphic contents */
    for(cur_page = 0; cur_page < 4; cur_page++)
    {
        DISPLAY_CHANGE_TO_COMMAND_MODE;
        quicksleep(10);

        /* Set page address */
        spi_send_recv(CMD_SET_PAGE_ADDRESS);
        spi_send_recv(cur_page);

        /* Set cursor at left column */
        spi_send_recv(0x00);
        spi_send_recv(0x10);

        DISPLAY_CHANGE_TO_DATA_MODE;
        quicksleep(10);

        /* Set each column in page to zero */
        for(cur_col = 0; cur_col < 128; cur_col++)
        {
          data_byte = screen_content[cur_col][cur_page];
          spi_send_recv(data_byte);
        }
    }
}


/* Brief  : Print text to screen_content buffer, starting at position (x, y)
 * Author : Rasmus Kallqvist
 *          original code by Fredrik Lundeval / Axel Isaksson */
void display_print(char *s, int x, int y)
{
    int cur_char, cur_slice, cur_col, cur_row;
    int c_printed; /* number of characters printed */
    char textbuff[16];

    /* Check if valid coordinates */
    if(x < 0 || x > (127 - 8) || y < 0 || y > (31 - 8))
        return;

    /* Check if empty string */
    if(!s)
        return;

    /* Copy string contents to text buffer */
    for(cur_char = 0; cur_char < 16; cur_char++)
    {
        if(*s)
        {
            textbuff[cur_char] = *s;
            s++;
        }
        else
        {
            textbuff[cur_char] = ' ';
        }
    }

    /* Copy string to screen buffer */
    for(c_printed = 0; c_printed < 16; c_printed++)
    {
        /* Get next ascii character to print */
        cur_char = textbuff[c_printed];

        /* Copy bitmap data to screen buffer */
        for(cur_col = 0; cur_col < 8; cur_col++)
        {
            /* Get 8 pixel high slice from character bitmap */
            cur_slice = font[cur_char*8 + cur_col];

            /* Copy current 8 pixel slice to screen buffer */
            for(cur_row = 0; cur_row < 8; cur_row++)
            {
                /* If pixel in slice is set, then set pixel in buffer */
                if( (cur_slice >> cur_row) & 0x1 )
                    display_set_pixel(x+8*c_printed+cur_col, y+cur_row);
            }
        }
    }
}



/* Helper functions ----------------------------------------------------------*/
/* Brief  : Simple function to create a short delay. Very inefficient use of
 *          computing resources, but very handy in some special cases.
 * Author : Fredrik Lundeval / Axel Isaksson */
void quicksleep(int cyc)
{
    int i;
    for(i = cyc; i > 0; i--);
}


/* Brief  : Send/receive one byte over spi connected to the i/o shield display
 * Author : Fredrik Lundeval / Axel Isaksson */
uint8_t spi_send_recv(uint8_t data)
{
    while(!(SPI2STAT & 0x08));
    SPI2BUF = data;
    while(!(SPI2STAT & 1));
    return SPI2BUF;
}

/* Brief  : Function to help debugging.
   Author : Fredrik Lundeval / Axel Isaksson
            Modified by Rasmus Kallqvist

   Note: When you use this function, you should comment out any
   repeated calls to display_image; display_image overwrites
   about half of the digits shown by display_debug.
*/
void display_debug(volatile int * const addr)
{
    char textbuffer[2][16];
    num32asc( &textbuffer[0][6], (int) addr );
    num32asc( &textbuffer[1][6], *addr );

    display_cls();
    display_print("Addr", 0,0);
    display_print("Data", 0,8);
    display_print(textbuffer[0], 48, 0);
    display_print(textbuffer[1], 48, 8);
    display_update();
}

/* Brief  : converts a number to hexadecimal ASCII digits.
   Author : Fredrik Lundeval / Axel Isaksson */
void num32asc(char * s, int n)
{
  int i;
  for( i = 28; i >= 0; i -= 4 )
    *s++ = "0123456789ABCDEF"[ (n >> i) & 15 ];
}

/* Brief  : converts a number to hexadecimal ASCII digits.
 * Author : Rasmus Kallqvist
 *          orignal code by Fredrik Lundeval / Axel Isaksson */
char int2char(int n)
{
  return "0123456789ABCDEF"[n & 0xF];
}