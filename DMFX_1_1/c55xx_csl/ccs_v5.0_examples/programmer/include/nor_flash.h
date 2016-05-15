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
*    nor_flash.h
**********************************************************************/
#ifndef NOR_FLASH_H
#define NOR_FLASH_H

#include "machine.h"

/**************************************************
**	NOR FLASH
**************************************************/
#define NOR_FLASH_CS2   0
#define NOR_FLASH_CS3   1
#define NOR_FLASH_CS4   2
#define NOR_FLASH_CS5   3

/* NOR-Flash base addresses for each chip-select (16-bit word address) */
#define NOR_FLASH_BASE_ADDRESS_CS2   0x400000
#define NOR_FLASH_BASE_ADDRESS_CS3   0x600000
#define NOR_FLASH_BASE_ADDRESS_CS4   0x700000
#define NOR_FLASH_BASE_ADDRESS_CS5   0x780000

/***************************************************************************
**    Flash Definitions
***************************************************************************/
#define DQ7  (1<<7)
#define DQ6  (1<<6)
#define DQ5  (1<<5)
#define DQ3  (1<<3)
#define DQ2  (1<<2)
#define DQ1  (1<<1)

#define DQE  (DQ6|DQ2)

// flash "Reset" command
#define FLASH_RESET_CMD1  0x00F0
#define FLASH_READARRAY_CMD1  0x00FF
// flash "Autoselect" command
#define FLASH_AUTOSELECT_CMD1  0x00AA
#define FLASH_AUTOSELECT_CMD2  0x0055
#define FLASH_AUTOSELECT_CMD3  0x0090
//flash program command
#define FLASH_PROGRAM_CMD1  0x00AA
#define FLASH_PROGRAM_CMD2  0x0055
#define FLASH_PROGRAM_CMD3  0x00A0

// flash "Unlock Bypass" command
#define FLASH_UNLOCK_BYPASS_CMD1  0x00AA
#define FLASH_UNLOCK_BYPASS_CMD2  0x0055
#define FLASH_UNLOCK_BYPASS_CMD3  0x0020
// flash "Unlock Bypass Program" command
#define FLASH_UNLOCK_BYPASS_PGM_CMD1  0x00A0
// flash "Unlock Bypass Reset" command
#define FLASH_UNLOCK_BYPASS_RESET_CMD1  0x0090
#define FLASH_UNLOCK_BYPASS_RESET_CMD2  0x0000
// flash "Chip Erase" command
#define FLASH_CHIP_ERASE_CMD1  0x00AA
#define FLASH_CHIP_ERASE_CMD2  0x0055
#define FLASH_CHIP_ERASE_CMD3  0x0080
#define FLASH_CHIP_ERASE_CMD4  0x00AA
#define FLASH_CHIP_ERASE_CMD5  0x0055
#define FLASH_CHIP_ERASE_CMD6  0x0010
// flash "Sector Erase" command
#define FLASH_SECTOR_ERASE_CMD1  0x00AA
#define FLASH_SECTOR_ERASE_CMD2  0x0055
#define FLASH_SECTOR_ERASE_CMD3  0x0080
#define FLASH_SECTOR_ERASE_CMD4  0x00AA
#define FLASH_SECTOR_ERASE_CMD5  0x0055
#define FLASH_SECTOR_ERASE_CMD6  0x0030

#define FLASH_STATUS_OK    0
#define FLASH_STATUS_ERROR 1

/***************************************************************************
**    Flash Structures
***************************************************************************/
typedef struct
{
   unsigned long NumSectors;     // Number of sectors
   unsigned long SectorSize;     // Number of 16-bit words in each sector
} FlashSectorMap_t;

#define FLASH_NUM_SECTOR_MAP_ENTRIES   4

typedef struct
{
   unsigned short  MfrId;
   unsigned short  DevId[3];
   FlashSectorMap_t SectorMap[FLASH_NUM_SECTOR_MAP_ENTRIES]; // Sector mapping
} FlashDeviceData_t;


/***************************************************************************
**    Function Prototypes
***************************************************************************/
int nor_flash_init(Uint16 ChipSelect);
UInt16 nor_flash_read_n_words(UInt32 address, UInt16 *buffer, UInt16 count);
UInt16 nor_flash_write_n_words(UInt32 address, UInt16 *buffer, UInt16 count);
int nor_flash_chip_erase(void);
int nor_flash_sector_erase(unsigned long SectorOffset);

/* Low-level flash routines */
void flashReset(void);
int flashGetDeviceData(FlashDeviceData_t *Data);
int flashGetDeviceIndex(FlashDeviceData_t *Data);
FlashDeviceData_t *flashGetDevicePtr(int DeviceIndex);
int flashToggleErase(unsigned long Offset);
int flashChipErase(void);
int flashSectorErase(unsigned long Offset);
int flashToggle(void);
int flashWrite(unsigned short data, unsigned long offset);

#endif // NOR_FLASH_H

