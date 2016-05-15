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
* This file includes low level MMC API
*********************************************************************
* Initial versioin is from old unknown project					   **
* Revision 1.0 1/08/08  Yihe Hu		 	                           **
********************************************************************/
#include "mmc_sd_c55.h"
#include "ata.h"
#include "ata_.h"
#include "mmc.h"
#include "ata_mmc.h"

//global variable
extern AtaSerialID MMC_MediaSerialID;

/*******************************************************************
* static void MMC_Get_Rsp(void *pAtaMediaState)
* input:
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*		None
*Description:
*		This function is to get all response
********************************************************************/
static void MMC_Get_Rsp(void *pAtaMediaState)
{
   PC55XX_MMC pMMC = (PC55XX_MMC) (((AtaMMCState *)pAtaMediaState)->C55XX_MMC_CTL_ADDR);

   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp0 = pMMC->rsp0;
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp1 = pMMC->rsp1;
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp2 = pMMC->rsp2;
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp3 = pMMC->rsp3;
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp4 = pMMC->rsp4;
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp5 = pMMC->rsp5;
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp6 = pMMC->rsp6;
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp7 = pMMC->rsp7;
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.drsp = pMMC->drsp;
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.etok = pMMC->etok;
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.cidx = pMMC->cidx;

}
/*******************************************************************
* static void MMC_Get_Card_Status(void *pAtaMediaState)
* input:
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*		None
*Description:
*		This function is to get card status
********************************************************************/
static void MMC_Get_Card_Status(void *pAtaMediaState)
{
   PC55XX_MMC pMMC = (PC55XX_MMC) (((AtaMMCState *)pAtaMediaState)->C55XX_MMC_CTL_ADDR);

   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp6 = pMMC->rsp6;

   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp7 = pMMC->rsp7;

   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardStatus.busy		  = ((((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp6 >> 8)  & 1   );
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardStatus.state		 = (CARD_STATE)((((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp6 >> 9)  & 0xf );
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardStatus.erase_clrd	= ((((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp6 >> 13) & 1   );
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardStatus.erase_partial = ((((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp6 >> 15) & 1   );
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardStatus.error_flags   = ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp7;

}
/*******************************************************************
* static void MMC_Get_Card_Cid(void *pAtaMediaState)
* input:
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*		None
*Description:
*		This function is to get card cid
********************************************************************/
static void MMC_Get_Card_Cid(void *pAtaMediaState)
{
unsigned long temp;
int i=0;

   for(i=0;i<8;i++)
      MMC_MediaSerialID[i] = 0;

   MMC_Get_Rsp(pAtaMediaState);

   MMC_MediaSerialID[i++] = ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCid.year			 = (((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp0 >> 8   ) & 0xF; // 11:8
   MMC_MediaSerialID[i++] = ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCid.month			= (((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp0 >> 12  ) & 0xF; // 12:15
   MMC_MediaSerialID[i++] = ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCid.serial			 = (unsigned long)(((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp1);// 39:16
   MMC_MediaSerialID[i++] = temp					 = (unsigned long)(((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp2 & 0x00FF);
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCid.serial		   |= (temp<<16);
   MMC_MediaSerialID[i++] = ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCid.fw_rev			= (((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp2 >>8  ) & 0x000F;  // 43:40
   MMC_MediaSerialID[i++] = ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCid.hw_rev			= (((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp2 >>12 ) & 0x000F;  // 47:44
   MMC_MediaSerialID[i++] = ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCid.mfg_id			= (((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp6 >>8  ) & 0x00FF;  //127:104
   MMC_MediaSerialID[i++] = temp					 = (unsigned long)(((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp7	  );
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCid.mfg_id			|= temp << 16;

}
/*******************************************************************
* static void MMC_Get_Card_Csd(void *pAtaMediaState)
* input:
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*		None
*Description:
*		This function is to get card csd
********************************************************************/
static void MMC_Get_Card_Csd(void *pAtaMediaState)
{
   MMC_Get_Rsp(pAtaMediaState);

   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCsd.ecc				= (((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp0 >>  8   ) & 3;		  // 9:8
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCsd.tmp_wp		  = (((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp0 >>  12  ) & 1;		  // 12:12
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCsd.perm_wp		 = (((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp0 >>  13  ) & 1;		  // 13:13
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCsd.copy			= (((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp0 >>  14  ) & 1;			// 14:14

   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCsd.wr_bl_part	  = (((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp1 >>   5  ) & 1;		  // 21:21
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCsd.wr_bl_len	   = (((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp1 >>   6  ) & 0xF;		// 25:22
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCsd.r2w_factor	  = (((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp1 >>  10  ) & 0x7;		// 28:26
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCsd.def_ecc		 = (((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp1 >>  13  ) & 0x3;		// 30:29
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCsd.wp_brp_ena	  = (((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp1 >>  15  ) & 1;		  // 31:31

   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCsd.wp_grp_size	 = (((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp2		 ) & 0x1f;	   // 36:32
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCsd.er_grp_size	 = (((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp2  >>  5  ) & 0x1f;	   // 41:37
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCsd.sect_size	   = (((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp2  >> 10  ) & 0x1f;	   // 46:42
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCsd.c_size_mult	 = (((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp2  >> 15  ) & 1;		  // 49:47
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCsd.c_size_mult	 |=( (((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp3 &  3  ) << 1);

   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCsd.vdd_w_curr_max  = (((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp3  >>  2  ) & 0x7;		  // 52:50
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCsd.vdd_w_curr_min  = (((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp3  >>  5  ) & 0x7;		// 55:53
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCsd.vdd_r_curr_max  = (((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp3  >>  8  ) & 0x7;		// 58:56
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCsd.vdd_r_curr_min	= (((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp3  >>  11 ) & 0x7;		// 61:59
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCsd.c_size		  = (((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp3  >>  14 ) & 0x3;		// 73:62
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCsd.c_size		  |=((((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp4  & 0x3FF) << 2);

   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCsd.dsr_imp		 = (((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp4  >> 12  ) & 1;			// 76:76
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCsd.rd_blk_misal	= (((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp4  >> 13  ) & 1;		  // 77:77
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCsd.wr_blk_misal	= (((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp4  >> 14  ) & 1;		  // 78:78
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCsd.rd_bl_part	  = (((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp4  >> 15  ) & 1;		  // 79:79

   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCsd.rd_bl_len	   =  1<< ((((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp5		) & 0xf);		// 83:80
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCsd.ccc			 =  (((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp5 >>  4  ) & 0xfff;		// 895:84

   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCsd.tran_speed	  =  (((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp6		) & 0xff;		// 103:96
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCsd.nsac			=  (((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp6  >>  8 ) & 0xff;		// 111:104

   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCsd.taac			=  (((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp7		) & 0xff;	   // 119:112
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCsd.mmc_prot		=  (((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp7  >> 10 ) & 0xf;		// 125:122
   ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCsd.csd_struct	  =  (((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->Mmc.rsp7  >> 14 ) & 0x3;		// 127:126

}

/*******************************************************************
static int wait_for(void *pAtaMediaState, unsigned short event )
* input:
*			pAtaMediaState		MMC state
*			event				event to wait
* Output:
*			None
* Return:
*			success or not
*Description:
*		This function is to get desired event
********************************************************************/
static int wait_for(void *pAtaMediaState, unsigned short event )
{
volatile unsigned short stat;

	PC55XX_MMC pMMC = (PC55XX_MMC) (((AtaMMCState *)pAtaMediaState)->C55XX_MMC_CTL_ADDR);


   // This function assumes that a command will timeout,otherwise we will get hung here forever.
   do
   {
      stat = pMMC->st0;
      stat &= (SPIERR | CRCRS | CRCRD | CRCWR | TOUTRS | TOUTRD | event);
   }while( stat == 0 );
   //If we got the event we were looking for then return pass. We may have gotten an event but
   // maybe not the one we were expecting, i.e. a time our or crc.

   return( ((stat & event) == event ) ? 0 : -1 );
}

/*******************************************************************
* static int MMC_Go_Idle_State(void *pAtaMediaState)
* input:
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*		 sucess or fail
*Description:
*		This function is to get IDLE state
********************************************************************/
static int MMC_Go_Idle_State(void *pAtaMediaState)
{
	PC55XX_MMC pMMC = (PC55XX_MMC) (((AtaMMCState *)pAtaMediaState)->C55XX_MMC_CTL_ADDR);

	pMMC->argh = STUFF_BITS;
	pMMC->argl = STUFF_BITS;

	// Add in the INIT bit so that we wait for the 80, clocks before sending the intial command
	pMMC->cmdl  = GO_IDLE_STATE | INIT | DCLR;

	// Resp = NONE, just check for command sent
	return( wait_for(pAtaMediaState, RSPDNE ));
}

#define VDD_32_34		0x0030
/*******************************************************************
* static int MMC_Send_Op_Cond(void *pAtaMediaState)
* input:
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*		 sucess or fail
*Description:
*		This function is to set the volatage range
********************************************************************/
static int MMC_Send_Op_Cond(void *pAtaMediaState)
{
volatile unsigned short stat;
long op_timeout;

	PC55XX_MMC pMMC = (PC55XX_MMC) (((AtaMMCState *)pAtaMediaState)->C55XX_MMC_CTL_ADDR);

	// Set flags to busy and fail
   op_timeout	   = OP_TIMEOUT;

   //   You will go through this loop at least twice.  The first time
   //   you will apply your requested voltage level and receive a
   //   busy.  The second time you re-apply your voltage level and
   //   generaly get a not busy.  This indicates the card has
   //   initialized.

	// Clear status
	stat = pMMC->st0;

	do
	{
      // Set minimal voltage range
      pMMC->argh   = VDD_32_34;
      pMMC->argl   = 0;
      pMMC->cmdl	= SEND_OP_COND;

      //  Break on response fail
      if( wait_for(pAtaMediaState, RSPDNE ))
         break;
      // Resp = R3  Busy bit is in MSB of rsp7.  If '1' (not busy) then we are done.
      stat = pMMC->rsp7;
      if( stat & 0x8000 )
      {
	      return( 0 );
		}
		op_timeout--;

	}while( op_timeout > 0 );

	// Timed out
	return( -1 );
}

/*******************************************************************
* static int MMC_All_Send_Cid( void *pAtaMediaState )
* input:
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*		 sucess or fail
*Description:
*		This function is to send CID command to MMC card
********************************************************************/
static int MMC_All_Send_Cid( void *pAtaMediaState )
{
	PC55XX_MMC pMMC = (PC55XX_MMC) (((AtaMMCState *)pAtaMediaState)->C55XX_MMC_CTL_ADDR);
	volatile unsigned short stat;

	stat = pMMC->st0;

	MMC_Clear_Rsp(pAtaMediaState);

	pMMC->argh = STUFF_BITS;
	pMMC->argl = STUFF_BITS;
	pMMC->cmdl  = ALL_SEND_CID;
	if( wait_for(pAtaMediaState, RSPDNE ) == 0 )
	{
		 // Resp = R2, Should probably check for something get_rsp();
		return( 0 );
	}

	return( -1 );
}


/*******************************************************************
* static int MMC_Set_Relative_Addr(void *pAtaMediaState, unsigned short rca)
* input:
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*		 sucess or fail
*Description:
*		This function is to set relative address
NOTE the default rca is 0x0001.  0x0000 is used by CMD7 to deselect
all the cards, so you cannot assign it to a card
********************************************************************/
static int MMC_Set_Relative_Addr(void *pAtaMediaState, unsigned short rca)
{

	PC55XX_MMC pMMC = (PC55XX_MMC) (((AtaMMCState *)pAtaMediaState)->C55XX_MMC_CTL_ADDR);

   MMC_Clear_Rsp(pAtaMediaState);

	pMMC->argh = rca;
	pMMC->argl = STUFF_BITS;
	pMMC->cmdl  = SET_RELATIVE_ADDR;
	if( wait_for(pAtaMediaState, RSPDNE ) == 0 )
	{
		// Resp = R1
		MMC_Get_Card_Status(pAtaMediaState);
		return(0);
	}
	return( -1 );
}
/*******************************************************************
* static int MMC_Send_Cid(void *pAtaMediaState, unsigned short rca)
* input:
*			pAtaMediaState		MMC state
*			rca					relative address
* Output:
*			None
* Return:
*		 sucess or fail
*Description:
*		This function is to send CID command
********************************************************************/
static int MMC_Send_Cid(void *pAtaMediaState, unsigned short rca)
{

	PC55XX_MMC pMMC = (PC55XX_MMC) (((AtaMMCState *)pAtaMediaState)->C55XX_MMC_CTL_ADDR);

	pMMC->argh = rca;
	pMMC->argl = STUFF_BITS;
	pMMC->cmdl  = SEND_CID;
	if( wait_for(pAtaMediaState, RSPDNE ) == 0 )
	{
		MMC_Get_Card_Cid(pAtaMediaState);
		return(0);
	}
	return( -1 );
}

/*******************************************************************
* static int MMC_Send_Csd(void *pAtaMediaState, unsigned short rca)
* input:
*			pAtaMediaState		MMC state
*			rca					relative address
* Output:
*			None
* Return:
*		 sucess or fail
*Description:
*		This function is to send CSD command
********************************************************************/
static int MMC_Send_Csd(void *pAtaMediaState, unsigned short rca)
{

	PC55XX_MMC pMMC = (PC55XX_MMC) (((AtaMMCState *)pAtaMediaState)->C55XX_MMC_CTL_ADDR);

	pMMC->argh = rca;
	pMMC->argl = STUFF_BITS;
	pMMC->cmdl  = SEND_CSD;
	if( wait_for(pAtaMediaState, RSPDNE ) == 0 )
	{
		MMC_Get_Card_Csd(pAtaMediaState);
		return(0);
	}
	return( -1 );
}

/*******************************************************************
* void MMC_Clear_Rsp(void *pAtaMediaState)
* input:
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*		None
*Description:
*		This function is to clear all response
********************************************************************/
void MMC_Clear_Rsp(void *pAtaMediaState)
{

	PC55XX_MMC pMMC = (PC55XX_MMC) (((AtaMMCState *)pAtaMediaState)->C55XX_MMC_CTL_ADDR);

	pMMC->rsp0 = 0;

	pMMC->rsp1 = 0;

	pMMC->rsp2 = 0;

	pMMC->rsp3 = 0;

	pMMC->rsp4 = 0;

	pMMC->rsp5 = 0;

	pMMC->rsp6 = 0;

	pMMC->rsp7 = 0;

//	pMMC->drsp = 0;

//	pMMC->etok = 0;

	pMMC->cidx = 0;

}

/*******************************************************************
* int MMC_Send_Status(void *pAtaMediaState, unsigned short rca)
* input:
*			pAtaMediaState		MMC state
*			rca					relative address
* Output:
*			None
* Return:
*		 sucess or fail
*Description:
*		This function is to send status command to MMC card
********************************************************************/
int MMC_Send_Status(void *pAtaMediaState, unsigned short rca)
{

	PC55XX_MMC pMMC = (PC55XX_MMC) (((AtaMMCState *)pAtaMediaState)->C55XX_MMC_CTL_ADDR);

   MMC_Clear_Rsp(pAtaMediaState);

	pMMC->argh = rca;

	pMMC->argl = STUFF_BITS;

	pMMC->cmdl  = SEND_STATUS;

	if( wait_for(pAtaMediaState, RSPDNE ) == 0 )
	{
      MMC_Get_Card_Status(pAtaMediaState);
      return(0);
	}
	return( -1 );
}

/*******************************************************************
* int MMC_SelectCard(void *pAtaMediaState, unsigned short rca)
* input:
*			pAtaMediaState		MMC state
*			rca					relative address
* Output:
*			None
* Return:
*		 sucess or fail
*Description:
*		This function is to select the card
********************************************************************/
int MMC_SelectCard(void *pAtaMediaState, unsigned short rca)
{

	PC55XX_MMC pMMC = (PC55XX_MMC) (((AtaMMCState *)pAtaMediaState)->C55XX_MMC_CTL_ADDR);

	// Select the card then check status to see if  we are in the proper state
	pMMC->argh = rca;
	pMMC->argl = STUFF_BITS;
	pMMC->cmdl  = SELECT_CARD;
	if( wait_for(pAtaMediaState, RSPDNE ) == 0 )
	{
		if( rca > 0 )
		{
			if(( MMC_Send_Status(pAtaMediaState, rca ) == 0 )
				&& ((((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardStatus.state ==  tran )
					|| (((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardStatus.state ==  prg  )))
				return(0);
		}
	}
	return( -1 );
}
/*******************************************************************
* int MMC_Set_Blocklen(void *pAtaMediaState, unsigned short block_len )
* input:
*			pAtaMediaState		MMC state
*			block_len			size of one block
* Output:
*			None
* Return:
*		 sucess or fail
*Description:
*		This function is to set bkick length
********************************************************************/
int MMC_Set_Blocklen(void *pAtaMediaState, unsigned short block_len )
{

	PC55XX_MMC pMMC = (PC55XX_MMC) (((AtaMMCState *)pAtaMediaState)->C55XX_MMC_CTL_ADDR);

	pMMC->argl = block_len;
	pMMC->argh = 0;
	pMMC->cmdl  = SET_BLOCKLEN;
	if( wait_for(pAtaMediaState, RSPDNE ) == 0 )
	{
		return(0);
	}
	return( -1 );
}
/*******************************************************************
void Mmc_Enable(MMC_CONTROLLER controller)
* input:
*			controller
* Output:
*			None
* Return:
*			none
*Description:
*		This function is to enable the MMC hardware
********************************************************************/
void Mmc_Enable(MMC_SD_CONTROLLER controller)
{
	// All system-level clock enable and pin-mux control handled by application
}
/*******************************************************************
* int MMC_Init(void *pAtaMediaState)
* input:
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*		 sucess or fail
*Description:
*		This function is to set lovel MMC slot hardware and MMC card
********************************************************************/
int MMC_Init(void *pAtaMediaState)
{
volatile long delay = 30000;

	// pMMC points to the MMC Module Base address
	PC55XX_MMC pMMC;
	//enable the relative slot
	if(((AtaMMCState *)pAtaMediaState)->mmcNumber == 0)
	{
		Mmc_Enable(MMC_SD_CONTROLLER_0);
		((AtaMMCState *)pAtaMediaState)->C55XX_MMC_CTL_ADDR = COROZON_MMC_SD_0_ADDRESS;
	}
	else if(((AtaMMCState *)pAtaMediaState)->mmcNumber == 1)
	{
		Mmc_Enable(MMC_SD_CONTROLLER_1);
		((AtaMMCState *)pAtaMediaState)->C55XX_MMC_CTL_ADDR = COROZON_MMC_SD_1_ADDRESS;
	}
	else
		return(ERROR_CONTROLLER);


	pMMC = (PC55XX_MMC) (((AtaMMCState *)pAtaMediaState)->C55XX_MMC_CTL_ADDR);
	// Reset the command & data
	pMMC->ctl= CMDRST | DATRST;

	// Turn off the clock before switching to new rate
	pMMC->clk &= ~CLKEN;

	// MMC clock = function clock / (MMCCLK_RATE + 1)
	pMMC->clk =	MMC_INIT_RATE;

	// Mask all interrupts
	pMMC->im  = 0;
	// Set Response TimeOut Value
   pMMC->tor = 16;
   pMMC->tod = ((AtaMMCState *)pAtaMediaState)->Data_timeout_count;

	//Need to set the length of a block in bytes-- must be same as in CSD register
	pMMC->blen = BLEN;

	// Now enable dat and cmd lines
	pMMC->ctl = 0;

	// Enable the clock
	pMMC->clk |= CLKEN;

	// Wait long enough MMC module to reset.
	while(delay--);

	// Go to Idle State
	if( MMC_Go_Idle_State(pAtaMediaState) != 0 )
	{
      return (ERROR_IDLE);
	}

	// Send the operating Voltage Range
	if( MMC_Send_Op_Cond(pAtaMediaState) != 0 )
	{
		return (ERROR_OP);
	}

	// Ask all cards to send their CIDs
	if( MMC_All_Send_Cid(pAtaMediaState) != 0 )
	{
		return (ERROR_ALL_CID);
	}
   //set address
	if( MMC_Set_Relative_Addr(pAtaMediaState, ((AtaMMCState *)pAtaMediaState)->MMC_CARD_ID ) != 0 )
	{
		return (ERROR_ADDR);
	}
   //set up clock and enable it
	pMMC->clk =	((AtaMMCState *)pAtaMediaState)->CLOCK_RATE;
	pMMC->clk |= CLKEN;


	// Ask the card to send its status
	if( MMC_Send_Status(pAtaMediaState, ((AtaMMCState *)pAtaMediaState)->MMC_CARD_ID ) != 0 )
	{
		return (ERROR_STATUS);
	}
	if( ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardStatus.state != stby )
	{
		return (ERROR_STATUS);
	}

	// Read the CID of the card via the CID command. Check for the Sandisk ID.
	if( MMC_Send_Cid(pAtaMediaState, ((AtaMMCState *)pAtaMediaState)->MMC_CARD_ID ) != 0 )
	{
		return (ERROR_CID);
	}

	/* Read the CSD and verify that structure is for MMC protocol version 1.4  */
	if( MMC_Send_Csd(pAtaMediaState, ((AtaMMCState *)pAtaMediaState)->MMC_CARD_ID ) != 0 )
	{
		return (ERROR_CSD);
	}
	#if 0
	if(  ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardCsd.csd_struct != 1 )
	{
		return (ERROR_CSD_VER);
	}
	#endif
	return( 0 );
}

/*******************************************************************
* AtaError _MMC_ReadNextSectorWords(void *pAtaMediaState, unsigned short *word)
* input:
*			pAtaMediaState		MMC state
*			word				point the data to be written
* Output:
*			None
* Return:
*		 error status
*Description:
*		This function is to send the specified command to the MMC Card
*   Make sure to set the argl and argh registers before writing
*   to the cmd register. In case of no arguments for the command,
*   stuff the srgument registers with zeroes.
*******************************************************************/
unsigned short MMC_SendCommand(unsigned short cmd,unsigned short argh,unsigned short argl,void *pAtaMediaState)
{

	PC55XX_MMC pMMC = (PC55XX_MMC) (((AtaMMCState *)pAtaMediaState)->C55XX_MMC_CTL_ADDR);

	do
	{
		if(MMC_Send_Status(pAtaMediaState, ((AtaMMCState *)pAtaMediaState)->MMC_CARD_ID))
      {
			return(1 );
		}
   }while( !((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardStatus.busy );

   do
	{
		if( MMC_Send_Status(pAtaMediaState, ((AtaMMCState *)pAtaMediaState)->MMC_CARD_ID ))
      {
			return(1 );
		}
	}while(((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardStatus.state != tran);


	pMMC->argl = argl;

	pMMC->argh = argh;

	pMMC->cmdl  = cmd;

	return(0);

}
/*******************************************************************
* AtaError _MMC_ReadNextSectorWords(void *pAtaMediaState, unsigned short *word)
* input:
*			pAtaMediaState		MMC state
*			word				point the data to be written
* Output:
*			None
* Return:
*		 error status
*Description:
*		This function is to read bext sector
********************************************************************/
AtaError _MMC_ReadNextSectorWords(void *pAtaMediaState, unsigned short *word)
{
register volatile unsigned short stat;
int error;
int ddr;
unsigned short fifoDDRReadCnt ;
unsigned short count = 256;
unsigned long ii=0;

   PC55XX_MMC pMMC = (PC55XX_MMC) (((AtaMMCState *)pAtaMediaState)->C55XX_MMC_CTL_ADDR);

   fifoDDRReadCnt= 8*(((AtaMMCState *)pAtaMediaState)->fifo_threshold + 1);
	do
   {
      if(pMMC->tod == 0)
      {
			do
			{
            stat  = pMMC->st0;
            // Check to see if the data ready bit is set
            ddr = ((stat & DRRDY) != 0);
            // Make sure there is no CRC Read Error or Command TimeOut
            error = ((stat & ( CRCRD | TOUTRD)) != 0 );
			}while(  !error && !ddr  );
      }
      else
      {
         do
         {

            stat  = pMMC->st0;
            // Check to see if the data ready bit is set
            ddr   = ((stat & DRRDY)			 != 0);
            // Make sure there is no CRC Read Error or Command TimeOut
            error = ((stat & ( CRCRD | TOUTRD)) != 0 );
            ii++;
            if(ii++>=(((AtaMMCState *)pAtaMediaState)->Data_timeout_count))
               return(ATA_ERROR_MEDIA_REMOVED);

				}while(  !error && !ddr  );
      }
      //read up to 16 byte which is decided by the fifo_level
		for(ii=0;ii<fifoDDRReadCnt;ii++)
			*word++ = pMMC->ddrl;
		count -=8;
	}while(!error && count);

	return(ATA_ERROR_NONE);
}
/*******************************************************************
* AtaError _MMC_WriteNextWord(void *pAtaMediaState, unsigned short *buffer)
* input:
*			pAtaMediaState		MMC state
*			buffer				point the data to be written
* Output:
*			None
* Return:
*		 error status
*Description:
*		This function is to write next fifo data
********************************************************************/
AtaError _MMC_WriteNextWord(void *pAtaMediaState, unsigned short *buffer)
{
register volatile unsigned short stat;
int error;
int dxr;
unsigned long ii=0;
unsigned short fifoDDRReadCnt = 0;

	PC55XX_MMC pMMC = (PC55XX_MMC) (((AtaMMCState *)pAtaMediaState)->C55XX_MMC_CTL_ADDR);
   fifoDDRReadCnt= 8*(((AtaMMCState *)pAtaMediaState)->fifo_threshold + 1);

   if(pMMC->tod == 0)
   {
      do
      {
      stat  = pMMC->st0;
      // Check to see if the data ready bit is set
      dxr   = ((stat & DXRDY)	  != 0 );
      // Make sure there is no CRC Write Error
      error = ((stat & ( CRCWR   ))!= 0 );
   	}while( !error && !dxr );
   }
   else
   {
      do
      {
         stat  = pMMC->st0;
         // Check to see if the data ready bit is set
         dxr   = ((stat & DXRDY)	  != 0 );
         // Make sure there is no CRC Write Error
         error = ((stat & ( CRCWR   ))!= 0 );
         ii++;
         if(ii++>=(((AtaMMCState *)pAtaMediaState)->Data_timeout_count))
            return(ATA_ERROR_MEDIA_REMOVED);
   	}while( !error && !dxr );
   }
	// Write the word to the Data Register
	for (ii= 0; ii<fifoDDRReadCnt;ii++)
		pMMC->dxrl = *buffer++;
	return(ATA_ERROR_NONE);

}

#if 0
/*******************************************************************
* AtaError MMC_cardLockUnlockPasswd(void *pAtaMediaState, int lock, char *password)
* input:
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*		 error status
*Description:
*		This function is to unlock card
********************************************************************/
AtaError MMC_cardLockUnlockPasswd(void *pAtaMediaState, int lock, char *password)
{
	PC55XX_MMC pMMC = (PC55XX_MMC) (((AtaMMCState *)pAtaMediaState)->C55XX_MMC_CTL_ADDR);

	do
	{
		if( MMC_Send_Status(pAtaMediaState, ((AtaMMCState *)pAtaMediaState)->MMC_CARD_ID))
      {
			return(ATA_ERROR_TIMEOUT);
		}
   }while( !((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardStatus.busy );

   do
	{
		if( MMC_Send_Status(pAtaMediaState, ((AtaMMCState *)pAtaMediaState)->MMC_CARD_ID ))
      {
			return(ATA_ERROR_TIMEOUT);
		}
	}while( ((MMC_internals *)(((AtaMMCState *)pAtaMediaState)->mmcptr))->CardStatus.state != tran );

	pMMC->nblk = 1;

	pMMC->blen = 512;

	pMMC->cmdl  = CARD_LOCK_UNLOCK_PASSWD;
	return(ATA_ERROR_NONE);
}
#endif

/*******************************************************************
* AtaError MMC_FIFOReset(void *pAtaMediaState)
* input:
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*		 ATA_ERROR_NONE
*Description:
*		This function is to reset FIFO
********************************************************************/
AtaError MMC_FIFOReset(void *pAtaMediaState)
{
	PC55XX_MMC pMMC = (PC55XX_MMC) (((AtaMMCState *)pAtaMediaState)->C55XX_MMC_CTL_ADDR);

	pMMC->mmcfifoctl |= FIFO_RESET;
	pMMC->mmcfifoctl &=~(1<<FIFO_THRESHOLD_SHIFT);
	return(ATA_ERROR_NONE);
}
/*******************************************************************
* AtaError MMC_FIFOReceive(void *pAtaMediaState)
* input:
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*		 ATA_ERROR_NONE
*Description:
*		This function is to set FIFO to receiver
********************************************************************/
AtaError MMC_FIFOReceive(void *pAtaMediaState)
{
	PC55XX_MMC pMMC = (PC55XX_MMC) (((AtaMMCState *)pAtaMediaState)->C55XX_MMC_CTL_ADDR);

	pMMC->mmcfifoctl &=~ FIFO_TRANSMITTE;
	pMMC->mmcfifoctl |=2<<FIFO_ACCESS_WIDTH_SHIFT;
	pMMC->mmcfifoctl |=	(((AtaMMCState *)pAtaMediaState)->fifo_threshold) << FIFO_THRESHOLD_SHIFT;
	return(ATA_ERROR_NONE);
}
/*******************************************************************
* AtaError MMC_FIFOTransmit(void *pAtaMediaState)
* input:
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*		 ATA_ERROR_NONE
*Description:
*		This function is to set FIFO to transmitter
********************************************************************/
AtaError MMC_FIFOTransmit(void *pAtaMediaState)
{
	PC55XX_MMC pMMC = (PC55XX_MMC) (((AtaMMCState *)pAtaMediaState)->C55XX_MMC_CTL_ADDR);

	pMMC->mmcfifoctl |= FIFO_TRANSMITTE;
	pMMC->mmcfifoctl |=2<<FIFO_ACCESS_WIDTH_SHIFT;
	pMMC->mmcfifoctl |=	(((AtaMMCState *)pAtaMediaState)->fifo_threshold) << FIFO_THRESHOLD_SHIFT;
	return(ATA_ERROR_NONE);
}
/*******************************************************************
* AtaError MMC_SetDataSize(unsigned short numblks, void *pAtaMediaState)
* input:
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*		 ATA_ERROR_NONE
*Description:
*		This function is to set data size
********************************************************************/
AtaError MMC_SetDataSize(unsigned short numblks, void *pAtaMediaState)
{
	PC55XX_MMC pMMC = (PC55XX_MMC) (((AtaMMCState *)pAtaMediaState)->C55XX_MMC_CTL_ADDR);

	pMMC->nblk = numblks;

	pMMC->blen = 512;

	return(ATA_ERROR_NONE);
}
/*******************************************************************
* AtaError MMC_SetTriger(void *pAtaMediaState)
* input:
*			pAtaMediaState		MMC state
* Output:
*			None
* Return:
*		 ATA_ERROR_NONE
*Description:
*		This function is to set DMA trigger
********************************************************************/
AtaError MMC_SetTriger(void *pAtaMediaState)
{
	PC55XX_MMC pMMC = (PC55XX_MMC) (((AtaMMCState *)pAtaMediaState)->C55XX_MMC_CTL_ADDR);
	pMMC->cmdh = DMATRIGER;
	return(ATA_ERROR_NONE);
}
