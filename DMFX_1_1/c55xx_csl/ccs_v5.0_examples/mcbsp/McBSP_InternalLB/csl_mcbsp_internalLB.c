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

/** @file csl_mcbsp_internalLB.c
 *
 *  @brief Test code to verify the internal loopback functionality of McBSP module
 *
 * \page    page9  McBSP EXAMPLE DOCUMENTATION
 *
 * \section McBSP1   McBSP EXAMPLE1 - INTERNAL LOOPBACK TEST
 *
 * \subsection McBSP1x    TEST DESCRIPTION:
 * This test verifies the operation of the C5517 McBSP (Multichannel Buffered
 * Serial Port) module in loopback mode.
 * In loopback mode the McBSP module does not communicate with any slave device.
 * Internally the McBSP transmit register will be connected to the McBSP receive
 * register. Any data copied to the McBSP transmitted data will be transmitted
 * to the McBSP receive registers. Loopback mode is helpful to verify the McBSP
 * data and shift registers and internals data paths.
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSION C5517.
 * MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 *
 * \subsection McBSP1y    TEST PROCEDURE:
 *  @li Open the CCS and connect the target (C5517 EVM)
 *  @li Open the project "CSL_McBSP_InternalLB.pjt" and build it
 *  @li Load the program on to the target
 *  @li Set the PLL frequency to 12.288MHz
 *  @li Run the program and observe the test result
 *  @li Repeat the test at the following PLL frequencies
 *      C5517: 60MHz, 100MHz, 150MHz and 200MHz
 *  @li Repeat the test in Release mode
 *
 * \subsection I2C4z    TEST RESULT:
 *  @li All the CSL APIs should return success
 *  @li Read and write data buffer comparison should be successful.
 *
 * ============================================================================
 */

/* ============================================================================
 * Revision History
 * ================
 * 10-Jul-2012 Added C5517 Documentation
 * ============================================================================
 */

/*
 *---------main_mcbsp1.c---------
 * In this example, the MCBSP is configured in digital loopback
 * mode, with 32 bit data transfer, in multi-frame mode, using
 * sample rate generator to sync frames
 */

#include <stdio.h>
#include <csl_mcbsp.h>
#include <csl_general.h>
#include "csl_sysctrl.h"
//#define USE_MCBSP_TARGET 1

#ifdef C5517_EVM
#define C5517_EVM_PLATFORM 1
#endif

#define CSL_MCBSP_TEST_PASSED      (0)
#define CSL_MCBSP_TEST_FAILED      (1)

//---------Global constants---------
#define N 1024

int j=0;
CSL_McbspHandle  hMcbsp;
CSL_McbspObj 	 McbspObj;

/* Create a MCBSP configuration structure */

CSL_McbspSetup McBSPconfig;


/* Create data buffers for transfer */
Uint32 xmt[N], rcv[N];
Uint16	mcbsp_rx_count = 0;
Uint16	mcbsp_tx_count = 0;

//---------Function prototypes---------
CSL_Status CSL_McBSPTest(void);

/*Reference the start of the interrupt vector table*/
extern void VECSTART(void);

/////INSTRUMENTATION FOR BATCH TESTING -- Part 1 --
/////  Define PaSs_StAtE variable for catching errors as program executes.
/////  Define PaSs flag for holding final pass/fail result at program completion.
volatile Int16 PaSs_StAtE = 0x0001; // Init to 1. Reset to 0 at any monitored execution error.
volatile Int16 PaSs = 0x0000; // Init to 0.  Updated later with PaSs_StAtE when and if
/////                                  program flow reaches expected exit point(s).
/////


void main() {
  CSL_Status result;

  printf("\r\nMCBSP Normal Mode test case with internal loop back mode!!\n");
#if (defined(CHIP_C5517))
  result = CSL_McBSPTest();
  if(result == CSL_MCBSP_TEST_PASSED)
  {
 	printf("\r\nMCBSP Normal Mode test case with internal loop back mode passed!!\n");
  }
  else
  {
	 printf("\r\nMCBSP Normal Mode test case with internal loop back mode failed!!\n");
	PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
  }
#else
	printf("\r\nMCBSP Normal Mode test not supported by this CHIP selection!!\n");
#endif
  /////INSTRUMENTATION FOR BATCH TESTING -- Part 3 --
  /////  At program exit, copy "PaSs_StAtE" into "PaSs".
  PaSs =PaSs_StAtE; //If flow gets here, override PaSs' initial 0 with
  /////                   // pass/fail value determined during program execution.
  /////  Note:  Program should next exit to C$$EXIT and halt, where DSS, under
  /////   control of a host PC script, will read and record the PaSs' value.
  /////


}
#if (defined(CHIP_C5517))
CSL_Status CSL_McBSPTest(void)
{

  Uint16 i;
  unsigned char error=0;
  CSL_Status status; //This is for the CSL status return


  /* Initilize data buffers. xmt will be 8 bit value      */
  /* with element number in 8 bits  */
  for(i=0;i<=N-1;i++)
  {
    xmt[i] = ((long)i<<16)+i;
    rcv[i] = 0;
  }

  /* Open MCBSP Port 0, this will return a MCBSP handle that will */
  /* be used in calls to other CSl functions.                     */

  hMcbsp = MCBSP_open(CSL_MCBSP_INST_0, &McbspObj,&status);
  if ( (hMcbsp == NULL) || (status != CSL_SOK) )
  {
   printf("MCBSP_open() Failed \n");
  }
  else
  {
	printf("MCBSP_open() Success \n");
  }

    status = SYS_setEBSR(CSL_EBSR_FIELD_SP0MODE,
                         CSL_EBSR_SP0MODE_3);
    if(CSL_SOK != status)
    {
        printf("SYS_setEBSR failed\n");
        return (status);
    }

  /* Write configuration structure values to MCBSP control register*/


#ifdef USE_MCBSP_TARGET
   /** Input clock in Hz -- On QT we are assign C5517_QTCLK to this*/
   McBSPconfig.clkInput = C5517_QTCLK;
#elif C5517_EVM_PLATFORM
	McBSPconfig.clkInput = 100000000;
#endif
   /* Operating mode */
   McBSPconfig.mcbsp_opmode = CSL_MCBSP_OPMODE_NORMAL;
   /**Word length selection
   Valid values - 8,12,16,20,24,32bits*/
   McBSPconfig.wordLength = CSL_MCBSP_WORD32;
   /**Frame length selection
   Valid values - (1-128)*/
   McBSPconfig.frameLength = CSL_MCBSP_FRMAELENGHT(1);
   /** single phase frame/double phase frame*/
   McBSPconfig.phase = CSL_MCBSP_SINGLEPHASE;
   /** data delay*/
   McBSPconfig.datadelay = CSL_MCBSP_DATADELAY0BIT;
   /** frame ignore flag */
   McBSPconfig.frameignore = CSL_MCBSP_FRMAEIGNOREFALSE;
   /**Loopback mode enable/disable*/
   McBSPconfig.loopBackmode = CSL_MCBSP_INTERNALLOOPBACK_ENABLE;
   /**Companding*/
   McBSPconfig.companding = CSL_MCBSP_COFF_MSBFIRST;
   /**Rjust mode*/
   McBSPconfig.rjust_mode = CSL_MCBSP_RJUST_RJZFMSB;
   /**Abis mode*/
   McBSPconfig.abismode = CSL_MCBSP_ABIS_DISABLE;
   /**DXENA enable/disable*/
   McBSPconfig.dxena = CSL_MCBSP_DXENA_DISABLE;
   /**CLKSTP */
   McBSPconfig.clkstp = CSL_MCBSP_SPCRL_CLKSTP_DISABLE;
   /**CLKXP,CLKRP */
   McBSPconfig.sample_polarity = CSL_MCBSP_DATASAMPLE_RISING;
   /**FRAMESYNC */
   McBSPconfig.fs_polarity = CSL_MCBSP_FS_ACTIVEHIGH;
   /** Clock Source */
   McBSPconfig.clocksource = CSL_MCBSP_CLOCKSOURCE_SYSCLK;
   /** Interrupt Source */
   McBSPconfig.intr_event = CSL_MCBSP_INTR_RDY;
   /*sample rate(samples per sec) */
   McBSPconfig.sample_rate = 310;
   /*FSX pin source */
  // McBSPconfig.fsxpin_status = CSL_MCBSP_FSX_SRGOP;
   McBSPconfig.fsxpin_status = CSL_MCBSP_FSX_DXRTOXSRCPYOP;


  Mcbsp_Configure(hMcbsp, &McBSPconfig); /* Currently hMcbsp is not used anywhere in the CSL, so please ignore the warn - Pramod*/
  /* This redoes Mcbsp_Configure as the above function doesnt work - Pramod*/

  //CSL_MCBSP_REGS->PCRL  =  (Uint16) (((CSL_MCBSP_REGS->PCRL & 0x0070) | 0x0A00));



//  CSL_MCBSP_REGS->SPCRL =  (Uint16) (((CSL_MCBSP_REGS->SPCRL & 0x0F0E) | 0x0001));

// CSL_MCBSP_REGS->SPCRU =  (Uint16) (((CSL_MCBSP_REGS->SPCRU & 0x030E) | 0x00C1));
// CSL_MCBSP_REGS->SPCRU |= 0x20;
//  CSL_MCBSP_REGS->SPCRU &= 0xFFEF;


   /* In the test case, the TX and RX should start before the srg starts. This is to avoid missing the first channel on mcbsp */
   /* Enable MCBSP transmit and receive */
   MCBSP_start(hMcbsp, MCBSP_RCV_START | MCBSP_XMIT_START, 0 );

   /* Start Sample Rate Generator and Frame Sync */
  MCBSP_start(hMcbsp,MCBSP_SRGR_START | MCBSP_SRGR_FRAMESYNC, 0x300 );



  i=0;
  j=0;
  for (i=0; i<=N-1;i++)
  {

    /* Wait for XRDY signal before writing data to DXR */
   while (!MCBSP_xrdy(hMcbsp));

    /* Write 32 bit data value to DXR */
    MCBSP_write32(hMcbsp,xmt[i]);

//    while(1);

    /* Wait for RRDY signal to read data from DRR */
    while (!MCBSP_rrdy(hMcbsp));

    /* Read 32 bit value from DRR */
    rcv[i] = MCBSP_read32(hMcbsp);
  }


   printf("\r\n Checking the data integrity\r\n");

  /* Check data to make sure transfer was successful */
  for(i=0; i<= N-1;i++) {
    if (xmt[i] != rcv[i]) {
       ++error;
       break;
    }
  }

  /* We are done with MCBSP, so close it */
  MCBSP_close(hMcbsp);

  if(error)
  {
	return(CSL_MCBSP_TEST_FAILED);
  }
  else
  {
	return(CSL_MCBSP_TEST_PASSED);
  }


}
#endif





