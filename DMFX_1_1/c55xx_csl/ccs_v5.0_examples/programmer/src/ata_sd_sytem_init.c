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

/********************************************************************
* This file includes subroutines used by the ATA higher level API	*
*********************************************************************
* Initial versioin is from old unknow project						*
* Revision 1.0 1/08/08  Yihe Hu		 	                        	*
********************************************************************/
#include <stdio.h>
#include "ata.h"
#include "ata_.h"
#include "ata_sd.h"


/*******************************************************************
* AtaError SD_systemInit(AtaState *pAtaDrive)
* input:
*			AtaState		pAtaDrive
* Output:
*			None
* Return:
*			ATA_ERROR_BAD_MEDIA			mmc card is not good
*			ATA_ERRIR_NONE					:reset done
*Description:
*		This function is to init the SD hardware/software
********************************************************************/
AtaError SD_systemInit(AtaState *pAtaDrive)
{

register AtaSector BootPhySector;
int ii;
AtaUint16 ReservedSectorCount;
#ifndef OLD_FAT_TYPE_DETECTION
  #ifdef ENABLE_LONGDIV
	  AtaSector ClusterCount;
  #else
	  AtaUint16 ClusterCount;
   #endif
#endif


   pAtaDrive->AtaReset=AtaSDReset;
   pAtaDrive->AtaBusyStatusCheck=AtaSDBusyStatusCheck;
   pAtaDrive->AtaGetSerialID=AtaSDGetSerialID;
   pAtaDrive->AtaIssueReadCommand=AtaSDIssueReadCommand;
   pAtaDrive->AtaDataReadyStatusCheck=AtaSDDataReadyStatusCheck;
   pAtaDrive->AtaReadNextWord=AtaSDReadNextWord;
   pAtaDrive->AtaWriteSector=AtaSDWriteSector;
   pAtaDrive->AtaWriteSectorFlush=AtaSDWriteSectorFlush;
   pAtaDrive->AtaInitAtaMediaState=NULL;
   pAtaDrive->AtaErrorCallback=NULL;


   pAtaDrive->CurrentPhySector = ATA_INVALID_SECTOR;

   //reset and init hardware and software
   pAtaDrive->AtaReset(pAtaDrive->pAtaMediaState);

  // Read first partition entry in master boot record, use the first partition found
   pAtaDrive->FatType = 0;
   for(ii=0; ii<4 && !pAtaDrive->FatType; ii++)				 /* Loop over each partition entry */
   {
      pAtaDrive->FatType = AtaSDReadByte(0L, pAtaDrive, (0x1C2+(ii*0x10)));
      pAtaDrive->BootPhySector = (AtaSector)(AtaSDReadDoubleWord(0L, pAtaDrive, 0x1C6+(ii*0x10)));
   }
   BootPhySector = pAtaDrive->BootPhySector;

   if(AtaSDReadWord(0L, pAtaDrive, 0x1FE) != 0xAA55) /* Check signature word */
   {
      return(ATA_ERROR_BAD_MEDIA);
   }

   pAtaDrive->WordsPerLogSector = AtaSDReadWord(BootPhySector, pAtaDrive, 0xB) / 2;
   pAtaDrive->LogSectorsPerCluster = AtaSDReadByte(BootPhySector, pAtaDrive, 0xD);
   ReservedSectorCount = AtaSDReadWord(BootPhySector, pAtaDrive, 0xE);
   pAtaDrive->NumberOfFats = AtaSDReadByte(BootPhySector, pAtaDrive, 0x10);
   pAtaDrive->RootDirEntries = AtaSDReadWord(BootPhySector, pAtaDrive, 0x11);
   pAtaDrive->TotalSectors = (AtaSector)AtaSDReadWord(BootPhySector, pAtaDrive, 0x13);
   pAtaDrive->LogSectorsPerFat = (AtaSector)AtaSDReadWord(BootPhySector, pAtaDrive, 0x16);

   if(pAtaDrive->TotalSectors == 0)
   {
      pAtaDrive->TotalSectors = (AtaSector)AtaSDReadDoubleWord(BootPhySector, pAtaDrive, 0x20);			/* Total Sector Count32 byte 32 - 35 */
   }

   if(pAtaDrive->LogSectorsPerFat == 0)
   {
      pAtaDrive->LogSectorsPerFat = (AtaUint32)AtaSDReadDoubleWord(BootPhySector, pAtaDrive, 0x24);			/* Fat Size/Sector Count  byte 36 - 39 */
   }

   //  Compute number of words per cluster
   pAtaDrive->WordsPerCluster = pAtaDrive->WordsPerLogSector*pAtaDrive->LogSectorsPerCluster;

   //Compute start of FAT table
   pAtaDrive->FirstFatSector =  BootPhySector +    (AtaSector)ReservedSectorCount;

   //   Compute start of root directory
   pAtaDrive->RootDirSector = pAtaDrive->FirstFatSector +
        ((AtaSector)pAtaDrive->NumberOfFats * (AtaSector)pAtaDrive->LogSectorsPerFat);

   //  Compute start of first data sector,Note: the sector needs to be rounded up as done here
   pAtaDrive->FirstDataSector =  pAtaDrive->RootDirSector +
         (AtaSector)((((AtaUint16)pAtaDrive->RootDirEntries*16) + ((AtaUint16)pAtaDrive->WordsPerLogSector-1)) /
           (AtaUint16)pAtaDrive->WordsPerLogSector);

   //  Compute the number of clusters to determine the FAT  type
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
* AtaUint16 AtaSDReadWord(AtaSector PhySector, AtaState *pAtaDrive,
	AtaUint16 ByteOffset)
* input:
*			physector		sector to be accessed
*			AtaState		pAtaDrive
*			byteoffset		offset
* Output:
*			None
* Return:
*			word
*Description:
*		This function is to read word from assigned locatioin
********************************************************************/
AtaUint16 AtaSDReadWord(AtaSector PhySector, AtaState *pAtaDrive, AtaUint16 ByteOffset)
{
AtaUint16 Word = ByteOffset >> 1;
AtaUint16 tempword;

   if((PhySector != pAtaDrive->CurrentPhySector) || (Word < pAtaDrive->CurrentWord))
   {
      // Flush the current sector from the ATA device
      AtaSDFlush(pAtaDrive);

      //  Setup current sector/word pointers
      pAtaDrive->CurrentPhySector = PhySector;
      pAtaDrive->CurrentWord = 0;

      // Start a read from the new sector
      pAtaDrive->AtaBusyStatusCheck(pAtaDrive->pAtaMediaState);
      pAtaDrive->AtaIssueReadCommand(PhySector, pAtaDrive->pAtaMediaState, 1);

      // Read the first word
      pAtaDrive->AtaBusyStatusCheck(pAtaDrive->pAtaMediaState);

      AtaSDTimeoutCheck(pAtaDrive->AtaDataReadyStatusCheck,pAtaDrive->pAtaMediaState);
      pAtaDrive->AtaReadNextWord(pAtaDrive->pAtaMediaState, &pAtaDrive->Data);

   }

   // Read data until we read the desired word
   for(; pAtaDrive->CurrentWord<Word; pAtaDrive->CurrentWord++)
   {
      pAtaDrive->AtaReadNextWord(pAtaDrive->pAtaMediaState, &pAtaDrive->Data);
   }


   // Read the desired word
   tempword = pAtaDrive->Data;

   //  If the ByteOffset is odd, take the LSB from the MSB
   //   of the first word and the MSB from the LSB of the
   //   next word.
   if(ByteOffset & 0x0001)
   {
      pAtaDrive->AtaReadNextWord(pAtaDrive->pAtaMediaState, &pAtaDrive->Data);
      pAtaDrive->CurrentWord++;
      tempword = (pAtaDrive->Data << 8) | ((tempword >> 8) & 0xFF);
   }
   return(tempword);
}
/*******************************************************************
* char AtaSDReadByte(AtaSector PhySector, AtaState *pAtaDrive, AtaUint16 ByteOffset)
* input:
*			physector		sector to be accessed
*			AtaState		pAtaDrive
*			byteoffset		offset
* Output:
*			None
* Return:
*			byte
*Description:
*		This function is to read the byte
********************************************************************/
char AtaSDReadByte(AtaSector PhySector, AtaState *pAtaDrive, AtaUint16 ByteOffset)
{
char ll;

   ll=(char)(AtaSDReadWord(PhySector, pAtaDrive, ByteOffset) & 0xFF);
   return(ll);
}
/*******************************************************************
* AtaUint32 AtaSDReadDoubleWord(AtaSector PhySector, AtaState *pAtaDrive,
*							 AtaUint16 ByteOffset)
* input:
*			physector		sector to be accessed
*			AtaState		pAtaDrive
*			byteoffset		offset
* Output:
*			None
* Return:
*			byte
*Description:
*		This function is to read the double word
********************************************************************/
AtaUint32 AtaSDReadDoubleWord(AtaSector PhySector, AtaState *pAtaDrive, AtaUint16 ByteOffset)
{
AtaUint32 ll;

   ll  =  (AtaUint32)AtaSDReadWord(PhySector, pAtaDrive, ByteOffset);
   ll |=  ((AtaUint32)AtaSDReadWord(PhySector, pAtaDrive, ByteOffset+2)) << 16;

   return(ll);
}
/*******************************************************************
* int AtaSDTimeoutCheck(AtaError (fpStatusCheck)(void *ptr), void *ptr)
* input:
* Output:
*			None
* Return:
*Description:
*		This function is to check time out
********************************************************************/
int AtaSDTimeoutCheck(AtaError (fpStatusCheck)(void *ptr), void *ptr)
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
* AtaError AtaSDFlush(AtaState *pAtaDrive)
* input:
*			AtaState		pAtaDrive
* Output:
*			None
* Return:
*			ATA_ERRIR_NONE
*Description:
*		This function is to flush current sector
********************************************************************/
AtaError AtaSDFlush(AtaState *pAtaDrive)
{
int ii;

  // Flush the current sector from the ATA device
   if(pAtaDrive->CurrentPhySector != ATA_INVALID_SECTOR)
   {
      pAtaDrive->AtaBusyStatusCheck(pAtaDrive->pAtaMediaState);
      for(ii=pAtaDrive->CurrentWord+1; ii<ATA_WORDS_PER_PHY_SECTOR; ii++)
      {
         pAtaDrive->AtaReadNextWord(pAtaDrive->pAtaMediaState, &pAtaDrive->Data);
      }

   }
   pAtaDrive->CurrentPhySector = ATA_INVALID_SECTOR;
   pAtaDrive->CurrentWord = ATA_INVALID_WORD;
   return(ATA_ERROR_NONE);
}




