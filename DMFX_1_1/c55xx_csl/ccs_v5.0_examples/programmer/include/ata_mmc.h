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

#ifndef ATA_MMC_H
#define ATA_MMC_H 1

#define     SERIAL_ID_LEN   16	  /* length in bytes of MMC Serial Number */
#define     ATA_WORDS_PER_PHY_SECTOR  0x100
/**********************************************************************
****					MMC card state structure				             ****
**********************************************************************/
typedef struct AtaMMCStateStruct
{
int mmcNumber;                      //MMC slot 0 or 1
int   *Buf;                         //internal buffer
int MMC_CARD_ID;                    //card ID
int C55XX_MMC_CTL_ADDR;             //MMC hardware address
int CLOCK_RATE;                     //clock
unsigned long Data_timeout_count;   //timeout value
unsigned short fifo_threshold;		//DMA fifo(0 or 1)
unsigned short   *BufR;				   //read buffer
unsigned short   *BufWR;			   //write buffer
AtaSector  currentsectorR;			   //current read sector
int currentwordR;					      //curret read word
AtaSector  currentsectorWR;			//current write sector
int currentwordWR;					   //cyrrebt wrute wird
void *mmcptr;						      //internal pointer
AtaState *pDrive;					      //ATA drive pointer
} AtaMMCState;


/*********************************************************************
This function should put the media back into its idle state with no
 sectors open and ready to accept commands.  The high-level API will
 attempt not to call this function until any open sector is read to
 the end.  If an error condition exists, this function will be called to
 clear the error condition.
*********************************************************************/
AtaError MMC_reset(void *pAtaMediaState);

/*********************************************************************
This function should return 0 to the high-level API if the media is
 ready to accept commands.  This allows the high-level API to yeild
 to other processes if the media is not ready to accept commands.
*********************************************************************/
AtaError MMC_busyStatusCheck(void *pAtaMediaState);

/*********************************************************************
This function is used to fetch the ATA GetIdentify data structure.  This
 data structure is defined in the ATA specification.  It contains the
 serial number and physical description of the media.  Today, only the
 serial number is fetched, but future high-level API functions will use
 this data for formatting the media.  On NAND devices, the very first
 sector on the media is reserved for this data and all subsequent sectors
 are shifted accordingly.  This data is always 512 16-bit words. All high-
 level APIs will call AtaMediaIssueGetIdentifyCommand once then call
 AtaMediaReadNextWord 512 times to fetch this data.
*********************************************************************/
AtaUint16 MMC_getSerialID(void *pAtaMediaState, AtaSerialID *pID);

/*********************************************************************
This function is used to fetch all normal data from the media.  A sector
 is always assumed to be 256 16-bit words by the high-level API.
*********************************************************************/
AtaError MMC_issueReadCommand
(
  AtaSector PhySector,
  void *pAtaMediaState,
  AtaUint16 SectorCount
);
/*********************************************************************
This function should return 0 to the high-level API if the media is
 NOT ready to provide data.  This allows the high-level API to yeild
 to other processes if the media is not ready to provide data.

AtaMediaDataReadyStatusCheck is only guaranteed to be called before the
 first call to AtaMediaReadNextWord following a AtaMediaIssue*Command,
 but may be called at any point.  It should always report if there is
 data available to be read by AtaMediaReadNextWord.
*********************************************************************/
int MMC_commandReadyStatusCheck(void *pAtaMediaState);
AtaError MMC_dataReadyStatusCheck(void *pAtaMediaState);

/*********************************************************************
This function should always return data.  It is the high-level API's
 reponsibility to verify that data will be ready by using the
 AtaMediaDataReadyStatusCheck function.  Once that function returns
 a non-zero value, any subsequent calls to AtaMediaReadNextWord should
 provide valid data.  This function will be called 256 times when it
 follows a AtaMediaIssueReadCommand function call.  It will be called
 512 times when it follows a AtaMediaIssueGetIdentifyCommand function
 call.  Any caching should occur below this function.  Because most
 media only have a delay in fetching the first word, and not subsequent
 words, no caching is generally required.  In the case the DMA is used
 to fetch data, an intermediate buffer below this function will be
 required.

AtaMediaDataReadyStatusCheck is only guaranteed to be called before the
 first call to AtaMediaReadNextWord following a AtaMediaIssue*Command,
 but may be called at any point.  It should always report if there is
 data available to be read by AtaMediaReadNextWord.
*********************************************************************/
AtaError MMC_readNextWord(void *pAtaMediaState, AtaUint16 *pWord);
AtaError MMC_readNextNWords(void *pAtaMediaState, AtaUint16 *Data, AtaUint16 Words);

/*********************************************************************
This function writes one (1) sector of data to the media.  It is
 possible to cache data being written to the media because the high-
 level API guarantees that AtaMediaWriteSectorFlush will be called
 before any data is read back from the media.  AtaMediaWriteSectorFlush
 is also called when the media is left idle for a long period of time
 and before power-down.  This function should not be called while read
 data is pending for AtaMediaReadNextWord.
*********************************************************************/
AtaError MMC_writeSector( AtaSector PhySector,void *pAtaMediaState, AtaUint16 *pWord,int ByteSwap);

/*********************************************************************
This function commits any data which is in a write cache to be written
 to the media.  In the case that write data is not cached, this should
 simply be a stub function.
*********************************************************************/
AtaError MMC_writeSectorFlush(void *pAtaMediaState);

/*********************************************************************
This function is to init the MMC state, should be called before any
other ATA/MMC API
*********************************************************************/
AtaError MMC_initState (void *pDrive);
AtaError MMC_systemInit(AtaState *pAtaDrive);

/**********************************************************************
** The following function are used in the ata_mmc_system_init.c file only
**********************************************************************/
AtaUint16 AtaMMCReadWord(AtaSector PhySector, AtaState *pAtaDrive, AtaUint16 ByteOffset);
int AtaMMCTimeoutCheck(int (fpStatusCheck)(void *ptr), void *ptr);
AtaUint32 AtaMMCReadDoubleWord(AtaSector PhySector, AtaState *pAtaDrive, AtaUint16 ByteOffset);
char AtaMMCReadByte(AtaSector PhySector, AtaState *pAtaDrive, AtaUint16 ByteOffset);
AtaError AtaMMCFlush(AtaState *pAtaDrive);

#endif

