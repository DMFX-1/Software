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

/** @file csl_dat_example.c
 *
 *  @brief Example code to test the CSL DAT module functionality
 *
 * \page    page1  DAT EXAMPLE DOCUMENTATION
 *
 * \section DAT    DAT EXAMPLE
 *
 * \subsection DATa TEST DESCRIPTION:
 *		This test verifies the operation of the CSL DAT module. DAT module
 * functionality is to support MEMORY-MEMORY data transfers using DMA hardware.
 * DAT module is useful to initialize/copy large data buffers with out using CPU.
 *
 * CSL DAT module supports two basic operations
 * 1. Fill Data - Filling a data buffer with a given value
 * 2. Copy Data - Copying one data buffer to another data buffer
 * DAT module configures the DMA module and starts the data transfer depending
 * on the request sent from the application program.
 *
 * During the test data buffer 'srcDARAM' initialized to all zeros is used to
 * verify the DAT functionality. This data buffer is divided into three parts
 * and each part(512 bytes) is used for different operation. In the first
 * step part1 of the data buffer is filled with a value '0xF5' using DAT_fill()
 * CSL API. In the second step data in part1 of the data buffer is copied to
 * the part2 of the data buffer using DAT_copy() API. In the third step part3
 * of the data buffer is filled with a value '0x5' using DAT_fill() API.
 * As the DMA module of C5515 DSP is having 16 channels, DAT module can support
 * data transfers on all the 16 channels. Above test is repeated and verified
 * on all the DMA channels. After each operation data buffer is verified to
 * detect any errors in the data transfer. DAT/DMA module considers data buffer
 * as the 32bit buffer. During fill operation given value is assigned to MSW of
 * the 32bit location in the buffer. For verifying the fill operation MSW of the
 * 32bit location should be compared with the filled value.
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5535, C5515, AND
 * C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 *
 * \subsection DATb TEST PROCEDURE:
 *  @li Open the CCS and connect the target (C5535/C5515 EVM or C5517 EVM)
 *  @li Open the project "CSL_DAT_Example_Out.pjt" and build it
 *  @li Load the program on to the target
 *  @li Set the PLL frequency to 12.288MHz
 *  @li Run the program and observe the test result
 *  @li Repeat the test at the following PLL frequencies
 *      C5515: 60MHz and 100MHz
 *      C5517: 60MHz, 100MHz, 150MHz and 200MHz
 *      C5535 eZdsp: 60MHz and 100MHz
 *  @li Repeat the test in Release mode
 *
 * \subsection DATc TEST RESULT:
 *  @li All the CSL APIs should return success
 *  @li Data fill and Data copy operations should be successful on all the 16
 *      channels
 *
 */

/* ============================================================================
 * Revision History
 * ================
 * 22-Sep-2008 Created
 * 09-Jul-2012 Added C5517 chip version
 * ============================================================================
 */

#include <stdio.h>
#include <string.h>
#include <csl_dat.h>
#include <csl_general.h>

#define BUFF_SIZE    (100u)

#pragma DATA_SECTION(srcDARAM, ".buffer1")
Uint16 srcDARAM[BUFF_SIZE * 3];

Uint32 hiFillValue = 0xF5;
Uint32 loFillValue = 5;

CSL_DAT_ChannelObj datChanObj;
CSL_DAT_Handle     myhDat;

//---------Function prototypes---------
void initBuff(Uint16 *pBuf, Uint16 len);

//---------main routine---------
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 1 --
   /////  Define PaSs_StAtE variable for catching errors as program executes.
   /////  Define PaSs flag for holding final pass/fail result at program completion.
        volatile Int16 PaSs_StAtE = 0x0001; // Init to 1. Reset to 0 at any monitored execution error.
        volatile Int16 PaSs = 0x0000; // Init to 0.  Updated later with PaSs_StAtE when and if
   /////                                  program flow reaches expected exit point(s).
   /////
void main()
{
	Uint16             delay;
	volatile Uint16    index;
	CSL_Status         status;
	Uint32             src;
	Uint16             chanNumber;

	status = CSL_SOK;
	src    = (Uint32) &srcDARAM[0];

	printf("CSL DAT Test!\n");

    /* Enable the corresponding DMA clock from PCGCR Registers*/
    CSL_FINST(CSL_SYSCTRL_REGS->PCGCR1, SYS_PCGCR1_DMA0CG, ACTIVE);
    CSL_FINST(CSL_SYSCTRL_REGS->PCGCR2, SYS_PCGCR2_DMA1CG, ACTIVE);
    CSL_FINST(CSL_SYSCTRL_REGS->PCGCR2, SYS_PCGCR2_DMA2CG, ACTIVE);
    CSL_FINST(CSL_SYSCTRL_REGS->PCGCR2, SYS_PCGCR2_DMA3CG, ACTIVE);
    /* set the reset clock cycle */
	CSL_FINS(CSL_SYSCTRL_REGS->PSRCR,
	                           SYS_PSRCR_COUNT, 0x4);
    CSL_FINST(CSL_SYSCTRL_REGS->PRCR, SYS_PRCR_DMA_RST, RST);


    /* Give some delay after reset */
    for (delay = 0; delay < 100; delay++);

	for(chanNumber = 0; chanNumber < CSL_DAT_CHAN_MAX; chanNumber++)
	{
	    printf("\nTest for DAT Channel No : %d ", chanNumber);

	    memset(srcDARAM, 0x0, sizeof(srcDARAM));

		myhDat = DAT_open((CSL_DATChanNum)chanNumber,
		             CSL_DAT_CHAN_PRIORITY_HIGH, &datChanObj, &status);
		if (myhDat == NULL)
		{
			printf("DAT_open() Failed \n");
			break ;
		}

		status = DAT_fill(myhDat, src, 2*BUFF_SIZE, (Uint32*)&hiFillValue);
		if (status != CSL_SOK)
		{
			printf("DAT_fill() Failed \n");
			break ;
		}

		status = DAT_wait(myhDat);
		if (status != CSL_SOK)
		{
			printf("DAT_wait() Failed \n");
			break ;
		}

		/* Verify the data */
		for(index = 1; index < BUFF_SIZE; index += 2)
		{
			if(srcDARAM[index] != hiFillValue)
			{
				printf("DAT_fill - Data verification failed\n");
				break;
			}
		}

		status = DAT_copy(myhDat, (src),
						(src + BUFF_SIZE), 2*BUFF_SIZE);
		if (status != CSL_SOK)
		{
			printf("DAT_copy() Failed \n");
			break ;
		}

		status = DAT_wait(myhDat);
		if (status != CSL_SOK)
		{
			printf("DAT_wait() Failed \n");
			break ;
		}

		/* Verify the data */
		for(index = 0; index < BUFF_SIZE; index++)
		{
			if(srcDARAM[index] != srcDARAM[index + BUFF_SIZE])
			{
				printf("DAT_copy - Data verification failed\n");
				break;
			}
		}

		status = DAT_fill(myhDat, (src + 2*BUFF_SIZE), 2 * BUFF_SIZE, &loFillValue);
		if (status != CSL_SOK)
		{
			printf("DAT_fill() Failed \n");
			break ;
		}

		status = DAT_wait(myhDat);
		if (status != CSL_SOK)
		{
			printf("DAT_wait() Failed \n");
			break ;
		}

		/* Verify the data */
		for(index = 1; index < BUFF_SIZE; index += 2)
		{
			if(srcDARAM[index + (2*BUFF_SIZE)] != loFillValue)
			{
				printf("DAT_fill - Data verification failed\n");
				break;
			}
		}

		status = DAT_close(myhDat);
		if (status != CSL_SOK)
		{
			printf("DAT_close() Failed \n");
			break ;
		}

		printf(" - Successful");
	}

	if(chanNumber == 16)
	{
		printf("\n\nCSL DAT Test Passed!!\n");
	}
	else
	{
		printf("\n\nCSL DAT Test Failed at Channel Number %d\n",chanNumber);
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

