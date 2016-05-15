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

/********************************************************************************/
/* $Id: ata_bytes.c,2.4+ $														*/
/*																				*/
/* This is proprietary information, not to be published -- TI INTERNAL DATA	    */
/* Copyright (C) 2000, Texas Instruments, Inc.  All Rights Reserved.			*/
/*																				*/
/* Author: Susmit Pal															*/
/* Date: November 6, 2002														*/
/* Modified Date: November 28, 2002												*/
/* Last Modified Date: February 11, 2003										*/
/********************************************************************************/
#include "ata.h"
#include "ata_.h"

AtaError ATA_read_b(AtaFile *pAtaFile, AtaUint16 *Data, AtaUint16 bytes)
{
  AtaError ret_stat = ATA_ERROR_NONE;
  if(bytes == 0) return(ATA_ERROR_NONE);
  ret_stat = _AtaRead_b(pAtaFile, Data, bytes);
  if(pAtaFile->CurrentByte > pAtaFile->Size) ret_stat = ATA_ERROR_EOF;
  return(ret_stat);
}


AtaError ATA_tell_b(AtaFile *pAtaFile, AtaFileSize *pByteOffsetInFile)
{
	*pByteOffsetInFile = pAtaFile->CurrentByte;

	return(ATA_ERROR_NONE);
}

