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
/* $Id: ataread_fatphy.c,2.4+ $												    */
/*										    									*/
/* This is proprietary information, not to be published -- TI INTERNAL DATA   	*/
/* Copyright (C) 2000, Texas Instruments, Inc.  All Rights Reserved.		  	*/
/*																				*/
/* Author: Susmit Pal													  		*/
/* Date: November 6, 2002														*/
/* Modified Date: November 28, 2002												*/
/* Last Modified Date: February 11, 2003										*/
/********************************************************************************/

#include "ata_.h"

/*#pragma CODE_SECTION (_AtaReadFatEntryAtPhySector, ".text:_AtaReadFatEntryAtPhySector")*/

AtaError _AtaReadFatEntryAtPhySector
(
  AtaSector PhySector,
  AtaState *pAtaDrive,
  AtaCluster Offset,
  AtaUint16 Alignment,
  int *LastCluster,
  AtaCluster *NeededCluster
)
{
  AtaCluster kk, qq;
  AtaUint16 ll;

#ifndef DISABLE_FAT32
  if(pAtaDrive->FatType == ATA_FAT_TYPE_32)
  {
    kk = _AtaReadDoubleWord(PhySector, pAtaDrive, Offset*2);  /* Read first word */
    /* check to see if we reached the last cluster */
    if ((kk&0x0FFFFFF0) == 0x0FFFFFF0)
    {
      *LastCluster = 1;
    }
    else
    {
      *LastCluster = 0;
    }
  }                                  /* end FAT_TYPE_32 */
  else
#endif
  if(pAtaDrive->FatType == ATA_FAT_TYPE_16)
  {
/*  kk = _AtaReadWord(PhySector, pAtaDrive, Offset*2);*//*FAT Read Fix - Susmit*/
    {
      ll = _AtaReadWord(PhySector, pAtaDrive, Offset*2);
      kk = (AtaCluster)ll;
    }

    /* check to see if we reached the last cluster */
    if((ll&0xFFF0) == 0xFFF0)
    {
      *LastCluster = 1;
    }
    else
    {
      *LastCluster = 0;
    }
  } /* end FAT_TYPE_16 */
#ifndef DISABLE_FAT12
  else if(pAtaDrive->FatType == ATA_FAT_TYPE_12)
  {
    {
      ll = _AtaReadWord(PhySector, pAtaDrive, Offset*2);
      kk = (AtaCluster)ll;
    }
    /* -----------------5/8/00 4:29PM--------------------
    This is only used if Alignment is == 1 or == 2, but
     code space is saved if 'if' statement is eliminated
     --------------------------------------------------*/
    Offset++;
    if(Offset >= ATA_WORDS_PER_PHY_SECTOR)
    {
      PhySector++;
      Offset = 0;
    }
    if(pAtaDrive->FatType != ATA_FAT_TYPE_32)
    {
      ll = _AtaReadWord(PhySector, pAtaDrive, Offset*2);
      qq = (AtaCluster)ll;
    }
    else
      qq = kk>>16;

    switch(Alignment)
    {
      case 0:
      {
        /* In lower 3/4 of word */
        kk = (kk & 0x0FFF);
        break;
      }
      case 1:
      {
        /* First nibble in first word */
        kk = ((qq<<4)&0x0FF0) | ((kk>>12)&0x000F);
        break;
      }
      case 2:
      {
        /* First byte in first word */
        kk = ((kk>>8)&0x00FF) | ((qq<<8)&0x0F00);
        break;
      }
      case 3:
      {
        /* In upper 3/4 of word */
        kk = (kk>>4)&0x0FFF;
        break;
      }
      default:
        break;
    }
    /* check to see if we reached the last cluster */
    if((kk&0x0FF0) == 0x0FF0)
    {
      *LastCluster = 1;
    }
    else
    {
      *LastCluster = 0;
    }
  } /* end FAT_TYPE_12 */
#endif
  else
  {
    /* currently unsupported partition (FAT) type */
    /* FatalErr(0xFAD); */
    while(1);
  }

  *NeededCluster = kk;
  return(pAtaDrive->Err_Status);
}

