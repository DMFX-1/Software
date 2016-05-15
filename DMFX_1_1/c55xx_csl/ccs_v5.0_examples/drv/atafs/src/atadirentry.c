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
/* $Id: atadirentry.c,2.4+ $                                                    */
/*                                                                              */
/* This is proprietary information, not to be published -- TI INTERNAL DATA     */
/* Copyright (C) 2000, Texas Instruments, Inc.  All Rights Reserved.            */
/*                                                                              */
/* Author: Susmit Pal                                                           */
/* Date: November 6, 2002                                                       */
/* Modified Date: November 28, 2002                                             */
/* Last Modified Date: February 11, 2003                                        */
/* April 21, 2010 - added *ret_stat = ATA_ERROR_NONE;  KR042110                 */
/********************************************************************************/

#include "ata.h"
#include "ata_.h"

unsigned long _AtaCalculatePhySectorAndOffsetFromDirEntry(AtaUint32 DirEntry, AtaUint16 *jj, AtaFile *pAtaFile, AtaError *ret_stat)
{
  unsigned long mm;
  AtaState *pAtaDrive=pAtaFile->pDrive;

  *ret_stat = ATA_ERROR_NONE; //KR042110

  if(pAtaFile->StartDirCluster == 0)
  {
    /* treat the root directory of a FAT12/16 disk specially */
  mm = pAtaFile->CurrentDirSector;
  mm += (unsigned long)((DirEntry * 16) / ATA_WORDS_PER_PHY_SECTOR);
  *jj = (DirEntry * 16) % ATA_WORDS_PER_PHY_SECTOR;
  }
  else
  {
    /* in a subdirectory */
    int logEntriesPerCluster;
    unsigned long tag;
    int DirEntryInCluster;

    /* Mistral: Commented the intrinsic function.
     * This is available in CCS 3.3.38 as "_norm"
     * and not "__norm".
     */
    /* intrinsic */
    //  unsigned int __norm(int src);

    /* calculate how many dir entries per cluster */
    /* Ata.LogSectorsPerCluster is actually just "sectors per cluster" */
    logEntriesPerCluster = 18 - _norm(pAtaDrive->LogSectorsPerCluster); /* Mistral: Using _norm instead of __norm. */
    tag = DirEntry>>logEntriesPerCluster;

    /* check the cached cluster */
    if (tag != pAtaFile->CurrentDirSectorTag)
    {
      /* treat the subdirectory as a file to find cluster */
      AtaFile dirAsFile = *pAtaFile;
      dirAsFile.StartCluster = dirAsFile.StartDirCluster;
      dirAsFile.Size = 0;
      *ret_stat = ATA_ERROR_NONE;
      *ret_stat = ATA_seek(&dirAsFile, DirEntry*16);
      if(*ret_stat == ATA_ERROR_EOF) *ret_stat = ATA_ERROR_FILE_NOT_FOUND;

      /* cache sector info */
      pAtaFile->CurrentDirSectorTag = tag;
      pAtaFile->CurrentDirSector = pAtaDrive->FirstDataSector +
        ((unsigned long)(dirAsFile.Cluster-2) * (unsigned long)pAtaDrive->LogSectorsPerCluster);
    }

    /* look up actual sector based off cached info */
    DirEntryInCluster = DirEntry - (tag<<logEntriesPerCluster);
    mm = pAtaFile->CurrentDirSector;
    mm += (unsigned long)((DirEntryInCluster * 16) / ATA_WORDS_PER_PHY_SECTOR);
    *jj = (DirEntryInCluster * 16) % ATA_WORDS_PER_PHY_SECTOR;
  }
  return (mm);
}

