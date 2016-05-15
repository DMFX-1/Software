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

#ifndef ATA_SD_H
#define ATA_SD_H

#define  SERIAL_ID_LEN			16
#define ATA_WORDS_PER_PHY_SECTOR  0x100
/********************************************************************
***					SD card state structure				               ***
********************************************************************/
typedef struct AtaSDState
{
int sdNumber;                       //sd slot
unsigned int data_lines;            //1 data or 4 data lines
int c55x_sd_ctl_addr;               //address
int card_number;                    //card number
unsigned int FDIV;
unsigned int CDIV;
unsigned short fifo_threshold;      //fifo_level
int  *buffer;                       //internal buffer
void *ptr;                          //internal buffer
}AtaSDState;

/********************************************************************
****		SD ATA interface function prototypes			      	  ****
********************************************************************/
AtaError SD_systemInit(AtaState *pAtaDrive);

AtaError AtaSDInitState(void *pAtaMediaState);

AtaError AtaSDReset(void *pAtaMediaState);

AtaError AtaSDBusyStatusCheck(void *pAtaMediaState);

AtaUint16 AtaSDGetSerialID(void *pAtaMediaState, AtaSerialID *pID);

AtaError AtaSDIssueReadCommand( AtaSector PhySector,  void *pAtaMediaState,  AtaUint16 SectorCount);

int AtaSDCommandReadyStatusCheck(void *pAtaMediaState);

AtaError AtaSDDataReadyStatusCheck(void *pAtaMediaState);

AtaError AtaSDReadNextWord(void *pAtaMediaState, AtaUint16 *pWord);

AtaError AtaSDReadNextNWords(void *pAtaMediaState, AtaUint16 *Data, AtaUint16 Words);

AtaError AtaSDWriteSector( AtaSector PhySector,  void *pAtaMediaState,  AtaUint16 *pWord,  int ByteSwap);

AtaError AtaSDWriteSectorFlush(void *pAtaMediaState);

// the following function is for init only
AtaUint16 AtaSDReadWord(AtaSector PhySector, AtaState *pAtaDrive, AtaUint16 ByteOffset);
int AtaSDTimeoutCheck(AtaError (fpStatusCheck)(void *ptr), void *ptr);
AtaUint32 AtaSDReadDoubleWord(AtaSector PhySector, AtaState *pAtaDrive, AtaUint16 ByteOffset);
char AtaSDReadByte(AtaSector PhySector, AtaState *pAtaDrive, AtaUint16 ByteOffset);
AtaError AtaSDFlush(AtaState *pAtaDrive);

#endif

