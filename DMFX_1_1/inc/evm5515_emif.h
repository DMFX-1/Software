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
 *  EMIF Header file
 *
 */


/* ------------------------------------------------------------------------ *
 *  Registers                                                               *
 * ------------------------------------------------------------------------ */

#define EMIF_REV                *(volatile ioport Uint16*)(0x1000)
#define EMIF_STATUS             *(volatile ioport Uint16*)(0x1001)
#define EMIF_AWCCR1             *(volatile ioport Uint16*)(0x1004)
#define EMIF_AWCCR2             *(volatile ioport Uint16*)(0x1005)
#define EMIF_ACS2CR1            *(volatile ioport Uint16*)(0x1010)
#define EMIF_ACS2CR2            *(volatile ioport Uint16*)(0x1011)
#define EMIF_ACS3CR1            *(volatile ioport Uint16*)(0x1014)
#define EMIF_ACS3CR2            *(volatile ioport Uint16*)(0x1015)
#define EMIF_ACS4CR1            *(volatile ioport Uint16*)(0x1018)
#define EMIF_ACS4CR2            *(volatile ioport Uint16*)(0x1019)
#define EMIF_ACS5CR1            *(volatile ioport Uint16*)(0x101C)
#define EMIF_ACS5CR2            *(volatile ioport Uint16*)(0x101D)
#define EMIF_EIRR               *(volatile ioport Uint16*)(0x1040)
#define EMIF_EIMR               *(volatile ioport Uint16*)(0x1044)
#define EMIF_EIMSR              *(volatile ioport Uint16*)(0x1048)
#define EMIF_EIMCR              *(volatile ioport Uint16*)(0x104C)
#define EMIF_NANDFCR            *(volatile ioport Uint16*)(0x1060)
#define EMIF_NANDFSR1           *(volatile ioport Uint16*)(0x1064)
#define EMIF_NANDFSR2           *(volatile ioport Uint16*)(0x1065)

