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
*    spi_eeprom.c
**********************************************************************/
#include "spi_eeprom.h"
#include "spi.h"
#include "tipack.h"

//#define BYTEPROGRAM
#define ATMELFLASH
//#define MICROCHIPFLASH

unsigned char spiFlashChipErase(void)
{
	unsigned short commandbuffer[8], buffer2[4], status; 
	volatile int i;   

#ifdef MICROCHIPFLASH
	// for microchip flash
	/* Set up the write-enable command */
	commandbuffer[0] = SPI_EEPROM_WREN;
	SPI_READ_WRITE_N_BYTES(buffer2, 0, commandbuffer, 5, SPI_CMD_WRITE);

	status = spi_eeprom_rdsr();
	// wait until the write-enable is done
    while( (status&0x02)==0x00)
    {
       /* Wait for the the eeprom to become ready */
		status = spi_eeprom_rdsr();
    }

	// change the BP0 and BP1 to no pretection
	// first enable the WRSR
	commandbuffer[0] = SPI_EEPROM_WRSREN;
	SPI_READ_WRITE_N_BYTES(buffer2, 0, commandbuffer, 1, SPI_CMD_WRITE);
	// change the BP0 and BP1 (bit2 and bit3 in status register) to 0
	commandbuffer[0] = SPI_EEPROM_WRSR;
	commandbuffer[1] = 0;
	SPI_READ_WRITE_N_BYTES(buffer2, 0, commandbuffer, 2, SPI_CMD_WRITE);
	   
	status = spi_eeprom_rdsr();
#endif

#ifdef	ATMELFLASH
	// for Atmel flash
	/* Set up the write-enable command */
	commandbuffer[0] = SPI_EEPROM_WREN;
	SPI_READ_WRITE_N_BYTES(buffer2, 0, commandbuffer, 5, SPI_CMD_WRITE);

	status = spi_eeprom_rdsr();
	// wait until the write-enable is done
    while( (status&0x02)==0x00)
    {
       /* Wait for the the eeprom to become ready */
		status = spi_eeprom_rdsr();
    }

	// set global unprotect	
	status = spi_eeprom_wrsr(0x00);

	// wait for global unprotect to complete (Bit 2 -- 5 return 0)
	status = spi_eeprom_rdsr();
    while( (status&0x3C)==0x00)
    {
       /* Wait for the the eeprom to become ready */
		status = spi_eeprom_rdsr();
    }
#endif

	/* Set up the write-enable command */
	commandbuffer[0] = SPI_EEPROM_WREN;
	SPI_READ_WRITE_N_BYTES(buffer2, 0, commandbuffer, 5, SPI_CMD_WRITE);

	status = spi_eeprom_rdsr();
	// wait until the write-enable is done
    while( (status&0x02)==0x00)
    {
       /* Wait for the the eeprom to become ready */
		status = spi_eeprom_rdsr();
    }

	// send erase command
    commandbuffer[0] = SPI_EEPROM_CHER;

    SPI_READ_WRITE_N_BYTES(buffer2, 0, commandbuffer, 1, SPI_CMD_WRITE);

	status = spi_eeprom_rdsr();

	// wait until the chip erase is done
    while( (status&0x01)==0x01)
    {
       /* Wait for the the eeprom to become ready */
		status = spi_eeprom_rdsr();
    }
    return (0);

}

/*********************************************************************
* spi_eeprom_init
********************************************************************/
int spi_eeprom_init(void)
{
   /* Use sys-clk divider of 25 for real-system (keep SPI-clk under 500kHz) */
   SPI_INIT(25);
	//SPI_INIT(4);
   return 0;
}

/*********************************************************************
* spi_eeprom_done
********************************************************************/
void spi_eeprom_done()
{
   SPI_RESET();
}

/*********************************************************************
* spi_eeprom_read_n_words
* This function reads N words from the Dataflash.
* It functions by sending out the 64-bit command with
* the command unpacked byte by byte in commandbuffer.
* The function then triggers the CLK by calling SPI_READ_WRITE_N_BYTES
* and the results of the read are placed within
* buffer2.  Finally, buffer2 is packed back into buffer.
********************************************************************/
UInt16 spi_eeprom_read_n_words(UInt32 address, UInt16 *buffer, UInt16 count)
{
   Uint32 byte_address = address*2;
#ifdef DISABLE_TI_PACK
   unsigned short ii;
#endif
   unsigned short buffer2[24];          /* Buffer for unpacked bytes */
   unsigned short commandbuffer[4];     /* Buffer for unpacked command */
   unsigned short sixteenbitdevice;

   /* Determine if 16- or 24-bit device is being used */
   sixteenbitdevice = (address & 0x80000000) ? 0 : 1;

   /* Create the command to be sent to the eeprom */
   if(sixteenbitdevice){	// 16-bit SPI device
      commandbuffer[0] = SPI_EEPROM_READ;
      commandbuffer[1] = byte_address >> 8;
      commandbuffer[2] = byte_address & 0xFF;
   }
   else {	// 24-bit SPI device
      commandbuffer[0] = SPI_EEPROM_READ;
      commandbuffer[1] = byte_address >> 16;
      commandbuffer[2] = (byte_address >> 8) & 0xFF;
      commandbuffer[3] = byte_address & 0xFF;
   }

   SPI_READ_WRITE_N_BYTES(buffer2, count*2, commandbuffer, (sixteenbitdevice ? 3 : 4), SPI_CMD_READ);

#ifdef DISABLE_TI_PACK
   for(ii = 0; ii < count; ii++)
   {
      buffer[ii] = buffer2[(2*ii)+1];
      buffer[ii] |= (buffer2[(2*ii)] << 8);
   }
#else
   TI_pack(buffer, (unsigned char *)buffer2, count*2);
#endif

   return 0;
}

/*********************************************************************
* spi_eeprom_write_n_words
* This routine copies the page of memory from the device into
* the buffer.  Then, it modifies the necessary bytes within the
* buffer.  Once it finishes, the routine copies the buffer back
* into device.
********************************************************************/
UInt16 spi_eeprom_write_n_words(UInt32 address, UInt16 *buffer, UInt16 count)
{
   Uint32 byte_address = address*2;
   unsigned short ii;
   unsigned short jj;
   unsigned short kk;
   unsigned short count2 = count * 2;
   unsigned short buffer2[24];
   unsigned short commandbuffer[4];
   unsigned short page_offset;
   unsigned short sixteenbitdevice;

   /* Determine if 16- or 24-bit device is being used */
   sixteenbitdevice = (address & 0x80000000) ? 0 : 1;
   address &= ~(0x80000000); // clear 24-bit SPI device indicator bit

   /* Unpack the write buffer */
#ifdef DISABLE_TI_PACK
   for(ii=0; ii < count; ii++)
   {
      buffer2[(2*ii)+1] = buffer[ii] & 0xFF;
      buffer2[(2*ii)] = (buffer[ii] >> 8) & 0xFF;
   }
#else
   TI_unpack((unsigned char *)buffer2, buffer, count2, 0);
#endif

   ii = 0;

   while(byte_address < (2*(address+count)) )
   {
      page_offset = byte_address % SPI_EEPROM_PAGE_SIZE;

#ifndef BYTEPROGRAM
      /* Set up the write-enable command */
      commandbuffer[0] = SPI_EEPROM_WREN;
      SPI_READ_WRITE_N_BYTES(buffer2, 0, commandbuffer, 1, SPI_CMD_WRITE);

      /* Send out the buffer write command */
      commandbuffer[0] = SPI_EEPROM_WRITE;

      /* Unpack the address into the command buffer */
	  if(sixteenbitdevice){
         commandbuffer[1] = byte_address >> 8;
         commandbuffer[2] = byte_address & 0xFF;
	  }
	  else {
         commandbuffer[1] = byte_address >> 16;
         commandbuffer[2] = (byte_address >> 8) & 0xFF;
         commandbuffer[3] = byte_address & 0xFF;
      }

      /* Check to see which is smaller, count2 or what's left on the page */
      jj = (count2 < (SPI_EEPROM_PAGE_SIZE-page_offset)) ? count2 : (SPI_EEPROM_PAGE_SIZE-page_offset);
      SPI_READ_WRITE_N_BYTES(&buffer2[ii], jj, commandbuffer, (sixteenbitdevice ? 3 : 4), SPI_CMD_WRITE);
#else
      /* Check to see which is smaller, count2 or what's left on the page */
      jj = (count2 < (SPI_EEPROM_PAGE_SIZE-page_offset)) ? count2 : (SPI_EEPROM_PAGE_SIZE-page_offset);

	  for (kk=0; kk<jj; kk++)
	  {
	      /* Set up the write-enable command */
	      commandbuffer[0] = SPI_EEPROM_WREN;
	      SPI_READ_WRITE_N_BYTES(buffer2, 0, commandbuffer, 1, SPI_CMD_WRITE);

	      /* Send out the buffer write command */
	      commandbuffer[0] = SPI_EEPROM_WRITE;
	      /* Unpack the address into the command buffer */
		  if(sixteenbitdevice){
	         commandbuffer[1] = (byte_address+kk) >> 8;
	         commandbuffer[2] = (byte_address+kk) & 0xFF;
		  }
		  else {
	         commandbuffer[1] = (byte_address+kk) >> 16;
	         commandbuffer[2] = ((byte_address+kk) >> 8) & 0xFF;
	         commandbuffer[3] = (byte_address+kk) & 0xFF;
	      }

	      SPI_READ_WRITE_N_BYTES(&buffer2[kk], 1, commandbuffer, (sixteenbitdevice ? 3 : 4), SPI_CMD_WRITE);
	      while( (spi_eeprom_rdsr() & 0x01) != 0x00)
	      {
	         /* Wait for the device to become ready */
	      }
	  }
#endif

      ii += jj;

      while( (spi_eeprom_rdsr() & 0x01) != 0x00)
      {
         /* Wait for the device to become ready */
      }
      byte_address += jj;
      count2 -= jj;
   }
   return 0;
}

/*********************************************************************
* spi_eeprom_rdsr
********************************************************************/
byte spi_eeprom_rdsr(void)
{
   unsigned short commandbuffer = SPI_EEPROM_RDSR;

   /* Send out the RDSR command and read the RDSR results */
   SPI_READ_WRITE_N_BYTES(&commandbuffer, 1, &commandbuffer, 1, SPI_CMD_READ);

   return commandbuffer;
}

/*********************************************************************
* spi_eeprom_wrsr
********************************************************************/
byte spi_eeprom_wrsr(unsigned int data)
{
   unsigned short commandbuffer[2];

   commandbuffer[0] = SPI_EEPROM_WREN;

   /* Unpack the address into the command buffer */
   commandbuffer[1] = data;

   /* Copy the memory page into the buffer */
   SPI_READ_WRITE_N_BYTES(commandbuffer, 0, commandbuffer, 1, SPI_CMD_WRITE);

   commandbuffer[0] = SPI_EEPROM_WRSR;
   SPI_READ_WRITE_N_BYTES(commandbuffer, 0, commandbuffer, 2, SPI_CMD_WRITE);

   while( (spi_eeprom_rdsr() & 0x01) != 0x00)
   {
      /* Wait for the device to become ready */
   }
   return 0;
}

/*********************************************************************
* spi_read
********************************************************************/
byte spi_read(UInt32 address)
{
   unsigned short buffer;
   unsigned short commandbuffer[4];     /* Buffer for unpacked command */
   unsigned short sixteenbitdevice;

   /* Determine if 16- or 24-bit device is being used */
   sixteenbitdevice = (address & 0x80000000) ? 0 : 1;
   address &= ~(0x80000000); // clear 24-bit SPI device indicator bit

   /* Create the command to be sent to the eeprom */
   if(sixteenbitdevice){
      commandbuffer[0] = SPI_EEPROM_READ;
      commandbuffer[1] = address >> 8;
      commandbuffer[2] = address & 0xFF;
   }
   else {
      commandbuffer[0] = SPI_EEPROM_READ;
      commandbuffer[1] = address >> 16;
      commandbuffer[2] = (address >> 8) & 0xFF;
      commandbuffer[3] = address & 0xFF;
   }

   /* Read one byte */
   SPI_READ_WRITE_N_BYTES(&buffer, 1, commandbuffer, (sixteenbitdevice ? 3 : 4), SPI_CMD_READ);

   buffer &= 0x00FF;
   return ( (byte)buffer );
}

/*********************************************************************
* spi_write
********************************************************************/
void spi_write(UInt16 data, UInt32 address)
{
   unsigned short commandbuffer[4];
   unsigned short sixteenbitdevice;

   /* Determine if 16- or 24-bit device is being used */
   sixteenbitdevice = (address & 0x80000000) ? 0 : 1;
   address &= ~(0x80000000); // clear 24-bit SPI device indicator bit

   /* Set up the command buffer */
   commandbuffer[0] = SPI_EEPROM_WREN;

   /* Unpack the address into the command buffer */
   if(sixteenbitdevice){
      commandbuffer[1] = address >> 8;
      commandbuffer[2] = address & 0xFF;
   }
   else {
      commandbuffer[1] = address >> 16;
      commandbuffer[2] = (address >> 8) & 0xFF;
      commandbuffer[3] = address & 0xFF;
   }

   /* Copy the memory page into the buffer */
   SPI_READ_WRITE_N_BYTES(commandbuffer, 0, commandbuffer, 1, SPI_CMD_WRITE);

   /* Send out the buffer write command */
   commandbuffer[0] = SPI_EEPROM_WRITE;
   SPI_READ_WRITE_N_BYTES(&data, 1, commandbuffer, (sixteenbitdevice ? 3 : 4), SPI_CMD_WRITE);

   while( (spi_eeprom_rdsr() & 0x01) != 0x00)
   {
      /* Wait for the the device to become ready */
   }
}

/*********************************************************************
* spi_read_id
********************************************************************/
void spi_read_id(UInt16 *id)
{
   unsigned short commandbuffer[4];     /* Buffer for unpacked command */

   /* Create the command to be read flash ID */
   commandbuffer[0] = 0x0090;
   //commandbuffer[0] = 0x009F;
   commandbuffer[1] = 0;
   commandbuffer[2] = 0;
   commandbuffer[3] = 0;

   /* Read the flash ID */
   SPI_READ_WRITE_N_BYTES(id, 2, commandbuffer, 4, SPI_CMD_READ);

   return;
}
