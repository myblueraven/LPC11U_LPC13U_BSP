/*******************************************************************
    Copyright (C) 2009 FreakLabs
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. Neither the name of the the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.

    Originally written by Christopher Wang aka Akiba.
    Please post support questions to the FreakLabs forum.
*******************************************************************/

/**************************************************************************/
/*!
    @file     cli.c
    @author   Christopher Wang (Freaklabs)
              Modified by: K. Townsend (microBuilder.eu)

    @ingroup  CLI

    Original code taken from the FreakUSB Open Source USB Device Stack
    http://freaklabs.org/index.php/FreakUSB-Open-Source-USB-Device-Stack.html

    If it works well, you can thank Akiba at Freaklabs.  If it fails
    miserably, you can blame me (since parts of it it were rather
    ungraciously modified). :-)

*/
/**************************************************************************/

#include "bspconfig.h"

#ifdef CFG_BSP_CMDUART

#include <stdio.h>
#include <string.h>

#include "cmduart.h"
#include "cmduart_print.h"

#include "core/uart/uart.h"

#if CFG_BSP_CMDUART_ENABLEIRQ == 1
  #include "core/gpio/gpio.h"
#endif

#define KEY_CODE_ESC        (27)    /* Escape key code */
#define KEY_CODE_ENTER      (13)    /* Enter key code  */

#define CFG_BSP_CMDUART_SHORTERRORS_TOOFEWARGS "Too few arguments"
#define CFG_BSP_CMDUART_SHORTERRORS_TOOMANYARGS "Too many arguments"
#define CFG_BSP_CMDUART_SHORTERRORS_UNKNOWNCOMMAND "Unknown command"
#define CFG_BSP_CMDUART_SHORTERRORS_CHECKSUMMISSING "Missing checksum"

static uint8_t cmduart_buffer[CFG_BSP_CMDUART_MAXMSGSIZE];
static uint8_t *cmduart_buffer_ptr;
bool cmduart_silentmode = false;

/**************************************************************************/
/*!
    @brief  Polls the relevant incoming message queue to see if anything
            is waiting to be processed.
*/
/**************************************************************************/
void cmduartPoll()
{
  while (uartRxBufferDataPending())
  {
    uint8_t c = uartRxBufferRead();
    cmduartRx(c);
  }
}

/**************************************************************************/
/*!
    @brief  Handles a single incoming character.  If a new line is
            detected, the entire command will be passed to the command
            parser.  If a text character is detected, it will be added to
            the message buffer until a new line is detected (up to the
            maximum queue size, CFG_BSP_INTERFACE_MAXMSGSIZE).

    @param[in]  c
                The character to parse.
*/
/**************************************************************************/
void cmduartRx(uint8_t c)
{
  // read out the data in the buffer and echo it back to the host.
  switch (c)
  {
    case '\r':
      #if CFG_BSP_CMDUART_DROPCR == 1
      break;
      #endif
    case '\n':
        // terminate the cmduart_buffer and reset the cmduart_buffer ptr. then send
        // it to the handler for processing.
        *cmduart_buffer_ptr = '\0';
        if (cmduart_silentmode == false)
        	cu_printf("%s", CFG_BSP_PRINTF_NEWLINE);
        cmduartParse((char *)cmduart_buffer);
        cmduart_buffer_ptr = cmduart_buffer;
        break;
    default:
        if (cmduart_silentmode == false)
        	cu_printf("%c",c);
        if(cmduart_buffer_ptr <= cmduart_buffer + CFG_BSP_CMDUART_MAXMSGSIZE)
            *cmduart_buffer_ptr++ = c;
        break;
  }
}

/**************************************************************************/
/*!
    @brief  Parse the command line. This function tokenizes the command
            input, then searches for the command table entry associated
            with the commmand. Once found, it will jump to the
            corresponding function.

    @param[in]  cmd
                The entire command string to be parsed
*/
/**************************************************************************/
void cmduartParse(char *cmd)
{
  int argc, i = 0;
  char *argv[30];

  argv[i] = strtok(cmd, ",");
  do
  {
      argv[++i] = strtok(NULL, ",");
  } while ((i < 30) && (argv[i] != NULL));

  // Repeat last argument
  i--;
  argv[i] = strtok(argv[i], "*");
  // Checksum
  argv[++i] = strtok(NULL, "*");

  if(argv[i] == NULL)
  {
	  cu_printf ("$PXERR,%s*00%s", CFG_BSP_CMDUART_SHORTERRORS_CHECKSUMMISSING, CFG_BSP_PRINTF_NEWLINE);
	  return;
  }

  // Take out command name from argument count
  argc = i - 1;
  for (i=0; cmduart_tbl[i].command != NULL; i++)
  {
      if (!strcmp(argv[0], cmduart_tbl[i].command))
      {
        if (argc < cmduart_tbl[i].minArgs)
        {
          // Too few arguments supplied
          cu_printf ("$PXERR,%s*00%s", CFG_BSP_CMDUART_SHORTERRORS_TOOFEWARGS, CFG_BSP_PRINTF_NEWLINE);
        }
        else if (argc > cmduart_tbl[i].maxArgs)
        {
          // Too many arguments supplied
          cu_printf ("$PXERR,%s*00%s", CFG_BSP_CMDUART_SHORTERRORS_TOOMANYARGS, CFG_BSP_PRINTF_NEWLINE);
        }
        else
        {
          #if CFG_BSP_CMDUART_ENABLEIRQ != 0
          // Set the IRQ pin high at start of a command
          gpioSetValue(CFG_BSP_CMDUART_IRQPORT, CFG_BSP_CMDUART_IRQPIN, 1);
          #endif
          // Dispatch command to the appropriate function
          cmduart_tbl[i].func(argc, &argv [1]);
          #if CFG_BSP_CMDUART_ENABLEIRQ  != 0
          // Set the IRQ pin low to signal the end of a command
          gpioSetValue(CFG_BSP_CMDUART_IRQPORT, CFG_BSP_CMDUART_IRQPIN, 0);
          #endif
        }
        return;
      }
  }
  // Command not recognized
  cu_printf ("$PXERR,%s*00%s", CFG_BSP_CMDUART_SHORTERRORS_UNKNOWNCOMMAND, CFG_BSP_PRINTF_NEWLINE);
}

/**************************************************************************/
/*!
    @brief Initialises the command line using the appropriate interface
*/
/**************************************************************************/
void cmduartInit(void)
{
  // Check if UART is already initialised

  uart_pcb_t *pcb = uartGetPCB();
  if (!pcb->initialised)
  {
    uartInit(CFG_BSP_UART_BAUDRATE);
  }

  #if CFG_BSP_CMDUART_ENABLEIRQ != 0
    // Set IRQ pin as output
    LPC_GPIO->DIR[CFG_BSP_CMDUART_IRQPORT] |= (1 << CFG_BSP_CMDUART_IRQPIN);
    LPC_GPIO->SET[CFG_BSP_CMDUART_IRQPORT] = (1 << CFG_BSP_CMDUART_IRQPIN);
  #endif

  // init the cmduart_buffer ptr
  cmduart_buffer_ptr = cmduart_buffer;

  // Set the IRQ pin low by default
  #if CFG_BSP_CMDUART_ENABLEIRQ  != 0
    LPC_GPIO->CLR[CFG_BSP_CMDUART_IRQPORT] = (1 << CFG_BSP_CMDUART_IRQPIN);
  #endif
}

#endif
