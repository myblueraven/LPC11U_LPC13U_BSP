/**************************************************************************/
/*! 
    @file     ST7735.h
    @author   K. Townsend (microBuilder.eu)

    @section LICENSE

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
#ifndef __ST7735_H__
#define __ST7735_H__

#include "bspconfig.h"
#include "drivers/displays/graphic/lcd.h"

#define ST7735_PANEL_WIDTH  128
#define ST7735_PANEL_HEIGHT 160

/**************************************************************************
    ST7735 CONNECTOR - HY-1.8
    -----------------------------------------------------------------------
    Pin   Function        Notes
    ===   ==============  ===============================
      1   GND
      2   VCC             3.3V
      3   NC
      4   NC
      5   NC
      6   RESET           Hardware Reset
      7   A0              Data/Command Select (1-data, 0-command)
      8   SDA             Serial Data
      9   SCK             Serial Clock
     10   CS              Chip Select
     11   SCK   --------+
     12   MISO          | SD-Card slot
     13   MOSI          |
     14   CS    --------+
     15   LED+
     16   LED-

 **************************************************************************/

// Comment this line out if RST on the LCD is tied to RST on the MCU
#define ST7735_USERESET

// Control pins
#define ST7735_DATA_PORT       (0)
#define ST7735_RS_PIN          (16)
#define ST7735_SDA_PIN         (23)
#define ST7735_SCL_PIN         (17)
#define ST7735_CS_PIN          (19)

#define ST7735_CTRL_PORT       (1)
#define ST7735_BL_PIN          (16)
#ifdef ST7735_USERESET
	#define ST7735_RES_PIN         (22)
#endif

// Macros for control line state
#define CLR_RS      do { LPC_GPIO->CLR[ST7735_DATA_PORT] = (1 << ST7735_RS_PIN); } while(0)
#define SET_RS      do { LPC_GPIO->SET[ST7735_DATA_PORT] = (1 << ST7735_RS_PIN); } while(0)
#define CLR_SDA     do { LPC_GPIO->CLR[ST7735_DATA_PORT] = (1 << ST7735_SDA_PIN); } while(0)
#define SET_SDA     do { LPC_GPIO->SET[ST7735_DATA_PORT] = (1 << ST7735_SDA_PIN); } while(0)
#define CLR_SCL     do { LPC_GPIO->CLR[ST7735_DATA_PORT] = (1 << ST7735_SCL_PIN); } while(0)
#define SET_SCL     do { LPC_GPIO->SET[ST7735_DATA_PORT] = (1 << ST7735_SCL_PIN); } while(0)
#define CLR_CS      do { LPC_GPIO->CLR[ST7735_DATA_PORT] = (1 << ST7735_CS_PIN); } while(0)
#define SET_CS      do { LPC_GPIO->SET[ST7735_DATA_PORT] = (1 << ST7735_CS_PIN); } while(0)

#define CLR_BL      do { LPC_GPIO->CLR[ST7735_CTRL_PORT] = (1 << ST7735_BL_PIN); } while(0)
#define SET_BL      do { LPC_GPIO->SET[ST7735_CTRL_PORT] = (1 << ST7735_BL_PIN); } while(0)
#ifdef ST7735_USERESET
  #define CLR_RES   do { LPC_GPIO->CLR[ST7735_CTRL_PORT] = (1 << ST7735_RES_PIN); } while(0)
  #define SET_RES   do { LPC_GPIO->SET[ST7735_CTRL_PORT] = (1 << ST7735_RES_PIN); } while(0)
#endif

// System control functions
#define ST7735_NOP       (0x0)
#define ST7735_SWRESET   (0x01)
#define ST7735_RDDID     (0x04)
#define ST7735_RDDST     (0x09)
#define ST7735_RDDPM     (0x0A)
#define ST7735_RDDMADCTL (0x0B)
#define ST7735_RDDCOLMOD (0x0C)
#define ST7735_RDDIM     (0x0D)
#define ST7735_RDDSM     (0x0E)
#define ST7735_SLPIN     (0x10)
#define ST7735_SLPOUT    (0x11)
#define ST7735_PTLON     (0x12)
#define ST7735_NORON     (0x13)
#define ST7735_INVOFF    (0x20)
#define ST7735_INVON     (0x21)
#define ST7735_GAMSET    (0x26)
#define ST7735_DISPOFF   (0x28)
#define ST7735_DISPON    (0x29)
#define ST7735_CASET     (0x2A)
#define ST7735_RASET     (0x2B)
#define ST7735_RAMWR     (0x2C)
#define ST7735_RAMRD     (0x2E)
#define ST7735_PTLAR     (0x30)
#define ST7735_TEOFF     (0x34)
#define ST7735_TEON      (0x35)
#define ST7735_MADCTL    (0x36)
#define ST7735_IDMOFF    (0x38)
#define ST7735_IDMON     (0x39)
#define ST7735_COLMOD    (0x3A)
#define ST7735_RDID1     (0xDA)
#define ST7735_RDID2     (0xDB)
#define ST7735_RDID3     (0xDC)

// Panel control functions
#define ST7735_FRMCTR1   (0xB1)
#define ST7735_FRMCTR2   (0xB2)
#define ST7735_FRMCTR3   (0xB3)
#define ST7735_INVCTR    (0xB4)
#define ST7735_DISSET5   (0xB6)
#define ST7735_PWCTR1    (0xC0)
#define ST7735_PWCTR2    (0xC1)
#define ST7735_PWCTR3    (0xC2)
#define ST7735_PWCTR4    (0xC3)
#define ST7735_PWCTR5    (0xC4)
#define ST7735_VMCTR1    (0xC5)
#define ST7735_VMOFCTR   (0xC7)
#define ST7735_WRID2     (0xD1)
#define ST7735_WRID3     (0xD2)
#define ST7735_PWCTR6    (0xFC)
#define ST7735_NVFCTR1   (0xD9)
#define ST7735_NVFCTR2   (0xDE)
#define ST7735_NVFCTR3   (0xDF)

#define ST7735_GMCTRP1   (0xE0)
#define ST7735_GMCTRN1   (0xE1)
#define ST7735_EXTCTRL   (0xF0)
#define ST7735_VCOM4L    (0xFF)

// Color definitions
#define ST7735_BLACK     0x0000
#define ST7735_BLUE      0x001F
#define ST7735_RED       0xF800
#define ST7735_GREEN     0x07E0
#define ST7735_CYAN      0x07FF
#define ST7735_MAGENTA   0xF81F
#define ST7735_YELLOW    0xFFE0
#define ST7735_WHITE     0xFFFF


#endif
