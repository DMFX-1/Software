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

/*********************************************************************
*
* Description:
*	This function is to initialize the MMC card hardware and software
*
**********************************************************************/
/*********************************************************************
* Initial versioin is from old unknown project
* Revision 1,0:  yihe Hu
*
* $History: ata_mmc_system_init.c $
*
*********************************************************************/
#include <stdio.h>
#include <string.h>
#include "ata.h"
#include "ata_.h"
#include "mmc_sd_c55.h"
#include "ata_mmc.h"
#include "mmc.h"

/*******************************************************************
* AtaError MMC_initState(void *pDrive)
* input:
*			viud 		pDrive
* Output:
*			None
* Return:
*			ATA_ERRIR_NONE					:reset done
*Description:
*		This function is to init the MMC software internal state
********************************************************************/
AtaError MMC_initState(void *pDrive)
{
MMC_internals *MMCSTATE;
AtaMMCState *AtaMMCState1 = ((AtaState *)pDrive)->pAtaMediaState;
MMCSTATE=(MMC_internals *)AtaMMCState1->Buf;

   AtaMMCState1->mmcptr=MMCSTATE;
   memset( &MMCSTATE->CardStatus,0, sizeof(CARD_STATUS));
   memset( &MMCSTATE->CardCid,	0, sizeof(CARD_CID));
   memset( &MMCSTATE->CardCsd,   0, sizeof(CARD_CSD));
   AtaMMCState1->pDrive = (AtaState *)pDrive;
   AtaMMCState1->currentsectorR = 0;
   AtaMMCState1->currentwordR = 0;

   ((AtaState *)pDrive)->AtaReset=MMC_reset;
   ((AtaState *)pDrive)->AtaBusyStatusCheck=MMC_busyStatusCheck;
   ((AtaState *)pDrive)->AtaGetSerialID=MMC_getSerialID;
   ((AtaState *)pDrive)->AtaIssueReadCommand=MMC_issueReadCommand;
   ((AtaState *)pDrive)->AtaCommandReadyStatusCheck=MMC_commandReadyStatusCheck;
   ((AtaState *)pDrive)->AtaDataReadyStatusCheck=MMC_dataReadyStatusCheck;
   ((AtaState *)pDrive)->AtaReadNextWord=MMC_readNextWord;
   ((AtaState *)pDrive)->AtaReadNextNWords=MMC_readNextNWords;
   ((AtaState *)pDrive)->AtaWriteSector=MMC_writeSector;
   ((AtaState *)pDrive)->AtaWriteSectorFlush=MMC_writeSectorFlush;
   ((AtaState *)pDrive)->AtaInitAtaMediaState=MMC_initState;
   ((AtaState *)pDrive)->AtaErrorCallback=NULL;

   return(ATA_ERROR_NONE);
}
/*******************************************************************
*	AtaError MMC_systemInit(AtaState *pAtaDrive)
* input:
*			AtaState		pAtaDrive
* Output:
*			None
* Return:
*			ATA_ERROR_BAD_MEDIA			mmc card is not good
*			ATA_ERRIR_NONE					:reset done
*Description:
*		This function is to init the MMC hardware/software
********************************************************************/
AtaError MMC_systemInit(AtaState *pAtaDrive)
{

register AtaSector BootPhySector;
int ii;
AtaUint16 ReservedSectorCount;
AtaSector ClusterCount;
AtaUint16 jumpcode = 0;

	//set for ATA high level API
   pAtaDrive->AtaReset=MMC_reset;
   pAtaDrive->AtaBusyStatusCheck=MMC_busyStatusCheck;
   pAtaDrive->AtaGetSerialID=MMC_getSerialID;
   pAtaDrive->AtaIssueReadCommand=MMC_issueReadCommand;
   pAtaDrive->AtaCommandReadyStatusCheck=MMC_commandReadyStatusCheck;
   pAtaDrive->AtaDataReadyStatusCheck=MMC_dataReadyStatusCheck;
   pAtaDrive->AtaReadNextWord=MMC_readNextWord;
   pAtaDrive->AtaReadNextNWords=MMC_readNextNWords;
   pAtaDrive->AtaWriteSector=MMC_writeSector;
   pAtaDrive->AtaWriteSectorFlush=MMC_writeSectorFlush;
   pAtaDrive->AtaInitAtaMediaState=MMC_initState;
   pAtaDrive->AtaErrorCallback=NULL;

   pAtaDrive->CurrentPhySector = ATA_INVALID_SECTOR;
   pAtaDrive->Err_Status = ATA_ERROR_NONE;

   if(pAtaDrive->AtaReset(pAtaDrive->pAtaMediaState)!=ATA_ERROR_NONE)
      return(ATA_ERROR_MEDIA_NOT_FOUND);

   //in the real test, found that sector 0 is be formated to boot sector
   //in this case, we will skip the FatType and bootPhySector check, set the
   //BootPhySector to 0 directly and go to grab the card information
   jumpcode = (AtaMMCReadByte(0L,pAtaDrive,0) & 0xFF) << 8;
   jumpcode = (AtaMMCReadByte(0l,pAtaDrive,2) & 0xFF) | jumpcode;
   if(jumpcode == 0xEB90)
   {
      pAtaDrive->BootPhySector = 0;
   }
	else
	{
      /* Read first partition entry in master boot record						 */
      /* Use the first partition found */
      pAtaDrive->FatType = 0;
      for(ii=0; ii<4 && !pAtaDrive->FatType; ii++)				 /* Loop over each partition entry */
      {
		   pAtaDrive->FatType = AtaMMCReadByte(0L, pAtaDrive, (0x1C2+(ii*0x10)));
		   pAtaDrive->BootPhySector = (AtaSector)(AtaMMCReadDoubleWord(0L, pAtaDrive, 0x1C6+(ii*0x10)));
		}

      if(AtaMMCReadWord(0L, pAtaDrive, 0x1FE) != 0xAA55) /* Check signature word */
      {
         return(ATA_ERROR_BAD_MEDIA);
      }
   }
   BootPhySector = pAtaDrive->BootPhySector;

   pAtaDrive->WordsPerLogSector = AtaMMCReadWord(BootPhySector, pAtaDrive, 0xB) / 2;
   pAtaDrive->LogSectorsPerCluster = AtaMMCReadByte(BootPhySector, pAtaDrive, 0xD);
   ReservedSectorCount = AtaMMCReadWord(BootPhySector, pAtaDrive, 0xE);
   pAtaDrive->NumberOfFats = AtaMMCReadByte(BootPhySector, pAtaDrive, 0x10);
   pAtaDrive->RootDirEntries = AtaMMCReadWord(BootPhySector, pAtaDrive, 0x11);
   pAtaDrive->TotalSectors = (AtaSector)AtaMMCReadWord(BootPhySector, pAtaDrive, 0x13);
   pAtaDrive->LogSectorsPerFat = (AtaSector)AtaMMCReadWord(BootPhySector, pAtaDrive, 0x16);

   if(pAtaDrive->TotalSectors == 0)
   {
      pAtaDrive->TotalSectors = (AtaSector)AtaMMCReadDoubleWord(BootPhySector, pAtaDrive, 0x20);			/* Total Sector Count32 byte 32 - 35 */
   }

   if(pAtaDrive->LogSectorsPerFat == 0)
   {
      pAtaDrive->LogSectorsPerFat = (AtaUint32)AtaMMCReadDoubleWord(BootPhySector, pAtaDrive, 0x24);			/* Fat Size/Sector Count  byte 36 - 39 */
   }

	/*****************************************
	  Compute number of words per cluster
	*****************************************/
   pAtaDrive->WordsPerCluster = pAtaDrive->WordsPerLogSector*pAtaDrive->LogSectorsPerCluster;

	/*****************************************
	  Compute start of FAT table
	*****************************************/
   pAtaDrive->FirstFatSector =	BootPhySector + 	(AtaSector)ReservedSectorCount;

	/*****************************************
	  Compute start of root directory
	*****************************************/
   pAtaDrive->RootDirSector =	pAtaDrive->FirstFatSector +	((AtaSector)pAtaDrive->NumberOfFats * (AtaSector)pAtaDrive->LogSectorsPerFat);

	/*****************************************
	  Compute start of first data sector
	   Note: the sector needs to be rounded up as done here
	*****************************************/
   pAtaDrive->FirstDataSector =	pAtaDrive->RootDirSector +	(AtaSector)
	     ((((AtaUint16)pAtaDrive->RootDirEntries*16) + ((AtaUint16)pAtaDrive->WordsPerLogSector-1)) /
		  (AtaUint16)pAtaDrive->WordsPerLogSector);

	/*****************************************
	  Compute the number of clusters to determine the FAT  type
	*****************************************/
   ClusterCount = pAtaDrive->TotalSectors - pAtaDrive->FirstDataSector;

   /*****************************************************
   Note that this for loop takes the place of a 32-bit by
   32-bit division which locks up the system when you are
   using FAT32.  This for loop does not increase the code
   size.
   *****************************************************/
   for(ii = pAtaDrive->LogSectorsPerCluster; ii > 1; ii=ii>>1)
   {
      ClusterCount = ClusterCount >> 1;
   }

   if(ClusterCount < 4085)
   {
      pAtaDrive->FatType = ATA_FAT_TYPE_12;
   }
   else if(ClusterCount < 65525)
   {
      pAtaDrive->FatType = ATA_FAT_TYPE_16;
   }
   else
   {
      pAtaDrive->FatType = ATA_FAT_TYPE_32;
   }

   pAtaDrive->MFWFlag = 0;
   pAtaDrive->_AtaWriteCurrentPhySector = ATA_INVALID_SECTOR;

   return(ATA_ERROR_NONE);
}
/*******************************************************************
* AtaUint16 AtaMMCReadWord(AtaSector PhySector, AtaState *pAtaDrive, AtaUint16 ByteOffset)
* input:
*			PhySector	sector to be accessed
*			void 		pDrive
*			byteoffset	offset from beginning of the sector
* Output:
*			None
* Return:
*			word
*Description:
*		This fucntion is to read word from assigned locatioin
********************************************************************/
AtaUint16 AtaMMCReadWord(AtaSector PhySector, AtaState *pAtaDrive, AtaUint16 ByteOffset)
{

AtaUint16 Word = ByteOffset >> 1;
AtaUint16 tempword;

   if((PhySector != pAtaDrive->CurrentPhySector) || ((ByteOffset >> 1) <pAtaDrive->CurrentWord ))
   {
      /* Flush the current sector from the ATA device */
      AtaMMCFlush(pAtaDrive);

      /* Setup current sector/word pointers */
      pAtaDrive->CurrentPhySector = PhySector;
      pAtaDrive->CurrentWord = 0;

      /* Start a read from the new sector */
      pAtaDrive->AtaIssueReadCommand(PhySector, pAtaDrive->pAtaMediaState, 1);
      pAtaDrive->AtaReadNextWord(pAtaDrive->pAtaMediaState, &pAtaDrive->Data);

   }
   for(;pAtaDrive->CurrentWord<Word;pAtaDrive->CurrentWord++)
   {
      pAtaDrive->AtaReadNextWord(pAtaDrive->pAtaMediaState, &pAtaDrive->Data);
   }
   tempword = pAtaDrive->Data;

   if(ByteOffset & 1)
   {
      pAtaDrive->AtaReadNextWord(pAtaDrive->pAtaMediaState,&pAtaDrive->Data);
      tempword = (pAtaDrive->Data << 8) | ((tempword >> 8) & 0xFF);
      pAtaDrive->CurrentWord +=1;
   }

   return(tempword);
}
/*******************************************************************
char AtaMMCReadByte(AtaSector PhySector, AtaState *pAtaDrive, AtaUint16 ByteOffset)
* input:
*			PhySector	sector to be accessed
*			void 		pDrive
*			byteoffset	offset from beginning of the sector
* Output:
*			None
* Return:
*			byte
*Description:
*		This fucntion is to read byte from assigned locatioin
********************************************************************/
char AtaMMCReadByte(AtaSector PhySector, AtaState *pAtaDrive, AtaUint16 ByteOffset)
{
char ll;

   ll=(char)(AtaMMCReadWord(PhySector, pAtaDrive, ByteOffset) & 0xFF);
   return(ll);
}
/*******************************************************************
*	AtaUint32 AtaMMCReadDoubleWord(AtaSector PhySector, AtaState *pAtaDrive, AtaUint16 ByteOffset)
* input:
*			PhySector	sector to be accessed
*			void 		pDrive
*			byteoffset	offset from beginning of the sector
* Output:
*			None
* Return:
*			double word
*Description:
*		This fucntion is to read double word from assigned location
********************************************************************/
AtaUint32 AtaMMCReadDoubleWord(AtaSector PhySector, AtaState *pAtaDrive, AtaUint16 ByteOffset)
{
AtaUint32 ll;

   ll  =  (AtaUint32)AtaMMCReadWord(PhySector, pAtaDrive, ByteOffset);
   ll |=  ((AtaUint32)AtaMMCReadWord(PhySector, pAtaDrive, ByteOffset+2)) << 16;

   return(ll);
}
/*******************************************************************
* int AtaMMCTimeoutCheck(int (fpStatusCheck)(void *ptr), void *ptr)
* input:
*			void 		pDrive
* Output:
*			None
* Return:
*			error ststus
*Description:
*		time out check
********************************************************************/
int AtaMMCTimeoutCheck(int (fpStatusCheck)(void *ptr), void *ptr)
{
int ii, jj;

   ii = 1000;
   do
   {
      jj = fpStatusCheck(ptr);
      ii--;
   } while((ii>0) && !jj);

   return(ii);
}
/*******************************************************************
* AtaError AtaMMCFlush(AtaState *pAtaDrive)
* input:
*			void 		pDrive
* Output:
*			None
* Return:
*			error ststus
*Description:
*		This function is to flush the current read sector
********************************************************************/
AtaError AtaMMCFlush(AtaState *pAtaDrive)
{

int ii;
AtaError error = ATA_ERROR_NONE;
   /* Flush the current sector from the ATA device */
   if(pAtaDrive->CurrentPhySector != ATA_INVALID_SECTOR)
   {
      error = pAtaDrive->AtaBusyStatusCheck(pAtaDrive->pAtaMediaState);

      for(ii=pAtaDrive->CurrentWord + 1; ii<ATA_WORDS_PER_PHY_SECTOR; ii++)
      {
         error += pAtaDrive->AtaReadNextWord(pAtaDrive->pAtaMediaState, &pAtaDrive->Data);
      }
   }

   pAtaDrive->CurrentPhySector = ATA_INVALID_SECTOR;
   pAtaDrive->CurrentWord = ATA_INVALID_WORD;
   return(error);
}

