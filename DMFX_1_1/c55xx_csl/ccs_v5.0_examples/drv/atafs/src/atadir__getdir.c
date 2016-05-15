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

AtaError _AtaGetDirEntry(AtaFile *pAtaFile)
{
  register char *cc;
  AtaUint32 ll;
  AtaUint16 jj;int i;
  AtaError ret_stat = ATA_ERROR_NONE;

  AtaState *pAtaDrive = pAtaFile->pDrive;

  /* calculate physical sector and word offset of current directory entry */
  ll = _AtaCalculatePhySectorAndOffsetFromDirEntry((AtaDirEntry)pAtaFile->CurrentDirEntry, (AtaUint16 *)&jj, pAtaFile, &ret_stat);
  if(ret_stat) return(ret_stat);
  jj = jj*2;

  cc = (char *)&pAtaFile->Filename[0];
  for(i=0;i<8;i++)
  *cc++ = _AtaReadByte(ll, pAtaDrive, jj++);
  *cc++ = '\0';
  cc = (char *)&pAtaFile->Ext[0];
  for(i=0;i<3;i++)
  *cc++ = _AtaReadByte(ll, pAtaDrive, jj++);
  *cc++ = '\0';
  pAtaFile->Attributes   = (unsigned char)_AtaReadByte(ll, pAtaDrive, jj);
#ifndef DISABLE_FAT32
  jj+=9;
  pAtaFile->StartCluster = ((AtaCluster)_AtaReadWord(ll, pAtaDrive, jj)) << 16;
  jj+=2;
#else
  jj+=11;
#endif
  pAtaFile->Time = _AtaReadWord(ll, pAtaDrive, jj);
  jj+=2;
  pAtaFile->Date = _AtaReadWord(ll, pAtaDrive, jj);
  jj+=2;
#ifndef DISABLE_FAT32
  pAtaFile->StartCluster|= (AtaCluster)_AtaReadWord(ll, pAtaDrive, jj);
#else
  pAtaFile->StartCluster = _AtaReadWord(ll, pAtaDrive, jj);
#endif
  jj+=2;
  pAtaFile->Size = _AtaReadDoubleWord(ll, pAtaDrive, jj);

  pAtaFile->PrevBrokenCluster = pAtaFile->NextBrokenCluster = pAtaFile->Cluster = pAtaFile->StartCluster;
  pAtaFile->WordInCluster = 0;

  return(pAtaDrive->Err_Status);
}


