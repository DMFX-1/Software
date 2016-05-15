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
/* $Id: atawrite.c,2.4+ $													    */
/*										    									*/
/* This is proprietary information, not to be published -- TI INTERNAL DATA   	*/
/* Copyright (C) 2000, Texas Instruments, Inc.  All Rights Reserved.		  	*/
/*																				*/
/* Author: Susmit Pal													  		*/
/* Date: November 6, 2002														*/
/* Modified Date: November 28, 2002												*/
/* Last Modified Date: February 11, 2003										*/
/* April 26, 2010 - added check of NULL pointers KR042610                       */
/********************************************************************************/

#include <stdlib.h>
#include "ata_.h"

extern AtaCluster start_FAT_cluster; //KR012210

AtaError ATA_create(AtaFile *pAtaFile)
{
  AtaError ret_stat = ATA_ERROR_NONE;
  if (pAtaFile==NULL) return(ATA_ERROR_INVALID_PARAM); //KR042610
  pAtaFile->Size = 0L;
  ret_stat = _AtaCreate(pAtaFile, 0, 0);
  return(ret_stat);
}

AtaError ATA_createDirectory(AtaFile *pAtaFile)
{
  AtaError ret_stat = ATA_ERROR_NONE;
  if (pAtaFile==NULL) return(ATA_ERROR_INVALID_PARAM); //KR042610
  ret_stat = _AtaCreateDirectory(pAtaFile, 0);
  return(ret_stat);
}

AtaError ATA_rename(AtaFile* pAtaFile)
{
  AtaError ret_stat = ATA_ERROR_NONE;
  if (pAtaFile==NULL) return(ATA_ERROR_INVALID_PARAM); //KR042610

  ret_stat = _AtaCreate(pAtaFile,2,0);	/* 2 is for rename, adjusts offset calculated */

  return(ret_stat);
}

AtaError ATA_delete(AtaFile *pAtaFile)
{
  int last_cluster;
  AtaError ret_stat = ATA_ERROR_NONE;

  AtaState *pAtaDrive = pAtaFile->pDrive;

  if ((pAtaFile==NULL)||(pAtaDrive==NULL)) return(ATA_ERROR_INVALID_PARAM); //KR042610

  pAtaFile->PrevBrokenCluster = pAtaFile->NextBrokenCluster = pAtaFile->Cluster = pAtaFile->StartCluster;
  pAtaFile->Filename[0] = '\xE5';
  if(_AtaCreate(pAtaFile,2,0))
  {
	return(1);
  }
/*SUSMIT - Directory Delete FAT Updation Bug Fix*/
/* if(pAtaFile->Size!=0) {*/
/* if((pAtaFile->Size!=0)||(pAtaFile->Attributes == ATA_ATTR_DIRECTORY)) {*//*SUSMIT - Directory Attribute Fix*/
 if((pAtaFile->Size!=0)||(ATA_isDir(pAtaFile))) {
/*SUSMIT - Directory Delete FAT Updation Bug Fix*/
  do
  {
/*SUSMIT - New delete change due to modification in _AtaFindNextBrokenCluster*/
/*	ret_stat = _AtaFindNextBrokenCluster(pAtaFile,1);*/
	ret_stat = _AtaFindNextBrokenCluster(pAtaFile,0);
/*SUSMIT - New delete change due to modification in _AtaFindNextBrokenCluster*/
	if(ret_stat == ATA_ERROR_EOF) ret_stat = ATA_ERROR_NONE;
	if(ret_stat) return(ret_stat);

	ret_stat = _AtaReadFatEntry(pAtaFile->NextBrokenCluster, pAtaDrive, &last_cluster, &pAtaFile->Cluster);
	if(ret_stat) return(ret_stat);

	ret_stat = _AtaWriteFatEntry(pAtaFile,1);
    if(ret_stat) return(ret_stat);
  }
  while(!last_cluster);
 }

  start_FAT_cluster=ATA_FIRST_CLUSTER; //KR012210

  return(ret_stat);
}

