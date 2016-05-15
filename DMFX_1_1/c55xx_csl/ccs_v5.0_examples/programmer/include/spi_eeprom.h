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
*    spi_eeprom.h
**********************************************************************/
#ifndef SPI_EEPROM_H
#define SPI_EEPROM_H

#include "machine.h"

#define SPI_EEPROM_PAGE_SIZE    0x20

/************* SPI EEPROM COMMANDS *****************/
#define SPI_EEPROM_READ       0x03  /* Read command */
#define SPI_EEPROM_WREN       0x06  /* Enable Write Operation */
#define SPI_EEPROM_WRDI       0x04  /* Disable Write Operation */
#define SPI_EEPROM_RDSR       0x05  /* Read Status Register */
#define SPI_EEPROM_WRSR       0x01  /* Write Status Register */
#define SPI_EEPROM_WRITE      0x02  /* Write command */
#define SPI_EEPROM_CHER		  0x60	/* chip erase command */
#define SPI_EEPROM_READID     0x90  /* Read ID command */
#define SPI_EEPROM_WRSREN     0x50  /* enable Write Status Register */


/************* Mid Level Media Drivers ****************
 ******************************************************/

int spi_eeprom_init(void);
void spi_eeprom_done();
UInt16 spi_eeprom_read_n_words(UInt32 address, UInt16 *buffer, UInt16 count);
UInt16 spi_eeprom_write_n_words(UInt32 address, UInt16 *buffer, UInt16 count);

/************* Low Level Media Drivers ****************
 ******************************************************/

/* This function reads the status register of the dataflash */
byte spi_eeprom_rdsr(void);
byte spi_eeprom_wrsr(unsigned int data);
byte spi_read(UInt32 address);
void spi_write(UInt16 data, UInt32 address);
void spi_read_id(UInt16 *id);

unsigned char spiFlashChipErase(void);

#endif // SPI_EEPROM_H
