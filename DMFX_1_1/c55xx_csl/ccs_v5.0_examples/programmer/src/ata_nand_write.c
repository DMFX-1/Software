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

/******************************************************************************
*    ata_nand_write.c
******************************************************************************/
#include <string.h>                     /* for memmove() prototype */
#include "ata_nand.h"                   /* also includes ata.h */
#include "nand.h"

extern NandStruct *NandFlashCtrl;

AtaUint16 NandReadStatus(AtaUint16 *kk);

/******************************************************************************
* AtaNANDWriteSector
* This function handles writing a complete single sector.
* NOTE: "PhySector" is the LBA ( == NAND-sector minus 1 )
******************************************************************************/
AtaError AtaNANDWriteSector(AtaSector PhySector, void *pAtaMediaState, AtaUint16 *Word, int ByteSwap)
{
   AtaUint16 ii;
   AtaSector NandPage;
   AtaUint32 NandBlockFirstPage;
   AtaSector NandSector;
   AtaError ret_stat = ATA_ERROR_NONE;
   AtaNANDState *pNand = pAtaMediaState;
   unsigned long mask = ~((pNand->pagesPerBlock) - 1);
   AtaUint16 *WriteBufPtr;

   /* SmartMedia reserves the first sector for CIS data */
   PhySector++;      /* PhySector is initially LBA, this converts it to equivalent NAND PhySector */

   if (pNand->ColumnAddrBytes == 1) /* small-block */
   {
      NandPage = PhySector;
   }
   else /* large-block */
   {
      NandPage = PhySector >> 2; /* 4 sectors/page */
   }

   /* Determine the block being modified */
   NandBlockFirstPage = NandPage & mask;

   /* If we are accessing a new NandBlock, flush the old one */
   if ((pNand->WritePending) && (pNand->WritePage != NandBlockFirstPage))
   {
      AtaNANDWriteSectorFlush(pAtaMediaState);
   }

   /* Read in the NAND physical block (to be partially overwritten with new data) */
   if (!pNand->WritePending)
   {
      pNand->WritePage = NandBlockFirstPage;

      for (ii=0; ii<(pNand->pagesPerBlock); ii++)
      {
         /* AtaNANDIssueReadCommand expects an LBA (sector) */
         NandSector = (NandBlockFirstPage + ii) * pNand->sectorsPerPage;
         AtaNANDIssueReadCommand(NandSector-1, pAtaMediaState, 1);
         if (!NandTimeoutCheck(AtaNANDDataReadyStatusCheck, pAtaMediaState))
         {
            return(ATA_ERROR_TIMEOUT);
         }

         WriteBufPtr = pNand->pWriteBuffer + (ii * pNand->wordsPerPage);
         ret_stat = AtaNANDReadNextNWords(pAtaMediaState, WriteBufPtr, pNand->wordsPerPage);
         if (ret_stat)
         {
            return (ret_stat);
         }
      }

      /* Remember that the block needs to be committed to disk */
      pNand->WritePending = 1;
   }

   /* Copy the sector data (sectors are 256 words) */
   NandSector = PhySector - (NandBlockFirstPage * pNand->sectorsPerPage);
   WriteBufPtr = pNand->pWriteBuffer + (NandSector * ATA_WORDS_PER_PHY_SECTOR);
   if (ByteSwap)
   {
      for (ii=0; ii<ATA_WORDS_PER_PHY_SECTOR; ii++)
      {
         WriteBufPtr[ii] = Word[ii]<<8 | ((Word[ii]&0xff00)>>8);
      }
   }
   else
   {
      memmove(WriteBufPtr, &Word[0], ATA_WORDS_PER_PHY_SECTOR);
   }

   return (ret_stat);
}

/******************************************************************************
* NandReadStatus
******************************************************************************/
AtaUint16 NandReadStatus(AtaUint16 *kk)
{
   int jj = 1000;
   do
   {
      NandFlashCtrl->cmd = NAND_CMD_STATUS;
      *kk = NandFlashCtrl->data;
      jj--;
   } while (!(*kk & NAND_STATUS_READY) && (jj > 0));
   return (jj);
}

/******************************************************************************
* AtaNANDWriteSectorFlush
******************************************************************************/
AtaError AtaNANDWriteSectorFlush(void *pAtaMediaState)
{
   AtaUint16 ii, jj, kk;
   AtaUint32 NandPhySector;
   AtaNANDState* pNand = pAtaMediaState;
   AtaUint16 *WriteBufPtr;

   if (pNand->WritePending)
   {
      pNand->WritePending = 0;
      pNand->CurrentReadPage = 0; /* reset the read logic */

      /* Erase the block */
      NandSelect();
      NandFlashCtrl->cmd = NAND_CMD_BLOCK_ERASE;          /* Erase block setup */
      NandPhySector = pNand->WritePage * pNand->sectorsPerPage;
      NandSetAddress(NandPhySector<<9, 1, pNand);         /* sectors are always 512 bytes */
      NandFlashCtrl->cmd  = NAND_CMD_BLOCK_ERASE_CONFIRM; /* Erase block commit */
      if (!NandTimeoutCheck(AtaNANDCommandReadyStatusCheck, pAtaMediaState))
      {
         return (ATA_ERROR_TIMEOUT);
      }
      NandSelect();
      NandFlashCtrl->cmd = NAND_CMD_STATUS;               /* Read status command */
      ii = NandFlashCtrl->data;                           /* fetch status */
      NandDeselect();
      if (ii & NAND_STATUS_FAIL)
      {
         return (ATA_ERROR_BAD_MEDIA);  /* Erase error */
      }

      /* Program the block (all pages -- in order, one page at a time) */
      WriteBufPtr = pNand->pWriteBuffer;
      for (ii=0; ii<(pNand->pagesPerBlock); ii++)
      {
         NandSelect();
         NandFlashCtrl->cmd = NAND_CMD_READ;
         NandFlashCtrl->cmd = NAND_CMD_PAGE_PROGRAM;
         NandPhySector = (pNand->WritePage + ii) * pNand->sectorsPerPage;
         NandSetAddress(NandPhySector<<9, 0, pNand);
         for (jj=0; jj<pNand->wordsPerPage; jj++)
         {
            NandFlashCtrl->data = *WriteBufPtr;
            NandFlashCtrl->data = *WriteBufPtr >> 8;
            WriteBufPtr++;
         }
         NandFlashCtrl->cmd = NAND_CMD_PAGE_PROGRAM_CONFIRM;  /* Write commit */

         /* Wait for NAND to be ready to accept commands */
         if (!NandTimeoutCheck(AtaNANDCommandReadyStatusCheck, pAtaMediaState))
         {
            return (ATA_ERROR_TIMEOUT);
         }

         /* Verify programming completed successfully */
         NandSelect();
         jj = NandReadStatus(&kk);
         NandDeselect();
         if (jj <= 0)
         {
            return (ATA_ERROR_TIMEOUT);
         }
         else if (kk & NAND_STATUS_FAIL)
         {
            return (ATA_ERROR_BAD_MEDIA);
         }
      }
   }

   return (ATA_ERROR_NONE);
}
