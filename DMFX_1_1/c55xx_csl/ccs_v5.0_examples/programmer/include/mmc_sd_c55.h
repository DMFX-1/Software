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

#ifndef MMC_SD_C55_DOT_H
#define MMC_SD_C55_DOT_H

#define COROZON_MMC_SD_0_ADDRESS		0x3A00
#define COROZON_MMC_SD_1_ADDRESS		0x3B00

#define   STUFF_BITS                0x0000
#define   CMD_TIMEOUT	            3000000
#define   OP_TIMEOUT	               1024
#define   MFG_ID_SANDISK            2
// ERROR CONDITIONS FOR MMC/SD
#define   ERROR_IDLE                -100
#define   ERROR_OP                  -101
#define   ERROR_ALL_CID             -102
#define   ERROR_ADDR                -103
#define   ERROR_STATUS              -104
#define   ERROR_CID                 -105
#define   ERROR_MFG_ID              -106
#define   ERROR_CSD                 -107
#define   ERROR_CSD_VER             -108
#define   ERROR_SELECT_CARD         -109
#define   ERROR_SET_BLOCK_LEN       -110
#define   ERROR_CONTROLLER	         -111

/***************************************************************************
*** 			MMCCTL (Control Register) 		            						 ***
***************************************************************************/
#define WPEG_DIS			            0x0000
#define WPEG_RISE		   	         0x0100
#define WPEG_FALL		   	         0x0200
#define WPEG_BOTH		   	         (WPEG_RISE | WPEG_FALL)
#define DATED_DIS		   	         0x0000
#define DATED_RISE		  	         0x0040
#define DATED_FALL		  	         0x0080
#define DATED_BOTH		  	         (DAT3EG_RISE | DAT3EG_FALL)
#define SPIEN			   	         0x0020
#define CSEN				            0x0010
#define SPICRC			  	            0x0008 // only in spi mode
#define WIDTH			   	         0x0004 // WIDTH = 1, data bus = 4. WIDTH = 0, data bus = 0
#define CMDRST			  	            0x0002
#define DATRST			  	            0x0001

/*****************************************************************************
*** 			MMCCLK (Clock Control Register 							            ***
*****************************************************************************/
#define CLKEN	         		   	0x0100
#define CLK_MASK			            0xff00
//MMC Clock = Function clock / (CLK_RATE + 1)
#define CLK_RATE		            	0x0001
#define MMC_INIT_RATE	   	      0x000F


/*****************************************************************************
***		  MMCST0 (Status Register 0) (Read only)						         ***
*****************************************************************************/
#define WPED				            0x1000
#define DATED			   	         0x0800
#define DRRDY			   	         0x0400
#define DXRDY			   	         0x0200
#define SPIERR			  	            0x0100
#define CRCRS        			   	0x0080
#define CRCRD			            	0x0040
#define CRCWR			   	         0x0020
#define TOUTRS       	   		  	0x0010
#define TOUTRD			     	         0x0008
#define RSPDNE		              	  	0x0004
#define BSYDNE			  	            0x0002
#define DATDNE			             	0x0001

/*****************************************************************************
***		  MMCST1 (Status Register 1) (Read Only)						         ***
*****************************************************************************/
#define WP		         	   	 	0x0020
#define DAT		           	   	 	0x0010
#define DXEMP	       		      	0x0008
#define DRFUL			            	0x0004
#define CLKSTP			            	0x0002
#define BUSY			             	0x0001

/****************************************************************************
***				 MMCIM (Interrupt Mask Register) 						        ***
****************************************************************************/
#define MMCIM_WPED		  	         0x1000
#define MMCIM_DATED		         	0x0800
#define MMCIM_RDFUL		         	0x0400
#define MMCIM_WREMP		         	0x0200
#define MMCIM_SPIERR	            	0x0100
#define MMCIM_CRCRS	      	   	0x0080
#define MMCIM_CRCRD	      	   	0x0040
#define MMCIM_CRCWR	      	    	0x0020
#define MMCIM_TOUTRS	            	0x0010
#define MMCIM_TOUTRD	            	0x0008
#define MMCIM_CMDDNE	            	0x0004
#define MMCIM_BSYDNE	            	0x0002
#define MMCIM_DATDNE	             	0x0001

/*****************************************************************************
***				MMCTOR (Response Time-Out Register)                         ***
*****************************************************************************/
#define TOR				 	            0x0000
                           //	 only bits 0-7, bits 8-15 are reserved
									//0 = no time out
									//in MMC mode, Time_out count = TOR CLK clocks
									//in SPI mode, Time_out_count = (TOR x 8) CLK clocks

/****************************************************************************
***			MMCTOD (Data Read Time-Out Register)						        ***
****************************************************************************/
#define TOD				            	0x0000 	//bits 0-15
									//0 = no time out
									//in MMC mode, Time_out count = TOR CLK clocks
									//in SPI mode, Time_out_count = (TOR x 8) CLK clocks

/****************************************************************************
***					MMCBLEN (Block Length Register)					      	  ***
****************************************************************************/
#define BLEN				            0x0200  //bit 15-12 are reserved

/*****************************************************************************
***				MMCNBLK (Number of Block Registers)						         ***
*****************************************************************************/
#define NBLK				            0x0000  //NBLK = 0 = infinite blocks

/*****************************************************************************
***						 MMCCMD (Command Register) 						         ***
*****************************************************************************/
#define DCLR                        0x8000
#define INIT			         	   0x4000
#define DATA			         	   0x2000
#define STREAM			         	   0x1000
#define WRITE				            0x0800
#define RSPFMT_NONE			         0x0000
#define RSPFMT_R1			            0x0200  	//response for R1, R4, R5, and R6
#define RSPFMT_R2			            0x0400
#define RSPFMT_R3		            	0x0600
#define BSYEXP			            	0x0100

#define DMATRIGER		            	0x1

/****************************************************************************
***				 MMCFIFO control				 						  ***
****************************************************************************/

#define FIFO_RESET						   0x1
#define FIFO_TRANSMITTE					   0x2
#define FIFO_THRESHOLD_32_BYTE_LEVEL   0x4
#define FIFO_THRESHOLD_SHIFT			   0x2
#define FIFO_ACCESS_WIDTH_SHIFT			0x3

/*****************************************************************
**		MMC/SD Structures
*****************************************************************/

typedef struct c55xx_mmc
{
    unsigned short ctl;             //control register
	unsigned short rsvd0[3];		   //clock register
    unsigned short clk;
	unsigned short rsvd1[3];
    unsigned short st0;            	//status 0 register
	unsigned short rsvd2[3];
    unsigned short st1; 			   //status 1 register
	unsigned short rsvd3[3];
    unsigned short im;             	//interrupt mask register
	unsigned short rsvd4[3];
    unsigned short tor;            	//response time out register
	unsigned short rsvd5[3];
    unsigned short tod;            	//data time out register
	unsigned short rsvd6[3];
    unsigned short blen;           	//block length
	unsigned short rsvd7[3];
    unsigned short nblk;			   //# blocks
	unsigned short rsvd8[3];
    unsigned short nblc;			   //block count
	unsigned short rsvd9[3];
    unsigned short ddrl; 			   //data receiver register
    unsigned short ddrh;
	unsigned short rsvd10[2];
    unsigned short dxrl;            //data transmitter register
    unsigned short dxrh;
	unsigned short rsvd11[2];
    unsigned short cmdl;            //command register
    unsigned short cmdh;
	unsigned short rsvd12[2];
    unsigned short argl;           	//command argumetns register
    unsigned short argh;
	unsigned short rsvd13[2];
    unsigned short rsp0;           	//response registers
    unsigned short rsp1;
	unsigned short rsvd14[2];
    unsigned short rsp2;
    unsigned short rsp3;
	unsigned short rsvd15[2];
    unsigned short rsp4;
    unsigned short rsp5;
	unsigned short rsvd16[2];
    unsigned short rsp6;
    unsigned short rsp7;
	unsigned short rsvd17[2];
    unsigned short drsp;
	unsigned short rsvd18[3];
    unsigned short etok;
	unsigned short rsvd19[3];
    unsigned short cidx;
	unsigned short rsvd20[3];
    unsigned short mmcckc;
    unsigned short rsvd21[3];
    unsigned short mmctorc;
    unsigned short rsvd22[3];
    unsigned short mmctodc;
    unsigned short rsvd23[3];
    unsigned short mmcblnc;
    unsigned short rsvd24[3];
    unsigned short sdioctl;
    unsigned short rsvd25[3];
    unsigned short sdiost0;
    unsigned short rsvd26[3];
    unsigned short sdioien;
    unsigned short rsvd27[3];
    unsigned short sdioist;
    unsigned short rsvd28[3];
    unsigned short mmcfifoctl;		//fifo control register

}C55XX_MMC;

typedef ioport volatile C55XX_MMC * PC55XX_MMC;

//	Define MMC/SD slot
typedef enum
{
	MMC_SD_CONTROLLER_0 = 0,
	MMC_SD_CONTROLLER_1
}MMC_SD_CONTROLLER;

// Define MMC/SD CSD structure
typedef struct csd_struct
{
	unsigned short slice127_113;
	unsigned short slice112_97;
	unsigned short slice96_81;
	unsigned short slice80_64;
	unsigned short slice63_48;
	unsigned short slice47_32;
	unsigned short slice31_16;
	unsigned short slice15_0;
}CSD_STRUCT;

// Define MMC/SD CID structure
typedef struct cid_struct
{
	unsigned short slice127_113;
	unsigned short slice112_97;
	unsigned short slice96_81;
	unsigned short slice80_64;
	unsigned short slice63_48;
	unsigned short slice47_32;
	unsigned short slice31_16;
	unsigned short slice15_0;

}CID_STRUCT;

// Define MMC/SD Status structure
typedef struct status
{
	unsigned short slice31_16;
	unsigned short slice15_0;

}STATUS_STRUCT;

 // Define MMC/SD card state
typedef enum card_state
{
	idle=0,
	ready,
	ident,
	stby,
	tran,
	data,
	rcv,
	prg,
	dis
}CARD_STATE;

//define MMC card CID structure
typedef struct card_cid
{
	unsigned short year;
	unsigned short month;
	unsigned long  serial;
	unsigned short fw_rev;
	unsigned short hw_rev;
	unsigned long  mfg_id;
}CARD_CID;

//define MMC card CSD structure
typedef struct card_csd
{
	unsigned short  ecc;
	unsigned short  tmp_wp;
	unsigned short  perm_wp;
	unsigned short  copy;
	unsigned short  wr_bl_part;
	unsigned short  wr_bl_len;
	unsigned short  r2w_factor;
	unsigned short  def_ecc;
	unsigned short  wp_brp_ena;
	unsigned short  wp_grp_size;
	unsigned short  er_grp_size;
	unsigned short  sect_size;
	unsigned short  c_size_mult;
	unsigned short  vdd_w_curr_max;
	unsigned short  vdd_w_curr_min;
	unsigned short  vdd_r_curr_max;
	unsigned short  vdd_r_curr_min;
	unsigned short  c_size;
	unsigned short  dsr_imp;
	unsigned short  rd_blk_misal;
	unsigned short  wr_blk_misal;
	unsigned short  rd_bl_part;
	unsigned short  rd_bl_len;
	unsigned short  ccc;
	unsigned short  tran_speed;
	unsigned short  nsac;
	unsigned short  taac;
	unsigned short  mmc_prot;
	unsigned short  csd_struct;
}CARD_CSD;

#endif

