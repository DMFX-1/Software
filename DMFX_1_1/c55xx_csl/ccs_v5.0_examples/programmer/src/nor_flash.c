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
*    nor_flash.c
**********************************************************************/
#include "registers.h"
#include "nor_flash.h"

/* Table of supported flash devices */
/* It is most important to include top-boot devices and non CFI-compliant devices */
const FlashDeviceData_t FlashDeviceTable[] =
{
   /* Spansion S29GL016A (Top) [2MB] */
   { 0x0001, 0x22C4, 0x0000, 0x0000, {{31,0x8000}, {8,0x1000}, {0,0x0000}, {0,0x0000}} },
   /* Spansion S29AL016M (Top) [2MB] */
   { 0x0001, 0x22C4, 0x0000, 0x0000, {{31,0x8000}, {1,0x4000}, {2,0x1000}, {1,0x2000}} },
   /* MXIC MX29LV160T (Top) [2MB] */
   { 0x00C2, 0x22C4, 0x0000, 0x0000, {{31,0x8000}, {1,0x4000}, {2,0x1000}, {1,0x2000}} },
   /* Spansion S29GL032A (Top) [4MB] */
   { 0x0001, 0x227E, 0x221A, 0x2201, {{63,0x8000}, {8,0x1000}, {0,0x0000}, {0,0x0000}} },
   /* SST SST39LF/VF200A (Uniform) [256kB] */
   { 0x00BF, 0x2789, 0x0000, 0x0000, {{64,0x0800}, {0,0x0000}, {0,0x0000}, {0,0x0000}} },
   /* SST SST39LF/VF400A (Uniform) [512kB] */
   { 0x00BF, 0x2780, 0x0000, 0x0000, {{128,0x0800}, {0,0x0000}, {0,0x0000}, {0,0x0000}} },
   /* SST SST39LF/VF800A (Uniform) [1MB] */
   { 0x00BF, 0x2781, 0x0000, 0x0000, {{256,0x0800}, {0,0x0000}, {0,0x0000}, {0,0x0000}} },
};

/* NOR-Flash Base-Address */
const Uint32 NorFlashBaseAddrTable[4] = { NOR_FLASH_BASE_ADDRESS_CS2, NOR_FLASH_BASE_ADDRESS_CS3,
                                          NOR_FLASH_BASE_ADDRESS_CS4, NOR_FLASH_BASE_ADDRESS_CS5 };
Uint32 NorFlashBaseAddr = NOR_FLASH_BASE_ADDRESS_CS2;


/***************************************************************
* nor_flash_init
*
*  Input:
*     none
*  Output:
*     None:
*  Return:
*     None;
*  Descriptions:
*       init the EMIF to access the NOR FLASH
***************************************************************/
int nor_flash_init(Uint16 ChipSelect)
{
   /* Access EMIF registers as 16-bit registers */
   IOPORT_REG_EMIF_CNTL = 0x0000;

	/* Configure ASYNC Wait config Registers */
   IOPORT_REG_EMIF_ASYNC_WAIT_0 = 0x0080; // max_ext_wait = 0x80
   IOPORT_REG_EMIF_ASYNC_WAIT_1 = 0x00E4; // wp(all) = 0

   /* Not using NAND here */
   IOPORT_REG_EMIF_NAND_CTRL = 0x0000;

	/* Configure as 16-bit data bus */
   IOPORT_REG_EMIF_ASYNC1_CONFIG_0 = 0xFFFD;
   IOPORT_REG_EMIF_ASYNC1_CONFIG_1 = 0x3FFF;
   IOPORT_REG_EMIF_ASYNC2_CONFIG_0 = 0xFFFD;
   IOPORT_REG_EMIF_ASYNC2_CONFIG_1 = 0x3FFF;
   IOPORT_REG_EMIF_ASYNC3_CONFIG_0 = 0xFFFD;
   IOPORT_REG_EMIF_ASYNC3_CONFIG_1 = 0x3FFF;
   IOPORT_REG_EMIF_ASYNC4_CONFIG_0 = 0xFFFD;
   IOPORT_REG_EMIF_ASYNC4_CONFIG_1 = 0x3FFF;
   if (ChipSelect > NOR_FLASH_CS5)
      return 1;
   NorFlashBaseAddr = NorFlashBaseAddrTable[ChipSelect];

   flashReset();
	return 0;
}

/***************************************************************
* nor_flash_read_n_words
*
*  Input:
*    address:	offset
*	  buffer:	destination data
*	  count:		size
*  Output:
*     None:
*  Return:
*     None;
*  Descriptions:
*       read from the NOR FLASH
***************************************************************/
UInt16 nor_flash_read_n_words(UInt32 address, UInt16 *buffer, UInt16 count)
{
   int i;
   unsigned short *flashData;

   flashData = (unsigned short *)NorFlashBaseAddr + address;
	for (i=0; i<count; i++)
	{
      *buffer++ = *flashData++;
	}
	return(0);
}

/***************************************************************
* nor_flash_write_n_words
*
*  Input:
*    address:  offset
*	  buffer:	destination data
*	  count:		size
*  Output:
*     None:
*  Return:
*     None;
*  Descriptions:
*       write to the NOR FLASH
***************************************************************/
UInt16 nor_flash_write_n_words(UInt32 address, UInt16 *buffer, UInt16 count)
{
   int i;

	for (i=0; i<count; i++)
	{
		flashWrite(*buffer++, address++);
	}
	return (0);
}

/*******************************************************************
* nor_flash_chip_erase
********************************************************************/
int nor_flash_chip_erase(void)
{
   return flashChipErase();
}

/*******************************************************************
* nor_flash_sector_erase
********************************************************************/
int nor_flash_sector_erase(unsigned long SectorOffset)
{
   return flashSectorErase(SectorOffset);
}


/***************************************************************
* flashReset
*
*  Input:
*     None:
*  Output:
*     None:
*  Return:
*     None;
*  Descriptions:
*       Reset the Flash
***************************************************************/
void flashReset(void)
{
   // flash "Reset" command
   ((volatile unsigned short*)NorFlashBaseAddr)[0] = FLASH_RESET_CMD1;
   ((volatile unsigned short*)NorFlashBaseAddr)[0] = FLASH_READARRAY_CMD1;             // add this command after reset (read array)
}

/***************************************************************
* flashGetDeviceData
*
*  Input:
*     none
*  Output:
*     Data:  Autoselect and CFI data
*  Return:
*     FLASH_STATUS_OK if device data found
*     FLASH_STATUS_ERROR if device data not found
*  Descriptions:
*       Get the Autoselect and CFI data for the flash
***************************************************************/
int flashGetDeviceData(FlashDeviceData_t *Data)
{
   Uint16 Tmp1, Tmp2, Tmp3;
   Uint16 NumEraseBlockRegions;
   Uint16 ReadIndex;
   Uint16 Region;
   int FlashDeviceIndex;

   // flash "Reset" command
   ((volatile unsigned short*)NorFlashBaseAddr)[0] = FLASH_RESET_CMD1;
   // flash "Autoselect" command
   ((volatile unsigned short*)NorFlashBaseAddr)[0x555] = FLASH_AUTOSELECT_CMD1;
   ((volatile unsigned short*)NorFlashBaseAddr)[0x2AA] = FLASH_AUTOSELECT_CMD2;
   ((volatile unsigned short*)NorFlashBaseAddr)[0x555] = FLASH_AUTOSELECT_CMD3;
   // get the manufacturer-id and device-id
   Data->MfrId = *(volatile unsigned short *)NorFlashBaseAddr;
   Data->DevId[0] = *((volatile unsigned short *)NorFlashBaseAddr + 0x1);
   Data->DevId[1] = *((volatile unsigned short *)NorFlashBaseAddr + 0xe);
   Data->DevId[2] = *((volatile unsigned short *)NorFlashBaseAddr + 0xf);
   // flash "Reset" command
   ((volatile unsigned short*)NorFlashBaseAddr)[0] = FLASH_RESET_CMD1;

   // flash "CFI Query" command
   ((volatile unsigned short*)NorFlashBaseAddr)[0x55] = 0x98;
   // verify "QRY"
   Tmp1 = ((volatile unsigned short*)NorFlashBaseAddr)[0x10];
   Tmp2 = ((volatile unsigned short*)NorFlashBaseAddr)[0x11];
   Tmp3 = ((volatile unsigned short*)NorFlashBaseAddr)[0x12];
   if ((Tmp1 == 'Q') && (Tmp2 == 'R') && (Tmp3 == 'Y'))
   {
      // read the number of erase-block regions
      NumEraseBlockRegions = ((volatile unsigned short*)NorFlashBaseAddr)[0x2C];
      if (NumEraseBlockRegions > FLASH_NUM_SECTOR_MAP_ENTRIES)
      {
         ((volatile unsigned short*)NorFlashBaseAddr)[0] = FLASH_RESET_CMD1;
         return FLASH_STATUS_ERROR;
      }
      ReadIndex = 0x2D;
      for (Region=0; Region<NumEraseBlockRegions; Region++)
      {
         // read the number of sectors in the region
         Tmp1 = ((volatile unsigned short*)NorFlashBaseAddr)[ReadIndex+0];
         Tmp2 = ((volatile unsigned short*)NorFlashBaseAddr)[ReadIndex+1];
         Data->SectorMap[Region].NumSectors = (Tmp2 << 8) + (Tmp1 & 0xFF) + 1;
         // read the size of the sectors in the region (words)
         Tmp1 = ((volatile unsigned short*)NorFlashBaseAddr)[ReadIndex+2];
         Tmp2 = ((volatile unsigned short*)NorFlashBaseAddr)[ReadIndex+3];
         Data->SectorMap[Region].SectorSize = ((Tmp2 << 8) + (Tmp1 & 0xFF)) << 7;
         ReadIndex += 4;
      }
      for (; Region<FLASH_NUM_SECTOR_MAP_ENTRIES; Region++)
      {
         Data->SectorMap[Region].NumSectors = 0;
         Data->SectorMap[Region].SectorSize = 0;
      }
      // check device-id to find match in table (needed for some top-boot devices)
      FlashDeviceIndex = flashGetDeviceIndex(Data); // -1 means not found
      if (FlashDeviceIndex >= 0) // if not found, just use data from device
      {
         // Copy the sector-map data (override needed for some top-boot devices).
         // The sector map data is reversed, as read from some top-boot devices.
         for (Region=0; Region<FLASH_NUM_SECTOR_MAP_ENTRIES; Region++)
         {
            Data->SectorMap[Region].NumSectors = FlashDeviceTable[FlashDeviceIndex].SectorMap[Region].NumSectors;
            Data->SectorMap[Region].SectorSize = FlashDeviceTable[FlashDeviceIndex].SectorMap[Region].SectorSize;
         }
      }
   }
   else
   {
      // try to find the device in the flash-device-table
      Data->SectorMap[0].NumSectors = 0; // set to 0 to indicate no erase-block data
      FlashDeviceIndex = flashGetDeviceIndex(Data); // -1 means not found
      if (FlashDeviceIndex < 0)
      {
         ((volatile unsigned short*)NorFlashBaseAddr)[0] = FLASH_RESET_CMD1;
         return FLASH_STATUS_ERROR;
      }
      // copy the sector-map data
      for (Region=0; Region<FLASH_NUM_SECTOR_MAP_ENTRIES; Region++)
      {
         Data->SectorMap[Region].NumSectors = FlashDeviceTable[FlashDeviceIndex].SectorMap[Region].NumSectors;
         Data->SectorMap[Region].SectorSize = FlashDeviceTable[FlashDeviceIndex].SectorMap[Region].SectorSize;
      }
   }

   // flash "Reset" command
   ((volatile unsigned short*)NorFlashBaseAddr)[0] = FLASH_RESET_CMD1;
   return FLASH_STATUS_OK;
}

/***************************************************************
* int flashGetDeviceIndex
*
*  Input:
*     Data:  the Autoselect/CFI data for the flash device
*  Output:
*     None:
*  Return:
*       DeviceIndex, or
*       -1 if device not found in device table
*  Descriptions:
*       Gets the device-index in the table of supported devices
***************************************************************/
int flashGetDeviceIndex(FlashDeviceData_t *Data)
{
   int i;
   int numDevices;
   int numEraseBlocks;
   int Region;

   numDevices = sizeof(FlashDeviceTable) / sizeof(FlashDeviceData_t);
   for (i=0; i<numDevices; i++)
   {
      if (((Data->MfrId & 0xFF) == FlashDeviceTable[i].MfrId) &&
          (Data->DevId[0] == FlashDeviceTable[i].DevId[0]) &&
          ((FlashDeviceTable[i].DevId[1] == 0) || (Data->DevId[1] == FlashDeviceTable[i].DevId[1])) &&
          ((FlashDeviceTable[i].DevId[2] == 0) || (Data->DevId[2] == FlashDeviceTable[i].DevId[2])))
      {
         if (Data->SectorMap[0].NumSectors == 0)
         {
            // no erase-block data provided
            return i; // device found in the table -- return its index
         }
         else
         {
            // erase-block data provided -- try to find match
            // determine the number of erase-blocks provided
            numEraseBlocks = 4;
            for (Region=1; Region<FLASH_NUM_SECTOR_MAP_ENTRIES; Region++)
            {
               if (Data->SectorMap[Region].NumSectors == 0)
                  numEraseBlocks = Region;
            }
            // check for bottom-boot or uniform devices (and some top-boot devices)
            if (FlashDeviceTable[i].SectorMap[0].NumSectors == Data->SectorMap[0].NumSectors)
            {
               // first erase-block number-of-sectors matches
               return i; // device found in the table -- return its index
            }
            // check for top-boot devices (erase-block data may be reversed in provided data)
            if (FlashDeviceTable[i].SectorMap[numEraseBlocks-1].NumSectors == Data->SectorMap[0].NumSectors)
            {
               // first erase-block number-of-sectors matches
               return i; // device found in the table -- return its index
            }
         }
      }
   }
   return (-1); // device not found in the table
}

/***************************************************************
* int flashGetDevicePtr
*
*  Input:
*     Data:  the Autoselect data for the flash device
*  Output:
*     None:
*  Return:
*       DeviceIndex, or
*       -1 if device not found in device table
*  Descriptions:
*       Gets the device-index in the table of supported devices
***************************************************************/
FlashDeviceData_t *flashGetDevicePtr(int DeviceIndex)
{
   int numDevices;

   numDevices = sizeof(FlashDeviceTable) / sizeof(FlashDeviceData_t);
   if ((DeviceIndex < 0) || (DeviceIndex >= numDevices))
      return NULL;
   return ((FlashDeviceData_t *)&FlashDeviceTable[DeviceIndex]);
}

/***************************************************************
* flashToggleErase
*
*  Input:
*     Offset:      offset to read toggle status from
*  Output:
*     None:
*  Return:
*       FLASH_STATUS_OK
*       FLASH_STATUS_ERROR
*  Descriptions:
*       Waits for erase operation to complete
***************************************************************/
int flashToggleErase(unsigned long Offset)
{
   unsigned short poll1, poll2, tog;

   poll1 = ((volatile unsigned short *)NorFlashBaseAddr)[Offset];

   do
   {
      poll2 = ((volatile unsigned short *)NorFlashBaseAddr)[Offset];

      // Check the toggle bit (bit-6)
      tog = poll1 ^ poll2;
      if ((tog & DQ6) == 0)
      {
			// not toggling -- operation complete
         // double check the erasing
         if (((volatile unsigned short *)NorFlashBaseAddr)[Offset] == 0xFFFF)
            return (FLASH_STATUS_OK);
      }

      // Prepare for next iteration
      poll1 = poll2;

		// Repeat while status bit-5 is NOT 1
	} while ((poll1 & DQ5) == 0);

   // read twice
   poll1 = ((volatile unsigned short *)NorFlashBaseAddr)[Offset];
   poll2 = ((volatile unsigned short *)NorFlashBaseAddr)[Offset];

   // Check the toggle bit (bit-6)
   tog = poll1 ^ poll2;
   if ((tog & DQ6) != 0)
   {
	   // Still toggling -- operation NOT complete
	   // Reset command
      flashReset();
	   return FLASH_STATUS_ERROR;
   }
   return (FLASH_STATUS_OK);
}

/***************************************************************
* flashChipErase
*
*  Input:
*     None;
*  Output:
*     None:
*  Return:
*       FLASH_STATUS_OK
*       FLASH_STATUS_ERROR
*  Descriptions:
*       Erases the entire FLASH Chip
***************************************************************/
int flashChipErase(void)
{
   flashReset();
   // flash "Chip Erase" command
   ((volatile unsigned short*)NorFlashBaseAddr)[0x555] = FLASH_CHIP_ERASE_CMD1;
   ((volatile unsigned short*)NorFlashBaseAddr)[0x2AA] = FLASH_CHIP_ERASE_CMD2;
   ((volatile unsigned short*)NorFlashBaseAddr)[0x555] = FLASH_CHIP_ERASE_CMD3;
   ((volatile unsigned short*)NorFlashBaseAddr)[0x555] = FLASH_CHIP_ERASE_CMD4;
   ((volatile unsigned short*)NorFlashBaseAddr)[0x2AA] = FLASH_CHIP_ERASE_CMD5;
   ((volatile unsigned short*)NorFlashBaseAddr)[0x555] = FLASH_CHIP_ERASE_CMD6;
   // Wait for erase to finish
   return flashToggleErase(0);
}

/***************************************************************
* flashSectorErase
*
*  Input:
*     SectorOffset:      offset of the sector to erase
*  Output:
*     None:
*  Return:
*       FLASH_STATUS_OK
*       FLASH_STATUS_ERROR
*  Descriptions:
*       Erase specified sector
***************************************************************/
int flashSectorErase(unsigned long SectorOffset)
{
   flashReset();

   ((volatile unsigned short*)NorFlashBaseAddr)[0x555] = FLASH_SECTOR_ERASE_CMD1;
   ((volatile unsigned short*)NorFlashBaseAddr)[0x2AA] = FLASH_SECTOR_ERASE_CMD2;
   ((volatile unsigned short*)NorFlashBaseAddr)[0x555] = FLASH_SECTOR_ERASE_CMD3;
   ((volatile unsigned short*)NorFlashBaseAddr)[0x555] = FLASH_SECTOR_ERASE_CMD4;
   ((volatile unsigned short*)NorFlashBaseAddr)[0x2AA] = FLASH_SECTOR_ERASE_CMD5;
   *(((volatile unsigned short*)NorFlashBaseAddr) + SectorOffset) = FLASH_SECTOR_ERASE_CMD6;

     // wait for erase to finish
   return (flashToggleErase(SectorOffset));
}

/***************************************************************
* flashToggle
*
*  Input:
*     None;
*  Output:
*     None:
*  Return:
*       FLASH_STATUS_OK
*       FLASH_STATUS_ERROR
*  Descriptions:
*       Waits for flash program operation to complete
***************************************************************/
int flashToggle(void)
{
   unsigned short poll1, poll2, tog;
   int i = 0;      // counter to record how many tries.

   poll1 = *(volatile unsigned short *)NorFlashBaseAddr;

   do
   {
      poll2 = *(volatile unsigned short *)NorFlashBaseAddr;

      // Check the toggle bit (bit-6)
      tog = poll1 ^ poll2;
      if ((tog & DQ6) == 0)
      {
			// not toggling -- operation complete
         return (FLASH_STATUS_OK);
      }

      // Prepare for next iteration
      poll1 = poll2;

      i++;
      if (i > 0x5000)       //over the limits but still try
      {
         return (FLASH_STATUS_ERROR);
      }
		// Repeat while status bit-5 is NOT 1
	} while ((poll1 & DQ5) == 0);

   // read twice
   poll1 = *(volatile unsigned short *)NorFlashBaseAddr;
   poll2 = *(volatile unsigned short *)NorFlashBaseAddr;

   // Check the toggle bit (bit-6)
   tog = poll1 ^ poll2;
   if ((tog & DQ6) != 0)
   {
	   // Still toggling -- operation NOT complete
	   // Reset command
      flashReset();
	   return FLASH_STATUS_ERROR;
   }
   return (FLASH_STATUS_OK);
}

/***************************************************************
* flashWrite
*
*  Input:
*     data:   data to write to flash
*     offset: offset (relative to base addr) to write to
*  Output:
*     None:
*  Return:
*       FLASH_STATUS_OK
*       FLASH_STATUS_ERROR
*  Description:
*       Writes "data" to flash device at specified offset
***************************************************************/
int flashWrite(unsigned short data, unsigned long offset)
{
   unsigned long status;

   // flash "Reset" command
   ((volatile unsigned short*)NorFlashBaseAddr)[0] = FLASH_RESET_CMD1;

   ((volatile unsigned short*)NorFlashBaseAddr)[0x555] = FLASH_PROGRAM_CMD1;
   ((volatile unsigned short*)NorFlashBaseAddr)[0x2AA] = FLASH_PROGRAM_CMD2;
   ((volatile unsigned short*)NorFlashBaseAddr)[0x555] = FLASH_PROGRAM_CMD3;

   // Write data to device
   ((volatile unsigned short*)NorFlashBaseAddr)[offset] = data;

	// Wait for programming to finish
   status = flashToggle();
   if (status != FLASH_STATUS_OK)
   {
      return (FLASH_STATUS_ERROR);
   }

   return (FLASH_STATUS_OK);
}
