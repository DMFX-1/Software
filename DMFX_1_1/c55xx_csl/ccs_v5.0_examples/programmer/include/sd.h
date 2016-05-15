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

#ifndef SD_DOT_H
#define SD_DOT_H

#define KEY_SIZE 4
#define MAX_SD_CARD_SUPPORTED 16
/**********************************************************************
* SD specific common commands start                                   *
**********************************************************************/
#define CMD0    0
#define GO_IDLE_STATE                        (CMD0 | RSPFMT_NONE)                   //Response: none
#define CMD2    2
#define ALL_SEND_CID                         (CMD2 | RSPFMT_R2)                     //Response: R2
#define CMD3    3
#define SEND_RELATIVE_ADDR                   (CMD3 | RSPFMT_R1)                     //Response: R1
#define CMD4    4
#define SET_DSR                              (CMD4 | RSPFMT_NONE)                   //Response: R-
#define CMD7    7
#define SELECT_CARD                          (CMD7 | RSPFMT_R1 | BSYEXP)            //Response: R1b
#define DESELECT_CARD                        (CMD7            )                     //Response: none
#define CMD8	8
#define INTERFACE_CONDITION				      (CMD8 | RSPFMT_R1)
#define CMD9    9
#define SEND_CSD                             (CMD9 | RSPFMT_R2)                     //Response: R2
#define CMD10   10
#define SEND_CID                             (CMD10 | RSPFMT_R2)                    //Response: R2
#define CMD12   12
#define STOP_TRANSMISSION                    (CMD12 | RSPFMT_R1 | BSYEXP)           //Response: R1
#define CMD13   13
#define SEND_STATUS                          (CMD13 | RSPFMT_R1)                    //Response: R1
#define CMD15   15
#define GO_INACTIVE_STATE                    (CMD15 | RSPFMT_NONE)                  //Response: Rnone
#define CMD16   16
#define SET_BLOCK_LEN                        (CMD16 | RSPFMT_R1)                    //Response: R1
#define CMD17   17
#define READ_SINGLE_BLOCK                    (CMD17 | RSPFMT_R1)                    //Response: R1
#define CMD18   18
#define READ_MULTIPLE_BLOCK                  (CMD18 | RSPFMT_R1)                    //Response: R1
#define CMD24   24
#define WRITE_SINGLE_BLOCK                   (CMD24 | RSPFMT_R1)                    //Response: R1
#define CMD25   25
#define WRITE_MULTIPLE_BLOCK                 (CMD25 | RSPFMT_R1)                    //Response: R1
#define CMD27   27
#define PROGRAM_CSD                          (CMD27 | RSPFMT_R1)                    //Response: R1
#define CMD28   28
#define SET_WRITE_PROT                       (CMD28 | RSPFMT_R1 | BSYEXP)           //Response: R1b
#define CMD29   29
#define CLR_WRITE_PROT                       (CMD29 | RSPFMT_R1 | BSYEXP)           //Response: R1b
#define CMD30   30
#define SEND_WRITE_PROT                      (CMD30 | RSPFMT_R1)                     //Response: R1
#define CMD32   32
#define TAG_SECTOR_START                     (CMD32 | RSPFMT_R1)                    //Response: R1
#define CMD33   33
#define TAG_SECTOR_END                       (CMD33 | RSPFMT_R1)  		            //Response: R1
#define CMD38   38
#define ERASE                                (CMD38 | RSPFMT_R1 | BSYEXP)           //Response: R1b
#define CMD41    41
#define SD_SEND_OP_COND                      (CMD41| RSPFMT_R3 )		               //Response: R3
#define CMD56   56
#define GEN_CMD                              (CMD56 | RSPFMT_R1)                    //Response: R1
#define CMD59   59
#define CRC_ON_OFF                           (CMD59 | RSPFMT_R1)
/************************************************************************
* SD specific common commands stop                                    	*
************************************************************************/

/************************************************************************
* SD specific secure commands follow                                   	*
************************************************************************/
#define CMD55	55
#define APP_CMD                              (CMD55 | RSPFMT_R1)
//get mkb
#define ACMD43   43
#define GET_MKB  	                           (ACMD43 | RSPFMT_R1)
//get mid
#define ACMD44   44
#define GET_MID                              (ACMD44 | RSPFMT_R1)
//set RN1
#define ACMD45   45
#define SET_CER_RN1                          (ACMD45 | RSPFMT_R1)
//get RN2
#define ACMD46   46
#define GET_CER_RN2                          (ACMD46 | RSPFMT_R1)
//set RES1
#define ACMD47   47
#define SET_CER_RES2                         (ACMD47 | RSPFMT_R1)
//get RES2
#define ACMD48   48
#define GET_CER_RES1                         (ACMD48 | RSPFMT_R1)
//secure read
#define ACMD18   18
#define SECURE_READ_MULTI_BLOCK              (ACMD18 | RSPFMT_R1)
//secure write
#define ACMD25   25
#define SECURE_WRITE_MULTI_BLOCK             (ACMD25 | RSPFMT_R1)
#define ACMD6   6
#define FOUR_DATA_LINE                       (ACMD6 | RSPFMT_R1)

#define ACMD49   49
#define CHANGE_SECURE_AREA                   (ACMD49 | RSPFMT_R1 |BSYEXP)

/************************************************************************
* SD specific secure commands end                                       *
************************************************************************/
//define SD card status
typedef struct card_status
{
	unsigned short busy;
	CARD_STATE     state;
	//used_for_SD
	unsigned short new_rca;

	unsigned short erase_clrd;
	unsigned short erase_partial;
	unsigned short error_flags;
}CARD_STATUS;

typedef struct AtaSDInternalState
{
int *nonsecurebuf;
int *securebuf;
int *readbuf;
unsigned int securecheck;
unsigned int CurrentBlen;
AtaSector  currentsector;
unsigned int currentword;
CSD_STRUCT      csd;
CID_STRUCT      cid;
STATUS_STRUCT   status;
C55XX_MMC       Mmc;
CARD_STATUS     CardStatus;
CARD_CID        CardCid;
CARD_CSD        CardCsd;
PC55XX_MMC 		pMMC;
unsigned int  sessionOpen;
unsigned int  akeInitialized;
AtaSector  currentsecuresector;
unsigned int currentsecureword;
int sd_first;
//MediaContext Media;
//SdDevice dev;
unsigned long _sessionKey[2];
unsigned long	KeyState_c2[4];
unsigned short *sessionKey;
unsigned short *pKeyState_c2;
int gByteIndex;
unsigned short byteOffset;
unsigned short unitOffset;
int firstBlock;
unsigned short RNG_Seed_Register[KEY_SIZE];
unsigned short K[KEY_SIZE];
unsigned char high_capacity_card;
}AtaSDInternalState;



/************************************************************************
****           SD Function declarations
************************************************************************/
void			SD_Enable(MMC_SD_CONTROLLER controller);
int 			SD_Init(void *pAtaMediaState);
unsigned short StartSecureCMD(unsigned int new_rca,void *pAtaMediaState);

int SD_TI_wait_for( unsigned short event, void *pAtaMediaState );
void clear_rsp(void *pAtaMediaState);
void dump_regs(void *pAtaMediaState);
void get_rsp(void *pAtaMediaState);
void get_card_status(void *pAtaMediaState);
void get_card_rca(void *pAtaMediaState);
void get_card_cid(void *pAtaMediaState);
void get_card_csd(void *pAtaMediaState);
int wait_for( unsigned short event,void *pAtaMediaState);
int go_idle_state(void *pAtaMediaState);
int send_interface_condition(void *pAtaMediaState);
int send_op_cond(void *pAtaMediaState);
int all_send_cid(void *pAtaMediaState);
int send_relative_addr(void *pAtaMediaState);
int send_status(unsigned short rca,void *pAtaMediaState);
int send_cid(unsigned short rca, void *pAtaMediaState);
int send_csd(unsigned short rca, void *pAtaMediaState);
int select_card(unsigned short rca, void *pAtaMediaState);
int deselect_card( unsigned short rca, void *pAtaMediaState);
int set_blocklen( unsigned short block_len,void *pAtaMediaState);
int read_single_block( unsigned long address, void *pAtaMediaState);
int write_single_block( unsigned long address,void *pAtaMediaState);
int read_sector( unsigned long address , unsigned short *pBuf,void *pAtaMediaState);
int write_sector( unsigned long address , unsigned short *pBuf, int byteswapflag,void *pAtaMediaState);

AtaError SD_FIFOReset(void *pAtaMediaState);
AtaError SD_FIFOReceive(void *pAtaMediaState);
AtaError SD_FIFOTransmit(void *pAtaMediaState);
AtaError SD_SetDataSize(unsigned short numblks, void *pAtaMediaState);
AtaError SD_SetTriger(void *pAtaMediaState);
#endif




