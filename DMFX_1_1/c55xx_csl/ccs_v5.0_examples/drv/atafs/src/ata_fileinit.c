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
/* $Id: ata_fileinit.c,2.4+ $												    */
/*										    									*/
/* This is proprietary information, not to be published -- TI INTERNAL DATA   	*/
/* Copyright (C) 2000, Texas Instruments, Inc.  All Rights Reserved.		  	*/
/*																				*/
/* Author: Susmit Pal													  		*/
/* Date: November 6, 2002														*/
/* Modified Date: November 28, 2002												*/
/* Last Modified Date: February 11, 2003										*/
/* April 21, 2010 - added NULL check of pointers KR042110                       */
/********************************************************************************/

#include "ata.h"
#include "ata_.h"

extern  AtaCluster start_FAT_cluster; //KR012210

AtaError ATA_fileInit(AtaState *pAtaDrive, AtaFile *pAtaFile)
{
  AtaError kk = ATA_ERROR_NONE;
  pAtaFile->pDrive = pAtaDrive;

  if ((pAtaDrive==NULL)||(pAtaFile==NULL)) return(ATA_ERROR_INVALID_PARAM); //KR042110

  start_FAT_cluster=ATA_FIRST_CLUSTER; //KR012210

  kk = ATA_cdRoot(pAtaFile);
  if(kk) return(kk);
/*  return(AtaFindFirst(pAtaFile));	*/	/*This is not a bug fix, this is a workaround so that ATA_fileInit doesn't return error for a formatted MMC - Susmit*/
  kk=ATA_findFirst(pAtaFile);
  if((kk==ATA_ERROR_NONE)||(kk==ATA_ERROR_FILE_NOT_FOUND)) return(ATA_ERROR_NONE);
  else return(kk);
}

