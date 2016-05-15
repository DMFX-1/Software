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
*    mcspi_eeprom.h
**********************************************************************/
#ifndef MCSPI_EEPROM_H
#define MCSPI_EEPROM_H

#include "machine.h"

#define MCSPI_EEPROM_PAGE_SIZE    0x20

/************* SPI EEPROM COMMANDS *****************/
#define MCSPI_EEPROM_READ       0x03  /* Read command */
#define MCSPI_EEPROM_WREN       0x06  /* Enable Write Operation */
#define MCSPI_EEPROM_WRDI       0x04  /* Disable Write Operation */
#define MCSPI_EEPROM_RDSR       0x05  /* Read Status Register */
#define MCSPI_EEPROM_WRSR       0x01  /* Write Status Register */
#define MCSPI_EEPROM_WRITE      0x02  /* Write command */
#define MCSPI_EEPROM_CHER		0xC7	/* chip erase command */
#define MCSPI_EEPROM_READID     0x90  /* Read device ID */
#define MCSPI_EEPROM_WRSREN     0x50  /* enable Write Status Register */

/************* Mid Level Media Drivers ****************
 ******************************************************/

int mcspi_eeprom_init(int div);
void mcspi_eeprom_done();
UInt16 mcspi_eeprom_read_n_words(UInt32 address, UInt16 *buffer, UInt16 count);
UInt16 mcspi_eeprom_write_n_words(UInt32 address, UInt16 *buffer, UInt16 count);

/************* Low Level Media Drivers ****************
 ******************************************************/

/* This function reads the status register of the dataflash */
byte mcspi_eeprom_rdsr();
byte mcspi_eeprom_wrsr(unsigned int data);
byte mcspi_read(UInt32 address);
void mcspi_write(UInt16 data, UInt32 address);
void mcspi_read_id(UInt16 *id);

unsigned char mcspiFlashChipErase(void);


#endif // MCSPI_EEPROM_H
