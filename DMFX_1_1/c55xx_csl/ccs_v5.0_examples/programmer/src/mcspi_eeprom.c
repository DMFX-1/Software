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
*    mcspi_eeprom.c
**********************************************************************/
#include "mcspi_eeprom.h"
#include "mcspi.h"
#include "tipack.h"

Uint16 commandbuffer[10];

#define ATMELFLASH
//#define MICROCHIPFLASH

unsigned char mcspiFlashChipErase(void)
{

	unsigned short commandbuffer[4], buffer2[4], status;
	volatile int i;

#ifdef MICROCHIPFLASH
	// for microchip flash
	/* Set up the write-enable command */
	commandbuffer[0] = MCSPI_EEPROM_WREN;
	MCSPI_READ_WRITE_N_BYTES(buffer2, 0, commandbuffer, 5, MCSPI_CMD_WRITE);

	status = mcspi_eeprom_rdsr();
	// wait until the write-enable is done
    while( (status&0x02)==0x00)
    {
       /* Wait for the the eeprom to become ready */
		status = mcspi_eeprom_rdsr();
    }

	// change the BP0 and BP1 to no pretection
	// first enable the WRSR
	commandbuffer[0] = MCSPI_EEPROM_WRSREN;
	MCSPI_READ_WRITE_N_BYTES(buffer2, 0, commandbuffer, 1, MCSPI_CMD_WRITE);
	// change the BP0 and BP1 (bit2 and bit3 in status register) to 0
	commandbuffer[0] = MCSPI_EEPROM_WRSR;
	commandbuffer[1] = 0;
	MCSPI_READ_WRITE_N_BYTES(buffer2, 0, commandbuffer, 2, MCSPI_CMD_WRITE);

	status = mcspi_eeprom_rdsr();
#endif


#ifdef	ATMELFLASH
	// for Atmel flash
	/* Set up the write-enable command */
	commandbuffer[0] = MCSPI_EEPROM_WREN;
	MCSPI_READ_WRITE_N_BYTES(buffer2, 0, commandbuffer, 5, MCSPI_CMD_WRITE);

	status = mcspi_eeprom_rdsr();
	// wait until the write-enable is done
    while( (status&0x02)==0x00)
    {
       /* Wait for the the eeprom to become ready */
		status = mcspi_eeprom_rdsr();
    }

	// set global unprotect
	status = mcspi_eeprom_wrsr(0x00);

	// wait for global unprotect to complete (Bit 2 -- 5 return 0)
	status = mcspi_eeprom_rdsr();
    while( (status&0x3C)==0x00)
    {
       /* Wait for the the eeprom to become ready */
		status = mcspi_eeprom_rdsr();
    }
#endif


	/* Set up the write-enable command */
	commandbuffer[0] = MCSPI_EEPROM_WREN;
	MCSPI_READ_WRITE_N_BYTES(buffer2, 0, commandbuffer, 1, MCSPI_CMD_WRITE);

	status = mcspi_eeprom_rdsr();
	// wait until the write-enable is done
    while( (status&0x02)==0x00)
    {
       /* Wait for the the eeprom to become ready */
		status = mcspi_eeprom_rdsr();
    }


   //send erase command
    commandbuffer[0] = MCSPI_EEPROM_CHER;

    MCSPI_READ_WRITE_N_BYTES(buffer2, 0, commandbuffer, 1, MCSPI_CMD_WRITE);

    while( (mcspi_eeprom_rdsr() & 0x01) != 0x00)
    {
       /* Wait for the the eeprom to become ready */
    }
    return (0);

}

/*********************************************************************
* mcspi_eeprom_init
********************************************************************/
int mcspi_eeprom_init(int div)
{
	/* Use sys-clk divider of 25 for real-system (keep MCSPI-clk under 500kHz) */
	MCSPI_INIT(div);

	return 0;
}

/*********************************************************************
* mcspi_eeprom_done
********************************************************************/
void mcspi_eeprom_done()
{
	MCSPI_RESET();
}

/*********************************************************************
* mcspi_eeprom_read_n_words
* This function reads N words from the Dataflash.
* It functions by sending out the 64-bit command with
* the command unpacked byte by byte in commandbuffer.
* The function then triggers the CLK by calling SPI_READ_WRITE_N_BYTES
* and the results of the read are placed within
* buffer2.  Finally, buffer2 is packed back into buffer.
********************************************************************/
UInt16 mcspi_eeprom_read_n_words(UInt32 address, UInt16 *buffer, UInt16 count)
{
	Uint32 byte_address = address*2;
#ifdef DISABLE_TI_PACK
	unsigned short ii;
#endif
	unsigned short buffer2[24];          /* Buffer for unpacked bytes */
	unsigned short commandbuffer[4];     /* Buffer for unpacked command */

	commandbuffer[0] = MCSPI_EEPROM_READ;
	commandbuffer[1] = byte_address >> 16;
	commandbuffer[2] = (byte_address >> 8) & 0xFF;
	commandbuffer[3] = byte_address & 0xFF;

	MCSPI_READ_WRITE_N_BYTES(buffer2, count*2, commandbuffer, 4, MCSPI_CMD_READ);

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
* mcspi_eeprom_write_n_words
* This routine copies the page of memory from the device into
* the buffer.  Then, it modifies the necessary bytes within the
* buffer.  Once it finishes, the routine copies the buffer back
* into device.
********************************************************************/
UInt16 mcspi_eeprom_write_n_words(UInt32 address, UInt16 *buffer, UInt16 count)
{
	Uint32 byte_address = address*2;
	unsigned short ii;
	unsigned short jj;
	unsigned short count2 = count * 2;
	unsigned short buffer2[24];
	unsigned short commandbuffer[4];
	unsigned short page_offset;
	unsigned short status;

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
		page_offset = byte_address % MCSPI_EEPROM_PAGE_SIZE;

		/* Set up the write-enable command */
		commandbuffer[0] = MCSPI_EEPROM_WREN;
		MCSPI_READ_WRITE_N_BYTES(buffer2, 0, commandbuffer, 1, MCSPI_CMD_WRITE);

		status = mcspi_eeprom_rdsr();
		// wait until the write-enable is done
	    while( (status&0x02)==0x00)
	    {
	       /* Wait for the the eeprom to become ready */
			status = mcspi_eeprom_rdsr();
	    }

		/* Send out the buffer write command */
		commandbuffer[0] = MCSPI_EEPROM_WRITE;

		/* Unpack the address into the command buffer */
		commandbuffer[1] = byte_address >> 16;
		commandbuffer[2] = (byte_address >> 8) & 0xFF;
		commandbuffer[3] = byte_address & 0xFF;

		/* Check to see which is smaller, count2 or what's left on the page */
		jj = (count2 < (MCSPI_EEPROM_PAGE_SIZE-page_offset)) ? count2 : (MCSPI_EEPROM_PAGE_SIZE-page_offset);
		MCSPI_READ_WRITE_N_BYTES(&buffer2[ii], jj, commandbuffer, 4, MCSPI_CMD_WRITE);

		ii += jj;

		while( (mcspi_eeprom_rdsr() & 0x01) != 0x00)
		{
		 /* Wait for the device to become ready */
		}
		byte_address += jj;
		count2 -= jj;
	}
	return 0;
}

/*********************************************************************
* mcspi_eeprom_rdsr
********************************************************************/
byte mcspi_eeprom_rdsr()
{
	unsigned short commandbuffer = MCSPI_EEPROM_RDSR;
	unsigned short data[2];

	/* Send out the RDSR command and read the RDSR results */
	MCSPI_READ_WRITE_N_BYTES(data, 1, &commandbuffer, 1, MCSPI_CMD_READ);

	return data[0];
}

/*********************************************************************
* mcspi_eeprom_wrsr
********************************************************************/
byte mcspi_eeprom_wrsr(unsigned int data)
{
   unsigned short commandbuffer[2];

   commandbuffer[0] = MCSPI_EEPROM_WREN;

   /* Unpack the address into the command buffer */
   commandbuffer[1] = data;

   /* Copy the memory page into the buffer */
   MCSPI_READ_WRITE_N_BYTES(commandbuffer, 0, commandbuffer, 1, MCSPI_CMD_WRITE);

   commandbuffer[0] = MCSPI_EEPROM_WRSR;
   MCSPI_READ_WRITE_N_BYTES(commandbuffer, 0, commandbuffer, 2, MCSPI_CMD_WRITE);

   while( (mcspi_eeprom_rdsr() & 0x01) != 0x00)
   {
      /* Wait for the device to become ready */
   }
   return 0;
}

/*********************************************************************
* mcspi_read
********************************************************************/
byte mcspi_read(UInt32 address)
{
   unsigned short buffer;
   unsigned short commandbuffer [4];    /* Buffer for unpacked command */

   /* Create the command to be sent to the eeprom */
   commandbuffer[0] = MCSPI_EEPROM_READ;
   commandbuffer[1] = address >> 16;
   commandbuffer[2] = (address >> 8) & 0xFF;
   commandbuffer[3] = address & 0xFF;

   /* Read one byte */
   MCSPI_READ_WRITE_N_BYTES(&buffer, 1, commandbuffer, 4, MCSPI_CMD_READ);

   buffer &= 0x00FF;
   return ( (byte)buffer );
}

/*********************************************************************
* mcspi_write
********************************************************************/
void mcspi_write(UInt16 data, UInt32 address)
{
   unsigned short commandbuffer[4];

   /* Set up the command buffer */
   commandbuffer[0] = MCSPI_EEPROM_WREN;

   /* Unpack the address into the command buffer */
   commandbuffer[1] = address >> 16;
   commandbuffer[2] = (address >> 8) & 0xFF;
   commandbuffer[3] = address & 0xFF;

   /* Copy the memory page into the buffer */
   MCSPI_READ_WRITE_N_BYTES(commandbuffer, 0, commandbuffer, 1, MCSPI_CMD_WRITE);

   /* Send out the buffer write command */
   commandbuffer[0] = MCSPI_EEPROM_WRITE;
   MCSPI_READ_WRITE_N_BYTES(&data, 1, commandbuffer, 4, MCSPI_CMD_WRITE);

   while( (mcspi_eeprom_rdsr() & 0x01) != 0x00)
   {
      /* Wait for the the device to become ready */
   }
}

/*********************************************************************
* mcspi_read_id
********************************************************************/
void mcspi_read_id(UInt16 *id)
{
   unsigned short commandbuffer[4];     /* Buffer for unpacked command */

   /* Create the command to be read flash ID */
   commandbuffer[0] = MCSPI_EEPROM_READID;
   commandbuffer[1] = 0;
   commandbuffer[2] = 0;
   commandbuffer[3] = 0;

   /* Read the flash ID */
   MCSPI_READ_WRITE_N_BYTES(id, 2, commandbuffer, 4, MCSPI_CMD_READ);

   return;
}

