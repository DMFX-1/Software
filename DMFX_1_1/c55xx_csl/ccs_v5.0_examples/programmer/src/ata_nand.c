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
*    ata_nand.c
**********************************************************************/
#include "ata_nand.h"
#include "nand.h"

NandStruct *NandFlashCtrl = (NandStruct *)NAND_FLASH_BASE_ADDRESS_CS2;

/* Local Function Prototypes */
void NandWaitloop(unsigned short count);


/******************************************************************************
* AtaNANDIssueReadCommand
* NOTE: "PhySector" is the LBA ( == NAND-sector minus 1 )
******************************************************************************/
AtaError AtaNANDIssueReadCommand(AtaSector PhySector, void *pAtaMediaState, AtaUint16 SectorCount)
{
   AtaNANDState *pNand = pAtaMediaState;

   /* SmartMedia reserves the first sector for CIS data */
   PhySector++;

   NandDeselect();                       /* This clears any previous CEx */
   NandIssueReadCommand(PhySector<<9, pNand);

   if (((AtaNANDState *)pAtaMediaState)->ColumnAddrBytes == 1) /* small-block */
   {
      ((AtaNANDState *)pAtaMediaState)->CurrentReadPage = PhySector;
      ((AtaNANDState *)pAtaMediaState)->ReadWordCount = 0;
   }
   else /* large-block */
   {
      ((AtaNANDState *)pAtaMediaState)->CurrentReadPage = PhySector >> 2; /* 4 sectors/page */
      ((AtaNANDState *)pAtaMediaState)->ReadWordCount = (PhySector & 0x3) << 8;
   }

#ifndef ENABLE_MULTISECTOR_READNWORDS
   /* assert(pNand->ReadSectorCount == 0);  */
   /* assert(SectorCount == 0);  */
   SectorCount = SectorCount; /* prevent compiler warning for unused parameter */
#else
   pNand->ReadSectorCount = SectorCount;
#endif

   return (ATA_ERROR_NONE);
}

/******************************************************************************
* AtaNANDReadNextWord
******************************************************************************/
AtaError AtaNANDReadNextWord(void *pAtaMediaState, AtaUint16 *pWord)
{
   return AtaNANDReadNextNWords(pAtaMediaState, pWord, 1);
}

/******************************************************************************
* AtaNANDReadNextNWords
******************************************************************************/
AtaError AtaNANDReadNextNWords(void *pAtaMediaState, AtaUint16 *pWord, AtaUint16 Count)
{
   AtaUint16 TempData1, TempData2;
   AtaUint16 ii, jj, num_words_per_page;
   /* num_word_mask;*/

   NandSelect();

   if (((AtaNANDState *)pAtaMediaState)->ColumnBits == 11)
   {
      num_words_per_page = 1024;
      /*num_word_mask = 0x03FF;*/
   }
   else /* ((AtaNANDState *)pAtaMediaState)->ColumnBits == 9 */
   {
      num_words_per_page = 256;
      /*num_word_mask = 0x0FF;*/
   }

#ifdef ENABLE_MULTISECTOR_READNWORDS
   do
   {
      /* -----------------04/07/2001 3:49PM----------------
      ii = number of words remaining to be read out of the
      current sector
      --------------------------------------------------*/
      ii = num_words_per_page - ((((AtaNANDState *)pAtaMediaState)->ReadWordCount) & num_word_mask);
      if (Count < ii)
      {
         ii = Count;
      }
      else
      {
         ((AtaNANDState *)pAtaMediaState)->ReadSectorCount--;
      }
#else
       ii = Count;
#endif

      /* -----------------04/07/2001 3:55PM----------------
      Read ii words to complete reading the current sector
       --------------------------------------------------*/
      ((AtaNANDState *)pAtaMediaState)->ReadWordCount += ii;
      Count -= ii;

      /**/
      if (((AtaNANDState *)pAtaMediaState)->dataBus16bit) /* 16-bit NAND */
      {
         for (jj=0; jj<ii; jj++)
         {
            if (pWord)
            {
               *pWord++ = NandFlashCtrl->data;
            }
         }
      }
      else /* 8-bit NAND */
      {
         for (jj=0; jj<ii; jj++)
         {
            TempData1 = NandFlashCtrl->data;
            TempData2 = NandFlashCtrl->data;
            if (pWord)
            {
               *pWord++ = (TempData2 << 8) | (TempData1 & 0xFF);
            }
         }
      }

#ifdef ENABLE_MULTISECTOR_READNWORDS

      /* -----------------04/07/2001 3:57PM----------------
      Issue read command for new sector if at the end of the
      current sector and a new sector is required
      --------------------------------------------------*/
      if ((((AtaNANDState *)pAtaMediaState)->ReadSectorCount != 0) &&
          (((AtaNANDState *)pAtaMediaState)->ReadWordCount > num_words_per_page - 1))
      {
         printf("this may cause problems with some devices");
         /* -----------------04/07/2001 2:38PM----------------
         Read past redundant bits
         --------------------------------------------------*/
         for (jj=0; jj<16; jj++)
         {
            TempData1 = NandFlashCtrl->data;
         }

         /* -----------------04/07/2001 3:08PM----------------
         Wait for new sector to become available
         --------------------------------------------------*/
         if (!NandTimeoutCheck(AtaNANDDataReadyStatusCheck,pAtaMediaState))
         {
            return (ATA_ERROR_TIMEOUT);
         }

         /* -----------------04/07/2001 2:42PM----------------
         Ready to read first value from the new sector
         --------------------------------------------------*/
         NandSelect();
         ((AtaNANDState *)pAtaMediaState)->ReadWordCount = 0;
      }
   } while((Count > 0) && (((AtaNANDState *)pAtaMediaState)->ReadSectorCount > 0));
#endif

   /* -----------------04/07/2001 2:54PM----------------
   Deselect NAND after last read
   --------------------------------------------------*/
   if
   (
      (((AtaNANDState *)pAtaMediaState)->ReadWordCount == num_words_per_page)
#ifdef ENABLE_MULTISECTOR_READNWORDS
       && (((AtaNANDState *)pAtaMediaState)->ReadSectorCount == 0)
#endif
   )
   {
      NandDeselect();
   }

   return (ATA_ERROR_NONE);
}

/******************************************************************************
* NandReadID
* Returns Manufacturer Code in upper-byte, Device Code in lower-byte
******************************************************************************/
unsigned int NandReadID()
{
   unsigned int temp;

   NandSelect();
   asm("\tnop");
   asm("\tnop");
   NandFlashCtrl->cmd = NAND_CMD_READ_ID;        /* Read ID Command */
   NandFlashCtrl->ale = NAND_READ_ID_ADDR;       /* Send ID address */
   NandWaitloop(3);  /* Give time for the EMIF to turn-around */
   temp = NandFlashCtrl->data << 8;              /* get maker code */
   temp |= (NandFlashCtrl->data & 0x00FF);       /* get device code */
   NandDeselect();
   return(temp);
}

#if 0 /* This was taken out due to lack of QT support for testing, and questions of reliability */
/******************************************************************************
* Nand_widthDetect
* Returns width of NAND (0 = 8-bit, 1 = 16-bit)
******************************************************************************/
int Nand_widthDetect(void *pAtaMediaState)
{
   unsigned int i, temp1, temp2;
   int num_ALEcycles=0;


   num_ALEcycles = ((AtaNANDState *)pAtaMediaState)->ColumnAddrBytes +
                   ((AtaNANDState *)pAtaMediaState)->RowAddrBytes;

   NandSelect();
   asm("\tnop");
   asm("\tnop");
   NandFlashCtrl->cmd = 0xAA00;                /* Send Read Command */
   for (i=0; i<num_ALEcycles; i++)
   {
      NandFlashCtrl->ale = 0xAA00;             /* Send address */
   }
   if (((AtaNANDState *)pAtaMediaState)->ColumnAddrBytes == 2)
   {
      NandFlashCtrl->cmd = 0xAA30;
   }
   NandWaitloop(10);
   if (!NandTimeoutCheck(AtaNANDDataReadyStatusCheck,pAtaMediaState))
   {
      return(ATA_ERROR_TIMEOUT);
   }

   NandSelect();
   temp1 = NandFlashCtrl->data;              /* read first word/byte  */
   NandWaitloop(20);
   NandFlashCtrl->cmd = 0x5500;                /* Send Read Command */
   for (i=0; i<num_ALEcycles; i++)
   {
      NandFlashCtrl->ale = 0x5500;             /* Send address */
   }
   if (((AtaNANDState *)pAtaMediaState)->ColumnAddrBytes == 2)
   {
      NandFlashCtrl->cmd = 0x5530;
   }
   NandWaitloop(10);
   if (!NandTimeoutCheck(AtaNANDDataReadyStatusCheck,pAtaMediaState))
   {
      return(ATA_ERROR_TIMEOUT);
   }

   NandSelect();
   temp2 = NandFlashCtrl->data;
   NandDeselect();

   if (temp1 != temp2)
      return(0); /* 8 bit */
   else
      return(1); /* 16 bit */
}
#endif

/******************************************************************************
* AtaNANDCommandReadyStatusCheck
******************************************************************************/
int AtaNANDCommandReadyStatusCheck(void *pAtaMediaState)
{
   return(AtaNANDDataReadyStatusCheck(pAtaMediaState));
}

/******************************************************************************
* NandIssueReadCommand
******************************************************************************/
void NandIssueReadCommand(unsigned long start_address, AtaNANDState *pNAND)
{
   NandSelect();

   if (pNAND->ColumnAddrBytes == 1) /* small-block */
   {
      if (start_address & 0x00000100) /* Test A8 */
      {
         NandFlashCtrl->cmd = NAND_CMD_READ_HIGH;
      }
      else
      {
         NandFlashCtrl->cmd = NAND_CMD_READ_LOW;
      }
   }
   else /* large-block */
   {
      NandFlashCtrl->cmd = NAND_CMD_READ;
   }
   NandSetAddress(start_address, 0, pNAND);
   NandWaitloop(3);    /* assume delay for R/B to go low */

   if (pNAND->ColumnAddrBytes != 1) /* large-block */
   {
      NandFlashCtrl->cmd = NAND_CMD_READ_CONFIRM;
   }
}

/******************************************************************************
* NandSetAddress
* The "page_only" parameter is used for the block-erase command.
******************************************************************************/
void NandSetAddress(unsigned long start_address, int page_only, AtaNANDState *pNAND)
{
   int ii, jj;
   unsigned long column_addr;

   if (!page_only)
   {
      if (pNAND->ColumnAddrBytes == 2)
      {
         column_addr = start_address & 0x000007FF; /* 11 bits of column address */
      }
      else /* pNAND->ColumnAddrBytes == 1 */
      {
         column_addr = start_address & 0x000001FF; /* 9 bits of column address  */
      }

      for (ii=0; ii<pNAND->ColumnAddrBytes; ii++)
      {
         NandFlashCtrl->ale = column_addr;
         column_addr >>= 8;
      }
   }
   start_address >>= pNAND->ColumnBits;       /* shift out column address  */
   for (jj=0; jj<pNAND->RowAddrBytes; jj++)
   {
      NandFlashCtrl->ale = start_address;
      start_address >>= 8;
   }
}

/******************************************************************************
* NandReadData
******************************************************************************/
unsigned char NandReadData()
{
   return (NandFlashCtrl->data);
}

/******************************************************************************
* NandTimeoutCheck
******************************************************************************/
int NandTimeoutCheck(int (fpStatusCheck)(void *ptr), void *ptr)
{
   int ii, jj;

   ii = 5000;
   do
   {
      jj = fpStatusCheck(ptr);
      ii--;
   } while((ii>0) && !jj);

   return(ii);
}

/******************************************************************************
* NandWaitloop
******************************************************************************/
void NandWaitloop(unsigned short count)
{
   volatile unsigned short counter = count;
   while (counter)
   {
      counter--;
   }
}
