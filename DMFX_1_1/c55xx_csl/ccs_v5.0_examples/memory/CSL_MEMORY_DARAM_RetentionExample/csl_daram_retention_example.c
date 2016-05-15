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

/** @file csl_daram_retention_example.c
 *
 *  @brief DARAM memory retention mode test example source file
 *
 *
 * \page    page11  MEMORY EXAMPLE DOCUMENTATION
 *
 * \section MEM2   MEMORY EXAMPLE1 - DARAM RETENTION MODE TEST
 *
 * \subsection MEM2x    TEST DESCRIPTION:
 *		This test verifies the memory retention mode feature of the C5535/C5515/C5517
 * DARAM. Internal memory of the C5535/C5515/C5517 DSP can be placed into a low power
 * memory retention while retaining the content stored in the memory. This low
 * power mode is activated through the Sleep Mode Memory Control. When the
 * memory is placed in this mode, no accesses can occur. When the memory
 * retention mode is enabled, inline diodes will be added to reduce the local
 * power supply to the on-chip RAM.Memory retention mode can be enabled/disabled
 * for the DARAM and SARAM independently.
 *
 * A data buffer is allocated in the DARAM is initialized with some
 * data. CSL memory module is initialized and memory retention mode for
 * DARAM is enabled using the CSL API MEM_enableRetentionMode(). Memory
 * retention mode will be disabled after few CPU cycles by using the CSL API
 * MEM_disableRetentionMode(). Data in the DARAM buffer is verified after
 * few CPU cycles of delay. Buffer should contain the data with which it was
 * initialized before enabling the memory retention mode. This proves that
 * the data stored in the DARAM is retained when it is placed into memory
 * retention mode and is accessible after disabling the memory retention mode.
 *
 * NOTE: DURING MEMORY RETENTION MODE TEST FOR DARAM ALL THE PROGRAM SECTIONS
 * SHOULD BE PLACED IN THE SARAM. ACCESSING THE DATA IN THE DARAM BY ANY WAY
 * (CCS WATCH WINDOW, MEMORY WINDOW OR PLACING THE MOUSE POINTER ON THE DARAM
 * DATA BUFFER TO VIEW THE VALUE) AFTER ENABLING THE MEMORY RETENTION MODE
 * WILL CORRUPT ALL THE DATA IN DARAM.
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5535, C5515
 * AND C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 *
 * \subsection MEM2y    TEST PROCEDURE:
 *  @li Open the CCS and connect the target (C5535/C5515/C5517 EVM)
 *  @li Open the project "CSL_MEMORY_DARAM_RetentionExample_Out.pjt" and build it
 *  @li Load the program on to the target
 *  @li Set the PLL frequency to 12.288MHz
 *  @li Run the program and observe the test result
 *  @li Repeat the test at the following PLL frequencies
 *      C5515: 60MHz and 100MHz
 *      C5517: 60MHz, 100MHz, 150MHz and 200MHz
 *      C5535 eZdsp: 60MHz and 100MHz
 *  @li Repeat the test in Release mode
 *
 * \subsection MEM2z    TEST RESULT:
 *  @li All the CSL APIs should return success
 *  @li DARAM buffer data verification after disabling the memory retention
 *      mode should be successful
 *
 * ============================================================================
 */

/* ============================================================================
 * Revision History
 * ================
 * 06-May-2008 Created for Testing DARAM memory retention mode on C5515 PG1.4
 * 10-Jul-2012 Added C5517 Compatibility
 * ============================================================================
 */

#include <csl_mem.h>
#include <csl_general.h>
#include <stdio.h>

#define CSL_DARAM_BUF_SIZE    (100u)
#define CSL_DARAM_TEST_DATA   (0x1234u)
#define CSL_DARAM_DELAY       (100u)

/* Allocate .global section in DARAM for testing DARAM Retention Mode */
#pragma DATA_SECTION (testData,".global")
Uint16    testData[CSL_DARAM_BUF_SIZE];

#define CSL_DARAM_TEST_PASSED    (1u)
#define CSL_DARAM_TEST_FAILED    (0)

/**
 *  \brief  Tests DARAM retention mode
 *
 *  \param  none
 *
 *  \return Test result
 */
CSL_Status CSL_DARAM_RetentionTest(void);


/**
 *  \brief  main function
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
	CSL_Status status;

	printf("DARAM Memory Retention Mode test!\n\n");

	status = CSL_DARAM_RetentionTest();
	if (CSL_DARAM_TEST_PASSED == status)
	{
		printf("\nDARAM Memory Retention Mode test Passed!!\n");
	}
	else
	{
		printf("\nDARAM Memory Retention Mode test Failed!!\n");
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 2 --
   /////  Reseting PaSs_StAtE to 0 if error detected here.
        PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
   /////
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
 *  \brief  Tests DARAM retention mode
 *
 *  \param  none
 *
 *  \return Test result
 */
CSL_Status CSL_DARAM_RetentionTest(void)
{
	CSL_Status    status;
	CSL_Status    result;
	Uint16 volatile looper;

	result = CSL_DARAM_TEST_FAILED;

	/* Initialize the memory module */
	status = MEM_init();
	if (status != CSL_SOK)
	{
		printf("MEM_init failed");
		return (result);
	}

	/* Initialize DARAM data buffer */
	for (looper = 0; looper < CSL_DARAM_BUF_SIZE; looper++)
	{
		testData[looper] = CSL_DARAM_TEST_DATA;
	}

	printf("Enabling the Memory Retention Mode\n");
	/* Enable memory retention mode for DARAM */
	status = MEM_enableRetentionMode (CSL_MEM_DARAM);
	if (status != CSL_SOK)
	{
		printf ("Enabling DARAM Memory Retention Mode Failed!\n");
		return (result);
	}

	printf("Wait for few CPU Cycles....\n");

	/* Give a small delay */
	for (looper = 0; looper < CSL_DARAM_DELAY; looper++)
	{
		__asm("\tNOP");
	}

	printf("Disabling the Memory Retention Mode\n");

	/* Disable memory retention mode for DARAM */
	status = MEM_disableRetentionMode (CSL_MEM_DARAM);
	if (status != CSL_SOK)
	{
		printf ("Disabling DARAM Memory Retention Mode Failed!\n");
		return (result);
	}

	/* Give a small delay */
	for (looper = 0; looper < CSL_DARAM_DELAY; looper++)
	{
		__asm("\tNOP");
	}

	/* Verify the data */
	for (looper = 0; looper < CSL_DARAM_BUF_SIZE; looper++)
	{
		if((testData[looper] != CSL_DARAM_TEST_DATA))
		{
			printf("DARAM data is not Retained!\n");
			return (result);
		}
	}

	printf("DARAM Data Buffer verification successful\n");
	printf("DARAM data is Retained!\n");

	result = CSL_DARAM_TEST_PASSED;

	return (result);
}

