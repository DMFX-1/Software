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

/*
 *  Flash interface for Intel/Micron Flash chips & Spansion Flash chips
 *
 */

#ifndef C5515EVM_NOR
#define C5515EVM_NOR

//#include "machine.h"

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

#define USE_SPANSION
//#define USE_INTEL_MICRON

/* ------------------------------------------------------------------------ *
 *  Spansino Flash Devices                                                  *
 * ------------------------------------------------------------------------ */
#define MFG_SPANSION                0x01

#define DEV_AM29LV256M_1            0x227E
#define DEV_AM29LV256M_2            0x2212
#define DEV_AM29LV256M_3            0x2201
#define DEV_AM29LV256M_SIZE         0x02000000

#define DEV_S29GL512N_1             0x227E
#define DEV_S29GL512N_2             0x2223
#define DEV_S29GL512N_3             0x2201
#define DEV_S29GL512N_SIZE          0x04000000

#define DEV_S29GL256N_1             0x227E
#define DEV_S29GL256N_2             0x2222
#define DEV_S29GL256N_3             0x2201
#define DEV_S29GL256N_SIZE          0x02000000

#define DEV_S29GL128N_1             0x227E
#define DEV_S29GL128N_2             0x2221
#define DEV_S29GL128N_3             0x2201
#define DEV_S29GL128N_SIZE          0x01000000

/* ------------------------------------------------------------------------ *
 *  Intel/Mircon Flash Devices                                              *
 * ------------------------------------------------------------------------ */
#define MFG_INTEL                   0x89

#define DEV_INTEL_64_TOP            0x8817
#define DEV_INTEL_64_TOP_PAGES      64  // 63@128K + 4@32K
#define DEV_INTEL_64_TOP_SIZE       0x00800000

#define DEV_INTEL_64_BOT            0x881A
#define DEV_INTEL_64_BOT_PAGES      64  // 63@128K + 4@32K
#define DEV_INTEL_64_BOT_SIZE       0x00800000


#define DEV_INTEL_128_TOP           0x8818
#define DEV_INTEL_128_TOP_PAGES     128 // 127@128K + 4@32K
#define DEV_INTEL_128_TOP_SIZE      0x01000000

#define DEV_INTEL_128_BOT           0x881B
#define DEV_INTEL_128_BOT_PAGES     128 // 127@128K + 4@32K
#define DEV_INTEL_128_BOT_SIZE      0x01000000


#define DEV_INTEL_256_TOP           0x8919
#define DEV_INTEL_256_TOP_PAGES     256 // 255@128K + 4@32K
#define DEV_INTEL_256_TOP_SIZE      0x02000000

#define DEV_INTEL_256_BOT           0x891C
#define DEV_INTEL_256_BOT_PAGES     256 // 255@128K + 4@32K
#define DEV_INTEL_256_BOT_SIZE      0x02000000


#define MFG_MICRON                  0x2C

/* ------------------------------------------------------------------------ *
 *  Flash Memory Property Definitions                                       *
 * ------------------------------------------------------------------------ */
#define FLASH_BASE                  ( 0x0400000 )
#define FLASH_END_OR_RANGE          ( 0x04000000 )
#define FLASH_PAGES                 ( norflash_getTotalPages( 1 ) )
#define FLASH_SUPPORT               1

/* Intel only */
#define FLASH_DEVICE_TOP_PAGES      1
#define FLASH_DEVICE_BOTTOM_PAGES   2

/* ------------------------------------------------------------------------ *
 *  Flash Memory Base Pointers                                              *
 * ------------------------------------------------------------------------ */
#define FLASH_BASE_PTR32    *( volatile Uint32* )FLASH_BASE
#define FLASH_BASE_PTR16    *( volatile Uint16* )FLASH_BASE
#define FLASH_BASE_PTR8     *( volatile Uint8* ) FLASH_BASE

/* Spansion only */
#define FLASH_CTL555        *( volatile Uint16* )( FLASH_BASE + 0x555 )
#define FLASH_CTL2AA        *( volatile Uint16* )( FLASH_BASE + 0x2AA )

/* ------------------------------------------------------------------------ *
 *  Flash Command Codes ( SPANSION )                                        *
 * ------------------------------------------------------------------------ */
#define FLASH_RESET                         0xF0
#define FLASH_CMD_AA                        0xAA
#define FLASH_CMD_55                        0x55
#define FLASH_ID                            0x90
#define FLASH_PROGRAM                       0xA0
#define FLASH_WRITE_BUFFER                  0x25
#define FLASH_PROGRAM_BUFFER                0x29
#define FLASH_ERASE                         0x80
#define FLASH_ERASE_CHIP                    0x10
#define FLASH_ERASE_SECTOR                  0x30
#define FLASH_ERASE_SUSPEND                 0xB0
#define FLASH_ERASE_RESUME                  0x10
#define FLASH_CFI_QUERY                     0x98

/* ------------------------------------------------------------------------ *
 *  Flash Command Codes ( Intel/Micron )                                    *
 * ------------------------------------------------------------------------ */
#define FLASH_READ_ARRAY                    0xFF
#define FLASH_READ_ID                       0x90
#define FLASH_READ_QUERY                    0x98
#define FLASH_READ_STATUS                   0x70
#define FLASH_CLEAR_STATUS                  0x50
#define FLASH_WORD_PROGRAM                  0x40 // -or- 0x10
#define FLASH_WRITE_TO_BUFFER               0xE8
#define FLASH_BUFFERED_EFP                  0x80
#define FLASH_BLOCK_ERASE                   0x20
#define FLASH_ERASE_PROGRAM_SUSPEND         0xB0
#define FLASH_ERASE_PROGRAM_RESUME          0xD0
#define FLASH_CONFIGURATION                 0xB8
#define FLASH_CONFIG_BLOCK_LOCK_BITS        0x60
#define FLASH_SET_LOCK_BITS                 0x01
#define FLASH_CLEAR_LOCK_BITS               0xD0
#define FLASH_PROTECTION_PROGRAM            0xC0

/* ------------------------------------------------------------------------ *
 *  Flash Memory Read Status Register Fields                                *
 * ------------------------------------------------------------------------ */
#define READ_STATUS_REGISTER_ISMS           0x80 // WRITE STATE MACHINE
#define READ_STATUS_REGISTER_ESS            0x40 // ERASE SUSPEND
#define READ_STATUS_REGISTER_ECLBS          0x20 // ERASE & CLEAR LOCK BITS
#define READ_STATUS_REGISTER_PSLBS          0x10 // PROGRAM & SET LOCK BIT
#define READ_STATUS_REGISTER_VPENS          0x08 // PROGRAMMING VOLTAGE
#define READ_STATUS_REGISTER_PSS            0x04 // PROGRAM SUSPEND
#define READ_STATUS_REGISTER_DPS            0x02 // DEVICE PROTECT

/* ------------------------------------------------------------------------ *
 *  Prototypes                                                              *
 * ------------------------------------------------------------------------ */
Int16  norflash_init        ( );
Uint16  norflash_read        ( Uint32 src, Uint16* dst, Uint16 length );
Uint16  norflash_write       ( Uint32 src, Uint16* dst, Uint16 length );
Int16  norflash_erase       ( Uint32 start, Uint32 length );
Uint32 norflash_checksum    ( Uint32 start, Uint32 length );
Uint32 norflash_getTotalPages( Uint16 num_flash_device );

/* Internal functions */
Int32  _FLASH_getMfg                ( );
Int32  _FLASH_getPageLength         ( );
Uint32 _FLASH_nextPage              ( Uint32 addr );

/* Spansion */
Int16 _FLASH_waitWhileErasing       ( Uint32 addr, Int32 timeout );
Int16 _FLASH_waitWhileProgramming   ( Uint32 addr, Uint16 data, Int32 timeout );

/* Intel/Micron */
Int16 _FLASH_waitWhileBusy          ( Uint32 timeout );
Int16 _FLASH_unlockBlocks           ( Uint32 start, Uint32 length );
//Uint16 _FLASH_getId                 ( Uint16* id);

#define FLASH_PAGESIZE              0x20000//( _FLASH_getPageLength( ) )

#endif
