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
* Revision 1.0 1/08/08  Yihe Hu		 	                            *
********************************************************************/
#include <string.h>
#include "ata.h"
#include "ata_sd.h"
#include "mmc_sd_c55.h"
#include "sd.h"

/*******************************************
*** globae variables
*******************************************/
AtaSerialID SD_MediaSerialID;
unsigned short SD_rca[MAX_SD_CARD_SUPPORTED];

/*******************************************************************
* AtaError AtaSDInitState(void *pAtaMediaState)
* input:
*			pAtaMediaState		SD state
* Output:
*			None
* Return:
*			ATA_ERROR_MEDIA_NOT_FOUND		:mmc card is not found
*			ATA_ERROR_NONE					:reset done
*Description:
*		This function is to init SD software internal status
********************************************************************/
AtaError AtaSDInitState(void *pAtaMediaState)
{
int i =0;
AtaSDState *handle = pAtaMediaState;
AtaSDInternalState *inhandle;

	i = sizeof(AtaSDInternalState);
	inhandle = (AtaSDInternalState *)((handle->buffer) + 768);
	handle->ptr = inhandle;
	inhandle->nonsecurebuf = handle->buffer;
	inhandle->securebuf = (handle->buffer) + 256;
	inhandle->readbuf = (handle->buffer) + 512;
	for(i=0; i <256; i++)
	{
	   inhandle->nonsecurebuf[i] = 0;
	   inhandle->securebuf[i] = 0;
	   inhandle->readbuf[i] = 0;
	}
	inhandle->CurrentBlen = 512;
	inhandle->securecheck = 0;
	inhandle->currentsector = 0;
	inhandle->currentword = 0;

	if(((AtaSDState *)pAtaMediaState)->sdNumber == 0)
	{
		SD_Enable(MMC_SD_CONTROLLER_0);
		((AtaSDState *)pAtaMediaState)->c55x_sd_ctl_addr = COROZON_MMC_SD_0_ADDRESS;
	}
	else if(((AtaSDState *)pAtaMediaState)->sdNumber == 1)
	{
		SD_Enable(MMC_SD_CONTROLLER_1);
		((AtaSDState *)pAtaMediaState)->c55x_sd_ctl_addr = COROZON_MMC_SD_1_ADDRESS;
	}
	else
		return(ATA_ERROR_UNSUPPORTED);

	inhandle->pMMC = (PC55XX_MMC) handle->c55x_sd_ctl_addr;
	// Set all the arguments to the default state
   memset( &inhandle->csd,		0, sizeof(CSD_STRUCT));
   memset( &inhandle->cid,		0, sizeof(CID_STRUCT));
   memset( &inhandle->status,	0, sizeof(STATUS_STRUCT));
   memset( &inhandle->CardStatus,0, sizeof(CARD_STATUS));
   memset( &inhandle->CardCid,	0, sizeof(CARD_CID));
   memset( &inhandle->CardCsd,   0, sizeof(CARD_CSD));
   inhandle->sessionOpen = 0;
   inhandle->akeInitialized = 0;
   inhandle->currentsecuresector=0;
   inhandle->currentsecureword = 0;
   inhandle->sd_first = 0;
   inhandle->sessionKey = (unsigned short *)&inhandle->_sessionKey[0];
   inhandle->pKeyState_c2 = (unsigned short *)&inhandle->KeyState_c2[0];
   inhandle->gByteIndex = 0;
   inhandle->byteOffset = 0;
   inhandle->unitOffset = 0;
   inhandle->firstBlock = 1;
   inhandle->RNG_Seed_Register[0] = 0xaaaa;
   inhandle->RNG_Seed_Register[1] = 0xbbbb;
   inhandle->RNG_Seed_Register[2] = 0xcccc;
   inhandle->RNG_Seed_Register[3] = 0xdddd;
   inhandle->K[0] = 0x12 ;
   inhandle->K[1] = 0x3456;
   inhandle->K[2] = 0x789a;
   inhandle->K[3] = 0xbcde;
   return(ATA_ERROR_NONE);
}
/*******************************************************************
* AtaError AtaSDReset(void *pAtaMediaState)
* input:
*			pAtaMediaState		SD state
* Output:
*			None
* Return:
*			ATA_ERROR_MEDIA_NOT_FOUND		:mmc card is not found
*			ATA_ERRIR_NONE					:reset done
*Description:
*		This function is to reset the SD hardware / software
********************************************************************/
AtaError AtaSDReset(void *pAtaMediaState)
{
AtaSDState *handle = pAtaMediaState;
AtaSDInternalState *inhandle = handle->ptr;
int i;


	AtaSDInitState(pAtaMediaState);
	inhandle = handle->ptr;


	if( SD_Init(pAtaMediaState))
	      return(ATA_ERROR_MEDIA_NOT_FOUND);

	if( select_card(inhandle->CardStatus.new_rca, pAtaMediaState))
		return(ATA_ERROR_BAD_MEDIA);

       // for multiple SD card support
RCA_LOOP:
   for(i=0; i<handle->card_number; i++)
   {
      if(inhandle->CardStatus.new_rca == SD_rca[i])
      {
			if( select_card(inhandle->CardStatus.new_rca, pAtaMediaState))
				return(ATA_ERROR_BAD_MEDIA);
			goto  RCA_LOOP;
		}
   }
     SD_rca[handle->card_number] = inhandle->CardStatus.new_rca;



RESENDI:
	do
	{
	}while(StartSecureCMD(inhandle->CardStatus.new_rca,pAtaMediaState) != 0);

   if(handle->data_lines == 1)
      inhandle->pMMC->argl = 0;
   else
      inhandle->pMMC->argl = 2;

	inhandle->pMMC->argh = 0;	//need to be zeros for_starting card!
	inhandle->pMMC->cmdl = FOUR_DATA_LINE;

	if(SD_TI_wait_for(RSPDNE,pAtaMediaState) != 0)
	{
      goto RESENDI;
	}

	return(ATA_ERROR_NONE);
}
/*******************************************************************
* AtaUint16 AtaSDGetSerialID(void *pAtaMediaState, AtaSerialID *pID)
* input:
*			pAtaMediaState		SD state
*			pID:				buffer to store the ID
* Output:
*			pID					MMC ID
* Return:
*		return the lengh
*Description:
*		This function is to return the serial ID of the SD card
********************************************************************/
AtaUint16 AtaSDGetSerialID(void *pAtaMediaState, AtaSerialID *pID)
{
int ii;

   for (ii = 0;ii<16;ii++)
   {
      (*pID)[ii] = SD_MediaSerialID[ii];
   }
   return(SERIAL_ID_LEN/2);
}
/*******************************************************************
* AtaError AtaSDBusyStatusCheck(void *pAtaMediaState)
* input:
*			pAtaMediaState		SD state
* Output:
*			None
* Return:
*			ATA_ERRIR_NONE					:reset done
*Description:
*			This function is to check the busy
********************************************************************/
AtaError AtaSDBusyStatusCheck(void *pAtaMediaState)
{
return(ATA_ERROR_NONE);
}
/*******************************************************************
* int AtaSDCommandReadyStatusCheck(void *pAtaMediaState)
* input:
*			pAtaMediaState		SD state
* Output:
*			None
* Return:
*			ATA_ERRIR_NONE					:reset done
*Description:
*			This function is to check command ready(NULL)
********************************************************************/
int AtaSDCommandReadyStatusCheck(void *pAtaMediaState)
{
return(ATA_ERROR_NONE);
}
/*******************************************************************
* AtaError AtaSDDataReadyStatusCheck(void *pAtaMediaState)
* input:
*			pAtaMediaState		SD state
* Output:
*			None
* Return:
*			ATA_ERRIR_NONE					:reset done
*Description:
*			This function is to check data ready
********************************************************************/
AtaError AtaSDDataReadyStatusCheck(void *pAtaMediaState)
{
return(ATA_ERROR_NONE);
}
/*******************************************************************
AtaError AtaSDIssueReadCommand( AtaSector PhySector,  void *pAtaMediaState,
*					 AtaUint16 SectorCount)
* input:
*			physector			sector to be accessed
*			pAtaMediaState		SD state
*			sectorCount			not used
* Output:
*			None
* Return:
*			ATA_ERRIR_NONE					:reset done
*Description:
*			This function is to issue read command
********************************************************************/
AtaError AtaSDIssueReadCommand( AtaSector PhySector,  void *pAtaMediaState,  AtaUint16 SectorCount)
{
	AtaSDState *handle = pAtaMediaState;
	AtaSDInternalState *inhandle = handle->ptr;
	inhandle->currentsector = PhySector;
	inhandle->currentword = 0;
	return(ATA_ERROR_NONE);
}
/*******************************************************************
* AtaError AtaSDReadNextWord(void *pAtaMediaState, AtaUint16 *pWord)
* input:
*			pAtaMediaState		SD state
*			pWord				buffer to store the data
* Output:
*			None
* Return:
*			ATA_ERRIR_NONE					:reset done
*Description:
*			This function is to read bext word
********************************************************************/
AtaError AtaSDReadNextWord(void *pAtaMediaState, AtaUint16 *pWord)
{
unsigned long address, factor ;
AtaSDState *handle = pAtaMediaState;
AtaSDInternalState *inhandle = handle->ptr;

	if(inhandle->currentword == 0)
	{
		if(inhandle->high_capacity_card == 1)
			address = inhandle->currentsector;
		else
		{
		    factor = 512;
		    address = inhandle->currentsector * factor;
		}
   	read_sector(address, (unsigned short *)inhandle->nonsecurebuf, pAtaMediaState);
   }

   *pWord=inhandle->nonsecurebuf[inhandle->currentword++];
	return(ATA_ERROR_NONE);

}
/*******************************************************************
AtaError AtaSDReadNextNWords(void *pAtaMediaState, AtaUint16 *Data, AtaUint16 Words)
* input:
*			pAtaMediaState		SD state
*			Data				buffer to store the data
*			words				bytes to read
* Output:
*			None
* Return:
*			ATA_ERRIR_NONE					:reset done
*Description:
*			This function is to read next N word
********************************************************************/
AtaError AtaSDReadNextNWords(void *pAtaMediaState, AtaUint16 *Data, AtaUint16 Words)
{
unsigned long address, factor, i;
AtaSDState *handle = pAtaMediaState;
AtaSDInternalState *inhandle = handle->ptr;

	if(inhandle->currentword == 0)
	{
		if(inhandle->high_capacity_card == 1)
			address = inhandle->currentsector;
		else
		{
		    factor = 512;
		    address = inhandle->currentsector * factor;
		}
  		read_sector(address, (unsigned short *)inhandle->nonsecurebuf,pAtaMediaState);
	}

  	for(i=0; i <Words; i++)
   	Data[i] =inhandle->nonsecurebuf[inhandle->currentword++];

	return(ATA_ERROR_NONE);
}

/*******************************************************************
* AtaError AtaSDWriteSectorFlush(void *pAtaMediaState)
* input:
*			pAtaMediaState		SD state
* Output:
*			None
* Return:
*			ATA_ERRIR_NONE					:reset done
*Description:
*			This function is to flush write sector
********************************************************************/
AtaError AtaSDWriteSectorFlush(void *pAtaMediaState)
{
   return(ATA_ERROR_NONE);
}
/*******************************************************************
* AtaError AtaSDWriteSector( AtaSector PhySector,  void *pAtaMediaState,
*			  AtaUint16 *pWord,  int ByteSwap)
* input:
*			physector			sector to write
*			pAtaMediaState		SD state
*			pword				buffer
*			bytewrap			swap or not
* Output:
*			None
* Return:
*			ATA_ERRIR_NONE					:reset done
*Description:
*			This function is to write 256 word to a sector
********************************************************************/
AtaError AtaSDWriteSector( AtaSector PhySector,  void *pAtaMediaState,  AtaUint16 *pWord,  int ByteSwap)
{
unsigned long address , factor ;
AtaSDState *handle = pAtaMediaState;
AtaSDInternalState *inhandle = handle->ptr;

	if(inhandle->high_capacity_card == 1)
		address = PhySector;
	else
	{
	    factor = 512;
	    address = PhySector * factor;
	}
	write_sector(address, (unsigned short *)pWord,ByteSwap,pAtaMediaState);
	return(ATA_ERROR_NONE);
}

/*******************************************************************
* int _AtaGetIdentify(AtaState *pAtaDrive)
* input:
*		pAtaDrive
* Output:
*			None
* Return:
*			ATA_ERRIR_NONE					:reset done
*Description:
*			This function is to get ID (NULL)
********************************************************************/
int _AtaGetIdentify(AtaState *pAtaDrive)
{
   return(0);
}
