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
*    mmc_sd.c
**********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "machine.h"
#include "sysctrl.h"
#include "ata.h"
#include "ata_mmc.h"
#include "ata_sd.h"
#include "mmc_sd.h"

AtaState AtaDriveMmcSd;
AtaState *pAtaDriveMmcSd = &AtaDriveMmcSd;
AtaUint16 MmcSdAtaWriteBuffer[ATA_WORDS_PER_PHY_SECTOR];
AtaUint16 MmcSdBuffer[1600]; // used for internal and read/write buffers
AtaMMCState MmcState = { 0, 0, 2, 0, 1, 1000, 0, 0 };
AtaSDState SdState = { 0, 4, 0, 0, 4, 1, 1, 0, NULL };
AtaFile MmcSdFile;

/* Local Function Prototypes */
AtaUint16 mmc_sd_get_time();
AtaUint16 mmc_sd_get_date();

/******************************************************************************
* mmc_sd_init
******************************************************************************/
int mmc_sd_init(Uint16 MmcSdBootType)
{
   if ((MmcSdBootType == MMC_SD_MMC0) || (MmcSdBootType == MMC_SD_MMC1))
   {
      if (MmcSdBootType == MMC_SD_MMC0)
         MmcState.mmcNumber = 0;
      else
         MmcState.mmcNumber = 1;

      /* Setup buffers */
      MmcState.Buf = (int *)MmcSdBuffer; // 200 words used
      MmcState.BufR = (unsigned short *)&MmcSdBuffer[200]; // 256 words used

      memset(pAtaDriveMmcSd, 0, sizeof(AtaState));
      memset(MmcState.Buf, 5, sizeof(int)*200);

      /* Initialize AtaState function pointers */
      AtaDriveMmcSd._AtaWriteBuffer = MmcSdAtaWriteBuffer;
      AtaDriveMmcSd.get_date = mmc_sd_get_date;
      AtaDriveMmcSd.get_time = mmc_sd_get_time;

      AtaDriveMmcSd.AtaInitAtaMediaState = MMC_initState;
      AtaDriveMmcSd.pAtaMediaState = &MmcState;

      pAtaDriveMmcSd->AtaInitAtaMediaState((void *)pAtaDriveMmcSd);

      if (MMC_systemInit(pAtaDriveMmcSd))
      {
         return 1; /* init error */
      }
   }
   else if ((MmcSdBootType == MMC_SD_SD0) || (MmcSdBootType == MMC_SD_SD1))
   {
      if (MmcSdBootType == MMC_SD_SD0)
         SdState.sdNumber = 0;
      else
         SdState.sdNumber = 1;

      /* Setup buffers */
      SdState.buffer = (int *)MmcSdBuffer;

      memset(pAtaDriveMmcSd, 0, sizeof(AtaState));

      /* Initialize AtaState function pointers */
      AtaDriveMmcSd._AtaWriteBuffer = MmcSdAtaWriteBuffer;
      AtaDriveMmcSd.get_date = mmc_sd_get_date;
      AtaDriveMmcSd.get_time = mmc_sd_get_time;

      AtaDriveMmcSd.AtaReset=AtaSDReset;
      AtaDriveMmcSd.AtaBusyStatusCheck=AtaSDBusyStatusCheck;
      AtaDriveMmcSd.AtaGetSerialID=AtaSDGetSerialID;
      AtaDriveMmcSd.AtaIssueReadCommand=AtaSDIssueReadCommand;
      AtaDriveMmcSd.AtaCommandReadyStatusCheck=AtaSDCommandReadyStatusCheck;
      AtaDriveMmcSd.AtaDataReadyStatusCheck=AtaSDDataReadyStatusCheck;
      AtaDriveMmcSd.AtaReadNextWord=AtaSDReadNextWord;
      AtaDriveMmcSd.AtaReadNextNWords=AtaSDReadNextNWords;
      AtaDriveMmcSd.AtaWriteSector=AtaSDWriteSector;
      AtaDriveMmcSd.AtaWriteSectorFlush=AtaSDWriteSectorFlush;
      AtaDriveMmcSd.AtaInitAtaMediaState=AtaSDInitState;

      AtaDriveMmcSd.pAtaMediaState = &SdState;

      if (ATA_systemInit(pAtaDriveMmcSd, 0))
      {
         return 1; /* init error */
      }
   }
   else
   {
      /* Unknown boot type */
      return 1;
   }
   return 0; /* success */
}

/******************************************************************************
* mmc_sd_file_list
******************************************************************************/
void mmc_sd_file_list(void)
{
   AtaError error;

   /* Get the first file/directory */
   error = ATA_fileInit(pAtaDriveMmcSd, &MmcSdFile);

   while (error == ATA_ERROR_NONE)
   {
      /* Check for a file (not directory) */
	   if (!(error=ATA_isDir(&MmcSdFile)))
	   {
         printf("%s.%s\n", MmcSdFile.Filename, MmcSdFile.Ext);
	   }

      /* Get the next file/directory (if there is one) */
	   error = ATA_findNext(&MmcSdFile);
   }
}

/******************************************************************************
* mmc_sd_file_read
******************************************************************************/
void mmc_sd_file_read(char *fileName)
{
   int ii;
   AtaError error;
   FILE *pOutputFile = NULL;
   unsigned short read_word;
   unsigned short FileData[2];
   size_t write_count;

   if (fileName[0] == '\0')
   {
      printf("Opening mmc_sd_readback.bin...\n");
      pOutputFile = fopen("mmc_sd_readback.bin","wb");
   }
   else
   {
      printf("Opening %s...\n", fileName);
      pOutputFile = fopen(fileName, "wb");
   }
   if (!pOutputFile)
   {
      printf("Error opening output file\n");
   }

   /* Get the first file/directory */
   error = ATA_fileInit(pAtaDriveMmcSd, &MmcSdFile);

   while (error == ATA_ERROR_NONE)
   {
      /* Check for a file (not directory) with name "BOOTIMG.BIN" */
	   if (!(ATA_isDir(&MmcSdFile)))
	   {
         /* Check for matching filename/extension */
	      if ((!strcmp(MmcSdFile.Filename, "BOOTIMG")) && (!strcmp(MmcSdFile.Ext, "BIN")))
         {
            error = ATA_seek(&MmcSdFile, (AtaFileSize)0);
            if (error != ATA_ERROR_NONE)
            {
               printf("Seek error\n");
               fclose(pOutputFile);
               return;
            }
            for (ii=0; ii<(MmcSdFile.Size>>1); ii++)
            {
               error = ATA_read(&MmcSdFile, (AtaUint16 *)&read_word, (AtaUint16)1);
               if (error != ATA_ERROR_NONE)
               {
                  printf("Read error\n");
                  fclose(pOutputFile);
                  return;
               }
               FileData[0] = read_word >> 8;
               FileData[1] = read_word & 0xFF;
               write_count = fwrite(&FileData, 1, 2, pOutputFile);
               if (write_count != 2)
               {
                  printf("Error writing to the file\n");
                  fclose(pOutputFile);
                  exit(1);
               }
            }
            printf("Read complete\n");
            return;
         }
	   }

      /* Get the next file/directory (if there is one) */
	   error = ATA_findNext(&MmcSdFile);
   }

   fclose(pOutputFile);
   printf("BOOTIMG.BIN not found\n");
}


/******************************************************************************
* mmc_sd_file_delete
******************************************************************************/
int mmc_sd_file_delete(void)
{
   AtaError error; AtaError ret;

   /* Get the first file/directory */
   error = ATA_fileInit(pAtaDriveMmcSd, &MmcSdFile);

   while (error == ATA_ERROR_NONE)
   {
      /* Check for a file (not directory) with name "BOOTIMG.BIN" */
	   if (!(ret=ATA_isDir(&MmcSdFile)))
	   {
         /* Check for matching filename/extension */
	      if ((!strcmp(MmcSdFile.Filename, "BOOTIMG")) && (!strcmp(MmcSdFile.Ext, "BIN")))
		   //if ((!strcmp(MmcSdFile.Filename, "sample")) && (!strcmp(MmcSdFile.Ext, "txt")))
         {
            error = ATA_delete(&MmcSdFile);
            if (error != ATA_ERROR_NONE)
            {
               printf("Delete error\n");
               return 1;
            }
            printf("Delete complete\n");
            return 0;
         }
	   }

      /* Get the next file/directory (if there is one) */
	   error = ATA_findNext(&MmcSdFile);
   }

   printf("BOOTIMG.BIN not found\n");
   return 0;
}

/******************************************************************************
* mmc_sd_file_write
******************************************************************************/
void mmc_sd_file_write(char *fileName)
{
   AtaError error;
   FILE *pInputFile = NULL;
   unsigned short FileData[2];
   unsigned long addr;
   unsigned short temp_sector[256];
   unsigned short *pTempSector;
   size_t rcv_count;

   if (fileName[0] == '\0')
   {
      printf("Opening flashimg.bin...\n");
      pInputFile = fopen("flashimg.bin","rb");
   }
   else
   {
      printf("Opening %s...\n", fileName);
      pInputFile = fopen(fileName, "rb");
   }
   if (!pInputFile)
   {
      printf("Error opening input file\n");
      exit(0);
   }
   else
   {
      printf("Input file opened\n");
   }

   /* Get the first file/directory */
   error = ATA_fileInit(pAtaDriveMmcSd, &MmcSdFile);

   error = ATA_setFileName(&MmcSdFile, "BOOTIMG", "BIN");
   if (error != ATA_ERROR_NONE)
   {
      printf("SetFileName() error\n");
      fclose(pInputFile);
      return;
   }
   error = ATA_cdRoot(&MmcSdFile);
   if (error != ATA_ERROR_NONE)
   {
      printf("CdRoot() error\n");
      fclose(pInputFile);
      return;
   }

   error = ATA_create(&MmcSdFile);
   if (error != ATA_ERROR_NONE)
   {
      printf("Create() error\n");
      fclose(pInputFile);
      return;
   }

   /* Read all data from file and write it to the device (write full sectors) */
   printf("Writing Boot-image to device...\n");
   pTempSector = &temp_sector[0];
   addr = 0;
   while (!feof(pInputFile))
   {
      rcv_count = fread(FileData, 1, 2, pInputFile);
      if (rcv_count == 0)
      {
         break; /* no more data to read */
      }
      if ((rcv_count & 1) != 0)
      {
         printf("ERROR: Input File has an odd number of bytes (invalid)\n");
         exit(1);
      }
      *pTempSector++ = (FileData[0] << 8) | FileData[1];
      addr++;
      if ((addr%0x100) == 0)
      {
         printf("Programming... [TotalSize=%ld (0x%lX)]\n", addr, addr);
         error = ATA_write(&MmcSdFile, temp_sector, 256);
         if (error != ATA_ERROR_NONE)
         {
            printf("ERROR in Write()\n");
            return;
         }
         pTempSector = &temp_sector[0];
      }
   }
   /* If there is unwritten data, write it */
   if (pTempSector != &temp_sector[0])
   {
      printf("Programming... [TotalSize=%ld (0x%lX)]\n", addr, addr);
      error = ATA_write(&MmcSdFile, temp_sector, addr%256);
      if (error != ATA_ERROR_NONE)
      {
         printf("ERROR in Write()\n");
         return;
      }
   }

   fclose(pInputFile);
   error = ATA_close(&MmcSdFile);
   if (error != ATA_ERROR_NONE)
   {
      printf("Close() error\n");
      return;
   }

   printf("BOOTIMG.BIN complete\n");
}


AtaUint16 mmc_sd_get_time()
{
   return (0x632a);
}

AtaUint16 mmc_sd_get_date()
{
   return (0x2b2b);
}
