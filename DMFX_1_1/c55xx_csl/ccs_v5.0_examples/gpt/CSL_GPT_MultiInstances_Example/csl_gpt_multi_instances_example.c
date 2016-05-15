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

 /* @file csl_gpt_multiple_instances_example.c
 *
 *  @brief Example test code to verify the Multiple Instance of CSL GPT module
 *  functionality
 */
 
/** \page    page4  GPT MULTIPLE INSTANCE EXAMPLE DOCUMENTATION
 *
 * \section GPT3   GPT3 EXAMPLE
 *
 * \subsection GPT3x    TEST DESCRIPTION:
 *		This test code verifies the functionality of the multiple instances of
 * CSL GPT (General Purpose Timer) module. GPT module of C5535/C5515/C5517 DSP
 * is used to provide interrupts to the CPU to schedule periodic tasks or a
 * delayed task. GPT can be configured with a counter and pre-scaler divider
 * value. When the GPT is started counter will be decremented to 0. The rate at
 * which this count happens can be controlled by the prescaler divider value.
 * GPT clock is derived by dividing the CPU clock with the pre-scaler divider
 * value.
 *
 * This test verifies the multiple instances of the GPT module by checking
 * whether the GTP is decrementing the counter or not. Each instance of the GPT
 * is configured with a count value 0xFFFF and pre-scaler divider value 256.
 * GPT is started and counter value is read. After few cycles of delay GPT
 * counter value is read again. Both the counter values are compared to verify
 * whether the second count value is less than the first counter value or not.
 *
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5535, C5515
 * AND C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 *
 * \subsection GPT1y    TEST PROCEDURE:
 *  @li Open the CCS and connect the target (C5535/C5515 EVM or C5517 EVM))
 *  @li Open the project "CSL_GPT_MultiInstances_Example_Out.pjt" and build it
 *  @li Load the program on to the target
 *  @li Set the PLL frequency to 12.288MHz
 *  @li Run the program and observe the test result
 *  @li Repeat the test at the following PLL frequencies
 *      C5515: 60MHz and 100MHz
 *      C5517: 60MHz, 100MHz, 150MHz and 200MHz
 *      C5535 eZdsp: 60MHz and 100MHz
 *  @li Repeat the test in Release mode
 *
 * \subsection GPT1z    TEST RESULT:
 *  @li All the CSL APIs should return success
 *  @li Timer count value comparison should be successful
 *
 */

/* ============================================================================
 * Revision History
 * ================
 * 08-Feb-2013 Created
 * ============================================================================
 */

#include <stdio.h>
#include "csl_gpt.h"
#include "csl_intc.h"
#include <csl_general.h>

#define CSL_TEST_FAILED    (1u)
#define CSL_TEST_PASSED    (0)

#define NO_OF_GPT_INSTANCES (3)
/**< Macro to hold the value of the maximum no of GPT instances */

/**
 *  \brief  GPT Count Read Test function for multiple instances
 *
 * This function verifies the multiple instances of the GPT module by reading
 * the counter values from the GPT for all the 3 instances of the GPT
 *
 *  \param  NONE
 *
 *  \return CSL_TEST_PASSED  - Success
 *          CSL_TEST_FAILED  - Failure
 */
Int16 CSL_gpt_multiInst_CountReadTest(void);

/**
 *  \brief  main function
 *
 *  This function calls the GPT test function and displays the
 *  test result.
 *
 *  \param  none
 *
 *  \return none
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
	Int16	result;

	printf("CSL GPT MULTIPLE INSTANCES TESTS!\n\n");

	printf("COUNT READ TEST FOR MULTIPLE INSTANCES OF THE TIMER!\n");
	result = CSL_gpt_multiInst_CountReadTest();
	if(CSL_TEST_FAILED == result)
	{
		printf("\n\nTIMER COUNT READ TEST FOR MULTIPLE INSTANCES FAILED!!\n");
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 2 --
   /////  Reseting PaSs_StAtE to 0 if error detected here.
        PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
   /////
	}
	else
	{
		printf("\n\nTIMER COUNT READ TEST FOR MULTIPLE INSTANCES PASSED!!\n");
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
 *  \brief  GPT Count Read Test function for multiple instances
 *
 * This function verifies the multiple instances of the GPT module by reading
 * the counter values from the GPT for all the 3 instances of the GPT
 *
 *  \param  NONE
 *
 *  \return CSL_TEST_PASSED  - Success
 *          CSL_TEST_FAILED  - Failure
 */
Int16 CSL_gpt_multiInst_CountReadTest(void)
{
	CSL_GptObj		   gptObj;
	CSL_Handle	       hGpt[NO_OF_GPT_INSTANCES];
	CSL_Status 	       status;
	CSL_Config 	       hwConfig;
	Uint32 		       timeCnt1;
	Uint32 		       timeCnt2;
	Uint16             looper;
	volatile Uint16	   delay;

	status = 0;

	for (looper = 0; looper < NO_OF_GPT_INSTANCES; looper++)
	{
		printf("\n\n");

		timeCnt1 = 0;
		timeCnt2 = 0;

		/* Open each instance of the CSL GPT module */
		hGpt[looper] = GPT_open ((CSL_Instance)looper, &gptObj, &status);
		if((NULL == hGpt[looper]) || (CSL_SOK != status))
		{
			printf("GPT Open Failed for Instance %d\n", looper);
			return (CSL_TEST_FAILED);
		}
		else
		{
			printf("GPT Open Successful for Instance %d\n", looper);
		}

		/* Reset each instance of the GPT module */
		status = GPT_reset(hGpt[looper]);
		if(CSL_SOK != status)
		{
			printf("GPT Reset Failed for Instance %d\n", looper);
			return (CSL_TEST_FAILED);
		}
		else
		{
			printf("GPT Reset Successful for Instance %d\n", looper);
		}

		/* Configure each instance of the GPT module */
		hwConfig.autoLoad 	 = GPT_AUTO_ENABLE;
		hwConfig.ctrlTim 	 = GPT_TIMER_ENABLE;
		hwConfig.preScaleDiv = GPT_PRE_SC_DIV_7;
		hwConfig.prdLow 	 = 0xFFFF;
		hwConfig.prdHigh 	 = 0x0000;

		status =  GPT_config(hGpt[looper], &hwConfig);
		if(CSL_SOK != status)
		{
			printf("GPT Config Failed for Instance %d\n", looper);
			return (CSL_TEST_FAILED);
		}
		else
		{
			printf("GPT Config Successful for Instance %d\n", looper);
		}

		/* Start the Timer for each instance of GPT */
		status = GPT_start(hGpt[looper]);
		if(CSL_SOK != status)
		{
			printf("GPT Start Failed for Instance %d\n", looper);
			return (CSL_TEST_FAILED);
		}

		/* Read the Timer Count */
		status = GPT_getCnt(hGpt[looper], &timeCnt1);
		if(CSL_SOK != status)
		{
			printf("GPT Count-1 Failed for Instance %d\n", looper);
			return (CSL_TEST_FAILED);
		}
		else
		{
			printf("GPT Count-1 Successful for Instance %d\n", looper);
		}

		/* Give Some Delay */
		for(delay = 0; delay < 100; delay++);

		/* Read the Timer Count Again */
		status = GPT_getCnt(hGpt[looper], &timeCnt2);
		if(CSL_SOK != status)
		{
			printf("GPT Count-2 Failed for Instance %d\n", looper);
			return (CSL_TEST_FAILED);
		}
		else
		{
			printf("GPT Count-2 Success for Instance %d\n", looper);
		}

		/* Compare the timer count to verify whether the timer is counting or not */
		if(timeCnt2 >= timeCnt1)
		{
			printf("GTP Time Count Compare Failed for Instance %d\n", looper);
			return(CSL_TEST_FAILED);
		}
		else
		{
			printf("GTP Time Count Compare Successful for Instance %d\n", looper);
		}
	}

	printf("\n");
	/* Stop the Timer for each instance of GPT */
	for (looper = 0; looper < NO_OF_GPT_INSTANCES; looper++)
	{
		status = GPT_stop(hGpt[looper]);
		if(CSL_SOK != status)
		{
			printf("GPT Stop Failed for Instance %d\n", looper);
			return (CSL_TEST_FAILED);
		}
		else
		{
			printf("GPT Stop Successful for Instance %d\n", looper);
		}

		/* Reset the timer count to zero */
		status = GPT_reset(hGpt[looper]);

		/* Close the GPT module */
		status = GPT_close(hGpt[looper]);
		if(CSL_SOK != status)
		{
			printf("GPT Close Failed for Instance %d\n", looper);
			return (CSL_TEST_FAILED);
		}
		else
		{
			printf("GPT Close Successful for Instance %d\n", looper);
		}
	}

	return (CSL_TEST_PASSED);
}
