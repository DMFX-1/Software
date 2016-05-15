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

/** @file csl_emif_nor_polled_example.c
 *
 *  @brief EMIF functional layer polled mode example source file
 *
 *
 * \page    page13  CSL EMIF NOR EXAMPLE DOCUMENTATION
 *
 * \section EMIF5x EMIF NOR EXAMPLE5 - POLL MODE TEST
 *
 * \subsection EMIF5x TEST DESCRIPTION:
 *      Following example code tests the read and write operations of CSL
 * EMIF module NOR flash in polled mode. A 1KB data is written and read
 * from NOR flash in this test.
 * Data buffer "gnorWriteBuf" will hold the data to be written to NOR.
 * Data buffer "gnorReadBuf" is used to store the data read from the NOR
 * device.
 *
 * During the test CSL EMIF module will be initialized and asynchronous
 * configurations are performed.Data is written to the NOR device using
 * CSL API NOR_writeNwords.NOR write operation is complete at this point.
 *
 * After the successful completion of the write operations read command is sent
 * to the NOR device. Data is read from the NOR device using CSL API NOR_readNwords.
 * NOR read operation is complete at this point.After completing the read
 * operation read and written buffers are compared to verify the test result.
 *
 *
 * NOTE: This test assumes that
 *  a. NOR flash is connected to emif cs2 on C5515 EVM and on C5517 EVM.
 * This example may not work with connections that are different from above
 * connections.
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5515 AND
 * C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 *
 * \subsection EMIF5y TEST PROCEDURE:
 *  @li Open the CCS and connect the target (C5515/C5517 EVM)
 *  @li Open the project "CSL_NOR_PollExample_Out.pjt" and build it
 *  @li Load the program on to the target
 *  @li Set the PLL frequency to 12.288MHz
 *  @li Run the program and observe the test result
 *  @li Repeat the test at the following PLL frequencies
 *      C5515: 60MHz and 100MHz
 *      C5517: 60MHz, 100MHz, 150MHz and 200MHz
 *  @li Repeat the test in Release mode
 *
 * \subsection EMIF5z TEST RESULT:
 *  @li All the CSL APIs should return success
 *  @li Read and write data buffer comparison should be successful.
 *
 * ============================================================================
 */

/* ============================================================================
 * Revision History
 * ================
 * 22-Feb-2013 Created
 * ============================================================================
 */
#include "nor.h"
#include <csl_emif.h>
#include <csl_general.h>
#include <stdio.h>

#define CSL_NOR_BUF_SIZE                    (1024u)
#define CSL_NOR_BLOCK_LEN                   (0x10000)
#define CSL_NOR_NUM_BLOCKS                  (32)
#define CSL_NOR_NUM_WORDS                   (64)

#define CSL_EMIF_ASYNCCFG_WSTROBE_NOR       (0x3F)
#define CSL_EMIF_ASYNCCFG_WHOLD_NOR         (0xF)
#define CSL_EMIF_ASYNCCFG_RSETUP_NOR        (0xA)
#define CSL_EMIF_ASYNCCFG_RSTROBE_NOR       (2)
#define CSL_EMIF_ASYNCCFG_RHOLD_NOR         (0)
#define CSL_EMIF_ASYNCCFG_ASIZE_NOR         (1)
#define CSL_EMIF_ASYNCWAITCFG_WAITCYCLE_NOR (4)

#define CSL_TEST_FAILED                     (1)
#define CSL_TEST_PASSED                     (0)


Uint16  gnorReadBuf[CSL_NOR_BUF_SIZE];
Uint16  gnorWriteBuf[CSL_NOR_BUF_SIZE];

/**
 *  \brief  Tests NOR polled mode operation
 *
 *  \param  none
 *
 *  \return Test result
 */
CSL_Status CSL_norPolledTest(void);


/**
 *  \brief  Tests EMIF CSL module
 *
 *  \param  None
 *
 *  \return None
 */
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 1 --
   /////  Define PaSs_StAtE variable for catching errors as program executes.
   /////  Define PaSs flag for holding final pass/fail result at program completion.
        volatile Int16 PaSs_StAtE = 0x0001; // Init to 1. Reset to 0 at any monitored execution error.
        volatile Int16 PaSs = 0x0000; // Init to 0.  Updated later with PaSs_StAtE when and if
   /////                                  program flow reaches expected exit point(s).
   /////
void main(void)
{
	CSL_Status    result;

	/* Tests the nor operation in Polled mode */
	result = CSL_norPolledTest();

	if(result != 0)
	{
		printf("\nNOR Polled Mode Test Failed!!\n");
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 2 --
   /////  Reseting PaSs_StAtE to 0 if error detected here.
        PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
   /////
	}
	else
	{
		printf("\nNOR Polled Mode Test Passed!!\n");
	}
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 3 --
   /////  At program exit, copy "PaSs_StAtE" into "PaSs".
        PaSs = PaSs_StAtE; //If flow gets here, override PaSs' initial 0 with
   /////                   // pass/fail value determined during program execution.
   /////  Note:  Program should next exit to C$$EXIT and halt, where DSS, under
   /////   control of a host PC script, will read and record the PaSs' value.
   /////
}

/**
 *  \brief  Tests NOR polled mode operation
 *
 *  \param  none
 *
 *  \return Test result
 */
CSL_Status  CSL_norPolledTest(void)
{
	CSL_EmifAsyncWaitCfg    asyncWaitConfig;
	CSL_EmifChipSelect      chipSel;
	CSL_EmifAsyncCfg        asyncConfig;
	CSL_AsyncConfig         asyncCfg;
	CSL_EmifHandle          hEmif;
	CSL_EmifWaitPin         waitPin;
	CSL_EmifObj             emifObj;
	CSL_Status              status;
	CSL_Status              result;
	Uint16                  looper;
	Uint16                  *buff;
	Uint16                  i;

	/* On C5515/C5517 EVM NOR Flash is connected to EMIF chip select 2 */
	chipSel  = CSL_EMIF_CS2;
	waitPin  = CSL_EM_WAIT0;

	result   = CSL_TEST_FAILED;
	hEmif    = &emifObj;

	for(looper=0; looper < CSL_NOR_BUF_SIZE; looper++)
	{
		gnorWriteBuf[looper] = looper;
		gnorReadBuf[looper]  = 0x0;
	}

	printf("NOR Polled mode test\n\n");

	/* Initialize NOR module */
	status = EMIF_init(&emifObj);
	if(status != CSL_SOK)
	{
		printf("EMIF NOR Init Failed!!\n");
		return(result);
	}

	/* Assign values to nor async wait config structure */
	asyncWaitConfig.waitPol    = CSL_EMIF_WP_LOW;
	asyncWaitConfig.waitPin    = waitPin;
	asyncWaitConfig.waitCycles = CSL_EMIF_ASYNCWAITCFG_WAITCYCLE_NOR;

	/* Assign values to nor async config structure */
	asyncConfig.selectStrobe = CSL_EMIF_ASYNCCFG_SELECTSTROBE_DEFAULT;
	asyncConfig.ewMode       = CSL_EMIF_ASYNCCFG_WEMODE_DEFAULT;
	asyncConfig.w_setup      = CSL_EMIF_ASYNCCFG_WSETUP_DEFAULT;
	asyncConfig.w_strobe     = CSL_EMIF_ASYNCCFG_WSTROBE_NOR;
	asyncConfig.w_hold       = CSL_EMIF_ASYNCCFG_WHOLD_NOR;
	asyncConfig.r_setup      = CSL_EMIF_ASYNCCFG_RSETUP_NOR;
	asyncConfig.r_strobe     = CSL_EMIF_ASYNCCFG_RSTROBE_NOR;
	asyncConfig.r_hold       = CSL_EMIF_ASYNCCFG_RHOLD_NOR;
	asyncConfig.turnAround   = CSL_EMIF_ASYNCCFG_TAROUND_DEFAULT;
	asyncConfig.aSize        = CSL_EMIF_ASYNCCFG_ASIZE_NOR;

	/* Assign values to nor config structure */
	asyncCfg.chipSelect   = chipSel;
	asyncCfg.emifAccess   = CSL_EMIF_16BIT;
	asyncCfg.asyncWaitCfg = &asyncWaitConfig;
	asyncCfg.asyncCfg     = &asyncConfig;

	/* Configure Asynchronous settings */
	status = EMIF_asyncConfig(hEmif, &asyncCfg);
	if(status != CSL_SOK)
	{
		printf("EMIF Asynchronous Config Failed!!\n");
		return(result);
	}
    /* Erase Nor flash block wise */
    status = NOR_eraseBlock(hEmif, CSL_NOR_START_ADDR, CSL_NOR_BLOCK_LEN, CSL_NOR_NUM_BLOCKS);
	if(status != CSL_SOK)
	{
		printf("NOR chip erase Failed!!\n");
		return(result);
	}
	else
	{
		printf("NOR chip erase Passed!!\n");
	}

    status = NOR_WriteNwords(hEmif, CSL_NOR_NUM_WORDS, 0, &gnorWriteBuf[0]);
	if(status != CSL_SOK)
	{
		printf("NOR Write Failed!!\n");
		return(result);
	}
    status = NOR_readNwords(hEmif, CSL_NOR_NUM_WORDS, 0, &gnorReadBuf[0]);
	if(status != CSL_SOK)
	{
		printf("NOR Read Failed!!\n");
		return(result);
	}

	/* Compare Read-Write Buffers */
	for(looper = 0; looper < CSL_NOR_NUM_WORDS; looper++)
	{
		if(gnorWriteBuf[looper] != gnorReadBuf[looper])
		{
			printf("\nNOR Read Write Buffers doesn't Match!\n");
			return(result);
		}
	}

	printf("\nNOR Read Write Buffers Match!\n");

	result = CSL_TEST_PASSED;
	return(result);
}




