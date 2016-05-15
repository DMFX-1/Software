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
/* $Id: ataread_.c,2.4+ $														*/
/*																				*/
/* This is proprietary information, not to be published -- TI INTERNAL DATA	   */
/* Copyright (C) 2000, Texas Instruments, Inc.  All Rights Reserved.			  */
/*																				*/
/* Author: Susmit Pal															  */
/* Date: November 6, 2002														*/
/* Modified Date: November 28, 2002												*/
/* Last Modified Date: February 11, 2003										*/
/* Mar 7, 2010 - added ATA_No_Swap control variable                             */
/********************************************************************************/

#include "ata_.h"

extern AtaUint16 ATA_No_Swap;

AtaError _AtaRead(AtaFile *pAtaFile, AtaUint16 *Data, AtaUint16 Words)
{
  AtaUint16 WordCount, OffsetInSector;
  AtaSector PhySector;
  AtaState *pAtaDrive = pAtaFile->pDrive;
  AtaError ret_stat = ATA_ERROR_NONE; AtaUint16 kk;

#ifdef DISABLE_ALWAYS_CALCULATE_SECTOR
  int CalculateSector = 1;
#endif

  for(WordCount=0; WordCount<Words; WordCount++, OffsetInSector++, pAtaFile->WordInCluster++)
  {
	if(pAtaFile->WordInCluster >= pAtaDrive->WordsPerCluster)
	{
	  pAtaFile->WordInCluster = 0;
	  if(pAtaFile->Cluster == pAtaFile->NextBrokenCluster)
	  {
#ifdef DISABLE_ALWAYS_CALCULATE_SECTOR
		CalculateSector = 1;
#endif
		ret_stat = _AtaFindNextBrokenCluster(pAtaFile,1);
		 if(ret_stat)  return(ret_stat);
	  }
	  else
	  {
		pAtaFile->Cluster++;
#ifdef DISABLE_ALWAYS_CALCULATE_SECTOR
		PhySector++;
		OffsetInSector = 0;
#endif
	  }
	}

	/* set up the first sector in the cluster */
#ifdef DISABLE_ALWAYS_CALCULATE_SECTOR
	else if(OffsetInSector >= ATA_WORDS_PER_PHY_SECTOR)
	{
	  PhySector++;
	  OffsetInSector = 0;
	}

	if(CalculateSector)
	{
	  /* calculate physical sector and word offset of current cluster */
	  CalculateSector = 0;
	  PhySector = _AtaCalculatePhySectorAndOffsetFromCluster(pAtaFile,&OffsetInSector);
	}
#else
	PhySector = _AtaCalculatePhySectorAndOffsetFromCluster(pAtaFile,&OffsetInSector);
#endif

	ATA_No_Swap=1; // KR030710
	kk = _AtaReadWord(PhySector, pAtaDrive, OffsetInSector*2);
	ATA_No_Swap=0; // KR030710
	Data[WordCount] = (AtaUint16)kk;
	ret_stat = pAtaDrive->Err_Status;
  }

  return(ret_stat);
}

