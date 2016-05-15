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
*    nand_flash.c
**********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "ata_nand.h"
#include "nand.h"
#include "nand_flash.h"

/** Structure to hold the pointers of MBR, BR and the Boot Sector Value */
typedef struct FormatStruct
{
   AtaUint16 *pMBR;      /**< Pointer to Master Boot Record */
   AtaUint16 *pBR;       /**< Pointer to Boot Record        */
   AtaSector BR_Sector;  /**< Boot Sector number            */
} Format;

AtaUint16 PS0_SSFDC_NAND[] =       /* organized as big endian */
{
/*   0001    0203    0405    0607    0809    0a0b    0c0d    0e0f    1011    1213    1415    1617    1819    1a1b    1c1d    1e1f            */
   0x0103, 0xd901, 0xff18, 0x02df, 0x0120, 0x0400, 0x0000, 0x0021, 0x0204, 0x0122, 0x0201, 0x0122, 0x0302, 0x0407, 0x1a05, 0x0103, /* 0x0000 */
   0x0002, 0x0f1b, 0x08c0, 0xc0a1, 0x0155, 0x0800, 0x021b, 0x0ac1, 0x4199, 0x0155, 0x64f0, 0xffff, 0x201b, 0x0c82, 0x4118, 0xea61, /* 0x0020 */
   0xf001, 0x07f6, 0x0301, 0xee1b, 0x0c83, 0x4118, 0xea61, 0x7001, 0x0776, 0x0301, 0xee15, 0x1405, 0x0020, 0x2020, 0x2020, 0x2020, /* 0x0040 */
   0x0220, 0x2020, 0x2000, 0x302e, 0x3000, 0xff14, 0x00ff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, /* 0x0060 */
   0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, /* 0x0080 */
   0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, /* 0x00a0 */
   0x0000, 0x0000, 0xd07f, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, /* 0x00c0 */
   0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, /* 0x00e0 */
   0x0103, 0xd901, 0xff18, 0x02df, 0x0120, 0x0400, 0x0000, 0x0021, 0x0204, 0x0122, 0x0201, 0x0122, 0x0302, 0x0407, 0x1a05, 0x0103, /* 0x0100 */
   0x0002, 0x0f1b, 0x08c0, 0xc0a1, 0x0155, 0x0800, 0x021b, 0x0ac1, 0x4199, 0x0155, 0x64f0, 0xffff, 0x201b, 0x0c82, 0x4118, 0xea61, /* 0x0120 */
   0xf001, 0x07f6, 0x0301, 0xee1b, 0x0c83, 0x4118, 0xea61, 0x7001, 0x0776, 0x0301, 0xee15, 0x1405, 0x0020, 0x2020, 0x2020, 0x2020, /* 0x0140 */
   0x0220, 0x2020, 0x2000, 0x302e, 0x3000, 0xff14, 0x00ff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, /* 0x0160 */
   0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, /* 0x0180 */
   0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, /* 0x01a0 */
   0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, /* 0x01c0 */
   0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, /* 0x01e0 */
};


/* 0x01ca-0x01cd stores the number of sectors. This was reduced from 0x7cd7 to
   0x7ca7 to reserve 48 sectors (24kb) for the loader boot record */

AtaUint16 MBR_16MB_SSFDC_NAND[] =      /* organized as big endian */
{                                                                                                                                 /* first byte */
   0xfa33, 0xc08e, 0xd0bc, 0x007c, 0x8bf4, 0x5007, 0x501f, 0xfbfc, 0xbf00, 0x06b9, 0x0001, 0xf2a5, 0xea1d, 0x0600, 0x00be, 0xbe07, /* 0x0000 */
   0xb304, 0x803c, 0x8074, 0x0e80, 0x3c00, 0x751c, 0x83c6, 0x10fe, 0xcb75, 0xefcd, 0x188b, 0x148b, 0x4c02, 0x8bee, 0x83c6, 0x10fe, /* 0x0020 */
   0xcb74, 0x1a80, 0x3c00, 0x74f4, 0xbe8b, 0x06ac, 0x3c00, 0x740b, 0x56bb, 0x0700, 0xb40e, 0xcd10, 0x5eeb, 0xf0eb, 0xfebf, 0x0500, /* 0x0040 */
   0xbb00, 0x7cb8, 0x0102, 0x57cd, 0x135f, 0x730c, 0x33c0, 0xcd13, 0x4f75, 0xedbe, 0xa306, 0xebd3, 0xbec2, 0x06bf, 0xfe7d, 0x813d, /* 0x0060 */
   0x55aa, 0x75c7, 0x8bf5, 0xea00, 0x7c00, 0x0049, 0x6e76, 0x616c, 0x6964, 0x2070, 0x6172, 0x7469, 0x7469, 0x6f6e, 0x2074, 0x6162, /* 0x0080 */
   0x6c65, 0x0045, 0x7272, 0x6f72, 0x206c, 0x6f61, 0x6469, 0x6e67, 0x206f, 0x7065, 0x7261, 0x7469, 0x6e67, 0x2073, 0x7973, 0x7465, /* 0x00a0 */
   0x6d00, 0x4d69, 0x7373, 0x696e, 0x6720, 0x6f70, 0x6572, 0x6174, 0x696e, 0x6720, 0x7379, 0x7374, 0x656d, 0x0000, 0x0000, 0x0000, /* 0x00c0 */
   0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, /* 0x00e0 */
   0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, /* 0x0100 */
   0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, /* 0x0120 */
   0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, /* 0x0140 */
   0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, /* 0x0160 */
   0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, /* 0x0180 */
   0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x8002, /* 0x01a0 */
   0x0A00, 0x0103, 0x50f3, 0x2900, 0x0000, 0xa77c, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, /* 0x01c0 */
   0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x55aa  /* 0x01e0 */
};

AtaUint16 BR_16MB_SSFDC_NAND[] =       /* organized as big endian */
{
/*   0001    0203    0405    0607    0809    0a0b    0c0d    0e0f    1011    1213    1415    1617    1819    1a1b    1c1d    1e1f            */
   0xeb3e, 0x9029, 0x6550, 0x6125, 0x4948, 0x4300, 0x0220, 0x0100, 0x0200, 0x01d7, 0x7cf8, 0x0300, 0x1000, 0x0400, 0x2900, 0x0000, /* 0x0000 */
   0x0000, 0x0000, 0x8000, 0x2925, 0x0409, 0x134e, 0x4f20, 0x4e41, 0x4d45, 0x2020, 0x2020, 0x4641, 0x5431, 0x3220, 0x2020, 0xf17d, /* 0x0020 */
   0xfa33, 0xc98e, 0xd1bc, 0xfc7b, 0x1607, 0xbd78, 0x00c5, 0x7600, 0x1e56, 0x1655, 0xbf22, 0x0589, 0x7e00, 0x894e, 0x02b1, 0x0bfc, /* 0x0040 */
   0xf3a4, 0x061f, 0xbd00, 0x7cc6, 0x45fe, 0x0f8b, 0x4618, 0x8845, 0xf9fb, 0x3866, 0x247c, 0x04cd, 0x1372, 0x3c8a, 0x4610, 0x98f7, /* 0x0060 */
   0x6616, 0x0346, 0x1c13, 0x561e, 0x0346, 0x0e13, 0xd150, 0x5289, 0x46fc, 0x8956, 0xfeb8, 0x2000, 0x8b76, 0x11f7, 0xe68b, 0x5e0b, /* 0x0080 */
   0x03c3, 0x48f7, 0xf301, 0x46fc, 0x114e, 0xfe5a, 0x58bb, 0x0007, 0x8bfb, 0xb101, 0xe894, 0x0072, 0x4738, 0x2d74, 0x19b1, 0x0b56, /* 0x00a0 */
   0x8b76, 0x3ef3, 0xa65e, 0x744a, 0x4e74, 0x0b03, 0xf983, 0xc715, 0x3bfb, 0x72e5, 0xebd7, 0x2bc9, 0xb8d8, 0x7d87, 0x463e, 0x3cd8, /* 0x00c0 */
   0x7599, 0xbe80, 0x7dac, 0x9803, 0xf0ac, 0x84c0, 0x7417, 0x3cff, 0x7409, 0xb40e, 0xbb07, 0x00cd, 0x10eb, 0xeebe, 0x837d, 0xebe5, /* 0x00e0 */
   0xbe81, 0x7deb, 0xe033, 0xc0cd, 0x165e, 0x1f8f, 0x048f, 0x4402, 0xcd19, 0xbe82, 0x7d8b, 0x7d0f, 0x83ff, 0x0272, 0xc88b, 0xc748, /* 0x0100 */
   0x488a, 0x4e0d, 0xf7e1, 0x0346, 0xfc13, 0x56fe, 0xbb00, 0x0753, 0xb104, 0xe816, 0x005b, 0x72c8, 0x813f, 0x4d5a, 0x75a7, 0x81bf, /* 0x0120 */
   0x0002, 0x424a, 0x759f, 0xea00, 0x0270, 0x0050, 0x5251, 0x9192, 0x33d2, 0xf776, 0x1891, 0xf776, 0x1842, 0x87ca, 0xf776, 0x1a8a, /* 0x0140 */
   0xf28a, 0x5624, 0x8ae8, 0xd0cc, 0xd0cc, 0x0acc, 0xb801, 0x02cd, 0x1359, 0x5a58, 0x7209, 0x4075, 0x0142, 0x035e, 0x0be2, 0xccc3, /* 0x0160 */
   0x0318, 0x0127, 0x0d0a, 0x496e, 0x7661, 0x6c69, 0x6420, 0x7379, 0x7374, 0x656d, 0x2064, 0x6973, 0x6bff, 0x0d0a, 0x4469, 0x736b, /* 0x0180 */
   0x2049, 0x2f4f, 0x2065, 0x7272, 0x6f72, 0xff0d, 0x0a52, 0x6570, 0x6c61, 0x6365, 0x2074, 0x6865, 0x2064, 0x6973, 0x6b2c, 0x2061, /* 0x01a0 */
   0x6e64, 0x2074, 0x6865, 0x6e20, 0x7072, 0x6573, 0x7320, 0x616e, 0x7920, 0x6b65, 0x790d, 0x0a00, 0x494f, 0x2020, 0x2020, 0x2020, /* 0x01c0 */
   0x5359, 0x534d, 0x5344, 0x4f53, 0x2020, 0x2053, 0x5953, 0x8001, 0x0057, 0x494e, 0x424f, 0x4f54, 0x2053, 0x5953, 0x0000, 0x55aa  /* 0x01e0 */
};


Format Format_16MB_SSFDC_NAND =
{
   &MBR_16MB_SSFDC_NAND[0],
   &BR_16MB_SSFDC_NAND[0],
   0x29L, /* BR_Sector */
};

AtaState AtaDrive1;
void init_disk(Format *pFormat, char *inputFileName);

AtaUint16 *WriteBuffer = (AtaUint16 *)0x8000; /* 128kB -- large-block erase-size */
AtaUint16 ReadBuffer[0x100];
AtaNANDState MyNandState = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };


/******************************************************************************
* nand_init
******************************************************************************/
int nand_init(Uint16 ChipSelect)
{
   volatile int i;
   unsigned short status;

   AtaDrive1.AtaReset=AtaNANDReset;
   AtaDrive1.AtaBusyStatusCheck=AtaNANDCommandReadyStatusCheck;
   AtaDrive1.AtaCommandReadyStatusCheck=AtaNANDCommandReadyStatusCheck;
   AtaDrive1.AtaGetSerialID=AtaNANDGetSerialID;
   AtaDrive1.AtaIssueReadCommand=AtaNANDIssueReadCommand;
   AtaDrive1.AtaDataReadyStatusCheck=AtaNANDDataReadyStatusCheck;
   AtaDrive1.AtaReadNextWord=AtaNANDReadNextWord;
   AtaDrive1.AtaReadNextNWords=AtaNANDReadNextNWords;
   AtaDrive1.AtaWriteSector=AtaNANDWriteSector;
   AtaDrive1.AtaWriteSectorFlush=AtaNANDWriteSectorFlush;
   AtaDrive1.AtaInitAtaMediaState=0;

   MyNandState.ChipSelect = ChipSelect;
   MyNandState.pWriteBuffer = &WriteBuffer[0];
   MyNandState.pReadBuffer = &ReadBuffer[0];
   MyNandState.timingMultiplier = 1;

   AtaDrive1.pAtaMediaState = &MyNandState;
   status = AtaDrive1.AtaReset(AtaDrive1.pAtaMediaState);
   while(status);

   return 0;
}

/******************************************************************************
* nand_main
******************************************************************************/
int nand_main(char *inputFileName)
{
   init_disk(&Format_16MB_SSFDC_NAND, inputFileName);

   return(0);
}

/******************************************************************************
* init_disk
******************************************************************************/
void init_disk(Format *pFormat, char *inputFileName)
{
   unsigned short ii;
   AtaState *pAtaDrive=&AtaDrive1;
   unsigned long addr;
   unsigned short nand_temp_sector[256];
   unsigned short *pTempSector;
   unsigned long nand_page=0x7f00;
   unsigned long nand_sector;
   FILE *pInputFile;
   unsigned short FileData[2];
   size_t rcv_count;
   AtaNANDState *pNand = (AtaNANDState *)(pAtaDrive->pAtaMediaState);


   PS0_SSFDC_NAND[0xc4/2] = ((nand_page & 0xFF) << 8) | ((nand_page & 0xFF00) >> 8);
   PS0_SSFDC_NAND[0xc6/2] = (nand_page & 0xFF0000) >> 8;

   /* -----------------5/2/00 2:33PM--------------------
   Write the MBR (master boot record)
   --------------------------------------------------*/
   pAtaDrive->CurrentPhySector = ATA_INVALID_SECTOR;
   pAtaDrive->CurrentWord = ATA_INVALID_WORD;

   /* -----------------5/2/00 2:33PM--------------------
   Write NAND Physical Sector 0
   --------------------------------------------------*/
   printf("Writing Sector 0...\n");
   ii = pAtaDrive->AtaWriteSector((AtaSector)(-1L), pAtaDrive->pAtaMediaState, &PS0_SSFDC_NAND[0], 1);
   if (ii)
   {
      printf("ERROR in AtaWriteSector\n");
      while(ii);
   }

   printf("Writing Sector 1 (MBR)...\n");
   ii = pAtaDrive->AtaWriteSector(0L, pAtaDrive->pAtaMediaState, pFormat->pMBR, 1);
   if (ii)
   {
      printf("ERROR in AtaWriteSector\n");
      while(ii);
   }
   printf("Flushing Data...\n");
   ii = pAtaDrive->AtaWriteSectorFlush(pAtaDrive->pAtaMediaState);
   if (ii)
   {
      printf("ERROR in AtaWriteSectorFlush\n");
      while(ii);
   }

   /* -----------------5/2/00 2:33PM--------------------
   Write the BR (boot record)
   --------------------------------------------------*/
   printf("Writing Boot Record...\n");
   ii = pAtaDrive->AtaWriteSector(pFormat->BR_Sector, pAtaDrive->pAtaMediaState, pFormat->pBR, 1);
   if (ii)
   {
      printf("ERROR in AtaWriteSector\n");
      while(ii);
   }
   printf("Flushing Data...\n");
   ii = pAtaDrive->AtaWriteSectorFlush(pAtaDrive->pAtaMediaState);
   if (ii)
   {
      printf("ERROR in AtaWriteSectorFlush\n");
      while(ii);
   }

   /**************************************************************************
   * Write the Boot-image to the NAND
   **************************************************************************/
   if (inputFileName[0] == '\0')
   {
      printf("Opening flashimg.bin...\n");
      pInputFile = fopen("flashimg.bin","rb");
   }
   else
   {
      printf("Opening %s...\n", inputFileName);
      pInputFile = fopen(inputFileName, "rb");
   }
   if (pInputFile == NULL)
   {
      printf("ERROR: Input file could not be opened\n");
      exit(0);
   }
   else
   {
      printf("Input file opened\n");
   }

   /* Read all data from file and write it to the device (write full sectors) */
   printf("Writing Boot-image to NAND...\n");
   pTempSector = &nand_temp_sector[0];
   addr = 0;
   nand_sector = nand_page * pNand->sectorsPerPage;
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
         if (pAtaDrive->AtaWriteSector(nand_sector-1, pAtaDrive->pAtaMediaState, (AtaUint16 *)&nand_temp_sector[0], 0))
         {
            printf("ERROR in AtaWriteSector\n");
            for (;;);
         }
         nand_sector++;
         pTempSector = &nand_temp_sector[0];
      }
   }
   /* If there is unwritten data, write the last sector */
   if (pTempSector != &nand_temp_sector[0])
   {
      printf("Programming... [TotalSize=%ld (0x%lX)]\n", addr, addr);
      if (pAtaDrive->AtaWriteSector(nand_sector-1, pAtaDrive->pAtaMediaState, (AtaUint16 *)&nand_temp_sector[0], 0))
      {
         printf("ERROR in AtaWriteSector\n");
         for (;;);
      }
   }
   /* Flush the last sector */
   printf("Flushing Data...\n");
   if (pAtaDrive->AtaWriteSectorFlush(pAtaDrive->pAtaMediaState))
   {
      printf("ERROR in AtaWriteSectorFlush\n");
      for (;;);
   }

   fclose(pInputFile);

   printf("done\n");

  /* -----------------5/2/00 2:32PM--------------------
  Zero out boot record array for use as zero buffer
   --------------------------------------------------*/
//  for(ii=0; ii<ATA_WORDS_PER_PHY_SECTOR; ii++)
//  {
//    pFormat->pBR[ii] = 0;
//  }

  /* -----------------5/17/2000 4:52PM-----------------
  Support needed here for media bytes and bad sectors!!!
   --------------------------------------------------*/
//  ii = AtaSystemInit(pAtaDrive);
//  while(ii);

  /* -----------------5/2/00 2:33PM--------------------
  Place zeros in all the root directory entries
   --------------------------------------------------*/
//  for(ll=pAtaDrive->RootDirSector; ll<pAtaDrive->FirstDataSector; ll++)
//  {
//    _AtaFlush(pAtaDrive);
//    ii = pAtaDrive->AtaWriteSector(ll, pAtaDrive->pAtaMediaState, pFormat->pBR, 0);
//    while(ii);
//    ii = pAtaDrive->AtaWriteSectorFlush(pAtaDrive->pAtaMediaState);
//    while(ii);
//  }

   exit(0); /* KLUDGE to put NAND programming into programmer.  This should be cleaned up later */
}


/******************************************************************************
* nand_readBack
******************************************************************************/
void nand_readBack(char *fileName)
{
   int ii, ll;
   FILE *pOutputFile = NULL;
   unsigned long nand_page;
   unsigned long nand_sector;
   AtaState *pAtaDrive=&AtaDrive1;
   unsigned short read_word;
   unsigned short FileData[2];
   size_t write_count;
   AtaNANDState *pNand = (AtaNANDState *)(pAtaDrive->pAtaMediaState);

   if (fileName[0] == '\0')
   {
      printf("Opening nand_readback.bin...\n");
      pOutputFile = fopen("nand_readback.bin","wb");
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

   nand_page = 0x7f00;
   nand_sector = nand_page * pNand->sectorsPerPage;

   for (ii=0; ii<3; ii++)
   {
      AtaNANDIssueReadCommand(nand_sector-1+ii, pAtaDrive->pAtaMediaState, 0);
      if (!NandTimeoutCheck(AtaNANDDataReadyStatusCheck, pAtaDrive->pAtaMediaState))
      {
         printf("Error on NAND read command\n");
         fclose(pOutputFile);
         exit(1);
      }

      for (ll=0; ll<256; ll++)
      {
         AtaNANDReadNextWord(pAtaDrive->pAtaMediaState, (AtaUint16 *)&read_word);

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
   }
}
