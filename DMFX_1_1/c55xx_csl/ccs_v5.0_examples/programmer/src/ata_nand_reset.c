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
*    ata_nand_reset.c
**********************************************************************/
#include "ata_nand.h"
#include "nand.h"


/******************************************************************************
* AtaNANDReset
******************************************************************************/
AtaError AtaNANDReset(void *pAtaMediaState)
{
  unsigned short i,nand_id,mfc_id, dev_id;
  unsigned short found;
#if 0
  NAND_TYPE_VALUES values[] =
  {
     /* A */ { 1, 2 }, /* column/row bytes */
     /* B */ { 1, 3 }, /* column/row bytes */
     /* C */ { 2, 2 } /* column/row bytes */
  #if 0
     ,
     /* D */ { 2, 3 } /* column/row bytes */
  #endif
   };
#endif

   AtaUint16 ids[] =
   {
     0x2201 /*, C */,  /*12 col bits*/ /*Renesas*/
     0x1233 /*, A */,
     0x1235 /*, A */,
     0x1336 /*, B */,
     0x1239 /*, A */,
     0x1243 /*, A */,
     0x1245 /*, A */,
     0x1346 /*, B */,
     0x1253 /*, A */,
     0x1255 /*, A */,

     0x1356 /*, B */,
     0x126B /*, A */,
     0x1372 /*, B */,
     0x1273 /*, A */,
     0x1374 /*, B */,
     0x1275 /*, A */,
     0x1376 /*, B */,
     0x1378 /*, B */,
     0x1379 /*, B */,
     0x22A1 /*, C */,  /*12 col bits*/
     /* { 0xAA, D }, default Samsung */ /*12 col bits*/
     /* { 0xAC, D }, default Samsung */ /*12 col bits*/
     0x22B1 /*, C */,
     /* { 0xB3, D }, default Samsung */
     /* { 0xB5, D }, default Samsung */
     /* { 0xBA, D }, default Samsung */
     /* { 0xBC, D }, default Samsung */
     0x22C1 /*, C */,
     /* { 0xC3, D }, default Samsung */
     /* { 0xC5, D }, default Samsung */
     /* { 0xCA, D }, default Samsung */
     /* { 0xCC, D }, default Samsung */
     /* { 0xD3, D }, default Samsung */
     /* { 0xD5, D }, default Samsung */
     /*{ 0xDA, D }, default Samsung, different Toshiba default */  /*12 col bits*/
     /*{ 0xDC, D }, default Samsung, different Toshiba default */  /*12 col bits*/
     0x12E3 /*, A */,
     0x12E5 /*, A */,
     0x12E6 /*, A */,
     0x22F1 /*, C */, /*12 col bits*/

     0x0000 /*, 0*/   /* dummy end ID */
   };

   NandDeselect();
   if (NandConfig(((AtaNANDState *)pAtaMediaState)->ChipSelect) != 0)
   {
      return (ATA_ERROR_BAD_MEDIA);
   }
   ((AtaNANDState *)pAtaMediaState)->WritePending = 0;
   ((AtaNANDState *)pAtaMediaState)->WritePage = 0;
   ((AtaNANDState *)pAtaMediaState)->ReadWordCount = 0;
   ((AtaNANDState *)pAtaMediaState)->CurrentReadPage = 0;
   nand_id = NandReadID();
   dev_id = nand_id&0x00ff;
   mfc_id = (nand_id&0xff00)>>8;
   if ((mfc_id==0x00) || (mfc_id==0x90) || (mfc_id==0xFF))
   {
      /* ID is blank or unchanged from 0x90 READ_ID command still on bus */
      return (ATA_ERROR_BAD_MEDIA);
   }
   for (i=0, found=0; ids[i]!=0; i++)
   {
      if (dev_id == (ids[i]&0xFF))
      {
         found = 1;
         ((AtaNANDState *)pAtaMediaState)->ColumnAddrBytes = (ids[i]&0xF000)>>12;
         ((AtaNANDState *)pAtaMediaState)->RowAddrBytes = (ids[i]&0x0F00)>>8;
      }
   }
#if 0 /* defaults*/
   if ((nand_id == 0x98DA) || (nand_id == 0x98DC))
   {
      found = 0; /* Most 0xDA devices are 2col/3row bytes, but this new Toshiba one is a 1/3 */
   }
#endif
   if (!found)
   {
      if ((mfc_id==0x98) || (mfc_id==0x45)) /* Toshiba NAND MLC default (1col/3row) */
      {
         ((AtaNANDState *)pAtaMediaState)->ColumnAddrBytes = 1;
         ((AtaNANDState *)pAtaMediaState)->RowAddrBytes = 3;
      }
      else /* Renesas/Samsung/other Large Block default (2col/3row) */
      {
         ((AtaNANDState *)pAtaMediaState)->ColumnAddrBytes = 2;
         ((AtaNANDState *)pAtaMediaState)->RowAddrBytes = 3;
      }
   }

   if (((AtaNANDState *)pAtaMediaState)->ColumnAddrBytes == 2) /* large-block */
   {
      ((AtaNANDState *)pAtaMediaState)->ColumnBits = 11;
      ((AtaNANDState *)pAtaMediaState)->sectorsPerPage = 4;
      ((AtaNANDState *)pAtaMediaState)->wordsPerPage = 1024;
      ((AtaNANDState *)pAtaMediaState)->pagesPerBlock = 64;
   }
   else /* pNAND->ColumnAddrBytes == 1 */ /* small-block */
   {
      ((AtaNANDState *)pAtaMediaState)->ColumnBits = 9;
      ((AtaNANDState *)pAtaMediaState)->sectorsPerPage = 1;
      ((AtaNANDState *)pAtaMediaState)->wordsPerPage = 256;
      ((AtaNANDState *)pAtaMediaState)->pagesPerBlock = 32;
   }

   /* This was taken out due to lack of QT support for testing, and questions of reliability */
   /*((AtaNANDState *)pAtaMediaState)->dataBus16bit = Nand_widthDetect(pAtaMediaState);*/
   ((AtaNANDState *)pAtaMediaState)->dataBus16bit = 0; /* force 8-bit only */

   if (!nand_id)
   {
      return (ATA_ERROR_BAD_MEDIA);
   }
   return (ATA_ERROR_NONE);
}

/******************************************************************************
* AtaNANDGetSerialID
******************************************************************************/
AtaUint16 AtaNANDGetSerialID(void *pAtaMediaState, AtaSerialID *pID)
{
   return (ATA_ERROR_NONE);
}
