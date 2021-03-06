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
/* $Id: ata_flush.c,2.4+ $													    */
/*										    									*/
/* This is proprietary information, not to be published -- TI INTERNAL DATA   	*/
/* Copyright (C) 2000, Texas Instruments, Inc.  All Rights Reserved.		  	*/
/*																				*/
/* Author: Susmit Pal													  		*/
/* Date: November 6, 2002														*/
/* Modified Date: November 28, 2002												*/
/* Last Modified Date: February 11, 2003										*/
/********************************************************************************/

#include "ata.h"
#include "ata_.h"

AtaError _AtaFlush(AtaState *pAtaDrive)
{
  int ii;
  AtaError kk = ATA_ERROR_NONE;

  /* Flush the current sector from the ATA device */
  if(pAtaDrive->CurrentPhySector != ATA_INVALID_SECTOR)
  {
#ifdef READNWORDSAPI
	if(!_AtaTimeoutCheck(pAtaDrive->AtaDataReadyStatusCheck,pAtaDrive->pAtaMediaState))
	  return(ATA_ERROR_TIMEOUT);
	ii = ATA_WORDS_PER_PHY_SECTOR - (pAtaDrive->CurrentWord+1);
	pAtaDrive->AtaReadNextNWords(pAtaDrive->pAtaMediaState, 0, ii);
#else
	kk = pAtaDrive->AtaBusyStatusCheck(pAtaDrive->pAtaMediaState);
	for(ii=pAtaDrive->CurrentWord+1; ii<ATA_WORDS_PER_PHY_SECTOR; ii++)
	{
	kk =  pAtaDrive->AtaReadNextWord(pAtaDrive->pAtaMediaState, &pAtaDrive->Data);
/*	if(kk) return(kk); *//*SUSMIT_2.2_TEST_DEBUG - This should be break to enable multiple ATaState on same media for simultaneous operations*/
    if(kk) break;
	}
#endif
  }
  pAtaDrive->CurrentPhySector = ATA_INVALID_SECTOR;
  pAtaDrive->CurrentWord = ATA_INVALID_WORD;
  return(ATA_ERROR_NONE);
}


