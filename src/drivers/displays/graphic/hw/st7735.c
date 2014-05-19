/**************************************************************************/
/*! 
    @file     st7735.c
    @author   K. Townsend (microBuilder.eu)

    @section  DESCRIPTION

    Driver for st7735 128x160 pixel TFT LCD displays.
    
    This driver uses a bit-banged SPI interface and a 16-bit RGB565
    colour palette.

    @section  LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2010, microBuilder SARL
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**************************************************************************/
#include "st7735.h"

#ifdef CFG_BSP_LCD_ST7735

#include "core/delay/delay.h"
#include "core/gpio/gpio.h"

static lcdOrientation_t lcdOrientation = LCD_ORIENTATION_PORTRAIT;
static lcdProperties_t st7735Properties = { ST7735_PANEL_WIDTH, ST7735_PANEL_HEIGHT, false, true, false, true, true };

/*************************************************/
/* Private Methods                               */
/*************************************************/

/*************************************************/
void st7735WriteCmd(uint8_t command) 
{
  CLR_CS;
  CLR_RS;
  uint8_t i = 0;
  for (i=0; i<8; i++) 
  { 
    if (command & 0x80) 
    { 
      SET_SDA;   
    } 
    else 
    { 
      CLR_SDA; 
    } 
    CLR_SCL; 
    command <<= 1; 
    SET_SCL;
  } 
  SET_CS; 
}

/*************************************************/
void st7735WriteData(uint8_t data)
{
  CLR_CS;
  SET_RS; 
  uint8_t i = 0;
  for (i=0; i<8; i++) 
  { 
    if (data & 0x80) 
    { 
      SET_SDA;
    } 
    else 
    { 
      CLR_SDA;
    } 
    CLR_SCL;
    data <<= 1; 
    SET_SCL;    
  } 
  SET_CS;
}

/*************************************************/
void st7735SetAddrWindowB(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
  st7735WriteCmd(ST7735_CASET);   // column addr set
  st7735WriteData(0x00);
  st7735WriteData(x0+2);          // XSTART 
  st7735WriteData(0x00);
  st7735WriteData(x1+2);          // XEND

  st7735WriteCmd(ST7735_RASET);   // row addr set
  st7735WriteData(0x00);
  st7735WriteData(y0+1);          // YSTART
  st7735WriteData(0x00);
  st7735WriteData(y1+1);          // YEND
}

/*************************************************/
void st7735SetAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
  st7735WriteCmd(ST7735_CASET);   // column addr set
  st7735WriteData(0x00);
  st7735WriteData(x0);          // XSTART
  st7735WriteData(0x00);
  st7735WriteData(x1);          // XEND

  st7735WriteCmd(ST7735_RASET);   // row addr set
  st7735WriteData(0x00);
  st7735WriteData(y0);          // YSTART
  st7735WriteData(0x00);
  st7735WriteData(y1);          // YEND
}

/*************************************************/
// https://github.com/adafruit/Adafruit-ST7735-Library/blob/master/Adafruit_ST7735.cpp
void st7735InitDisplayB(void)
{
  st7735WriteCmd(ST7735_SWRESET); // software reset
  delay(50);
  st7735WriteCmd(ST7735_SLPOUT);  // out of sleep mode
  delay(500);
  
  st7735WriteCmd(ST7735_COLMOD);  // set color mode
  st7735WriteData(0x05);          // 16-bit color
  delay(10);
  
  st7735WriteCmd(ST7735_FRMCTR1); // frame rate control
  st7735WriteData(0x00);          // fastest refresh
  st7735WriteData(0x06);          // 6 lines front porch
  st7735WriteData(0x03);          // 3 lines backporch
  delay(10);
  
  st7735WriteCmd(ST7735_MADCTL);  // memory access control (directions)
  st7735WriteData(0x08);          // row address/col address, bottom to top refresh, BGR, right to left refresh
  
  st7735WriteCmd(ST7735_DISSET5); // display settings #5
  st7735WriteData(0x15);          // 1 clock cycle nonoverlap, 2 cycle gate rise, 3 cycle oscil. equalize
  st7735WriteData(0x02);          // fix on VTL
 
  st7735WriteCmd(ST7735_INVCTR);  // display inversion control
  st7735WriteData(0x0);           // line inversion
 
  st7735WriteCmd(ST7735_PWCTR1);  // power control
  st7735WriteData(0x02);          // GVDD = 4.7V 
  st7735WriteData(0x70);          // 1.0uA
  delay(10);
  st7735WriteCmd(ST7735_PWCTR2);  // power control
  st7735WriteData(0x05);          // VGH = 14.7V, VGL = -7.35V 
  st7735WriteCmd(ST7735_PWCTR3);  // power control
  st7735WriteData(0x01);          // Opamp current small 
  st7735WriteData(0x02);          // Boost frequency
  
  
  st7735WriteCmd(ST7735_VMCTR1);  // power control
  st7735WriteData(0x3C);          // VCOMH = 4V
  st7735WriteData(0x38);          // VCOML = -1.1V
  delay(10);
  
  st7735WriteCmd(ST7735_PWCTR6);  // power control
  st7735WriteData(0x11); 
  st7735WriteData(0x15);
  
  st7735WriteCmd(ST7735_GMCTRP1);
  st7735WriteData(0x09);
  st7735WriteData(0x16);
  st7735WriteData(0x09);
  st7735WriteData(0x20);
  st7735WriteData(0x21);
  st7735WriteData(0x1B);
  st7735WriteData(0x13);
  st7735WriteData(0x19);
  st7735WriteData(0x17);
  st7735WriteData(0x15);
  st7735WriteData(0x1E);
  st7735WriteData(0x2B);
  st7735WriteData(0x04);
  st7735WriteData(0x05);
  st7735WriteData(0x02);
  st7735WriteData(0x0E);
  st7735WriteCmd(ST7735_GMCTRN1);
  st7735WriteData(0x0B); 
  st7735WriteData(0x14); 
  st7735WriteData(0x08); 
  st7735WriteData(0x1E); 
  st7735WriteData(0x22); 
  st7735WriteData(0x1D); 
  st7735WriteData(0x18); 
  st7735WriteData(0x1E); 
  st7735WriteData(0x1B); 
  st7735WriteData(0x1A); 
  st7735WriteData(0x24); 
  st7735WriteData(0x2B); 
  st7735WriteData(0x06); 
  st7735WriteData(0x06); 
  st7735WriteData(0x02); 
  st7735WriteData(0x0F); 
  delay(10);
  
  st7735WriteCmd(ST7735_CASET);   // column addr set
  st7735WriteData(0x00);
  st7735WriteData(0x02);          // XSTART = 2
  st7735WriteData(0x00);
  st7735WriteData(0x81);          // XEND = 129

  st7735WriteCmd(ST7735_RASET);   // row addr set
  st7735WriteData(0x00);
  st7735WriteData(0x02);          // XSTART = 1
  st7735WriteData(0x00);
  st7735WriteData(0x81);          // XEND = 160

  st7735WriteCmd(ST7735_NORON);   // normal display on
  delay(10);
  
  st7735WriteCmd(ST7735_DISPON);
  delay(500);
}

/*************************************************/
void st7735InitDisplayR(void)
{
  st7735WriteCmd(ST7735_SWRESET); // software reset
  delay(150);
  st7735WriteCmd(ST7735_SLPOUT);  // out of sleep mode
  delay(500);

  st7735WriteCmd(ST7735_FRMCTR1); // frame rate control
  st7735WriteData(0x01);          // fastest refresh
  st7735WriteData(0x2C);          // 6 lines front porch
  st7735WriteData(0x2D);          // 3 lines backporch

  st7735WriteCmd(ST7735_FRMCTR2); // frame rate control
  st7735WriteData(0x01);          // fastest refresh
  st7735WriteData(0x2C);          // 6 lines front porch
  st7735WriteData(0x2D);          // 3 lines backporch

  st7735WriteCmd(ST7735_FRMCTR3); // frame rate control
  st7735WriteData(0x01);          // fastest refresh
  st7735WriteData(0x2C);          // 6 lines front porch
  st7735WriteData(0x2D);          // 3 lines backporch
  st7735WriteData(0x01);          // fastest refresh
  st7735WriteData(0x2C);          // 6 lines front porch
  st7735WriteData(0x2D);          // 3 lines backporch

  st7735WriteCmd(ST7735_INVCTR);  // display inversion control
  st7735WriteData(0x07);           // line inversion

  st7735WriteCmd(ST7735_PWCTR1);  // power control
  st7735WriteData(0xA2);          // GVDD = 4.7V
  st7735WriteData(0x02);          // 1.0uA
  st7735WriteData(0x84);          // 1.0uA
  st7735WriteCmd(ST7735_PWCTR2);  // power control
  st7735WriteData(0xC5);          // VGH = 14.7V, VGL = -7.35V
  st7735WriteCmd(ST7735_PWCTR3);  // power control
  st7735WriteData(0x0A);          // Opamp current small
  st7735WriteData(0x00);          // Boost frequency
  st7735WriteCmd(ST7735_PWCTR4);  // power control
  st7735WriteData(0x8A);
  st7735WriteData(0x2A);
  st7735WriteCmd(ST7735_PWCTR5);  // power control
  st7735WriteData(0x8A);
  st7735WriteData(0xEE);

  st7735WriteCmd(ST7735_VMCTR1);  // power control
  st7735WriteData(0x0E);          // VCOMH = 4V

  st7735WriteCmd(ST7735_INVOFF);

  st7735WriteCmd(ST7735_MADCTL);  // memory access control (directions)
//  st7735WriteData(0xC8);          // row address/col address, bottom to top refresh, BGR, right to left refresh
  st7735WriteData(0xC0);          // row address/col address, bottom to top refresh, RGB, right to left refresh

  st7735WriteCmd(ST7735_COLMOD);  // set color mode
  st7735WriteData(0x05);          // 16-bit color

//============= RED ======================
  st7735WriteCmd(ST7735_CASET);   // column addr set
  st7735WriteData(0x00);
  st7735WriteData(0x00);          // XSTART = 0
  st7735WriteData(0x00);
  st7735WriteData(0x7F);          // XEND = 127

  st7735WriteCmd(ST7735_RASET);   // row addr set
  st7735WriteData(0x00);
  st7735WriteData(0x00);          // YSTART = 0
  st7735WriteData(0x00);
  st7735WriteData(0x9F);          // YEND = 159
//========================================

  st7735WriteCmd(ST7735_GMCTRP1);
  st7735WriteData(0x02);
  st7735WriteData(0x1C);
  st7735WriteData(0x07);
  st7735WriteData(0x12);
  st7735WriteData(0x37);
  st7735WriteData(0x32);
  st7735WriteData(0x29);
  st7735WriteData(0x2D);
  st7735WriteData(0x29);
  st7735WriteData(0x25);
  st7735WriteData(0x2B);
  st7735WriteData(0x39);
  st7735WriteData(0x00);
  st7735WriteData(0x01);
  st7735WriteData(0x03);
  st7735WriteData(0x10);
  st7735WriteCmd(ST7735_GMCTRN1);
  st7735WriteData(0x03);
  st7735WriteData(0x1D);
  st7735WriteData(0x07);
  st7735WriteData(0x06);
  st7735WriteData(0x2E);
  st7735WriteData(0x2C);
  st7735WriteData(0x29);
  st7735WriteData(0x2D);
  st7735WriteData(0x2E);
  st7735WriteData(0x2E);
  st7735WriteData(0x37);
  st7735WriteData(0x3F);
  st7735WriteData(0x00);
  st7735WriteData(0x00);
  st7735WriteData(0x02);
  st7735WriteData(0x10);

  st7735WriteCmd(ST7735_NORON);   // normal display on
  delay(10);

  st7735WriteCmd(ST7735_DISPON);
  delay(500);
}

/*************************************************/
void st7735InitDisplayG(void)
{
	  st7735WriteCmd(ST7735_SWRESET); // software reset
	  delay(150);
	  st7735WriteCmd(ST7735_SLPOUT);  // out of sleep mode
	  delay(500);

	  st7735WriteCmd(ST7735_FRMCTR1); // frame rate control
	  st7735WriteData(0x01);          // fastest refresh
	  st7735WriteData(0x2C);          // 6 lines front porch
	  st7735WriteData(0x2D);          // 3 lines backporch

	  st7735WriteCmd(ST7735_FRMCTR2); // frame rate control
	  st7735WriteData(0x01);          // fastest refresh
	  st7735WriteData(0x2C);          // 6 lines front porch
	  st7735WriteData(0x2D);          // 3 lines backporch

	  st7735WriteCmd(ST7735_FRMCTR3); // frame rate control
	  st7735WriteData(0x01);          // fastest refresh
	  st7735WriteData(0x2C);          // 6 lines front porch
	  st7735WriteData(0x2D);          // 3 lines backporch
	  st7735WriteData(0x01);          // fastest refresh
	  st7735WriteData(0x2C);          // 6 lines front porch
	  st7735WriteData(0x2D);          // 3 lines backporch

	  st7735WriteCmd(ST7735_INVCTR);  // display inversion control
	  st7735WriteData(0x07);           // line inversion

	  st7735WriteCmd(ST7735_PWCTR1);  // power control
	  st7735WriteData(0xA2);          // GVDD = 4.7V
	  st7735WriteData(0x02);          // 1.0uA
	  st7735WriteData(0x84);          // 1.0uA
	  st7735WriteCmd(ST7735_PWCTR2);  // power control
	  st7735WriteData(0xC5);          // VGH = 14.7V, VGL = -7.35V
	  st7735WriteCmd(ST7735_PWCTR3);  // power control
	  st7735WriteData(0x0A);          // Opamp current small
	  st7735WriteData(0x00);          // Boost frequency
	  st7735WriteCmd(ST7735_PWCTR4);  // power control
	  st7735WriteData(0x8A);
	  st7735WriteData(0x2A);
	  st7735WriteCmd(ST7735_PWCTR5);  // power control
	  st7735WriteData(0x8A);
	  st7735WriteData(0xEE);

	  st7735WriteCmd(ST7735_VMCTR1);  // power control
	  st7735WriteData(0x0E);          // VCOMH = 4V

	  st7735WriteCmd(ST7735_INVOFF);

	  st7735WriteCmd(ST7735_MADCTL);  // memory access control (directions)
	  st7735WriteData(0xC8);          // row address/col address, bottom to top refresh, BGR, right to left refresh

	  st7735WriteCmd(ST7735_COLMOD);  // set color mode
	  st7735WriteData(0x05);          // 16-bit color

	//============= GREEN ====================
	  st7735WriteCmd(ST7735_CASET);   // column addr set
	  st7735WriteData(0x00);
	  st7735WriteData(0x02);          // XSTART = 2
	  st7735WriteData(0x00);
	  st7735WriteData(0x7F+0X02);          // XEND = 129

	  st7735WriteCmd(ST7735_RASET);   // row addr set
	  st7735WriteData(0x00);
	  st7735WriteData(0x01);          // XSTART = 1
	  st7735WriteData(0x00);
	  st7735WriteData(0x9F+0X01);          // XEND = 160
	//========================================

	  st7735WriteCmd(ST7735_GMCTRP1);
	  st7735WriteData(0x02);
	  st7735WriteData(0x1C);
	  st7735WriteData(0x07);
	  st7735WriteData(0x12);
	  st7735WriteData(0x37);
	  st7735WriteData(0x32);
	  st7735WriteData(0x29);
	  st7735WriteData(0x2D);
	  st7735WriteData(0x29);
	  st7735WriteData(0x25);
	  st7735WriteData(0x2B);
	  st7735WriteData(0x39);
	  st7735WriteData(0x00);
	  st7735WriteData(0x01);
	  st7735WriteData(0x03);
	  st7735WriteData(0x10);
	  st7735WriteCmd(ST7735_GMCTRN1);
	  st7735WriteData(0x03);
	  st7735WriteData(0x1D);
	  st7735WriteData(0x07);
	  st7735WriteData(0x06);
	  st7735WriteData(0x2E);
	  st7735WriteData(0x2C);
	  st7735WriteData(0x29);
	  st7735WriteData(0x2D);
	  st7735WriteData(0x2E);
	  st7735WriteData(0x2E);
	  st7735WriteData(0x37);
	  st7735WriteData(0x3F);
	  st7735WriteData(0x00);
	  st7735WriteData(0x00);
	  st7735WriteData(0x02);
	  st7735WriteData(0x10);

	  st7735WriteCmd(ST7735_NORON);   // normal display on
	  delay(10);

	  st7735WriteCmd(ST7735_DISPON);
	  delay(500);
}

/*************************************************/
/* Public Methods                                */
/*************************************************/

/*************************************************/
void lcdInit(void)
{
  // Set control pins to output
  gpioSetDir(ST7735_DATA_PORT, ST7735_RS_PIN, 1);
  gpioSetDir(ST7735_DATA_PORT, ST7735_SDA_PIN, 1);
  gpioSetDir(ST7735_DATA_PORT, ST7735_SCL_PIN, 1);
  gpioSetDir(ST7735_DATA_PORT, ST7735_CS_PIN, 1);

  gpioSetDir(ST7735_CTRL_PORT, ST7735_BL_PIN, 1);
#ifdef ST7735_USERESET
  gpioSetDir(ST7735_CTRL_PORT, ST7735_RES_PIN, 1);
#endif

  // Set pins low by default (except reset)
  CLR_RS;
  CLR_SDA;
  CLR_SCL;
  CLR_CS;
  CLR_BL;
#ifdef ST7735_USERESET
  SET_RES;
#endif

  // Turn backlight on
  lcdBacklight(TRUE);

  // Reset display
#ifdef ST7735_USERESET
  SET_RES;
  delay(50);
  CLR_RES;
  delay(50);
  SET_RES;
  delay(50);
#else
#endif

  // Run LCD init sequence
//  st7735InitDisplayB();
  st7735InitDisplayR();
//  st7735InitDisplayG();

  lcdSetOrientation(LCD_ORIENTATION_PORTRAIT);

  // Fill black
  lcdFillRGB(COLOR_BLACK);
//  lcdFillRGB(ST7735_GREEN);
}

/*************************************************/
void lcdBacklight(bool state)
{
  // Set the backlight
  // Note: Depending on the type of transistor used
  // to control the backlight, you made need to invert
  // the values below
  if (state)
    CLR_BL;
    // SET_BL;
  else
    SET_BL;
    // CLR_BL;
}

/*************************************************/
void lcdTest(void)
{
  uint8_t i = 0;
  for (i = 0; i < 100; i++)
  {
    lcdDrawPixel(i, i, 0xFFFF);
  }
}

/*************************************************/
void lcdFillRGB(uint16_t color)
{
  uint8_t x, y;
  st7735SetAddrWindow(0, 0, lcdGetWidth() - 1, lcdGetHeight() - 1);
  st7735WriteCmd(ST7735_RAMWR);  // write to RAM
  for (x=0; x < lcdGetWidth(); x++) 
  {
    for (y=0; y < lcdGetHeight(); y++) 
    {
      st7735WriteData(color >> 8);    
      st7735WriteData(color);    
    }
  }
  st7735WriteCmd(ST7735_NOP);
}

/*************************************************/
void lcdDrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
  st7735SetAddrWindow(x,y,x+1,y+1);
  st7735WriteCmd(ST7735_RAMWR);  // write to RAM
  st7735WriteData(color >> 8);  
  st7735WriteData(color);
}

/**************************************************************************/
/*! 
    @brief  Draws an array of consecutive RGB565 pixels (much
            faster than addressing each pixel individually)
*/
/**************************************************************************/
void lcdDrawPixels(uint16_t x, uint16_t y, uint16_t *data, uint32_t len)
{
  // ToDo: Optimise this function ... currently only a placeholder
  uint32_t i = 0;
  do
  {
    lcdDrawPixel(x+i, y, data[i]);
    i++;
  } while (i<len);
}

/*************************************************/
void lcdDrawHLine(uint16_t x0, uint16_t x1, uint16_t y, uint16_t color)
{
  // Allows for slightly better performance than setting individual pixels
  uint16_t x, pixels;

  if (x1 < x0)
  {
    // Switch x1 and x0
    x = x1;
    x1 = x0;
    x0 = x;
  }

  // Check limits
  if (x1 >= lcdGetWidth())
  {
    x1 = lcdGetWidth() - 1;
  }
  if (x0 >= lcdGetWidth())
  {
    x0 = lcdGetWidth() - 1;
  }

  st7735SetAddrWindow(x0, y, lcdGetWidth(), y + 1);
  st7735WriteCmd(ST7735_RAMWR);  // write to RAM
  for (pixels = 0; pixels < x1 - x0 + 1; pixels++)
  {
    st7735WriteData(color >> 8);  
    st7735WriteData(color);
  }
  st7735WriteCmd(ST7735_NOP);
}

/*************************************************/
void lcdDrawVLine(uint16_t x, uint16_t y0, uint16_t y1, uint16_t color)
{
  // Allows for slightly better performance than setting individual pixels
  uint16_t y, pixels;

  if (y1 < y0)
  {
    // Switch y1 and y0
    y = y1;
    y1 = y0;
    y0 = y;
  }

  // Check limits
  if (y1 >= lcdGetHeight())
  {
    y1 = lcdGetHeight() - 1;
  }
  if (y0 >= lcdGetHeight())
  {
    y0 = lcdGetHeight() - 1;
  }

  st7735SetAddrWindow(x, y0, x, lcdGetHeight());
  st7735WriteCmd(ST7735_RAMWR);  // write to RAM
  for (pixels = 0; pixels < y1 - y0 + 1; pixels++)
  {
    st7735WriteData(color >> 8);  
    st7735WriteData(color);
  }
  st7735WriteCmd(ST7735_NOP);
}

/*************************************************/
uint16_t lcdGetPixel(uint16_t x, uint16_t y)
{
  // ToDo
  return 0;
}

/*************************************************/
void lcdSetOrientation(lcdOrientation_t orientation)
{
  if(orientation == LCD_ORIENTATION_PORTRAIT)
  {
	  st7735WriteCmd(ST7735_MADCTL);  // Memory Data Access Control
	  st7735WriteData(0x00);          // 000 - Normal
	                                  // 0 - Vertical refresh top to bottom
	                                  // 0 - RGB order
	                                  // 0 - Horizontal refresh left to right
	  st7735Properties.width = ST7735_PANEL_WIDTH;
	  st7735Properties.height = ST7735_PANEL_HEIGHT;
  }
  else if (orientation == LCD_ORIENTATION_LANDSCAPE)
  {
	  st7735WriteCmd(ST7735_MADCTL);  // Memory Data Access Control
	  st7735WriteData(0xA0);          // 101 - X-Y Exchange, Y-Mirror
	                                  // 0 - Vertical refresh top to bottom
	                                  // 0 - RGB order
	                                  // 0 - Horizontal refresh left to right
	  st7735Properties.width = ST7735_PANEL_HEIGHT;
	  st7735Properties.height = ST7735_PANEL_WIDTH;
  }
}

/*************************************************/
lcdOrientation_t lcdGetOrientation(void)
{
  return lcdOrientation;
}

/*************************************************/
uint16_t lcdGetWidth(void)
{
  return st7735Properties.width;
}

/*************************************************/
uint16_t lcdGetHeight(void)
{
  return st7735Properties.height;
}

/*************************************************/
void lcdScroll(int16_t pixels, uint16_t fillColor)
{
  // ToDo
}

/*************************************************/
uint16_t lcdGetControllerID(void)
{
  return 0x7735;
}

/*************************************************/
lcdProperties_t lcdGetProperties(void)
{
  return st7735Properties;
}

#endif
