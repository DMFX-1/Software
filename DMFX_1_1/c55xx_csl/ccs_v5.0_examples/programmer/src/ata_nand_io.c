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
*    ata_nand_io.c
**********************************************************************/
#include "registers.h"
#include "ata_nand.h"
#include "nand.h"

Uint16 NandFlashChipSelect = NAND_FLASH_CS2;
extern NandStruct *NandFlashCtrl;


/******************************************************************************
* AtaNANDDataReadyStatusCheck
******************************************************************************/
int AtaNANDDataReadyStatusCheck(void *pAtaMediaState)
{
   unsigned short NandFlashStatus;

   pAtaMediaState = pAtaMediaState; /* prevent compiler warning for unused parameter */
   NandDeselect();

   /* The NAND's R/B- pin is attached to the RDY-pin corresponding to the ChipSelect used */
   /* Use NandFlashStatus register bits [3..0] */
   NandFlashStatus = IOPORT_REG_EMIF_NAND_STATUS;
   return ((NandFlashStatus & NandFlashChipSelect));
}

/******************************************************************************
* NandConfig
******************************************************************************/
int NandConfig(Uint16 ChipSelect)
{
   /* Access EMIF registers as 16-bit registers */
   IOPORT_REG_EMIF_CNTL = 0x0000;

   /* Configure ASYNC Wait config Registers */
   IOPORT_REG_EMIF_ASYNC_WAIT_0 = 0x0080; // max_ext_wait = 0x80
   IOPORT_REG_EMIF_ASYNC_WAIT_1 = 0x00E4; // wp(all) = 0

   /* Initialize NAND as deselected */
   IOPORT_REG_EMIF_NAND_CTRL = 0;

   /* Configure as 8-bit data bus */
   switch (ChipSelect)
   {
   case NAND_FLASH_CS2:
      IOPORT_REG_EMIF_ASYNC1_CONFIG_0 = 0xFFFC;
      IOPORT_REG_EMIF_ASYNC1_CONFIG_1 = 0x3FFF;
      NandFlashCtrl = (NandStruct *)NAND_FLASH_BASE_ADDRESS_CS2;
      break;
   case NAND_FLASH_CS3:
      IOPORT_REG_EMIF_ASYNC2_CONFIG_0 = 0xFFFC;
      IOPORT_REG_EMIF_ASYNC2_CONFIG_1 = 0x3FFF;
      NandFlashCtrl = (NandStruct *)NAND_FLASH_BASE_ADDRESS_CS3;
      break;
   case NAND_FLASH_CS4:
      IOPORT_REG_EMIF_ASYNC3_CONFIG_0 = 0xFFFC;
      IOPORT_REG_EMIF_ASYNC3_CONFIG_1 = 0x3FFF;
      NandFlashCtrl = (NandStruct *)NAND_FLASH_BASE_ADDRESS_CS4;
      break;
   case NAND_FLASH_CS5:
      IOPORT_REG_EMIF_ASYNC4_CONFIG_0 = 0xFFFC;
      IOPORT_REG_EMIF_ASYNC4_CONFIG_1 = 0x3FFF;
      NandFlashCtrl = (NandStruct *)NAND_FLASH_BASE_ADDRESS_CS5;
      break;
   default:
      return 1; /* ERROR: Invalid chip-select */
   }
   NandFlashChipSelect = ChipSelect;

   /* Set EMIF to Byte access (required for data transfer to NAND) */
   IOPORT_REG_EMIF_CNTL = 0x0002;
   return 0;
}

/******************************************************************************
* NandSelect
******************************************************************************/
void NandSelect()
{
   IOPORT_REG_EMIF_NAND_CTRL |= NandFlashChipSelect; /* set the bit to select */
}

/******************************************************************************
* NandDeselect
******************************************************************************/
void NandDeselect()
{
   IOPORT_REG_EMIF_NAND_CTRL &= ~NandFlashChipSelect; /* clear the bit to deselect */
}
