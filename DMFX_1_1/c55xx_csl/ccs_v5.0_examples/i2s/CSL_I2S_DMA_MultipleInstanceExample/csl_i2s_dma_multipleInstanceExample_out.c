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

/** @file csl_i2s_DmaExample.c
 *
 *  @brief Test code to verify the CSL I2S multiple instance functionality in
 *  DMA mode
 *
 *
 * \page    page6  I2S EXAMPLE DOCUMENTATION
 *
 * \section I2S5   I2S EXAMPLE5 - I2S MULTIPLE INSTANCES TEST IN DMA MODE
 *
 * \subsection I2S5x    TEST DESCRIPTION:
 *		This test verifies the operation of CSL I2S module for multiple
 * instances, in DMA mode. I2S module on C5535/C5515/C5517 DSP is used to
 * exchange the audio data between the audio codec and DSP.
 *
 * Testing of the I2S with the audio codec is not possible as codec is not
 * part of the CSL package. This test verifies the I2S operation in loopback
 * mode. For each instance of I2S in loopback mode of operation, data written
 * to the I2S data transmit registers will be transmitted to the I2S data
 * receive registers. CSL DMA module should be configured for the I2S Tx and
 * Rx operations. DMA module is used to transfer the data between CPU memory
 * and I2S data registers. I2S is configured to master mode, stereo, 16bit
 * data length and to loop back mode. Due to word swap behavior of the DMA
 * while transferring the data to/from I2S, I2S write data buffer should be
 * word swapped before transferring it to the I2S registers. In case of I2S no
 * word swap is required on the data received due internal word swap by the I2S
 * in loopback mode. I2S data transfer is enabled and DMA is started using
 * DMA_start() API. DMA writes the data to the I2s Tx registers. After
 * successful completion of write operation DMA is configured for Rx operation
 * and again started to read the data from I2S Rx registers. I2S write and read
 * buffers are compared for the data verification. The above procedure is
 * carried out for each instance of the I2S, that are supported on the
 * platform.
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5535, C5515
 * AND C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 *
 * \subsection I2S5y    TEST PROCEDURE:
 *  @li Open the CCS and connect the target (C5535/C5515 EVM or C5517 EVM)
 *  @li Open the project "CSL_I2S_DMA_MultipleInstanceExample_out.pjt" and
 *      build it
 *  @li Load the program on to the target
 *  @li Set the PLL frequency to 12.288MHz
 *  @li Run the program and observe the test result
 *  @li Repeat the test at the following PLL frequencies
 *      C5515: 60MHz and 100MHz
 *      C5517: 60MHz, 100MHz, 150MHz and 200MHz
 *      C5535 eZdsp: 60MHz and 100MHz
 *  @li Repeat the test in Release mode
 *
 * \subsection I2S5z    TEST RESULT:
 *  @li All the CSL APIs should return success
 *  @li Read and write data buffer comparison should be successful on all the
 *      I2S instances, supported by the Platform.
 *
 * ============================================================================
 */

/* ============================================================================
 * Revision History
 * ================
 * 18-Feb-2013 Created
 * ============================================================================
 */

#include "csl_dma.h"
#include "csl_i2s.h"
#include <csl_general.h>
#include <stdio.h>
#include "csl_sysctrl.h"

#define CSL_TEST_FAILED         (1)
#define CSL_TEST_PASSED         (0)
#define CSL_DMA0_CH0            (0)
#define CSL_I2S_DMA_BUF_LEN		(1)

#define NO_OF_I2S_INSTANCES     (4)

extern Uint16 PG4_flag;
extern Uint16 PG3_flag;

CSL_DMA_ChannelObj dmaObj;
CSL_DMA_Config dmaConfig;

CSL_DMA_Handle dmaLeftTxHandle_inst1;
CSL_DMA_Handle dmaRightTxHandle_inst1;
CSL_DMA_Handle dmaLeftRxHandle_inst1;
CSL_DMA_Handle dmaRightRxHandle_inst1;

Uint32 i2sDmaWriteLeftBuff[CSL_I2S_DMA_BUF_LEN] = {0xDEADBEEF};
Uint32 i2sDmaReadLeftBuff[CSL_I2S_DMA_BUF_LEN] = {0x00000000};
Uint32 i2sDmaWriteRightBuff[CSL_I2S_DMA_BUF_LEN] = {0x12345678};
Uint32 i2sDmaReadRightBuff[CSL_I2S_DMA_BUF_LEN] = {0x00000000};

/**
 * \brief This function tests a particular instance of I2S.
 *
 * This function first configures the I2S in loopback mode and the DMA for the
 * left and right channels of the I2S. Then it transfers the data to the I2S TX
 * registers using DMA, reads the data from the I2S RX registers using DMA and
 * finally compares the written and read values.
 *
 * \param i2shandle   [IN] I2S Handle
 * \param i2sInstance [IN] I2S instance to be tested
 *
 * \return Error Status
 */
Int16 startDmaTransfer (CSL_I2sHandle i2shandle, I2S_Instance i2sInstance);

/*
	This is functionality is to configure DMA
	for the source and destination address.

	Function returns:
	CSL_DMA_Handle              - Valid handler
	NULL                        - Invalid handler
*/

CSL_DMA_Handle CSL_configDmaForI2s(CSL_DMAChanNum  chanNum)
{
	CSL_DMA_Handle    dmaHandle;
	CSL_Status        status;

    status = DMA_init();
    if (status != CSL_SOK)
    {
        printf("DMA_init() Failed \n");
        dmaHandle = NULL;
    }

	dmaHandle = DMA_open(chanNum, &dmaObj,&status);
    if (dmaHandle == NULL)
    {
        printf("DMA_open() Failed \n");
        dmaHandle = NULL;
    }

	status = DMA_config(dmaHandle, &dmaConfig);
    if (status != CSL_SOK)
    {
        printf("DMA_config() Failed \n");
        dmaHandle = NULL;
    }

    return (dmaHandle);
}

/*
	This is functionality Transmit  and
	receive data with DMA mode.
	The data transmission and receive happen in stereo mode.

	Function returns:
	CSL_TEST_FAILED                -Failure
	CSL_TEST_PASSED                -Success
*/
Int16 i2s_DMA_sample(void)
{
	Int16 			status;
	Int16 			result;
	CSL_I2sHandle	hI2s[NO_OF_I2S_INSTANCES];
	I2S_Config		hwConfig;
	Uint16 			looper;

	status = CSL_TEST_PASSED;

	/** Open the device for each instance of the I2S */
	for (looper = 0; looper < NO_OF_I2S_INSTANCES; looper++)
	{
#if (defined(CHIP_C5517))
		if (1 == looper) /* I2S1 is not supported on C5517 */
		{
			looper++;
		}
#endif

		hI2s[looper] = I2S_open((I2S_Instance)looper, DMA_POLLED, I2S_CHAN_STEREO);
		if (NULL == hI2s)
		{
			status = CSL_TEST_FAILED;
			return (status);
		}
		else
		{
			printf ("I2S Module Instance %d opened successfully\n", looper);
		}
	}

    result = SYS_setEBSR(CSL_EBSR_FIELD_SP0MODE,
                         CSL_EBSR_SP0MODE_1);
#if (!(defined(CHIP_C5517)))
    result = SYS_setEBSR(CSL_EBSR_FIELD_SP1MODE,
                         CSL_EBSR_SP1MODE_1);
#endif
    result = SYS_setEBSR(CSL_EBSR_FIELD_PPMODE,
                         CSL_EBSR_PPMODE_6);
    if (CSL_SOK != result)
    {
        printf("SYS_setEBSR failed\n");
        return (result);
    }

	/** Set the value for the I2S configure structure				*/
	hwConfig.dataType 			= I2S_STEREO_ENABLE;
	hwConfig.loopBackMode 		= I2S_LOOPBACK_ENABLE;
	hwConfig.fsPol 				= I2S_FSPOL_LOW;
	hwConfig.clkPol				= I2S_FALLING_EDGE;
	hwConfig.datadelay			= I2S_DATADELAY_ONEBIT;
	hwConfig.datapack			= I2S_DATAPACK_ENABLE;
	hwConfig.signext			= I2S_SIGNEXT_DISABLE;
	hwConfig.wordLen			= I2S_WORDLEN_16;
	hwConfig.i2sMode			= I2S_MASTER;
	hwConfig.clkDiv				= I2S_CLKDIV4;
	hwConfig.fsDiv				= I2S_FSDIV32;
	hwConfig.FError				= I2S_FSERROR_DISABLE;
	hwConfig.OuError			= I2S_OUERROR_DISABLE;

	for (looper = 0; looper < NO_OF_I2S_INSTANCES; looper++)
	{
#if (defined(CHIP_C5517))
		if (1 == looper) /* I2S1 is not supported on C5517 */
		{
			looper++;
		}
#endif

		/* Configure the I2S hardware for each of the I2S */
		result = I2S_setup(hI2s[looper], &hwConfig);
		if (result != CSL_SOK)
		{
			status = CSL_TEST_FAILED;
			return (status);
		}
		else
		{
			printf ("I2S Module Instance %d Configured successfully\n", looper);
		}
	}

	for (looper = 0; looper < NO_OF_I2S_INSTANCES; looper++)
	{
#if (defined(CHIP_C5517))
		if (1 == looper) /* I2S1 is not supported on C5517 */
		{
			looper++;
		}
#endif

		result = startDmaTransfer (hI2s[looper], (I2S_Instance)looper);
		if (result != CSL_SOK)
		{
			printf ("I2S transfer for Instance %d Failed\n", looper);
			status = CSL_TEST_FAILED;
		}
		else
		{
			printf ("I2S transfer for Instance %d Successful\n", looper);
		}
	}

	/* Reset the registers for each instance of I2S */
	for (looper = 0; looper < NO_OF_I2S_INSTANCES; looper++)
	{
#if (defined(CHIP_C5517))
		if (1 == looper) /* I2S1 is not supported on C5517 */
		{
			looper++;
		}
#endif

		result = I2S_reset(hI2s[looper]);
		if (result != CSL_SOK)
		{
			status = CSL_TEST_FAILED;
			return (status);
		}
		else
		{
			printf ("I2S Reset for Instance %d Successful\n", looper);
		}
	}

	/* Close the I2S operation for each instance of the I2S */
	for (looper = 0; looper < NO_OF_I2S_INSTANCES; looper++)
	{
#if (defined(CHIP_C5517))
		if (1 == looper) /* I2S1 is not supported on C5517 */
		{
			looper++;
		}
#endif

		result = I2S_close(hI2s[looper]);
		if (result != CSL_SOK)
		{
			status = CSL_TEST_FAILED;
			return (status);
		}
		else
		{
			printf ("I2S Close for Instance %d Successful\n", looper);
		}
	}

	return (status);
}

/**
 * \brief This function tests a particular instance of I2S.
 *
 * This function first configures the I2S in loopback mode and the DMA for the
 * left and right channels of the I2S. Then it transfers the data to the I2S TX
 * registers using DMA, reads the data from the I2S RX registers using DMA and
 * finally compares the written and read values.
 *
 * \param i2shandle   [IN] I2S Handle
 * \param i2sInstance [IN] I2S instance to be tested
 *
 * \return Error Status
 */
Int16 startDmaTransfer (CSL_I2sHandle i2shandle, I2S_Instance i2sInstance)
{
	Int16  status;
	Uint16 looper;
	Uint16 leftChanNum;
	Uint16 rightChanNum;

	/* Initialize data buffers */
	for (looper = 0; looper < CSL_I2S_DMA_BUF_LEN; looper++)
	{
		i2sDmaWriteLeftBuff[looper]  = 0x12345678;
		i2sDmaWriteRightBuff[looper] = 0x12345678;

		i2sDmaReadLeftBuff[looper]	= 0;
		i2sDmaReadRightBuff[looper]	= 0;
	}

	/* Configure DMA channel for I2S write, for Left Channel */
#if (defined(CHIP_C5505_C5515) || defined(CHIP_C5504_C5514) || defined (CHIP_C5517))
	dmaConfig.pingPongMode = CSL_DMA_PING_PONG_DISABLE;
#endif
	dmaConfig.autoMode     = CSL_DMA_AUTORELOAD_DISABLE;
	dmaConfig.burstLen     = CSL_DMA_TXBURST_1WORD;
	dmaConfig.trigger      = CSL_DMA_EVENT_TRIGGER;
	dmaConfig.dmaInt       = CSL_DMA_INTERRUPT_DISABLE;
	dmaConfig.chanDir      = CSL_DMA_WRITE;
	dmaConfig.trfType      = CSL_DMA_TRANSFER_IO_MEMORY;
	dmaConfig.dataLen      = 4;
	dmaConfig.srcAddr      = (Uint32)i2sDmaWriteLeftBuff;

	switch (i2sInstance)
	{
		case I2S_INSTANCE0:
			dmaConfig.dmaEvt   = CSL_DMA_EVT_I2S0_TX;
			leftChanNum        = CSL_DMA_CHAN0;
			dmaConfig.destAddr = (Uint32)(0x2808);
		    break;

#if (!(defined(CHIP_C5517)))
		case I2S_INSTANCE1:
			dmaConfig.dmaEvt   = CSL_DMA_EVT_I2S1_TX;
			leftChanNum        = CSL_DMA_CHAN12;
			dmaConfig.destAddr = (Uint32)(0x2908);
		    break;
#endif

		case I2S_INSTANCE2:
			dmaConfig.dmaEvt   = CSL_DMA_EVT_I2S2_TX;
			leftChanNum        = CSL_DMA_CHAN4;
			dmaConfig.destAddr = (Uint32)(0x2A08);
		    break;

		case I2S_INSTANCE3:
			dmaConfig.dmaEvt   = CSL_DMA_EVT_I2S3_TX;
			leftChanNum        = CSL_DMA_CHAN8;
			dmaConfig.destAddr = (Uint32)(0x2B08);
		    break;
	}

	dmaLeftTxHandle_inst1 = CSL_configDmaForI2s((CSL_DMAChanNum)leftChanNum);
	if (dmaLeftTxHandle_inst1 == NULL)
	{
		printf("DMA Config for I2S Write Failed!\n!");
		return (CSL_TEST_FAILED);
	}

	I2S_transEnable(i2shandle, TRUE);

	status = DMA_start(dmaLeftTxHandle_inst1);
	if (status != CSL_SOK)
	{
		printf("I2S Dma Write Failed!!\n");
		return (status);
	}
	while(DMA_getStatus(dmaLeftTxHandle_inst1));

	status = DMA_close(dmaLeftTxHandle_inst1);
	if (status != CSL_SOK)
	{
		printf("DMA_close Failed!!\n");
		return (status);
	}

	/* Configure DMA channel for I2S write, for Right Channel */
#if (defined(CHIP_C5505_C5515) || defined(CHIP_C5504_C5514) || defined(CHIP_C5517))
	dmaConfig.pingPongMode = CSL_DMA_PING_PONG_DISABLE;
#endif
	dmaConfig.autoMode     = CSL_DMA_AUTORELOAD_DISABLE;
	dmaConfig.burstLen     = CSL_DMA_TXBURST_1WORD;
	dmaConfig.trigger      = CSL_DMA_EVENT_TRIGGER;
	dmaConfig.dmaInt       = CSL_DMA_INTERRUPT_DISABLE;
	dmaConfig.chanDir      = CSL_DMA_WRITE;
	dmaConfig.trfType      = CSL_DMA_TRANSFER_IO_MEMORY;
	dmaConfig.dataLen      = 4;
	dmaConfig.srcAddr      = (Uint32)i2sDmaWriteRightBuff;

	switch (i2sInstance)
	{
		case I2S_INSTANCE0:
			dmaConfig.dmaEvt   = CSL_DMA_EVT_I2S0_TX;
			rightChanNum       = CSL_DMA_CHAN1;
			dmaConfig.destAddr = (Uint32)(0x280C);
		    break;

#if (!(defined(CHIP_C5517)))
		case I2S_INSTANCE1:
			dmaConfig.dmaEvt   = CSL_DMA_EVT_I2S1_TX;
			rightChanNum       = CSL_DMA_CHAN13;
			dmaConfig.destAddr = (Uint32)(0x290C);
		    break;
#endif

		case I2S_INSTANCE2:
			dmaConfig.dmaEvt   = CSL_DMA_EVT_I2S2_TX;
			rightChanNum       = CSL_DMA_CHAN5;
			dmaConfig.destAddr = (Uint32)(0x2A0C);
		    break;

		case I2S_INSTANCE3:
			dmaConfig.dmaEvt   = CSL_DMA_EVT_I2S3_TX;
			rightChanNum       = CSL_DMA_CHAN9;
			dmaConfig.destAddr = (Uint32)(0x2B0C);
		    break;
	}

	dmaRightTxHandle_inst1 = CSL_configDmaForI2s((CSL_DMAChanNum)rightChanNum);
	if (dmaRightTxHandle_inst1 == NULL)
	{
		printf("DMA Config for I2S Write Failed!\n!");
		return (CSL_TEST_FAILED);
	}

	I2S_transEnable(i2shandle, TRUE);

	status = DMA_start(dmaRightTxHandle_inst1);
	if (status != CSL_SOK)
	{
		printf("I2S Dma Write Failed!!\n");
		return (status);
	}
	while(DMA_getStatus(dmaRightTxHandle_inst1));

	status = DMA_close(dmaRightTxHandle_inst1);
	if (status != CSL_SOK)
	{
		printf("DMA_close Failed!!\n");
		return (status);
	}

	/* Configure DMA channel for I2S Read, for Left Channel */
#if (defined(CHIP_C5505_C5515) || defined(CHIP_C5504_C5514) || defined(CHIP_C5517))
	dmaConfig.pingPongMode = CSL_DMA_PING_PONG_DISABLE;
#endif
	dmaConfig.autoMode     = CSL_DMA_AUTORELOAD_DISABLE;
	dmaConfig.burstLen     = CSL_DMA_TXBURST_1WORD;
	dmaConfig.trigger      = CSL_DMA_EVENT_TRIGGER;
	dmaConfig.dmaInt       = CSL_DMA_INTERRUPT_DISABLE;
	dmaConfig.chanDir      = CSL_DMA_READ;
	dmaConfig.trfType      = CSL_DMA_TRANSFER_IO_MEMORY;
	dmaConfig.dataLen      = 4;
	dmaConfig.destAddr     = (Uint32)i2sDmaReadLeftBuff;

	switch (i2sInstance)
	{
		case I2S_INSTANCE0:
			dmaConfig.dmaEvt  = CSL_DMA_EVT_I2S0_RX;
			leftChanNum       = CSL_DMA_CHAN2;
			dmaConfig.srcAddr = (Uint32)(0x2828);
		    break;

#if (!(defined(CHIP_C5517)))
		case I2S_INSTANCE1:
			dmaConfig.dmaEvt  = CSL_DMA_EVT_I2S1_RX;
			leftChanNum       = CSL_DMA_CHAN14;
			dmaConfig.srcAddr = (Uint32)(0x2928);
		    break;
#endif

		case I2S_INSTANCE2:
			dmaConfig.dmaEvt  = CSL_DMA_EVT_I2S2_RX;
			leftChanNum       = CSL_DMA_CHAN6;
			dmaConfig.srcAddr = (Uint32)(0x2A28);
		    break;

		case I2S_INSTANCE3:
			dmaConfig.dmaEvt  = CSL_DMA_EVT_I2S3_RX;
			leftChanNum       = CSL_DMA_CHAN10;
			dmaConfig.srcAddr = (Uint32)(0x2B28);
		    break;
	}

	dmaLeftRxHandle_inst1 = CSL_configDmaForI2s((CSL_DMAChanNum)leftChanNum);
	if (dmaLeftRxHandle_inst1 == NULL)
	{
		printf("DMA Config for I2S Read Failed!\n!");
		return (CSL_TEST_FAILED);
	}

	status = DMA_start(dmaLeftRxHandle_inst1);
	if (status != CSL_SOK)
	{
		printf("I2S Dma Read Failed!!\n");
		return (status);
	}
	while(DMA_getStatus(dmaLeftRxHandle_inst1));

	status = DMA_close(dmaLeftRxHandle_inst1);
	if (status != CSL_SOK)
	{
		printf("DMA_close Failed!!\n");
		return (status);
	}

	/* Configure DMA channel for I2S Read, for Right Channel */
#if (defined(CHIP_C5505_C5515) || defined(CHIP_C5504_C5514) || defined(CHIP_C5517))
	dmaConfig.pingPongMode = CSL_DMA_PING_PONG_DISABLE;
#endif
	dmaConfig.autoMode     = CSL_DMA_AUTORELOAD_DISABLE;
	dmaConfig.burstLen     = CSL_DMA_TXBURST_1WORD;
	dmaConfig.trigger      = CSL_DMA_EVENT_TRIGGER;
	dmaConfig.dmaInt       = CSL_DMA_INTERRUPT_DISABLE;
	dmaConfig.chanDir      = CSL_DMA_READ;
	dmaConfig.trfType      = CSL_DMA_TRANSFER_IO_MEMORY;
	dmaConfig.dataLen      = 4;
	dmaConfig.destAddr     = (Uint32)i2sDmaReadRightBuff;

	switch (i2sInstance)
	{
		case I2S_INSTANCE0:
			dmaConfig.dmaEvt  = CSL_DMA_EVT_I2S0_RX;
			rightChanNum      = CSL_DMA_CHAN3;
			dmaConfig.srcAddr = (Uint32)(0x282C);
		    break;

#if (!(defined(CHIP_C5517)))
		case I2S_INSTANCE1:
			dmaConfig.dmaEvt  = CSL_DMA_EVT_I2S1_RX;
			rightChanNum      = CSL_DMA_CHAN15;
			dmaConfig.srcAddr = (Uint32)(0x292C);
		    break;
#endif

		case I2S_INSTANCE2:
			dmaConfig.dmaEvt  = CSL_DMA_EVT_I2S2_RX;
			rightChanNum      = CSL_DMA_CHAN7;
			dmaConfig.srcAddr = (Uint32)(0x2A2C);
		    break;

		case I2S_INSTANCE3:
			dmaConfig.dmaEvt  = CSL_DMA_EVT_I2S3_RX;
			rightChanNum      = CSL_DMA_CHAN11;
			dmaConfig.srcAddr = (Uint32)(0x2B2C);
		    break;
	}

	dmaRightRxHandle_inst1 = CSL_configDmaForI2s((CSL_DMAChanNum)rightChanNum);
	if (dmaRightRxHandle_inst1 == NULL)
	{
		printf("DMA Config for I2S Read Failed!\n!");
		return (CSL_TEST_FAILED);
	}

	I2S_transEnable(i2shandle, TRUE);

	status = DMA_start(dmaRightRxHandle_inst1);
	if (status != CSL_SOK)
	{
		printf("I2S Dma Read Failed!!\n");
		return (status);
	}
	while(DMA_getStatus(dmaRightRxHandle_inst1));

	I2S_transEnable(i2shandle, FALSE);

	status = DMA_close(dmaRightRxHandle_inst1);
	if (status != CSL_SOK)
	{
		printf("DMA_close Failed!!\n");
		return (status);
	}

	/* Swap words in I2S write buffer */
	status = DMA_swapWords((Uint16*)i2sDmaReadLeftBuff, 2*CSL_I2S_DMA_BUF_LEN);
	if (status != CSL_SOK)
	{
		printf ("DMA word swap API failed\n");
		return (status);
	}

	status = DMA_swapWords((Uint16*)i2sDmaReadRightBuff, 2*CSL_I2S_DMA_BUF_LEN);
	if (status != CSL_SOK)
	{
		printf ("DMA word swap API failed\n");
		return (status);
	}

	/** Compare the read and write buffer for Left Channel */
	for (looper = 0; looper < CSL_I2S_DMA_BUF_LEN; looper++)
	{
		if (i2sDmaWriteLeftBuff[looper] != i2sDmaReadLeftBuff[looper])
		{
			printf("\nI2S Read & Write Buffers for Left Channel doesn't Match!!!\n");
			status = CSL_TEST_FAILED;
			return (status);
		}
	}

	if (looper == CSL_I2S_DMA_BUF_LEN)
	{
		printf("\nI2S Read & Write Buffers for Left Channel Match!!!\n");
	}

	/** Compare the read and write buffer for Right Channel */
	for (looper = 0; looper < CSL_I2S_DMA_BUF_LEN; looper++)
	{
		if (i2sDmaWriteRightBuff[looper] != i2sDmaReadRightBuff[looper])
		{
			printf("I2S Read & Write Buffers for Right Channel doesn't Match!!!\n");
			status = CSL_TEST_FAILED;
			return (status);
		}
	}

	if (looper == CSL_I2S_DMA_BUF_LEN)
	{
		printf("I2S Read & Write Buffers for Right Channel Match!!!\n");
	}

	status = CSL_TEST_PASSED;
	return (status);
}

/*
 This is the main function to call sample program
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
	Int16    status;

	printf("CSL I2S DMA MODE TEST!\n\n");

	status = i2s_DMA_sample();

	if (status != CSL_TEST_PASSED)
	{
		printf("\nCSL I2S DMA MODE TEST FAILED!!\n\n");
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 2 --
   /////  Resetting PaSs_StAtE to 0 if error detected here.
        PaSs_StAtE = 0x0000; // Was initialized to 1 at declaration.
   /////
	}
	else
	{
		printf("\nCSL I2S DMA MODE TEST PASSED!\n\n");
	}
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 3 --
   /////  At program exit, copy "PaSs_StAtE" into "PaSs".
        PaSs = PaSs_StAtE; //If flow gets here, override PaSs' initial 0 with
   /////                   // pass/fail value determined during program execution.
   /////  Note:  Program should next exit to C$$EXIT and halt, where DSS, under
   /////   control of a host PC script, will read and record the PaSs' value.
   /////
}

