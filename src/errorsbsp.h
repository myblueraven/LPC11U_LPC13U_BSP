/**************************************************************************/
/*!
    @defgroup Errors Error Handling

    @brief    System wide error codes and error-handling

    @details

    In order to improve the stability of the system the code base uses a
    global error type that any critical function returns upon completion.

    Functions that execute properly return ERROR_NONE, which equals '0' so
    that we can perform a simple check like 'if(error) { ... }'.
*/
/**************************************************************************/

/**************************************************************************/
/*!
    @file     errors.h
    @author   K. Townsend (microBuilder.eu)

    @ingroup  Errors

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2013, K. Townsend (microBuilder.eu)
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
#ifndef _ERRORS_BSP_H_
#define _ERRORS_BSP_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************/
/*!
    Common error messages used across the system
*/
/**************************************************************************/

  /*=======================================================================
    SIMPLE BINARY PROTOCOL ERRORS                          0x0120 .. 0x013F
    -----------------------------------------------------------------------
    Errors relating to the simple binary protocol (/src//protocol)
    -----------------------------------------------------------------------*/
#define ERROR_PROT_INVALIDMSGTYPE                   (0x121)  /**< Unexpected msg type encountered */
#define ERROR_PROT_INVALIDCOMMANDID                 (0x122)  /**< Unknown or out of range command ID */
#define ERROR_PROT_INVALIDPAYLOAD                   (0x123)  /**< Message payload has a problem (invalid len, etc.) */
  /*=======================================================================*/


/*=======================================================================
  TIMESPAN ERRORS                                        0x0150 .. 0x015F
  -----------------------------------------------------------------------
  Errors relating to the timespan.c helper class
  -----------------------------------------------------------------------*/
#define ERROR_TIMESPAN_OUTOFRANGE                   (0x151)  /**< timespan_t must be kept within int64_t nanosecond range */
/*=======================================================================*/


  /*=======================================================================
    RTC ERRORS                                             0x0140 .. 0x014F
    -----------------------------------------------------------------------
    Real time clock (RTC) errors
    -----------------------------------------------------------------------*/
#define ERROR_RTC_OUTOFEPOCHRANGE                   (0x141)  /**< RTC time must be kept in epoch range */
  /*=======================================================================*/


  /*=======================================================================
    FATFS ERRORS                                           0x0160 .. 0x016F
    -----------------------------------------------------------------------
    These error codes can be used anywhere in the system
    -----------------------------------------------------------------------*/
#define ERROR_FATFS_NODISK                          (0x161)  /**< No SD card present (based on polling the CD pin) */
#define ERROR_FATFS_INITFAILED                      (0x162)  /**< Failed trying to initialise FatFS */
#define ERROR_FATFS_FAILEDTOMOUNTDRIVE              (0x163)  /**< Failed trying to mount the drive (not FAT formatted?) */
#define ERROR_FATFS_UNABLETOCREATEFILE              (0x164)  /**< Failed trying to create a file on the SD card */
#define ERROR_FATFS_UNABLETOOPENFILE                (0x165)  /**< Failed trying to open the specified file */
#define ERROR_FATFS_WRITEFAILED                     (0x166)  /**< Failed trying to write to the SD card */
  /*=======================================================================*/


  /*=======================================================================
    USB ERRORS                                             0x0200 .. 0x02FF
    -----------------------------------------------------------------------
    USB error codes
    TODO: Harmonise LPC codes with error IDs here!
    -----------------------------------------------------------------------*/

  /*=======================================================================*/


#ifdef __cplusplus
}
#endif

#endif
