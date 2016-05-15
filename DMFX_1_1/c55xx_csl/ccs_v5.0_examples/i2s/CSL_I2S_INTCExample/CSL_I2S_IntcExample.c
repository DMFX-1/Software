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

/** @file csl_i2s_IntcExample.c
 *
 *  @brief Test code to verify the CSL I2S functionality in Interrupt mode
 *
 *
 * \page    page6  I2S EXAMPLE DOCUMENTATION
 *
 * \section I2S3   I2S EXAMPLE3 - INTERRUPT MODE TEST
 *
 * \subsection I2S3x    TEST DESCRIPTION:
 *		This test verifies the operation of CSL I2S module in interrupt mode.
 * I2S module on C5535/C5515/C5517 DSP is used to exchange the audio data between
 * the audio codec and DSP.
 *
 * Testing of the I2S with the audio codec is not possible as codec is not
 * part of the CSL package. This test verifies the I2S operation in loopback
 * mode. In loopback mode of operation data written to the I2S data transmit
 * registers will be transmitted to the I2S data receive registers. CSL INTC
 * module should be configured and I2S ISR should be registered before starting
 * the data transfer. Separate ISRs are registered for I2S Tx and Rx. Interrupts
 * are used to check the Tx/Rx status of the I2S. I2S is configured to master
 * mode,stereo, 16bit data length and to loop back mode. When the I2S data
 * transfer is enabled I2S starts generating the Tx interrupts. Data is written
 * to the I2S Tx registers in the ISR 'i2s_txIsr'. As the I2S is configured in
 * loopback mode when the data is written to Tx register it will be transmitted
 * to Rx register and Rx interrupt will be generated. Data will be read from the
 * I2S Rx registers in the ISR 'i2s_rxIsr'.I2S write and read buffers are
 * compared for the data verification.
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5535, C5515
 * AND C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 *
 * \subsection I2S3y    TEST PROCEDURE:
 *  @li Open the CCS and connect the target (C5535/C5515 EVM or C5517 EVM)
 *  @li Open the project "CSL_I2S_INTCExample_Out.pjt" and build it
 *  @li Load the program on to the target
 *  @li Set the PLL frequency to 12.288MHz
 *  @li Run the program and observe the test result
 *  @li Repeat the test at the following PLL frequencies
 *      C5515: 60MHz and 100MHz
 *      C5517: 60MHz, 100MHz, 150MHz and 200MHz
 *      C5535 eZdsp: 60MHz and 100MHz
 *  @li Repeat the test in Release mode
 *
 * \subsection I2S3z    TEST RESULT:
 *  @li All the CSL APIs should return success
 *  @li Read and write data buffer comparison should be successful.
 *
 *
 */

/* ============================================================================
 * Revision History
 * ================
 * 05-Sep-2008 Created
 * 10-Jul-2012 Added C5517 Compatibility
 * ============================================================================
 */

#include "csl_i2s.h"
#include "csl_intc.h"
#include <csl_general.h>
#include <stdio.h>
#include "csl_sysctrl.h"

#define CSL_TEST_FAILED         (1)
#define CSL_TEST_PASSED         (0)
#define	CSL_I2S_BUF_LEN			(4)

CSL_IrqDataObj  CSL_IrqData;
Uint16 i2sIntcWriteBuff[CSL_I2S_BUF_LEN];
Uint16 i2sIntcReadBuff[CSL_I2S_BUF_LEN];
CSL_I2sHandle	i2sHandle;
Uint16 writeCompete = 0;
Uint16 readComplete = 0;
extern void VECSTART(void);

/*
	This is ISR for the data transmission.
	This function returns:
	Void               -void
*/
interrupt void i2s_rxIsr()
{
	Int16 			result;

	IRQ_disable(PROG1_EVENT);
	/** Read data from register and buffered					*/
	result = I2S_read(i2sHandle, i2sIntcReadBuff, CSL_I2S_BUF_LEN);
	if(result == CSL_SOK)
	{
		printf ("I2S Read Successful\n");
		readComplete = 1;
	}
}

/*
	This is ISR for the data Receive.
	This function returns:
	Void               -void
*/
interrupt void i2s_txIsr()
{
	Int16 			result;

	IRQ_disable(PROG0_EVENT);

	/** Write the buffer data to the registers					*/
	result = I2S_write(i2sHandle, i2sIntcWriteBuff, CSL_I2S_BUF_LEN);
	if(result == CSL_SOK)
	{
		printf ("I2S Write Successful\n");
		writeCompete = 1;
	}
}


/*
	This is functionality Transmit  and
	receive data with interrupt mode.
	The data transmission and receive happen in stereo mode.

	Function returns:
	CSL_TEST_FAILED                -Failure
	CSL_TEST_PASSED                -Success
*/

Int16 i2s_IntcSample(void)
{
	Int16 			status = CSL_TEST_FAILED;
	Int16 			result;
	I2S_Config		hwConfig;
	Uint16 			looper;
	CSL_IRQ_Config 	config;

	/* Initialize the read and write buffers					*/
	for(looper=0; looper < CSL_I2S_BUF_LEN; looper++)
	{
		i2sIntcWriteBuff[looper] = 0x001 + looper;
		i2sIntcReadBuff[looper]  = 0xffff;
	}


	/* Open the device with instance 0							*/
	i2sHandle = I2S_open(I2S_INSTANCE0, I2S_INTERRUPT, I2S_CHAN_STEREO);
	if(NULL == i2sHandle)
	{
		status = CSL_TEST_FAILED;
		return (status);
	}
	else
	{
		printf ("I2S Module Instance Open Successful\n");
	}

    result = SYS_setEBSR(CSL_EBSR_FIELD_SP0MODE,
                         CSL_EBSR_SP0MODE_1);
    if(CSL_SOK != result)
    {
        printf("SYS_setEBSR failed\n");
        return (result);
    }

	/* Set the value for the configure structure				*/
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
	hwConfig.FError				= I2S_FSERROR_ENABLE;
	hwConfig.OuError			= I2S_OUERROR_ENABLE;

	/* Configure hardware registers							*/
	result = I2S_setup(i2sHandle, &hwConfig);

	if(result != CSL_SOK)
	{
		status = CSL_TEST_FAILED;
		return (status);
	}
	else
	{
		printf ("I2S Module Configuration Successful\n");
	}

	/* Clear any pending interrupts */
	IRQ_clearAll();

	/* Disable all the peripheral interrupts */
	IRQ_disableAll();

	IRQ_setVecs((Uint32)(&VECSTART));
	/* Configuring Interrupt */

    config.funcAddr = &i2s_txIsr;
	IRQ_plug (PROG0_EVENT, config.funcAddr);
	/* Enabling Interrupt */
	IRQ_enable(PROG0_EVENT);

	IRQ_globalEnable();

	result = I2S_transEnable(i2sHandle, TRUE);
	while(writeCompete != 1);
	IRQ_globalDisable();
	result = I2S_transEnable(i2sHandle, FALSE);

	config.funcAddr = &i2s_rxIsr;
	IRQ_plug (PROG1_EVENT, config.funcAddr);
	/* Enabling Interrupt */
	IRQ_enable(PROG1_EVENT);

	IRQ_globalEnable();
	result = I2S_transEnable(i2sHandle, TRUE);
	while(readComplete != 1);
	IRQ_globalDisable();
	result = I2S_transEnable(i2sHandle, FALSE);

	/* Reset the registers										*/
	result = I2S_reset(i2sHandle);

	if(result != CSL_SOK)
	{
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf ("I2S Reset Successful\n");
	}

	/* Close the operation										*/
	result = I2S_close(i2sHandle);

	if(result != CSL_SOK)
	{
		status = CSL_TEST_FAILED;
		return (status);
	}
	else
	{
		printf ("I2S Close Successful\n");
	}

	/* Clear any pending interrupts */
	IRQ_clearAll();

	/* Disable all the peripheral interrupts */
	IRQ_disableAll();

	/* Compare the read and write buffer						*/
	for(looper=0; looper < CSL_I2S_BUF_LEN; looper++)
	{
		if(i2sIntcWriteBuff[looper] != i2sIntcReadBuff[looper])
		{
			printf("I2S Read & Write Buffers doesn't Match!!!\n");
			status = CSL_TEST_FAILED;
			return(status);
		}
	}

	if(looper == CSL_I2S_BUF_LEN)
	{
		printf("I2S Read & Write Buffers Match!!!\n");
	}

	return (CSL_TEST_PASSED);
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

	printf("CSL I2S INTERRUPT MODE TEST!\n\n");

	IRQ_globalDisable();
	status = i2s_IntcSample();
	if(status != CSL_TEST_PASSED)
	{
		printf("\nCSL I2S INTERRUPT MODE TEST FAILED!!\n\n");
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 2 --
   /////  Reseting PaSs_StAtE to 0 if error detected here.
        PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
   /////
	}
	else
	{
		printf("\nCSL I2S INTERRUPT MODE TEST PASSED!!\n\n");
	}

	IRQ_globalDisable();
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 3 --
   /////  At program exit, copy "PaSs_StAtE" into "PaSs".
        PaSs = PaSs_StAtE; //If flow gets here, override PaSs' initial 0 with
   /////                   // pass/fail value determined during program execution.
   /////  Note:  Program should next exit to C$$EXIT and halt, where DSS, under
   /////   control of a host PC script, will read and record the PaSs' value.
   /////
}


