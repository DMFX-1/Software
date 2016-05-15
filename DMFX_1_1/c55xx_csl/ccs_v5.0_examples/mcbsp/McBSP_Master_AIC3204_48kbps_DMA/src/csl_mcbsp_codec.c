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

/*
 *---------main_mcbsp1.c---------
 * In this example, the MCBSP is configured in digital loopback
 * mode, with 32 bit data transfer, in multi-frame mode, using
 * sample rate generator to sync frames
 */

#include "csl_general.h"
#include "csl_sysctrl.h"

#if (defined(CHIP_C5517))
#include <stdio.h>
#include <csl_mcbsp.h>
#include <csl_dma.h>

//#define USE_MCBSP_TARGET 1

#define CSL_MCBSP_TEST_PASSED      (0)
#define CSL_MCBSP_TEST_FAILED      (1)

//---------Global constants---------
#define N 128
#define CSL_MCBSP_BUF_SIZE (1u)

int j=0;
CSL_McbspHandle  hMcbsp;
CSL_McbspObj 	 McbspObj;

/* Create a MCBSP configuration structure */
CSL_McbspSetup McBSPconfig;

/* CSL DMA data structures used for McBSP data transfers */
CSL_DMA_Handle dmaTxHandle;
CSL_DMA_Handle dmaRxHandle;
CSL_DMA_Config dmaConfig;
CSL_DMA_ChannelObj    dmaTxChanObj;
CSL_DMA_ChannelObj    dmaRxChanObj;

/**Dma write is happening in 32 bits,if use character array*/
Uint32 gmcbspDmaWriteBuf[CSL_MCBSP_BUF_SIZE]={0};
Uint32 gmcbspDmaReadBuf[CSL_MCBSP_BUF_SIZE]={0};

CSL_DMA_Handle CSL_configDmaForMcbsp(CSL_DMA_ChannelObj    *dmaChanObj,
                                    CSL_DMAChanNum        chanNum);
//---------Function prototypes---------
CSL_Status Configure_McBSP(void)   ;

/*Reference the start of the interrupt vector table*/
extern void VECSTART(void);

CSL_Status Configure_McBSP(void)
{
  unsigned char error=0;
  CSL_Status status; //This is for the CSL status return

#ifdef C5517_EVM
	status = ioExpander_Setup();
	status = ioExpander_Write(1, 0, 1); // Select SEL_MMC0_I2S to 1
	if(CSL_SOK != status)
		printf("\nSelect SEL_MMC0_I2S to 1 Failed \n");
	status = ioExpander_Write(0, 1, 0); // Select AIC_McBSP_MODE to 0
	if(CSL_SOK != status)
		printf("Select AIC_McBSP_MODE to 0 Failed \n");
#endif

  /* Open MCBSP Port 0, this will return a MCBSP handle that will */
  /* be used in calls to other CSl functions.                     */
  hMcbsp = MCBSP_open(CSL_MCBSP_INST_0, &McbspObj,&status);
  if ( (hMcbsp == NULL) || (status != CSL_SOK) )
  {
   	printf("MCBSP_open() Failed \n");
   	error= CSL_MCBSP_TEST_FAILED;
  }
  else
  {
	printf("MCBSP_open() Success \n");
	error= CSL_MCBSP_TEST_PASSED;
  }

    status = SYS_setEBSR(CSL_EBSR_FIELD_SP0MODE,
                         CSL_EBSR_SP0MODE_3);
    if(CSL_SOK != status)
    {
        printf("SYS_setEBSR failed\n");
        return (status);
    }

  /* Initialize Dma */
  status = DMA_init();
  if (status != CSL_SOK)
  {
    printf("DMA_init Failed!\n");
  }

   /* Configure the DMA channel for mcbsp transmit */
#if ((defined(CHIP_C5505_C5515)) || (defined(CHIP_C5504_C5514)) || (defined(CHIP_C5517)))
	dmaConfig.pingPongMode = CSL_DMA_PING_PONG_DISABLE;
#endif
	//dmaConfig.autoMode = CSL_DMA_AUTORELOAD_DISABLE;
	dmaConfig.autoMode = CSL_DMA_AUTORELOAD_ENABLE;
	dmaConfig.burstLen = CSL_DMA_TXBURST_1WORD;
	dmaConfig.trigger  = CSL_DMA_EVENT_TRIGGER;
	dmaConfig.dmaEvt   = CSL_DMA_EVT_MCBSP_TX;
	dmaConfig.dmaInt   = CSL_DMA_INTERRUPT_ENABLE;
	dmaConfig.chanDir  = CSL_DMA_WRITE;
	dmaConfig.trfType  = CSL_DMA_TRANSFER_IO_MEMORY;
	dmaConfig.dataLen  = (CSL_MCBSP_BUF_SIZE*4);
	dmaConfig.srcAddr  = (Uint32)gmcbspDmaWriteBuf;
	//dmaConfig.destAddr = (Uint32)&(hMcbsp->Regs->DXRL);
	dmaConfig.destAddr = 0x00006010; // DMA byte address of hMcbsp->Regs->DXRL

    dmaTxHandle = CSL_configDmaForMcbsp(&dmaTxChanObj, CSL_DMA_CHAN12);
	if(dmaTxHandle == NULL)
	{
		printf("DMA Config for mcbsp DMA Write Failed!\n!");
		return(CSL_MCBSP_TEST_FAILED);
	}



    // Configure the DMA channel for mcbsp receive
#if ((defined(CHIP_C5505_C5515)) || (defined(CHIP_C5504_C5514)) || (defined(CHIP_C5517)))
	dmaConfig.pingPongMode = CSL_DMA_PING_PONG_DISABLE;
#endif
	//dmaConfig.autoMode = CSL_DMA_AUTORELOAD_DISABLE;
	dmaConfig.autoMode = CSL_DMA_AUTORELOAD_ENABLE;
	dmaConfig.burstLen = CSL_DMA_TXBURST_1WORD;
    dmaConfig.trigger  = CSL_DMA_EVENT_TRIGGER;
    dmaConfig.dmaEvt   = CSL_DMA_EVT_MCBSP_RX;
	dmaConfig.dmaInt   = CSL_DMA_INTERRUPT_ENABLE;
	dmaConfig.chanDir  = CSL_DMA_READ;
	dmaConfig.trfType  = CSL_DMA_TRANSFER_IO_MEMORY;
	dmaConfig.dataLen  = (CSL_MCBSP_BUF_SIZE*4);
	//dmaConfig.srcAddr  = (Uint32)&(hMcbsp->Regs->DRRL);
	dmaConfig.srcAddr  = 0x00006000; // DMA byte address of hMcbsp->Regs->DRRL
	dmaConfig.destAddr = (Uint32)gmcbspDmaReadBuf;

	dmaRxHandle = CSL_configDmaForMcbsp(&dmaRxChanObj,CSL_DMA_CHAN13);
	if(dmaRxHandle == NULL)
	{
		printf("DMA Config for mcbsp DMA Read Failed!\n!");
		return(CSL_MCBSP_TEST_FAILED);
	}

  /* Write configuration structure values to MCBSP control register*/
   /*CLK INPUT is 100Mhz*/
   McBSPconfig.clkInput = 100000000;
   /* Operating mode */
   McBSPconfig.mcbsp_opmode = (CSL_McbspOpMode)CSL_MCBSP_OPMODE_NORMAL;
   /*Word Length-32*/
   McBSPconfig.wordLength = CSL_MCBSP_WORD32;   // 16-bit Left + 16-bit Right = 32 bits
   /*Frame length selection
   Valid values - (1-128)*/
   McBSPconfig.frameLength = CSL_MCBSP_FRMAELENGHT(1); // One 32 bit data in one frame
   /** single phase frame/double phase frame*/
   McBSPconfig.phase = CSL_MCBSP_SINGLEPHASE;
   /** data delay*/
   McBSPconfig.datadelay = CSL_MCBSP_DATADELAY0BIT;
   /** frame ignore flag */
   McBSPconfig.frameignore = CSL_MCBSP_FRMAEIGNOREFALSE;
   /**Loopback mode enable/disable*/
   McBSPconfig.loopBackmode = CSL_MCBSP_INTERNALLOOPBACK_DISABLE;
   /**Companding*/
   McBSPconfig.companding = CSL_MCBSP_COFF_MSBFIRST;
   /**Rjust mode*/
   //McBSPconfig.rjust_mode = CSL_MCBSP_RJUST_RJZFMSB;
   McBSPconfig.rjust_mode = CSL_MCBSP_RJUST_LJZFLSB;  // Data is aligned as LJUST
   /**Abis mode*/
   McBSPconfig.abismode = CSL_MCBSP_ABIS_DISABLE;
   /**DXENA enable/disable*/
   McBSPconfig.dxena = CSL_MCBSP_DXENA_DISABLE;
   /**CLKSTP */
   McBSPconfig.clkstp = CSL_MCBSP_SPCRL_CLKSTP_DISABLE;
   /**CLKXP,CLKRP */
   //Transmit data sampled on rising edge of CLKX, CLKXP=0
   //Receive data sampled on falling edge of CLKR, CLKRP=0
   McBSPconfig.sample_polarity = CSL_MCBSP_DATASAMPLE_FALLING;
   /**FRAMESYNC */
   McBSPconfig.fs_polarity = CSL_MCBSP_FS_ACTIVEHIGH;
   /** Clock Source */
   McBSPconfig.clocksource = CSL_MCBSP_CLOCKSOURCE_SYSCLK;
   /** Interrupt Source */
   McBSPconfig.intr_event = CSL_MCBSP_INTR_RDY;
   /*sample rate(samples per sec) */
   McBSPconfig.sample_rate = 48828;      // Set sampling Rate to 48,828 Hz (48Khz)
   /*FSX pin source */
   McBSPconfig.fsxpin_status = CSL_MCBSP_FSX_SRGOP;

   error = Mcbsp_Configure(hMcbsp, &McBSPconfig); /* Currently hMcbsp is not used anywhere in the CSL, so please ignore the warn - Pramod*/

   // Below settings is for setting the bits with correct values
   /*	CLKX = 1.5625 Mhz, FSX  = 48.828 Khz  */
   CSL_FINS(hMcbsp->Regs->SRGRL,MCBSP_SRGRL_CLKGDV,63); // CLKDIV = (100 Mhz/1.5625Mhz)-1 = 63

   //Not handled as a CSL argument.
   CSL_FINS(hMcbsp->Regs->SRGRL,MCBSP_SRGRL_FWID,0); // Select Frame pulse width as (0+1)=1 CLK period

   //Not handled in CSL CLKSP and GSYNC of SRGRU
   CSL_FINS(hMcbsp->Regs->SRGRU,MCBSP_SRGRU_CLKSP,0);
   CSL_FINS(hMcbsp->Regs->SRGRU,MCBSP_SRGRU_GSYNC,0);

   //Not handled bits in CSL FPER in SRGR
   CSL_FINS(hMcbsp->Regs->SRGRU,MCBSP_SRGRU_FPER,31); // Select ((31+1)*1)=32 clock per frame length

   //Not handled bits in PCR by CSL
   CSL_FINST(hMcbsp->Regs->PCRL,MCBSP_PCRL_CLKXM,ONE);	// Sample rate generator clock, CLKG, drives transmit clock
   CSL_FINST(hMcbsp->Regs->PCRL,MCBSP_PCRL_CLKRM,ZERO); // Rececive clock pin set as input

	status = DMA_start(dmaRxHandle);
	if(status != CSL_SOK)
	{
	  printf("McBSP Dma Read start Failed!!\n");
	  return(status);
	}

 	/* Start UART tx Dma transfer */
	status = DMA_start(dmaTxHandle);
	if(status != CSL_SOK)
	{
		printf("McBSP Dma write start Failed!!\n");
		return(status);
	}

   /* In the test case, the TX and RX should start before the srg starts. This is to avoid missing the first channel on mcbsp */
   /* Enable MCBSP transmit and receive */
   MCBSP_start(hMcbsp, MCBSP_RCV_START | MCBSP_XMIT_START, 0 );

   /* Start Sample Rate Generator and Frame Sync */
   MCBSP_start(hMcbsp,MCBSP_SRGR_START | MCBSP_SRGR_FRAMESYNC, 0x300 );

   if(error)
   {
	return(CSL_MCBSP_TEST_FAILED);
   }
   else
   {
    printf("MCBSP Configured with DMA\n\n\n");
	return(CSL_MCBSP_TEST_PASSED);
   }


}

CSL_DMA_Handle CSL_configDmaForMcbsp(CSL_DMA_ChannelObj    *dmaChanObj,
                                    CSL_DMAChanNum        chanNum)
{
	CSL_DMA_Handle    dmaHandle;
	CSL_Status        status;

	dmaHandle = NULL;

	/* Open A Dma channel */
	dmaHandle = DMA_open(chanNum, dmaChanObj, &status);
    if(dmaHandle == NULL)
    {
        printf("DMA_open Failed!\n");
    }

	/* Configure a Dma channel */
 	status = DMA_config(dmaHandle, &dmaConfig);
    if(status != CSL_SOK)
    {
        printf("DMA_config Failed!\n");
        dmaHandle = NULL;
    }

    return(dmaHandle);
}
#endif /* #if (defined(CHIP_C5517)) */




