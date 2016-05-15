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
/* $Id: atadir_.c,2.4+ $													    */
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


AtaError _AtaFindNext(AtaFile *pAtaFile, int available)
{
  AtaState *pAtaDrive = pAtaFile->pDrive;
  AtaError ret_stat = ATA_ERROR_NONE;
  pAtaFile->CurrentByte = 0;  /*SUSMIT - CurrentByte Updation*/

  while(1)
  {
	/* -----------------9/8/99 2:46PM--------------------
	Check to see if we are out of directory entry locations
	--------------------------------------------------*/
#ifndef DISABLE_FAT32
/*SUSMIT - Max File Creation Error code Bug FIX*/
/*	if(pAtaFile->CurrentDirSector == pAtaDrive->FirstDataSector)*/
	if((pAtaFile->CurrentDirSector == pAtaDrive->FirstDataSector)||((pAtaFile->CurrentDirSector == pAtaDrive->RootDirSector)&&(pAtaFile->StartDirCluster == 0)))
/*SUSMIT - Max File Creation Error code Bug FIX*/
	{
	  if((pAtaFile->CurrentDirEntry >= pAtaDrive->RootDirEntries) && !(pAtaDrive->FatType == ATA_FAT_TYPE_32))
#else
	if(pAtaFile->CurrentDirSector == pAtaDrive->RootDirSector)
	{
	  if(pAtaFile->CurrentDirEntry >= pAtaDrive->RootDirEntries)
#endif
	  {
		ret_stat = ATA_ERROR_FILE_NOT_FOUND;
		break;
	  }
	}
	else
	{
	  /* -----------------9/8/99 2:52PM--------------------
	  If the directory is a sub-directory, the length of the
	   directory entry table is determined by the FAT.  A
	   test must be placed here to see if we are about to run
	   over the directory entry table into a file.
	   --------------------------------------------------*/
	}

	/* -----------------9/8/99 3:28PM--------------------
	Read directory entry
	 --------------------------------------------------*/
	ret_stat = _AtaGetDirEntry(pAtaFile);
	if(ret_stat) return(ret_stat);

	/* -----------------9/8/99 3:28PM--------------------
	Determine validity of the directory entry
	 --------------------------------------------------*/
	if(pAtaFile->Filename[0] == '\0')
	{
	  /* end of directory entries */
	  if((pAtaFile->CurrentDirEntry>0) && !available)
	  {
		pAtaFile->CurrentDirEntry--;
	  }
	  ret_stat = (available) ? ATA_ERROR_NONE : ATA_ERROR_FILE_NOT_FOUND;
	  break;
	}
	else if(pAtaFile->Filename[0] == '\xE5')
	{
	  /* deleted file */
	  if(available)
	  {
		ret_stat = ATA_ERROR_NONE;
		break;
	  }
	}
	else if(pAtaFile->Attributes == 0xF)
	{
	  /* -----------------9/8/99 3:26PM--------------------
	  Long filename entry
	   --------------------------------------------------*/
	}
	else if(pAtaFile->Attributes & ATA_ATTR_VOLUME)
	{
	  /*-----------------1/19/00 4:03PM-------------------
	   * Volume label
	   * --------------------------------------------------*/
	}
	else
	{
	  /* valid entry found */
	  if(pAtaFile->Filename[0] == '\x05')
	  {
		pAtaFile->Filename[0] = '\xE5';
	  }
	  if(!available)
	  {
		ret_stat = ATA_ERROR_NONE;
		break;
	  }
	}
	pAtaFile->CurrentDirEntry++;
  }

  return(ret_stat);
}

