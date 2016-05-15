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
*    spi.h
**********************************************************************/
#ifndef SPI_H
#define SPI_H

/*********************************************************************
* Bit-field definitions for SPI registers...
**********************************************************************/

/* clock regsiter */
#define SPI_CLK_ENABLE       0x8000
/* device config register */
/* command register */
#define SPI_CMD_WIRQ         0x4000
#define SPI_CMD_FIRQ         0x8000
#define SPI_CMD_FLEN         0xFFF
#define SPI_CMD_WRITE        0x2     /* Write command */
#define SPI_CMD_READ         0x1     /* Read command */
#define SPI_CMD_WLEN_SHIFT   0x3     /* word length shift */
#define SPI_CMD_8BIT_WLEN    0x7
#define SPI_CMD_16BIT_WLEN   0x15
#define SPI_CMD_CS0          0x0     /* CS0 transfer */
#define SPI_CMD_CS1          0x1     /* CS1 transfer */
#define SPI_CMD_CS2          0x2     /* CS2 transfer */
#define SPI_CMD_CS3          0x3     /* CS3 transfer */
#define SPI_CMD_CS_SHIFT     12
/* status register */
#define SPI_STATUS_BUSY      0x1
#define SPI_STATUS_WC        0x2
#define SPI_STATUS_FC        0x4
#define SPI_STATUS_AE        0x8


/******************************************************
      Function prototypes
******************************************************/
void SPI_RESET(void);
void SPI_INIT(unsigned char clock_divider);
void SPI_READ_WRITE_N_BYTES(unsigned short *buffer, unsigned short count,
               unsigned short *cmdbuffer, unsigned short cmdcount, unsigned short readWrite );

#endif // SPI_H
