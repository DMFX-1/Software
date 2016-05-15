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
* This file includes low level SD API
*********************************************************************
* Initial versioin is from old unknow project						     *
* Revision 1.0 1/10/08  Yihe Hu		 	                          	  *
********************************************************************/
#include "ata.h"
#include "mmc_sd_c55.h"
#include "sd.h"
#include "ata_sd.h"

/****************************************************************
***		Global Variables
****************************************************************/
extern AtaSerialID SD_MediaSerialID;
static unsigned long VDD_32_33;//	=	0x007F0000  // 0x007f

/* Local Function Prototypes */
unsigned short byte_swap(unsigned short input);


// Byte swap function for write
unsigned short byte_swap(unsigned short input)
{
	return (((input & 0xFF)<<8) | ((input & 0xFF00)>>8));
}

#if 0
/*******************************************************************
* void dump_regs(void *pAtaMediaState)
* input:
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*		None
*Description:
*		This function is to dump all MMC/SD hardware registers
********************************************************************/
void dump_regs(void *pAtaMediaState)
{
AtaSDState *handle = pAtaMediaState;
AtaSDInternalState *inhandle = handle->ptr;

   inhandle->Mmc.ctl        = inhandle->pMMC->ctl;
   inhandle->Mmc.clk        = inhandle->pMMC->clk;
   inhandle->Mmc.st0        = inhandle->pMMC->st0;
   inhandle->Mmc.st1        = inhandle->pMMC->st1;
   inhandle->Mmc.im         = inhandle->pMMC->im;
   inhandle->Mmc.tor        = inhandle->pMMC->tor;
   inhandle->Mmc.tod        = inhandle->pMMC->tod;
   inhandle->Mmc.blen       = inhandle->pMMC->blen;
   inhandle->Mmc.nblk       = inhandle->pMMC->nblk;
   inhandle->Mmc.nblc       = inhandle->pMMC->nblc;
   inhandle->Mmc.ddrl        = inhandle->pMMC->ddrl;
   inhandle->Mmc.dxrl        = inhandle->pMMC->dxrl;
   inhandle->Mmc.cmdl        = inhandle->pMMC->cmdl;
   inhandle->Mmc.argl       = inhandle->pMMC->argl;
   inhandle->Mmc.argh       = inhandle->pMMC->argh;
   inhandle->Mmc.rsp0       = inhandle->pMMC->rsp0;
   inhandle->Mmc.rsp1       = inhandle->pMMC->rsp1;
   inhandle->Mmc.rsp2       = inhandle->pMMC->rsp2;
   inhandle->Mmc.rsp3       = inhandle->pMMC->rsp3;
   inhandle->Mmc.rsp4       = inhandle->pMMC->rsp4;
   inhandle->Mmc.rsp5       = inhandle->pMMC->rsp5;
   inhandle->Mmc.rsp6       = inhandle->pMMC->rsp6;
   inhandle->Mmc.rsp7       = inhandle->pMMC->rsp7;
   inhandle->Mmc.drsp       = inhandle->pMMC->drsp;
   inhandle->Mmc.etok       = inhandle->pMMC->etok;
   inhandle->Mmc.cidx       = inhandle->pMMC->cidx;
}
#endif

/*******************************************************************
* void clear_rsp(void *pAtaMediaState)
* input:
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*		None
*Description:
*		This function is to clear the response
********************************************************************/
void clear_rsp(void *pAtaMediaState)
{
AtaSDState *handle = pAtaMediaState;
AtaSDInternalState *inhandle = handle->ptr;

   inhandle->pMMC->rsp0 = 0;
   inhandle->pMMC->rsp1 = 0;
   inhandle->pMMC->rsp2 = 0;
   inhandle->pMMC->rsp3 = 0;
   inhandle->pMMC->rsp4 = 0;
   inhandle->pMMC->rsp5 = 0;
   inhandle->pMMC->rsp6 = 0;
   inhandle->pMMC->rsp7 = 0;
   //	inhandle->pMMC->drsp = 0;
   //	inhandle->pMMC->etok = 0;
   inhandle->pMMC->cidx = 0;
}
/*******************************************************************
* void get_rsp(void *pAtaMediaState)
* input:
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*		None
*Description:
*		This function is to get the response
********************************************************************/
void get_rsp(void *pAtaMediaState)
{
AtaSDState *handle = pAtaMediaState;
AtaSDInternalState *inhandle = handle->ptr;

   inhandle->Mmc.rsp0 = inhandle->pMMC->rsp0;
   inhandle->Mmc.rsp1 = inhandle->pMMC->rsp1;
   inhandle->Mmc.rsp2 = inhandle->pMMC->rsp2;
   inhandle->Mmc.rsp3 = inhandle->pMMC->rsp3;
   inhandle->Mmc.rsp4 = inhandle->pMMC->rsp4;
   inhandle->Mmc.rsp5 = inhandle->pMMC->rsp5;
   inhandle->Mmc.rsp6 = inhandle->pMMC->rsp6;
   inhandle->Mmc.rsp7 = inhandle->pMMC->rsp7;
   inhandle->Mmc.drsp = inhandle->pMMC->drsp;
   inhandle->Mmc.etok = inhandle->pMMC->etok;
   inhandle->Mmc.cidx = inhandle->pMMC->cidx;
}
/*******************************************************************
* void get_card_status(void *pAtaMediaState)
* input:
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*		None
*Description:
*		This function is to get the SD card status
********************************************************************/
void get_card_status(void *pAtaMediaState)
{
AtaSDState *handle = pAtaMediaState;
AtaSDInternalState *inhandle = handle->ptr;

   inhandle->Mmc.rsp6 = inhandle->pMMC->rsp6;
   inhandle->Mmc.rsp7 = inhandle->pMMC->rsp7;
   inhandle->CardStatus.busy          = ((inhandle->Mmc.rsp6 >> 8)  & 1   );
   inhandle->CardStatus.state         = (CARD_STATE)((inhandle->Mmc.rsp6 >> 9)  & 0xf );
   inhandle->CardStatus.erase_clrd    = ((inhandle->Mmc.rsp6 >> 13) & 1   );
   inhandle->CardStatus.erase_partial = ((inhandle->Mmc.rsp6 >> 15) & 1   );
}
/*******************************************************************
* void get_card_rca(void *pAtaMediaState)
* input:
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*		None
*Description:
*		This function is to get the SD card RCA
********************************************************************/
void get_card_rca(void *pAtaMediaState)
{
AtaSDState *handle = pAtaMediaState;
AtaSDInternalState *inhandle = handle->ptr;

   inhandle->Mmc.rsp7 = inhandle->pMMC->rsp7;
   //added_for SD
   inhandle->CardStatus.new_rca = inhandle->Mmc.rsp7;
}
/*******************************************************************
* void get_card_cid(void *pAtaMediaState)
* input:
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*		None
*Description:
*		This function is to get the SD card CID
********************************************************************/
void get_card_cid(void *pAtaMediaState)
{
AtaSDState *handle = pAtaMediaState;
AtaSDInternalState *inhandle = handle->ptr;
unsigned long temp;

	int i=0;
		for(i=0;i<8;i++) SD_MediaSerialID[i] = 0;

	get_rsp(pAtaMediaState);

	SD_MediaSerialID[i++] = inhandle->CardCid.year             = (inhandle->Mmc.rsp0 >> 8   ) & 0xF; // 11:8
	SD_MediaSerialID[i++] = inhandle->CardCid.month            = (inhandle->Mmc.rsp0 >> 12  ) & 0xF; // 12:15
	SD_MediaSerialID[i++] = 	inhandle->CardCid.serial	         = (unsigned long)(inhandle->Mmc.rsp1);// 39:16
	SD_MediaSerialID[i++] = 	temp                     = (unsigned long)(inhandle->Mmc.rsp2 & 0x00FF);
	inhandle->CardCid.serial           |= (temp<<16);
	SD_MediaSerialID[i++] = 	inhandle->CardCid.fw_rev            = (inhandle->Mmc.rsp2 >>8  ) & 0x000F;  // 43:40
	SD_MediaSerialID[i++] = 	inhandle->CardCid.hw_rev            = (inhandle->Mmc.rsp2 >>12 ) & 0x000F;  // 47:44
	SD_MediaSerialID[i++] = 	inhandle->CardCid.mfg_id            = (inhandle->Mmc.rsp6 >>8  ) & 0x00FF;  //127:104
	SD_MediaSerialID[i++] = temp                     = (unsigned long)(inhandle->Mmc.rsp7      );
	inhandle->CardCid.mfg_id            |= temp << 16;
}
/*******************************************************************
* void get_card_csd(void *pAtaMediaState)
* input:
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*		None
*Description:
*		This function is to get the SD card CSD
********************************************************************/
void get_card_csd(void *pAtaMediaState)
{
AtaSDState *handle = pAtaMediaState;
AtaSDInternalState *inhandle = handle->ptr;

	get_rsp(pAtaMediaState);

	inhandle->CardCsd.ecc				= (inhandle->Mmc.rsp0 >>  8   ) & 3;  		// 9:8
	inhandle->CardCsd.tmp_wp          = (inhandle->Mmc.rsp0 >>  12  ) & 1;          // 12:12
	inhandle->CardCsd.perm_wp         = (inhandle->Mmc.rsp0 >>  13  ) & 1;          // 13:13
	inhandle->CardCsd.copy            = (inhandle->Mmc.rsp0 >>  14  ) & 1;	        // 14:14

	inhandle->CardCsd.wr_bl_part      = (inhandle->Mmc.rsp1 >>   5  ) & 1;          // 21:21
	inhandle->CardCsd.wr_bl_len       = (inhandle->Mmc.rsp1 >>   6  ) & 0xF;        // 25:22
	inhandle->CardCsd.r2w_factor      = (inhandle->Mmc.rsp1 >>  10  ) & 0x7;        // 28:26
	inhandle->CardCsd.def_ecc         = (inhandle->Mmc.rsp1 >>  13  ) & 0x3;		// 30:29
	inhandle->CardCsd.wp_brp_ena      = (inhandle->Mmc.rsp1 >>  15  ) & 1;          // 31:31

	inhandle->CardCsd.wp_grp_size     = (inhandle->Mmc.rsp2         ) & 0x1f;       // 36:32
	inhandle->CardCsd.er_grp_size     = (inhandle->Mmc.rsp2  >>  5  ) & 0x1f;       // 41:37
	inhandle->CardCsd.sect_size       = (inhandle->Mmc.rsp2  >> 10  ) & 0x1f;       // 46:42
	inhandle->CardCsd.c_size_mult     = (inhandle->Mmc.rsp2  >> 15  ) & 1;          // 49:47
	inhandle->CardCsd.c_size_mult     |=( (inhandle->Mmc.rsp3 &  3  ) << 1);

	inhandle->CardCsd.vdd_w_curr_max  = (inhandle->Mmc.rsp3  >>  2  ) & 0x7;  		// 52:50
	inhandle->CardCsd.vdd_w_curr_min  = (inhandle->Mmc.rsp3  >>  5  ) & 0x7;        // 55:53
	inhandle->CardCsd.vdd_r_curr_max  = (inhandle->Mmc.rsp3  >>  8  ) & 0x7;        // 58:56
	inhandle->CardCsd.vdd_r_curr_min	= (inhandle->Mmc.rsp3  >>  11 ) & 0x7;        // 61:59
	inhandle->CardCsd.c_size          = (inhandle->Mmc.rsp3  >>  14 ) & 0x3;        // 73:62
	inhandle->CardCsd.c_size          |=((inhandle->Mmc.rsp4  & 0x3FF) << 2);

	inhandle->CardCsd.dsr_imp         = (inhandle->Mmc.rsp4  >> 12  ) & 1;    		// 76:76
	inhandle->CardCsd.rd_blk_misal    = (inhandle->Mmc.rsp4  >> 13  ) & 1;          // 77:77
	inhandle->CardCsd.wr_blk_misal    = (inhandle->Mmc.rsp4  >> 14  ) & 1;          // 78:78
	inhandle->CardCsd.rd_bl_part      = (inhandle->Mmc.rsp4  >> 15  ) & 1;          // 79:79

	inhandle->CardCsd.rd_bl_len       =  (inhandle->Mmc.rsp5        ) & 0xf;		// 83:80
	inhandle->CardCsd.ccc             =  (inhandle->Mmc.rsp5 >>  4  ) & 0xfff;		// 895:84

	inhandle->CardCsd.tran_speed      =  (inhandle->Mmc.rsp6        ) & 0xff;		// 103:96
	inhandle->CardCsd.nsac            =  (inhandle->Mmc.rsp6  >>  8 ) & 0xff;		// 111:104

	inhandle->CardCsd.taac            =  (inhandle->Mmc.rsp7        ) & 0xff;       // 119:112
	inhandle->CardCsd.mmc_prot        =  (inhandle->Mmc.rsp7  >> 10 ) & 0xf;        // 125:122
	inhandle->CardCsd.csd_struct      =  (inhandle->Mmc.rsp7  >> 14 ) & 0x3;        // 127:126
}

/*******************************************************************
* int SD_TI_wait_for( unsigned short event, void *pAtaMediaState)
* input:
*			event				event to wait
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*		None
*Description:
*		This function is to wait the event until timeout
********************************************************************/
int SD_TI_wait_for( unsigned short event, void *pAtaMediaState)
{
unsigned long i;
volatile unsigned short stat;
AtaSDState *handle = pAtaMediaState;
AtaSDInternalState *inhandle = handle->ptr;

	for(i=0;i<12500;i++)
	//'this value is from observation of response delays
	{
		stat = inhandle->pMMC->st0;
		stat &= (SPIERR | CRCRS | CRCRD | CRCWR | TOUTRS | TOUTRD | event);
		if(stat != 0)
		//some event happened
			break;
	}
	/* If we got the event we were looking for then
	 return pass. We may have gotten an event but
	 maybe not the one we were expecting, i.e. a time our or crc. */
	return( ((stat & event) == event ) ? 0 : -1 );
}
/*******************************************************************
* int wait_for( unsigned short event, void *pAtaMediaState)
* input:
*			event				event to wait
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*		None
*Description:
*		This function is to wait the event until timeout
********************************************************************/
int wait_for( unsigned short event,void *pAtaMediaState)
{

	// This function assumes that a command will timeout otherwise we will get hung here forever.
volatile unsigned short stat;
static int gcount=0;
AtaSDState *handle = pAtaMediaState;
AtaSDInternalState *inhandle = handle->ptr;

	do
	{

		stat = inhandle->pMMC->st0;
		stat &= (SPIERR | CRCRS | CRCRD | CRCWR | TOUTRS | TOUTRD | event);
		gcount++;

	}while( stat == 0 );
	/* If we got the event we were looking for then
	 return pass. We may have gotten an event but
	 maybe not the one we were expecting, i.e. a time our or crc. */

	return( ((stat & event) == event ) ? 0 : -1 );
}
/*******************************************************************
* int go_idle_state(void *pAtaMediaState)
* input:
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*			error status
*Description:
*		This function is to let the SD into idle state
********************************************************************/
int go_idle_state(void *pAtaMediaState)
{
AtaSDState *handle = pAtaMediaState;
AtaSDInternalState *inhandle = handle->ptr;
   inhandle->pMMC->argh = STUFF_BITS;
   inhandle->pMMC->argl = STUFF_BITS;

   // Add in the INIT bit so that we wait for the 80
   // clocks before sending the intial command
   inhandle->pMMC->cmdl  = GO_IDLE_STATE | INIT | DCLR;

   // Resp = NONE, just check for command sent
   return( wait_for( RSPDNE,pAtaMediaState));
}
/*******************************************************************
* int send_interface_condition(void *pAtaMediaState)
* input:
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*			error status
*Description:
*		This function is to send the CMD8 for the VER2.0 SD card
*	this is requirement for the all SD card, old sd card will ignore
*	this commmand with a response timeout status
********************************************************************/
int send_interface_condition(void *pAtaMediaState)
{
volatile unsigned short stat;
AtaSDState *handle = pAtaMediaState;
AtaSDInternalState *inhandle = handle->ptr;

   // Clear status
   stat = inhandle->pMMC->st0;

   inhandle->pMMC->argh   = 0x0;
   inhandle->pMMC->argl   = 0x1AA;
   inhandle->pMMC->cmdl    = INTERFACE_CONDITION;


	if(wait_for(RSPDNE,pAtaMediaState))
		return(-1);

	stat = inhandle->pMMC->rsp6;
	if(stat == 0x1AA)
		return (0);
	else
		return(-1);
}
/*******************************************************************
* int send_op_cond(void *pAtaMediaState)
* input:
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*			error status
*Description:
*		This function is to send volatage range to the SD card
*	for the high capacity card, bit30 should be set to 1. high capacticy card
*	will response with relative bit to 1. This is critical
********************************************************************/
int send_op_cond(void *pAtaMediaState)
{
volatile unsigned short stat;
long op_timeout;
unsigned long           OpLoop;
AtaSDState *handle = pAtaMediaState;
AtaSDInternalState *inhandle = handle->ptr;

op_timeout       = OP_TIMEOUT;
OpLoop           = 0;

   // You will go through this loop at least twice.  The first time
   // you will apply your requested voltage level and receive a
   // busy.  The second time you re-apply your voltage level and
   // generaly get a not busy.  This indicates the card has
   // initialized.

   // Clear status
   stat = inhandle->pMMC->st0;

   do
   {
      // For debug.  Want to know how many loops it takes.
      OpLoop++;

      // Set minimal voltage range
      inhandle->pMMC->argh   = (VDD_32_33 >> 16) & 0xFFFF;
      inhandle->pMMC->argl   = (VDD_32_33 & 0xFFFF);
      inhandle->pMMC->cmdl    = SD_SEND_OP_COND;

      // Break on response fail

      if( wait_for( RSPDNE,pAtaMediaState))
         break;

 		// Resp = R3, Busy bit is in MSB of rsp7.  If '1' (not busy) then we are done.
      stat = inhandle->pMMC->rsp7;
      if( stat & 0x8000 )
      {
         //This is for the high capacity card, which will affect the address
         //since in the high capacity, block address replace byte address, 01/10/2008 yihehu
         if(stat & 0x4000)	//high capacity card
	         inhandle->high_capacity_card = 1;
         else
	         inhandle->high_capacity_card = 0;
         return( 0 );
      }
	}while( op_timeout > 0 );

   // Timed out
   return( -1 );
}
/*******************************************************************
* int all_send_cid(void *pAtaMediaState)
* input:
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*			error status
*Description:
*		This function is to send get CID command
********************************************************************/
int all_send_cid(void *pAtaMediaState)
{
volatile unsigned short stat;
AtaSDState *handle = pAtaMediaState;
AtaSDInternalState *inhandle = handle->ptr;

   stat = inhandle->pMMC->st0;

   clear_rsp(pAtaMediaState);

   inhandle->pMMC->argh = STUFF_BITS;
   inhandle->pMMC->argl = STUFF_BITS;
   inhandle->pMMC->cmdl  = ALL_SEND_CID;
	if( wait_for( RSPDNE,pAtaMediaState ) == 0 )
	{
      // Resp = R2, Should probably check for something
      get_rsp(pAtaMediaState);
      return( 0 );
	}
	return( -1 );
}
/*******************************************************************
* int send_relative_addr(void *pAtaMediaState)
* input:
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*			error status
*Description:
*		This function is to send get RCA command and get the rca
********************************************************************/
int send_relative_addr(void *pAtaMediaState)
{
AtaSDState *handle = pAtaMediaState;
AtaSDInternalState *inhandle = handle->ptr;

   clear_rsp(pAtaMediaState);

   inhandle->pMMC->argh = STUFF_BITS;
   inhandle->pMMC->argl = STUFF_BITS;
   inhandle->pMMC->cmdl  = SEND_RELATIVE_ADDR;
   if( wait_for( RSPDNE,pAtaMediaState ) == 0 )
   {
      // Resp = R1
      get_card_rca(pAtaMediaState);
      return(0);
   }
   return( -1 );
}
/*******************************************************************
* int send_status(unsigned short rca,void *pAtaMediaState)
* input:
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*			error status
*Description:
*		This function is to send SD status command
********************************************************************/
int send_status(unsigned short rca,void *pAtaMediaState)
{
AtaSDState *handle = pAtaMediaState;
AtaSDInternalState *inhandle = handle->ptr;

   clear_rsp(pAtaMediaState);


   inhandle->pMMC->argh = rca;
   inhandle->pMMC->argl = STUFF_BITS;
   inhandle->pMMC->cmdl  = SEND_STATUS;

   if(SD_TI_wait_for( RSPDNE,pAtaMediaState ) == 0 )
   {
      // Resp = R1
      get_card_status(pAtaMediaState);
      return(0);
   }
   return( -1 );
}
/*******************************************************************
* int send_cid(unsigned short rca, void *pAtaMediaState)
* input:
*			rca					sd relative address
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*			error status
*Description:
*		This function is to send CID to SD card
********************************************************************/
int send_cid(unsigned short rca, void *pAtaMediaState)
{
AtaSDState *handle = pAtaMediaState;
AtaSDInternalState *inhandle = handle->ptr;

   inhandle->pMMC->argh = rca;
   inhandle->pMMC->argl = STUFF_BITS;
   inhandle->pMMC->cmdl  = SEND_CID;
	if( wait_for( RSPDNE, pAtaMediaState) == 0 )
	{
      // Resp = R2
      get_card_cid(pAtaMediaState);
      return(0);
	}
   return( -1 );
}
/*******************************************************************
* int send_csd(unsigned short rca, void *pAtaMediaState)
* input:
*			rca					sd relative address
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*			error status
*Description:
*		This function is to send CSD to SD card
********************************************************************/
int send_csd(unsigned short rca, void *pAtaMediaState)
{
AtaSDState *handle = pAtaMediaState;
AtaSDInternalState *inhandle = handle->ptr;

   inhandle->pMMC->argh = rca;
   inhandle->pMMC->argl = STUFF_BITS;
   inhandle->pMMC->cmdl  = SEND_CSD;
   if( wait_for( RSPDNE,pAtaMediaState) == 0 )
   {
      // Resp = R2
      get_card_csd(pAtaMediaState);
      return(0);
	}
   return( -1 );
}
/*******************************************************************
* int select_card(unsigned short rca, void *pAtaMediaState)
* input:
*			rca					sd relative address
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*			error status
*Description:
*		This function is to select the SD card matched with RCA
********************************************************************/
int select_card(unsigned short rca, void *pAtaMediaState)
{
AtaSDState *handle = pAtaMediaState;
AtaSDInternalState *inhandle = handle->ptr;
   // Select the card then check status to see if
   // we are in the proper state

   inhandle->pMMC->argh = rca;
   inhandle->pMMC->argl = STUFF_BITS;
   inhandle->pMMC->cmdl  = SELECT_CARD;
   if( wait_for( RSPDNE,pAtaMediaState ) == 0 )
   {
      if( rca > 0 )
      {
         if(( send_status( rca,pAtaMediaState) == 0 )
          && (( inhandle->CardStatus.state ==  tran )
	           || ( inhandle->CardStatus.state ==  prg)))
            return(0);
      }
	}
	return( -1 );
}
/*******************************************************************
* int deselect_card( unsigned short rca, void *pAtaMediaState)
* input:
*			rca					sd relative address
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*			error status
*Description:
*		This function is to deselect the SD card matched with RCA
********************************************************************/
int deselect_card( unsigned short rca, void *pAtaMediaState)
{
AtaSDState *handle = pAtaMediaState;
AtaSDInternalState *inhandle = handle->ptr;

   // Deselect the card then check status to see if
   // we are in the proper state.  Because we are
   // using rca = 0 to deselect there will not be
   // the typical R1 response.

   inhandle->pMMC->argh = 0;
   inhandle->pMMC->argl = STUFF_BITS;
   inhandle->pMMC->cmdl  = DESELECT_CARD;
   if( wait_for( RSPDNE,pAtaMediaState) == 0 )
   {
      if(( send_status( rca,pAtaMediaState) == 0)
	       && (( inhandle->CardStatus.state ==  stby)
		        ||( inhandle->CardStatus.state ==  dis)))
         return(0);
   }
	return( -1 );
}
/*******************************************************************
* int set_blocklen( unsigned short block_len,void *pAtaMediaState)
* input:
*			block_len			length of one block(512)
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*			error status
*Description:
*		This function is to set the block length(512)
********************************************************************/
int set_blocklen( unsigned short block_len,void *pAtaMediaState)
{
AtaSDState *handle = pAtaMediaState;
AtaSDInternalState *inhandle = handle->ptr;

   inhandle->pMMC->argh = 0;
   inhandle->pMMC->argl = block_len;
   inhandle->pMMC->cmdl  = SET_BLOCK_LEN;
   if( wait_for( RSPDNE,pAtaMediaState) == 0 )
   {
      inhandle->CurrentBlen = block_len;
      return(0);
   }
   return( -1 );
}
/*******************************************************************
* int read_single_block( unsigned long address, void *pAtaMediaState)
* input:
*			address			byte address or block address
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*			error status
*Description:
*		This function is to read one block data from assigned address
********************************************************************/
int read_single_block( unsigned long address, void *pAtaMediaState)
{
AtaSDState *handle = pAtaMediaState;
AtaSDInternalState *inhandle = handle->ptr;
unsigned short   count = 256;
int * pBuf  = inhandle->nonsecurebuf;
register volatile unsigned short stat;
int error;
int ddr;
int Done =0;
unsigned short fifo_level = 0;
int i;

   set_blocklen(512,pAtaMediaState);
   //fifo_level tell how many words are available in the fifo to read
   fifo_level = 8 * (handle->fifo_threshold + 1);

   do
   {
	   if( send_status(inhandle->CardStatus.new_rca,pAtaMediaState))
		   return(-1 );
   }while( !inhandle->CardStatus.busy );
	//clear all response
	clear_rsp	(pAtaMediaState);
   //set block length
   inhandle->pMMC->nblk = 1;
   inhandle->pMMC->blen = 512;
   //reset the fifo and set fifo to receiver
   SD_FIFOReset(pAtaMediaState);
   SD_FIFOReceive(pAtaMediaState);
   //send out read command
   inhandle->pMMC->argl = (unsigned short)address;
   inhandle->pMMC->argh = (unsigned short)(address>>16);
   inhandle->pMMC->cmdl  = READ_SINGLE_BLOCK | DATA ;

	do
	{
		do
		{
			stat  = inhandle->pMMC->st0;
			ddr   = ((stat & DRRDY)             != 0);
			error = ((stat & ( CRCRD | TOUTRD)) != 0 );
			// Checking for early DATDNE
			Done  |= (stat & 1);
		}while(  !error && !ddr  );

		//each ddr signal states that there are either 8/16 word data available
		// in the fifo,which is up to the value of the fifo_level 01/10/2008 yihehu
		if(ddr)
		{
			for(i=0;i<fifo_level;i++)
			{
				*pBuf++ = inhandle->pMMC->ddrl;
			}
			count-=fifo_level;
		}

 	}while( !error && count );

   // Test to make sure that we see a DATDNE signal
   if( (error == 0) && (Done == 0) )
   {
      error = SD_TI_wait_for( DATDNE,pAtaMediaState);
      if( error == 0 )
         Done = 1;
   }

   // Check for errors.
   if(error == 0)
      return( 0);
   else
      return(-1);
}
/*******************************************************************
* int write_single_block( unsigned long address,void *pAtaMediaState)
* input:
*			address			byte address or block address
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*			error status
*Description:
*		This function is to write one block data from assigned address
********************************************************************/
int write_single_block( unsigned long address,void *pAtaMediaState)
{
AtaSDState *handle = pAtaMediaState;
AtaSDInternalState *inhandle = handle->ptr;
unsigned short   count = 256;
int * pBuf  = inhandle->nonsecurebuf;
register volatile unsigned short stat;
int error;
int dxr;
int Done = 0;
unsigned short fifo_level = 0;
int i ;


   //fifo_level tells how many words to write each time
   fifo_level = 8 * (handle->fifo_threshold + 1);
   //set the block length
   set_blocklen(512,pAtaMediaState);

	do
	{
   if( send_status(inhandle->CardStatus.new_rca,pAtaMediaState))
      return(-1 );
	}while( !inhandle->CardStatus.busy );

   //set block length
   inhandle->pMMC->nblk = 1;
   inhandle->pMMC->blen = 512;
   //reset fifo and set fifo to transmitter
   SD_FIFOReset(pAtaMediaState);
   SD_FIFOTransmit(pAtaMediaState);
   //send out write command
   inhandle->pMMC->argl = (unsigned short)address;
   inhandle->pMMC->argh = (unsigned short)(address>>16);
   //'using DCLR =0 as per takashi sans suggestion
   //pMMC->cmd  = WRITE_SINGLE_BLOCK | DATA | WRITE| DCLR;
   inhandle->pMMC->cmdl  = WRITE_SINGLE_BLOCK | DATA | WRITE;
   SD_SetTriger (pAtaMediaState);

   do
   {
      do
      {
         stat  = inhandle->pMMC->st0;
         dxr   = ((stat & DXRDY)      != 0 );
         error = ((stat & ( CRCWR   ))!= 0 );
         // Checking for early DATDNE
         Done  |= ( stat & 1 );

		}while( !error && !dxr );

      if(dxr)
      {
         //each dxr event, we can write either 8/16 words up to the fifo_level
         for(i=0;i<fifo_level;i++)
	         inhandle->pMMC->dxrl = *pBuf++;
         count -=fifo_level;
		}

 	}while( !error && count );


      // Test to make sure that we see a DATDNE signal
      if( (error == 0) && (Done == 0) )
      {
         error = SD_TI_wait_for( DATDNE,pAtaMediaState);
         if( error == 0 )
            Done = 1;
      }

   // Check for errors.
   if(error == 0)
      return( 0) ;
   else
      return(-1);
}

/*******************************************************************
* void SD_Enable(MMC_SD_CONTROLLER controller)
* input:
*		controller			slot 0 or 1
* Output:
*			None
* Return:
*			error status
*Description:
*		This function is to enable the mmc/sd slot hardware
********************************************************************/
void SD_Enable(MMC_SD_CONTROLLER controller)
{
	// All system-level clock enable and pin-mux control handled by application
}

/*******************************************************************
* int SD_Init(void *pAtaMediaState)
* input:
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*			error status
*Description:
*		This function is to init the SD card to ready to receive higher
*	level function
********************************************************************/
int SD_Init(void *pAtaMediaState)
{
volatile long delay = 10000;
volatile unsigned short chkCtl = 0;
AtaSDState *handle = pAtaMediaState;
AtaSDInternalState *inhandle = handle->ptr;
int error = 0;

   VDD_32_33	=	0x00FF8000;  // 0x007f

   inhandle->pMMC->ctl= CMDRST | DATRST ;
   inhandle->pMMC->clk &= ~CLKEN; // Turn off the clock before switching to new rate
   inhandle->pMMC->clk =	handle->CDIV;       // Clock divide by 8 ~400KHz
   inhandle->pMMC->im  = 0;   //all interrupts are masked
   inhandle->pMMC->tor = 16;  //response time out
   inhandle->pMMC->tod = 0;   //no data read time out

   //need to set the length of a block in bytes-- must be same as in CSD register
   inhandle->pMMC->blen = BLEN;

   if(handle->data_lines == 4)
      inhandle->pMMC->ctl = 4; //enable dat and cmd lines
   else
      inhandle->pMMC->ctl = 0; //enable dat and cmd lines

   // enable the clock
   inhandle->pMMC->clk |= CLKEN;

   // Wait long enough MMC module to reset.
   while(delay--);
   //idle first
   if( go_idle_state(pAtaMediaState) != 0 )
	{
		return(ERROR_IDLE);
	}
	//send out CMD8 to indication host support SD ver2.0,
	/******************************************************************
	SD VEr2.0 card behaves slightly different with old version. CMD8 is
	required to send before any other commands. if the card response this
	command, we should set HCS bit in the op_cond command to verify whether
	this card is a high capacity or not 01/10/2008 yihehu
	*******************************************************************/
	if(send_interface_condition(pAtaMediaState) == 0)
	{
		//if host ack the CMD8, set the HCS(high capacity) to 1 in the OP_COND command
		VDD_32_33 |= 0x40000000;
	}
	//repeat send op_cond
	do
	{
		delay=10000;
		while(delay--);
		inhandle->securecheck = 0;
		if(StartSecureCMD(0,pAtaMediaState) != 0)
			return(100);

		delay =10000;
		while(delay--);
		error = send_op_cond(pAtaMediaState);

	}while(error !=0);

	inhandle->securecheck = 1;

	// For simulation test there is only one card so skip
	// multiple card stuff
	if( all_send_cid(pAtaMediaState) != 0 )
	{
		return(ERROR_ALL_CID);
	}

	// Set the RCA to something other then the
	// default of 0x0001
	if( send_relative_addr(pAtaMediaState) != 0 )
	{
		return(ERROR_ADDR);
	}

	// Make sure the card is in Standby at this point
	if( send_status( inhandle->CardStatus.new_rca,pAtaMediaState) != 0 )
	{
		return(ERROR_STATUS);
	}
	if( inhandle->CardStatus.state != stby )
	{
		return(ERROR_STATUS);
	}
	// Read the CID via the CID command.
	if( send_cid( inhandle->CardStatus.new_rca,pAtaMediaState) != 0 )
	{
		return(ERROR_CID);
	}

	// Read the CSD and verify that structure is SD v1
	if( send_csd( inhandle->CardStatus.new_rca,pAtaMediaState ) != 0 )
	{
		return(ERROR_CSD);
	}

#if 0	//???? will we support 2.0 or not so we can, 01/10/2008 yihehu
	if(  inhandle->CardCsd.csd_struct != 0 )
	//0 indicates version 1.0
	{
		return(ERROR_CSD_VER);
	}
#endif
   return( 0 );

}
/*******************************************************************
* unsigned short StartSecureCMD(unsigned int rca, void *pAtaMediaState){
* input:
*			rca					relative address
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*			error status
*Description:
*		This function is to precede any secure command
********************************************************************/
unsigned short StartSecureCMD(unsigned int rca, void *pAtaMediaState)
{
AtaSDState *handle = pAtaMediaState;
AtaSDInternalState *inhandle = handle->ptr;

   if(inhandle->securecheck)
   {
      do
      {
	      if(send_status(inhandle->CardStatus.new_rca,pAtaMediaState))
		      return(1);
      }while( !inhandle->CardStatus.busy );
   }

	//make the arguments
 	inhandle->pMMC->argl = STUFF_BITS;
	inhandle->pMMC->argh = rca;	//need to be zeros for_starting card!
	//send the command
	inhandle->pMMC->cmdl = APP_CMD;
	//wait for response
	if(SD_TI_wait_for(RSPDNE,pAtaMediaState) == 0)
	{
		return(0); 	//response was ok
	}
	else
	{
		return(1);	//response was not ok
	}
}
/*******************************************************************
* int read_sector( unsigned long address , unsigned short *pBuf,void *pAtaMediaState)
* input:
*			address				byte address or block address
*			pBuf				pBuf to hold the data
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*			error status
*Description:
*		This function is to read one sector data
********************************************************************/
int read_sector( unsigned long address , unsigned short *pBuf,void *pAtaMediaState)
{
int i,TestFail;

	TestFail = read_single_block(address, pAtaMediaState);
#if 0
	for(i=0; i<256;i++)
			pBuf[i] = byte_swap(pBuf[i]);
#else
	for(i=0; i<256;i++)
			pBuf[i] = pBuf[i];
#endif
	return(TestFail);
}
/*******************************************************************
* int write_sector( unsigned long address , unsigned short *pBuf,
*						int byteswapflag,void *pAtaMediaState)
* input:
*			address				byte address or block address
*			pBuf				pBuf to hold the data
*			byteswapflag		swap 16bit word
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*			error status
*Description:
*		This function is to write one sector data
********************************************************************/
int write_sector( unsigned long address , unsigned short *pBuf, int byteswapflag,void *pAtaMediaState)
{
int i,TestFail;
AtaSDState *handle = pAtaMediaState;
AtaSDInternalState *inhandle = handle->ptr;

#if 1
   if(byteswapflag)
   {
      for(i=0; i<256; i++)
         inhandle->nonsecurebuf[i] = byte_swap(pBuf[i]);
   }
   else
   {
      for(i=0; i<256; i++)
         inhandle->nonsecurebuf[i] = (pBuf[i]);
   }
#else
   for(i=0; i<256; i++)
      inhandle->nonsecurebuf[i] = (pBuf[i]);
#endif


   TestFail = write_single_block(address, pAtaMediaState);

   return(TestFail);
}
/*******************************************************************
* AtaError SD_FIFOReset(void *pAtaMediaState)* input:
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*			error status
*Description:
*		This function is to reset fifo
********************************************************************/
AtaError SD_FIFOReset(void *pAtaMediaState)
{
AtaSDState *handle = pAtaMediaState;
AtaSDInternalState *inhandle = handle->ptr;

	inhandle->pMMC->mmcfifoctl |= FIFO_RESET;
	inhandle->pMMC->mmcfifoctl &=~(1<<FIFO_THRESHOLD_SHIFT);
	return(ATA_ERROR_NONE);
}
/*******************************************************************
* AtaError SD_FIFOReceive(void *pAtaMediaState)
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*			error status
*Description:
*		This function is to set fifo to receiver
********************************************************************/
AtaError SD_FIFOReceive(void *pAtaMediaState)
{
AtaSDState *handle = pAtaMediaState;
AtaSDInternalState *inhandle = handle->ptr;

	inhandle->pMMC->mmcfifoctl &=~ FIFO_TRANSMITTE;
	inhandle->pMMC->mmcfifoctl |=2<<FIFO_ACCESS_WIDTH_SHIFT;
	inhandle->pMMC->mmcfifoctl |=	(((AtaSDState *)pAtaMediaState)->fifo_threshold) << FIFO_THRESHOLD_SHIFT;
	return(ATA_ERROR_NONE);
}
/*******************************************************************
* AtaError SD_FIFOTransmit(void *pAtaMediaState)
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*			error status
*Description:
*		This function is to set fifo to transmitter
********************************************************************/
AtaError SD_FIFOTransmit(void *pAtaMediaState)
{
AtaSDState *handle = pAtaMediaState;
AtaSDInternalState *inhandle = handle->ptr;

	inhandle->pMMC->mmcfifoctl |= FIFO_TRANSMITTE;
	inhandle->pMMC->mmcfifoctl |=2<<FIFO_ACCESS_WIDTH_SHIFT;
	inhandle->pMMC->mmcfifoctl |=	(((AtaSDState *)pAtaMediaState)->fifo_threshold) << FIFO_THRESHOLD_SHIFT;
	return(ATA_ERROR_NONE);
}
/*******************************************************************
* AtaError SD_SetDataSize(unsigned short numblks, void *pAtaMediaState)
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*			error status
*Description:
*		This function is to set data size
********************************************************************/
AtaError SD_SetDataSize(unsigned short numblks, void *pAtaMediaState)
{
AtaSDState *handle = pAtaMediaState;
AtaSDInternalState *inhandle = handle->ptr;

	inhandle->pMMC->nblk = numblks;
	inhandle->pMMC->blen =512;

	return(ATA_ERROR_NONE);
}
/*******************************************************************
* AtaError SD_SetTriger(void *pAtaMediaState)
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*			error status
*Description:
*		This function is to set DMA trigger in the command register
********************************************************************/
AtaError SD_SetTriger(void *pAtaMediaState)
{
AtaSDState *handle = pAtaMediaState;
AtaSDInternalState *inhandle = handle->ptr;

	inhandle->pMMC->cmdh = DMATRIGER;
	return(ATA_ERROR_NONE);
}

