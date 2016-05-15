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

#include "csl_general.h"
#include "csl_spi.h"
#include <stdio.h>

/* SPI Commands */
#define SPI_CMD_WRSR            (0x01)
#define SPI_CMD_WRITE           (0x02)
#define SPI_CMD_READ            (0x03)
#define SPI_CMD_WRDI            (0x04)
#define SPI_CMD_RDSR            (0x05)
#define SPI_CMD_WREN            (0x06)
#define SPI_CMD_ERASE           (0x20)

Int16 spi_eeprom_init(CSL_SpiHandle spiHandle);

Int16 SPI_EEPROM_read (CSL_SpiHandle spiHandle,
                       Uint16        src,
                       Uint32        dst,
                       Uint32        length);

Int16 SPI_EEPROM_write (CSL_SpiHandle spiHandle,
                        Uint32        src,
                        Uint16        dst,
                        Uint32        length);


