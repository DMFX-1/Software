/* ============================================================================
 * Copyright (c) 2008-2012 Texas Instruments Incorporated.
 * Except for those rights granted to you in your license from TI, all rights
 * reserved.
 *
 * Software License Agreement
 * Texas Instruments (TI) is supplying this software for use solely and
 * exclusively on TI devices. The software is owned by TI and/or its suppliers,
 * and is protected under applicable patent and copyright laws.  You may not
 * combine this software with any open-source software if such combination would
 * cause this software to become subject to any of the license terms applicable
 * to such open source software.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
 * NO WARRANTIES APPLY TO THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY.
 * EXAMPLES OF EXCLUDED WARRANTIES ARE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE AND WARRANTIES OF NON-INFRINGEMENT,
 * BUT ALL OTHER WARRANTY EXCLUSIONS ALSO APPLY. FURTHERMORE, TI SHALL NOT,
 * UNDER ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, CONSEQUENTIAL
 * OR PUNITIVE DAMAGES, FOR ANY REASON WHATSOEVER.
 * ============================================================================
 */

/*********************************************************************
*    spi.c
**********************************************************************
*   TMS320C55x code for accessing SPI devices on C5505.
**********************************************************************/
#include "registers.h"
#include "sysctrl.h"
#include "spi.h"

/*******************************************************************
* void SPI_RESET(void)
* input:
*        None
* Output:
*        None
* Return:
*        None;
*Description:
*     This function is to reset the SPI module in the MD100
********************************************************************/
void vSPI_RESET(void)
{
   /* Reset SPI Module */
   PeripheralReset(PERIPH_RESET_LCD_IIS2_IIS3_UART_SPI, 0x04, 100);
}

/*******************************************************************
* void SPI_INIT(unsigned char clock_divider)
* input:
*        clock_divider     set the clock divider for SPI
* Output:
*        None
* Return:
*        None;
*Description:
*     This function is to init the SPI module in the MD100
********************************************************************/
void vSPI_INIT(unsigned char clock_divider)
{
   //reset the SPI
   vSPI_RESET();

   //SPI init
   IOPORT_REG_SPI_CLKCTR = 0;
   //Setup the clock
   IOPORT_REG_SPI_CLKDIV = clock_divider - 1;
   //CS0 active low, Data delay = 00, data shifted out on falling edge, clock inactive when low
   IOPORT_REG_SPI_DEVCS10 = 0;
   //enable the clock
   IOPORT_REG_SPI_CLKCTR = SPI_CLK_ENABLE;
}

/*************************************************************************
*void SPI_READ_WRITE_N_BYTES(unsigned short *buffer, unsigned short count,
*                    unsigned short *cmdbuffer, unsigned short cmdcount,
*                                   unsigned short readWrite )
* input:
*        buffer: read/write buffer
*        count:   read/write count
*        cmddbuffer: commandbuffer
*        cmdcount:   command length
*        readWrote:  read/write operation
* Output:
*        None
* Return:
*        None;
*Description:
*     This function is to read/write from SPI module
********************************************************************/
void SPI_READ_WRITE_N_BYTES(unsigned short *buffer, unsigned short count,
               unsigned short *cmdbuffer, unsigned short cmdcount, unsigned short readWrite )
{
   unsigned short total_size;
   unsigned short spi_cmd = 0;
   unsigned short status;
   int i;

   //enable WIRQ//this should put to FLEN field in the command 1 register;
   total_size = count + cmdcount;
   //enable WIRQ and FIRQ
   spi_cmd |= SPI_CMD_WIRQ | SPI_CMD_FIRQ;
   //Frame length = total - 1;
   spi_cmd |= (total_size - 1);
   IOPORT_REG_SPI_CMD1 = spi_cmd;
   //preconfig CMD2, 8bit per bytes, CS0
   spi_cmd = (SPI_CMD_8BIT_WLEN << SPI_CMD_WLEN_SHIFT) | (SPI_CMD_CS0 << SPI_CMD_CS_SHIFT);
   IOPORT_REG_SPI_CMD2 = spi_cmd;

   //send out the command first
   for (i=0;i<total_size;i++)
   {
      //send out the command first
      if (i <cmdcount)
         IOPORT_REG_SPI_DAT2 = (*cmdbuffer++ << 8);

      //write data out
      if ((readWrite == SPI_CMD_WRITE ) && (i >= cmdcount))
      {
         IOPORT_REG_SPI_DAT2 = (*buffer++ << 8);
      }
      //set the LSB to 0 first
      IOPORT_REG_SPI_DAT1 = 0;
      //send out write or right command
      IOPORT_REG_SPI_CMD2 |= readWrite;

      //check the status, word complete
      do
      {
         status = IOPORT_REG_SPI_STAT1;
      }
      while ((status & SPI_STATUS_WC) != SPI_STATUS_WC);

      //check the status, bus busy
      do
      {
         status = IOPORT_REG_SPI_STAT1;
      }
      while ((status & SPI_STATUS_BUSY) == SPI_STATUS_BUSY);

      //read data in
      if ((readWrite == SPI_CMD_READ) && (i>=cmdcount))
      {
         *buffer++ = IOPORT_REG_SPI_DAT1 & 0xFF;
         IOPORT_REG_SPI_DAT1 = 0;
         IOPORT_REG_SPI_DAT2 = 0;
      }
   }
}
