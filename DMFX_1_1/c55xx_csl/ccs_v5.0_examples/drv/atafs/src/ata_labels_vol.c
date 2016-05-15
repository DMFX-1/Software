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
/* $Id: ata_labels.c,2.4+ $													    */
/*										    									*/
/* This is proprietary information, not to be published -- TI INTERNAL DATA   	*/
/* Copyright (C) 2000, Texas Instruments, Inc.  All Rights Reserved.		  	*/
/*																				*/
/* Author: Susmit Pal													  		*/
/* Date: November 6, 2002														*/
/* Modified Date: November 28, 2002												*/
/* Last Modified Date: February 11, 2003										*/
/* April 27, 2010 - added check for NULL pointer KR042710                       */
/********************************************************************************/
/*Moving some functions here for use in ROM - Susmit*/
#include "ata.h"
#include "ata_.h"

AtaError ATA_vol(AtaState *pAtaDrive, char *volname)
{
  int i,jj;
  AtaError ret_stat = ATA_ERROR_NONE;
  AtaSector testSector;
  AtaUint32 ll; unsigned short kk;
  AtaUint16 *_AtaWriteBuffer = pAtaDrive->_AtaWriteBuffer;
  if(pAtaDrive->FatType == ATA_FAT_TYPE_32) {
  AtaFile Pfile;

  if (pAtaDrive==NULL) return(ATA_ERROR_INVALID_PARAM); //KR042710

  Pfile.pDrive = pAtaDrive;
  ret_stat = ATA_cdRoot(&Pfile);
  if(ret_stat) return(ret_stat);
  ret_stat = _AtaFindVol(&Pfile);
  if((ret_stat == ATA_ERROR_FILE_NOT_FOUND)||(ret_stat == ATA_ERROR_EOF)) return(ATA_ERROR_NONE);
  if(ret_stat) return(ret_stat);
  ll = _AtaCalculatePhySectorAndOffsetFromDirEntry((AtaUint32)Pfile.CurrentDirEntry, (AtaUint16 *)&kk, &Pfile, &ret_stat);
		  if(ret_stat) return(ret_stat);
  ret_stat = _AtaReadSector(ll, pAtaDrive, &_AtaWriteBuffer[0], 0);
		  if(ret_stat) return(ret_stat);
  for(i=0;i<11;i++)
  volname[i] = (char)_AtaReadByte(ll, pAtaDrive, kk*2+i); }
  else {
  testSector = pAtaDrive->RootDirSector;
  while(testSector<pAtaDrive->FirstDataSector) {
  jj=0;
  ret_stat = _AtaReadSector(testSector, pAtaDrive, &_AtaWriteBuffer[0], 0);
		  if(ret_stat) return(ret_stat);
  while(jj<256) {
  if((_AtaWriteBuffer[jj+5]&0x0f00)==0x0800) {
  for(i=0;i<11;i++)
  volname[i] = (char)_AtaReadByte(testSector, pAtaDrive, jj*2+i);
  break;
  }
  else jj+=16; }
  testSector++; } }
  return(ATA_ERROR_NONE);
}


