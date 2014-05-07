/*
 * cmduart_printf.c
 *
 *  Created on: 2014-04-03
 *      Author: iceman
 */
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

#include "core/uart/uart.h"
/*

void __putchar(const char c)
{
    uartSendByte(c);
}
*/

int cu_puts(const char * str)
{
    while(*str) uartSendByte(*str++);
//    while(*str) __putchar(*str++);

  return 0;
}

signed int cu_vprintf(const char *pFormat, va_list ap)
{
  char pStr[CFG_LIB_PRINTF_MAXSTRINGSIZE];
  char pError[] = "stdio.c: increase CFG_BSP_PRINTF_MAXSTRINGSIZE\r\n";

  // Write formatted string in buffer
  if (vsprintf(pStr, pFormat, ap) >= CFG_LIB_PRINTF_MAXSTRINGSIZE) {

	  cu_puts(pError);
    while (1); // Increase CFG_BSP_PRINTF_MAXSTRINGSIZE
  }

  // Display string
  return cu_puts(pStr);
}

signed int cu_printf(const char *pFormat, ...)
{
    va_list ap;
    signed int result;

    // Forward call to vprintf
    va_start(ap, pFormat);
    result = cu_vprintf(pFormat, ap);
    va_end(ap);

    return result;
}
