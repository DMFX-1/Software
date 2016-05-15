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
* This file includes subroutines used by the ATA higher level 		*
* API
*********************************************************************
* Initial versioin is from old unknown project						*
* Revision 1.0 1/08/08  Yihe Hu		 	                        	*
********************************************************************/
#include "mmc_sd_c55.h"
#include "ata.h"
#include "ata_.h"
#include "mmc.h"
#include "ata_mmc.h"


/*******************************************************
* global variable
********************************************************/
AtaSerialID MMC_MediaSerialID;
/*******************************************************
* local function
********************************************************/
static void _AtaMMCIssueWriteCommand(AtaSector PhySector, unsigned short SectorCount, void *pAtaMediaState);

/*******************************************************************
* AtaError MMC_reset(void *pAtaMediaState)
* input:
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*			ATA_ERROR_MEDIA_NOT_FOUND		:mmc card is not found
*			ATA_ERRIR_NONE					:reset done
*Description:
*		This function is to reset the MMC hardware into indl state
********************************************************************/
AtaError MMC_reset(void *pAtaMediaState)
{
	// Init the MMC hardware(slot and card and software
   if(MMC_Init(pAtaMediaState) !=0)
	   return(ATA_ERROR_MEDIA_NOT_FOUND);
	//select the card
	MMC_SelectCard(pAtaMediaState, ((AtaMMCState *)pAtaMediaState)->MMC_CARD_ID);
	return(ATA_ERROR_NONE);
}
/*******************************************************************
*AtaUint16 MMC_getSerialID(void* pAtaMediaState, AtaSerialID *pID)
* input:
*			pAtaMediaState		MMC state
*			pID:				buffer to store the ID
* Output:
*			pID					MMC ID
* Return:
*		return the lengh
*Description:
*		This function is to return the serial ID of the MMC card
********************************************************************/
AtaUint16 MMC_getSerialID(void* pAtaMediaState, AtaSerialID *pID)
{
int ii;

   for (ii = 0;ii<16;ii++)
   {
      (*pID)[ii] = MMC_MediaSerialID[ii];
   }
   return(SERIAL_ID_LEN/2);
}

/*******************************************************************************
void _AtaMMCIssueCommand(unsigned long PhySector, unsigned short SectorCount,
*								unsigned short Command,void *pAtaMediaState)
* input:
*			PhySector			sector to be accessed
*			SectorCount:		not used
*			command:			command to MMC card
*			pAtaMediaState		MMC state
* Output:
*			none
* Return:
*			None
*Description:
*	Issues a command to the MMC. It assumes the previous command was a read or
*  write that was not terminated. The previous command operated on a whole
*  sector, so all that's left is to read the last 2 bytes of CRC, flush, then
*  raise CS. Now we execute the new command by dropping CS, send the command,
*  wait for R1 response, then look for Start Byte.
*
*  Following this the MMC will be left without clocks waiting fro subsequent
*  _AtaReadNextWord to read a word at a time
*
*******************************************************************************/
void _AtaMMCIssueCommand(unsigned long PhySector, unsigned short SectorCount, unsigned short Command,void *pAtaMediaState)
{
unsigned long address;
unsigned short argh,argl;

	address = (PhySector << 9);

	argh = (unsigned short) (address>>16);
	argl = (unsigned short) address;
	/* Fill up cmd and arg registers */
	MMC_SendCommand(Command,argh,argl,pAtaMediaState);
}
/*******************************************************************
* AtaError MMC_busyStatusCheck(void *pAtaMediaState)
* input:
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*			ATA_ERRIR_NONE					:reset done
*Description:
*			This function is to check the busy
********************************************************************/
AtaError MMC_busyStatusCheck(void *pAtaMediaState)
{
	return(ATA_ERROR_NONE);
}
/*******************************************************************
* int MMC_commandReadyStatusCheck(void *pAtaMediaState)
* input:
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*			ATA_ERRIR_NONE					:reset done
*Description:
*	This function is check the command ready
********************************************************************/
int MMC_commandReadyStatusCheck(void *pAtaMediaState)
{
	return(ATA_ERROR_NONE);
}
/*******************************************************************
*	AtaError MMC_dataReadyStatusCheck(void *pAtaMediaState)
* input:
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*			ATA_ERRIR_NONE					:reset done
*Description:
*	This functioni is to check the data ready
********************************************************************/
AtaError MMC_dataReadyStatusCheck(void *pAtaMediaState)
{
	return(ATA_ERROR_NONE);
}
/*******************************************************************
* AtaError MMC_readNextWord(void *pAtaMediaState, AtaUint16 *pWord)
* input:
*			pAtaMediaState		MMC state
*			pWord				buffer to store the word
* Output:
*			pWord				next word
* Return:
*			error status
*Description:
*	This functioni is to read the next word from curretn sector
********************************************************************/
AtaError MMC_readNextWord(void *pAtaMediaState, AtaUint16 *pWord)
{

AtaMMCState *MMCState = pAtaMediaState;
AtaError error = ATA_ERROR_NONE;

	if(MMCState->currentwordR == 0)
	{
		error = _MMC_ReadNextSectorWords(pAtaMediaState,MMCState->BufR);
	}
	*pWord = MMCState->BufR[MMCState->currentwordR++];
	return(error);
}
/*******************************************************************
AtaError MMC_readNextNWords(void *pAtaMediaState, AtaUint16 *Data, AtaUint16 Words)
* input:
*			pAtaMediaState		MMC state
*			Data				buffer to store the words
*			Words				bytes to read
* Output:
*			pWord				next N word
* Return:
*			error status
*Description:
*	This functioni is to read the next N word from curretn sector
********************************************************************/
AtaError MMC_readNextNWords(void *pAtaMediaState, AtaUint16 *Data, AtaUint16 Words)
{
AtaMMCState *MMCState = pAtaMediaState;
int i;
unsigned int size = 256 - MMCState->currentwordR;
AtaError error = ATA_ERROR_NONE;

	if(MMCState->currentwordR == 0)
	{
		error = _MMC_ReadNextSectorWords(pAtaMediaState,MMCState->BufR);
	}
	if(Words > size)
		Words = size;
  	for(i=0; i <Words; i++)
		*Data++ = MMCState->BufR[MMCState->currentwordR++];

	return(error);

}
/*******************************************************************
* AtaError MMC_issueReadCommand(AtaSector PhySector, void *pAtaMediaState, AtaUint16 SectorCount)
* input:
*			PhySector			sector to be accessed
*			pAtaMediaState		MMC state
*			SectorCount			not used
* Output:
*			None
* Return:
*			error status
*Description:
*		This function is to issue a read command to MMC card
********************************************************************/
AtaError MMC_issueReadCommand(AtaSector PhySector, void *pAtaMediaState, AtaUint16 SectorCount)
{
	//set the internal information
	((AtaMMCState *)pAtaMediaState)->currentsectorR = PhySector;
	((AtaMMCState *)pAtaMediaState)->currentwordR = 0;

	MMC_Set_Blocklen(pAtaMediaState,512);
	//clear response
	MMC_Clear_Rsp(pAtaMediaState);
	//set data size
	MMC_SetDataSize(1,pAtaMediaState);
	//reset FIFO
	MMC_FIFOReset(pAtaMediaState);
	//set the FIFO to receiver
	MMC_FIFOReceive(pAtaMediaState);
 	//issue the read command
   _AtaMMCIssueCommand(PhySector, SectorCount, READ_SINGLE_BLOCK | DATA ,pAtaMediaState);//| DCLR
   return (ATA_ERROR_NONE);
}

/*******************************************************************
*	AtaError MMC_writeSector(AtaSector PhySector, void *pAtaMediaState,
*									AtaUint16 *Word, int ByteSwap)
* input:
*			PhySector			sector to be accessed
*			pAtaMediaState		MMC state
*			Word				buffer store the data to be written
*			ByteSwap			swap or not
* Output:
*			None
* Return:
*			error status
*Description:
*		This function is to write 256 word to assigend sector
********************************************************************/
AtaError MMC_writeSector(AtaSector PhySector, void *pAtaMediaState, AtaUint16 *Word, int ByteSwap)
{

unsigned short ii, jj;
AtaError error = ATA_ERROR_NONE;
AtaUint16 fifo_level;
AtaUint16 fifoWriteCnt = 0;

AtaUint16 buffer[16];
unsigned short kk;
   /****************************************************
    Issue the WRITESECTOR command
   ****************************************************/
   _AtaMMCIssueWriteCommand(PhySector, 1,pAtaMediaState);

   fifo_level = ((AtaMMCState *)pAtaMediaState)->fifo_threshold;
   fifoWriteCnt = ATA_WORDS_PER_PHY_SECTOR >>(3 + fifo_level);
   /****************************************************
   Send the sector data
   ****************************************************/

   jj = 0;
   for(ii=0; ii<fifoWriteCnt; ii++)
   {
#if 1
      if(ByteSwap == 1)
      {
		   for(kk=0;kk<(  8 * (fifo_level + 1));kk++)
			   buffer[kk] = ((Word[jj + kk] & 0xFF) << 8) | ((Word[jj + kk] & 0xFF00) >> 8);
         error += _MMC_WriteNextWord(pAtaMediaState, (unsigned short *)buffer);
      }
    else
	    error += _MMC_WriteNextWord(pAtaMediaState, (unsigned short *)&Word[jj]);

#else
      error += _MMC_WriteNextWord(pAtaMediaState, (unsigned short *)&Word[jj]);
#endif
      jj += 1 <<(3 + fifo_level);
   }
   return(error);
}
/*******************************************************************
*	AtaError MMC_writeSectorFlush(void *pAtaMediaState)
* input:
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*			error			ATA_ERROR_NONE
*Description:
*		This function is to flush current sector
********************************************************************/
AtaError MMC_writeSectorFlush(void *pAtaMediaState)
{
   return(ATA_ERROR_NONE);
}

/*******************************************************************************
void _AtaMMCIssueWriteCommand(unsigned long PhySector, unsigned short SectorCount,
*							void *pAtaMediaState)
* input:
*			PhySetor		sector to be written
*			pAtaMediaState		MMC state
*			SectorCount:		not used
* Output:
*			None
* Return:
*			none
*Description:
 *
 *	Issues a command to the MMC. It assumes the previous command was a read or
 *  write that was not terminated. The previous command operated on a whole
 *  sector, so all that's left is to read the last 2 bytes of CRC, flush, then
 *  raise CS. If it was already terminated the extra clocks do not hurt
 *  Now we execute the new command by dropping CS, send the command,
 *  wait for R1 response, then send  Start Byte.
 *
 *  Following this the MMC will be left without clocks waiting for subsequent
 *  _AtaWriteNextWord to write a word at a time
 *
 *******************************************************************************/
void _AtaMMCIssueWriteCommand(unsigned long PhySector, unsigned short SectorCount, void *pAtaMediaState)
{
	//set data size
	MMC_SetDataSize(1,pAtaMediaState);
	//set block command
	MMC_Set_Blocklen(pAtaMediaState,512);
	//reset fifo
	MMC_FIFOReset(pAtaMediaState);
	//set fifo to transmitter
	MMC_FIFOTransmit(pAtaMediaState);
	//issue write command
	_AtaMMCIssueCommand(PhySector, SectorCount, WRITE_BLOCK | WRITE | DATA ,pAtaMediaState);
	//set the DMA trigger
	MMC_SetTriger(pAtaMediaState);
}



