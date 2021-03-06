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
/* $Id: atacreate.c,2.4+ $													    */
/*										    									*/
/* This is proprietary information, not to be published -- TI INTERNAL DATA   	*/
/* Copyright (C) 2000, Texas Instruments, Inc.  All Rights Reserved.		  	*/
/*																				*/
/* Author: Susmit Pal													  		*/
/* Date: November 6, 2002														*/
/* Modified Date: November 28, 2002												*/
/* Last Modified Date: February 11, 2003										*/
/********************************************************************************/

#include <stdlib.h>
#include "ata_.h"

AtaError _AtaFindNextAvailableCluster(AtaFile *pAtaFile, int write_fat)
{
  int found_cluster = 0;
  int last_cluster = 0;
  int free_cluster_count = 0;

  AtaState *pAtaDrive = pAtaFile->pDrive;

  AtaCluster Cluster;
  AtaCluster NumClusters;
  AtaError ret_stat = ATA_ERROR_NONE;
  AtaCluster tempCluster;

/*  NumClusters = pAtaDrive->TotalSectors/pAtaDrive->LogSectorsPerCluster;*//*Original - Susmit*/
  NumClusters = (AtaCluster)((pAtaDrive->TotalSectors-pAtaDrive->RootDirSector)/pAtaDrive->LogSectorsPerCluster);/*For exact free disk space check - Susmit*/
#if 0
#ifndef DISABLE_FAT12
  if(pAtaDrive->FatType == ATA_FAT_TYPE_12)
  {
	NumClusters *= 4;
	NumClusters /= 3;
  }
#endif
#endif
  Cluster = pAtaFile->Cluster;

  while(1)
  {
	/* -----------------9/10/99 8:01AM-------------------
	Point to the next cluster
	 --------------------------------------------------*/
	Cluster++;

	/* -----------------9/3/99 4:31PM--------------------
	Check to see if we ran out of space on the disk
	 --------------------------------------------------*/
	if(Cluster >= NumClusters)
	{
	  if(!found_cluster)
	  {
/*		return(1);*//* Original*/
		return(ATA_ERROR_DISK_FULL); /*susmit - Might check for disk full from here*/
	  }
	  else
	  {
		/* -----------------9/3/99 5:09PM--------------------
		At least one available cluster has been found
		 --------------------------------------------------*/
		return(0);
	  }
	}

	/* -----------------9/3/99 4:32PM--------------------
	Check to see if the cluster is available
	 --------------------------------------------------*/
	ret_stat = _AtaReadFatEntry(Cluster, pAtaDrive, &last_cluster, &tempCluster);
	if(ret_stat) return(ret_stat);
	if(tempCluster == 0)
	{
	  /* -----------------4/26/00 12:22PM------------------
	  Need check here to prevent wasting time scanning
	   the entire fat if we have more than about 0x800 free
	   contiguous clusters.  If this case is verified, we
	   should return(0);
	   --------------------------------------------------*/
	   free_cluster_count++;
/*	   if(free_cluster_count>=256) return(0);*/ /*SUSMIT - LATEST cluster finding opotimzations*/
/*SUSMIT- Newer OPtimizations for MFW*/
		if(pAtaDrive->MFWFlag == 1)
		{if(free_cluster_count>=4) return(0);}
		else {if(free_cluster_count>=256) return(0);}
	  /* -----------------4/26/00 12:24PM------------------
	  An available cluster was found
	   --------------------------------------------------*/
	  if(!found_cluster)
	  {
		/* -----------------9/3/99 5:09PM--------------------
		First available cluster found
		 --------------------------------------------------*/
		pAtaFile->Cluster = Cluster;
		found_cluster = 1;
		if(write_fat)
		{
		  ret_stat = _AtaWriteFatEntry(pAtaFile,0);
		  if(ret_stat) return(ret_stat);
		}
		pAtaFile->PrevBrokenCluster = Cluster;

	  }
	  pAtaFile->NextBrokenCluster = Cluster;
	}
	else if(found_cluster)
	{
	  /* -----------------9/3/99 5:09PM--------------------
	  At least one available cluster has been found
	   --------------------------------------------------*/
	  return(0);
	}
  }
}

