/*
********************************************************************************
* name   :  display.c
* author :  Rasmus Kallqvist, 2017-02-21
*           Original code by Fredrik Lundeval and Axel Isaksson, 2015
* brief  :  Contains driver for interfacing with and initialize the chipkit 
*           uno32 basic i/o shield oled display. For more information about the 
*           display, see reference manual for SSD1306 display controller and
*           reference document for the chipkit i/o shield.
********************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "display.h"

/* Local variables -----------------------------------------------------------*/
static uint8_t screen_status[128][4];

/* Function definitions ------------------------------------------------------*/
/* Brief  : Sets a single pixel in the byte-representation of the oled display. 
 *          The display is comprised of a sequency of 8 pixel high columns, so
 *          we have to translate a given y coordinate to two components, the 
 *          byte to write to and then the offset within a given byte.
 * Author : Rasmus Kallqvist */
void display_set_pixel(uint8_t x, uint8_t y)
{
  uint8_t bit_offset = y % 8;
  uint8_t byte_select = (y - bit_offset) / 8;

  screen_status[x][byte_select] |= 0x1 << bit_offset;

  return;
}

/* Brief  : Draw a cosine with period and phase determined by arguments 
 * Author : Rasmus Kallqvist */
void display_draw_cos(uint32_t period, uint32_t phase)
{
  uint8_t x, y;
  double angle = 0.0;
  for(x = 0; x < 128; x++)
  {
    y = 16 - 16 * cos(angle * PI / 180 + phase);
    display_set_pixel(x, y);
    angle += 360/period;
  }
}

void display_clear_screen(void)
{
  uint8_t i, j;
  for(i = 0; i < 4; i++)
  {
    for(j = 0; j < 128; j++)
    {
      screen_status[j][i] = 0x00;
    }
  }
}

/* Brief  : Performs low level initiation of the i/o shield OLED-display 
 * Author : Original code by Fredrik Lundeval / Axel Isaksson (note: which one?)
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


/* Brief  : Writies the contents of screen_status to the display graphic ram. 
 *          To change screen_status contents, use display_set_pixel(). 
 * Author : Rasmus Kallqvist */
void update_display(void)
{
  uint8_t cur_page;
  uint8_t cur_col;
  uint8_t data_byte;

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
      data_byte = screen_status[cur_col][cur_page]; 
      spi_send_recv(data_byte);
    }
  } 
}


/* Brief  : a simple function to create a small delay. Very inefficient use of 
 *          computing resources, but very handy in some special cases.
 * Author : Fredrik Lundeval / Axel Isaksson */
void quicksleep(int cyc) 
{
  int i;
  for(i = cyc; i > 0; i--);
}

/* Brief  : send/receive one byte over spi connected to the i/o shield display
 * Author : Fredrik Lundeval / Axel Isaksson */
uint8_t spi_send_recv(uint8_t data) 
{
  while(!(SPI2STAT & 0x08));
  SPI2BUF = data;
  while(!(SPI2STAT & 1));
  return SPI2BUF;
}