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

/** @file csl_mmcsd_SdCard_multiInstance_Example.c
 *
 *  @brief MMCSD example to test Multiple instances of the Secure Digital(SD)
 * card operation in POLL mode
 *
 *
 * \page    page13  MMCSD MULTIPLE INSTANCES EXAMPLE DOCUMENTATION
 *
 * \section MMC8   MMCSD EXAMPLE8 - MULTIPLE INSTANCES TEST
 *
 * \subsection MMC8x    TEST DESCRIPTION:
 *		This test code verifies the multiple instances functionality of the CSL
 * MMCSD module with Secure Digital(SD) card. This Functionality test includes
 * detecting, initializing, configuring, writing, reading the SD card using CSL
 * MMCSD module, for each instance of the MMCSD module supported on C5517 EVM.
 * All these steps are carried out in sequence by different CSL APIs, for each
 * instance of the MMCSD module on C5517 EVM. After the card detection phase
 * test code verifies whether the detected card is SD or not. Test code stops
 * execution and returns with error incase of card is not SD. After the
 * successful detection of the SD card, it is initialized and configured. Two
 * data buffers 'pWriteBuff' and 'pReadBuff' are used for SD card write and
 * read operations respectively, for each instance of the MMCSD module. SD card
 * write buffer is initialized to values 0 to 255 and is written to SD card
 * sector zero using MMC_write API. Same SD card sector is read using MMC_read
 * API and the data read is stored in the SD card read buffer. Data in the
 * buffers 'pWriteBuff' and 'pReadBuff' is compared to check whether SD card
 * read and write operations are successful or not. This test code uses POLLED
 * mode to read and write the SD card data.
 *
 * NOTE: THIS TEST IS INTENDED TO BE USED WITH SD CARD. IT FAILS WITH MMC.
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH ONLY CHIP VERSION C5517.
 * MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 *
 * \subsection MMC8y    TEST PROCEDURE:
 *  @li Insert "SD" cards into the MMC/SD socket(J16) and socket(J21) on the
 *  C5517 EVM
 *  @li Open the CCS and connect the target (C5517 EVM)
 *  @li Open the project "CSL_MMCSD_SdCard_MultiInstance_Example_Out.pjt" and build it
 *  @li Load the program on to the target
 *  @li Set the PLL frequency to 12.288MHz
 *  @li Run the program and observe the test result
 *  @li Repeat the test at the following PLL frequencies
 *      C5515: 60MHz and 100MHz
 *      C5517: 60MHz, 100MHz, 150MHz and 200MHz
 *      C5535 eZdsp: 60MHz and 100MHz
 *  @li Repeat the test in Release mode
 *
 * \subsection MMC8z    TEST RESULT:
 *  @li All the CSL APIs should return success
 *  @li Data in the read and write buffers should match, for all the MMCSD
 *  instances supported on the Platform
 *
 * ============================================================================
 */

/* ============================================================================
 * Revision History
 * ================
 * 08-Feb-2013 Created
 * ============================================================================
 */

#include <csl_mmcsd.h>
#include <csl_intc.h>
#include <csl_pll.h>
#include <csl_general.h>
#include <stdio.h>
#include "csl_sysctrl.h"

#define CSL_TEST_FAILED  (1)
#define CSL_TEST_SUCCESS (0)

#define NO_OF_MMCSD_INSTANCES (2)

/* SD card Buffer size in Bytes                                              */
#define BUFFER_MAX_SIZE    (512u)
/* SD card physical address with respect to sector number                    */
#define CARD_START_ADDR    (0x0)

/*
 * Macro to define SD card clock maximum value in KHz. Depending on the clock
 * at which CPU is running, SD clock will be configured to the possible
 * value that is nearest to the value defined by this macro. SD clock will
 * be configured to a value less than or equal to but not greater
 * than the value defined by this macro. Changing this macro value
 * will automatically change the memory clock divider value.
 * memory clock will be generated from the system clock based on equation
 *
 * memory clock = (system clock) / (2 * (CLKRT + 1)
 *    - memory clock is clock for the memory card
 *    - system clock is clock at which CPU us running
 *    - CLKRT is value of clock rate configured in clock control register
 *
 * As per this equation memory clock that can be generated is always less than
 * or equal to half of the system clock value. Value of 'CLKRT' can range
 * from 0 to 255. Maximum and minimum memory clock values that can be generated
 * at a particular CPU frequency are limited by the minimum and maximum value
 * of the memory clock rate (CLKRT).
 *
 * NOTE: SD SPECIFICATION DEFINED MAXIMUM CLOCK VALUE IS 25MHZ FOR NORMAL SD
 * CARDS AND IS 50MHZ FOR HIGH SPEED CARDS. VALUE OF THIS MACRO SHOULD NOT
 * CROSS THIS MAXIMUM LIMITS.
 */
#define CSL_SD_CLOCK_MAX_KHZ      (20000u)

/* MMCSD Data structures for all the instances of MMCSD */
CSL_MMCControllerObj 	pMmcsdContObj[NO_OF_MMCSD_INSTANCES];
CSL_MmcsdHandle 		mmcsdHandle[NO_OF_MMCSD_INSTANCES];
CSL_MMCCardObj			mmcCardObj[NO_OF_MMCSD_INSTANCES];
CSL_MMCCardIdObj 		sdCardIdObj[NO_OF_MMCSD_INSTANCES];
CSL_MMCCardCsdObj 		sdCardCsdObj[NO_OF_MMCSD_INSTANCES];

/* SD card Data buffers */
Uint16 pReadBuff[BUFFER_MAX_SIZE/2];
Uint16 pWriteBuff[BUFFER_MAX_SIZE/2];

/**
 *  \brief  Tests SD card operation in POLLED mode
 *
 *   This function tests the multiple instances of MMCSD module by configuring
 *   the MMCSD module in POLLED mode and verifies the operation with SD Card.
 *   Function returns failure incase of no SD card is detected.
 *
 *  \param  NONE
 *
 *  \return Test result
 */
CSL_Status CSL_sdCard_multiInstance_PollTest(void);

/**
 *  \brief    Function to calculate the memory clock rate
 *
 * This function computes the memory clock rate value depending on the
 * CPU frequency. This value is used as clock divider value for
 * calling the API MMC_sendOpCond(). Value of the clock rate computed
 * by this function will change depending on the system clock value
 * and SD card maximum clock value defined by macro 'CSL_SD_CLOCK_MAX_KHZ'.
 * Minimum clock rate value returned by this function is 0 and
 * maximum clock rate value returned by this function is 255.
 * Clock derived using the clock rate returned by this API will be
 * the nearest value to 'CSL_SD_CLOCK_MAX_KHZ'.
 *
 *  \param    none
 *
 *  \return   MMC clock rate value
 */
Uint16 computeClkRate(void);

CSL_Status ioExpander_Setup(void);
CSL_Status ioExpander_Read(Uint16 port, Uint16 pin, Uint16 *rValue);
CSL_Status ioExpander_Write(Uint16 port, Uint16 pin, Uint16 value);

/**
 *  \brief  main function
 *
 *   This function calls the SD test function and displays
 *   the test result
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
	CSL_Status     status;

	printf ("CSL MMCSD - SD CARD MULTIPLE INSTANCE TEST!\n\n");

	status = CSL_sdCard_multiInstance_PollTest();
	if (status != CSL_SOK)
	{
		printf ("\nCSL MMCSD - SD CARD POLL MODE TEST FAILED!!\n");
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 2 --
   /////  Reseting PaSs_StAtE to 0 if error detected here.
        PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
   /////
	}
	else
	{
		printf ("\nCSL MMCSD - SD CARD MULTIPLE INSTANCE TEST PASSED!!\n");
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
 *  \brief  Tests SD card operation in POLLED mode
 *
 *   This function tests the multiple instances of MMCSD module by configuring
 *   the MMCSD module in POLLED mode and verifies the operation with SD Card.
 *   Function returns failure incase of no SD card is detected.
 *
 *  \param  NONE
 *
 *  \return Test result
 */
CSL_Status CSL_sdCard_multiInstance_PollTest(void)
{
	CSL_Status	 mmcStatus;
	CSL_Status   status;
	Uint16		 count;
	Uint16       looper;
	Uint16		 actCard;
	Uint32       sectCount;
	Uint32       cardAddr;
	Uint16       clockDiv;
	Uint16       rca;
	Uint32       cardStatus;

	sectCount = 0;

#ifdef CHIP_C5517

    status = ioExpander_Setup();
	if (status != CSL_SOK)
    {
        printf ("ERROR: ioExpander_Setup Failed\n");
        exit(CSL_TEST_FAILED);
    }

    status = ioExpander_Write(1, 4, 1);  /* Enable SEL0_MMC1_MCSPI Mode */
	if (status != CSL_SOK)
    {
        printf ("ERROR: ioExpander_Write Failed\n");
        exit(CSL_TEST_FAILED);
    }

    status = ioExpander_Write(1, 5, 0);  /* Disable SEL1_MMC1_MCSPI Mode */
	if (status != CSL_SOK)
    {
        printf ("ERROR: ioExpander_Write Failed\n");
        exit(CSL_TEST_FAILED);
    }

	/* Initialize data buffers */
	for (count = 0; count < (BUFFER_MAX_SIZE/2); count++)
	{
	    pReadBuff[count]   = 0x0;
		pWriteBuff[count] = count;
	}

	/* Get the clock divider value for the current CPU frequency */
	clockDiv = computeClkRate();

	/* Initialize the CSL MMCSD module */
	mmcStatus = MMC_init();
	if (mmcStatus != CSL_SOK)
	{
		printf ("API: MMC_init Failed\n");
		return (mmcStatus);
	}

    mmcStatus = SYS_setEBSR(CSL_EBSR_FIELD_SP0MODE,
                            CSL_EBSR_SP0MODE_0);
    mmcStatus |= SYS_setEBSR(CSL_EBSR_FIELD_SP1MODE,
                             CSL_EBSR_SP1MODE_0);
    if (CSL_SOK != mmcStatus)
    {
        printf ("SYS_setEBSR failed\n");
        return (mmcStatus);
    }

	/* Open the MMCSD module in POLLED mode */
	for (looper = 0; looper < NO_OF_MMCSD_INSTANCES; looper++)
	{
		mmcsdHandle[looper] = MMC_open(&pMmcsdContObj[looper],
		                               (CSL_MmcsdInstId)looper,
		                               CSL_MMCSD_OPMODE_POLLED,
		                               &mmcStatus);
		if (mmcStatus != CSL_SOK)
		{
			printf ("API: MMC_open Failed for Instance %d\n", looper);
			return (mmcStatus);
		}
		else
		{
			printf ("API: MMC_open Successful for Instance %d\n", looper);
		}

		/* Send CMD0 to the card */
		mmcStatus = MMC_sendGoIdle(mmcsdHandle[looper]);
		if (mmcStatus != CSL_SOK)
		{
			printf ("API: MMC_sendGoIdle Failed for Instance %d\n", looper);
			return (mmcStatus);
		}

		/* Check for the card */
		mmcStatus = MMC_selectCard(mmcsdHandle[looper], &mmcCardObj[looper]);
		if ((mmcStatus == CSL_ESYS_BADHANDLE) ||
		   (mmcStatus == CSL_ESYS_INVPARAMS))
		{
			printf ("API: MMC_selectCard Failed for Instance %d\n", looper);
			return (mmcStatus);
		}

		/* Verify whether the SD card is detected or not */
		if (mmcCardObj[looper].cardType == CSL_SD_CARD)
		{
			printf ("INSTANCE %d: SD card Detected!\n", looper);

			/* Check if the card is high capacity card */
			if (mmcsdHandle[looper]->cardObj->sdHcDetected == TRUE)
			{
				printf ("SD card is High Capacity Card for Instance %d\n", looper);
				printf ("Memory Access will use Block Addressing for Instance %d\n\n", looper);

				/* For the SDHC card Block addressing will be used.
				   Sector address will be same as sector number */
				cardAddr = sectCount;
			}
			else
			{
				printf ("SD card is Standard Capacity Card for Instance %d\n", looper);
				printf ("Memory Access will use Byte Addressing for Instance %d\n\n", looper);

				/* For the SD card Byte addressing will be used.
				   Sector address will be product of  sector number
				   and sector size */
				cardAddr = (sectCount)*(CSL_MMCSD_BLOCK_LENGTH);
			}
		}
		else
		{
			/* Check if No card is inserted */
			if (mmcCardObj[looper].cardType == CSL_CARD_NONE)
			{
				printf ("INSTANCE %d: No Card Detected!\n", looper);
			}
			else
			{
				printf ("INSTANCE %d: SD card is not Detected!\n", looper);
			}

			printf ("INSTANCE %d: Please Insert SD card!!\n", looper);
			return (CSL_ESYS_FAIL);
		}

		/* Set the init clock */
		mmcStatus = MMC_sendOpCond(mmcsdHandle[looper], 70);
		if (mmcStatus != CSL_SOK)
		{
			printf ("API: MMC_sendOpCond Failed for Instance %d\n", looper);
			return (mmcStatus);
		}

		/* Send the card identification Data */
		mmcStatus = SD_sendAllCID(mmcsdHandle[looper], &sdCardIdObj[looper]);
		if (mmcStatus != CSL_SOK)
		{
			printf ("API: SD_sendAllCID Failed for Instance %d\n", looper);
			return (mmcStatus);
		}

		/* Set the Relative Card Address */
		mmcStatus = SD_sendRca(mmcsdHandle[looper], &mmcCardObj[looper], &rca);
		if (mmcStatus != CSL_SOK)
		{
			printf ("API: SD_sendRca Failed for Instance %d\n", looper);
			return (mmcStatus);
		}

		/* Read the SD Card Specific Data */
		mmcStatus = SD_getCardCsd(mmcsdHandle[looper], &sdCardCsdObj[looper]);
		if (mmcStatus != CSL_SOK)
		{
			printf ("API: SD_getCardCsd Failed for Instance %d\n", looper);
			return (mmcStatus);
		}

		/* Set bus width - Optional */
		mmcStatus = SD_setBusWidth(mmcsdHandle[looper], 1);
		if (mmcStatus != CSL_SOK)
		{
			printf ("API: SD_setBusWidth Failed for Instance %d\n", looper);
			return (mmcStatus);
		}

		/* Disable SD card pull-up resistors - Optional */
		mmcStatus = SD_configurePullup(mmcsdHandle[looper], 0);
		if (mmcStatus != CSL_SOK)
		{
			printf ("API: SD_configurePullup Failed for Instance %d\n", looper);
			return (mmcStatus);
		}

		/* Set the card type in internal data structures */
		mmcStatus = MMC_setCardType(&mmcCardObj[looper], mmcCardObj[looper].cardType);
		if (mmcStatus != CSL_SOK)
		{
			printf ("API: MMC_setCardType Failed for Instance %d\n", looper);
			return (mmcStatus);
		}

		/* Set the card pointer in internal data structures */
		mmcStatus = MMC_setCardPtr(mmcsdHandle[looper], &mmcCardObj[looper]);
		if (mmcStatus != CSL_SOK)
		{
			printf ("API: MMC_setCardPtr Failed for Instance %d\n", looper);
			return (mmcStatus);
		}

		/* Get the number of cards */
		mmcStatus = MMC_getNumberOfCards(mmcsdHandle[looper], &actCard);
		if (mmcStatus != CSL_SOK)
		{
			printf ("API: MMC_getNumberOfCards Failed for Instance %d\n", looper);
			return (mmcStatus);
		}

		/* Set clock for read-write access */
		mmcStatus = MMC_sendOpCond(mmcsdHandle[looper], clockDiv);
		if (mmcStatus != CSL_SOK)
		{
			printf ("API: MMC_sendOpCond Failed for Instance %d\n", looper);
			return (mmcStatus);
		}

		/* Set Endian mode for read and write operations */
		mmcStatus = MMC_setEndianMode(mmcsdHandle[looper], CSL_MMCSD_ENDIAN_LITTLE,
									  CSL_MMCSD_ENDIAN_LITTLE);
		if (mmcStatus != CSL_SOK)
		{
			printf ("API: MMC_setEndianMode Failed for Instance %d\n", looper);
			return (mmcStatus);
		}

		/* Set block length for the memory card
		 * For high capacity cards setting the block length will have
		 * no effect
		 */
		mmcStatus = MMC_setBlockLength(mmcsdHandle[looper], CSL_MMCSD_BLOCK_LENGTH);
		if (mmcStatus != CSL_SOK)
		{
			printf ("API: MMC_setBlockLength Failed for Instance %d\n", looper);
			return (mmcStatus);
		}

		/* Write data to the SD card */
		mmcStatus = MMC_write(mmcsdHandle[looper], cardAddr, BUFFER_MAX_SIZE, pWriteBuff);
		if (mmcStatus != CSL_SOK)
		{
			printf ("API: MMC_write Failed for Instance %d\n", looper);
			return (mmcStatus);
		}
		else
		{
			printf ("API: MMC_write Successful for Instance %d\n", looper);
		}

		/* Read data from the SD card */
		mmcStatus = MMC_read(mmcsdHandle[looper], cardAddr, BUFFER_MAX_SIZE, pReadBuff);
		if (mmcStatus != CSL_SOK)
		{
			printf ("API: MMC_read Failed for Instance %d\n", looper);
			return (mmcStatus);
		}
		else
		{
			printf ("API: MMC_read Successful for Instance %d\n", looper);
		}

		/* Get card stataus */
		mmcStatus = MMC_getCardStatus(mmcsdHandle[looper], &cardStatus);
		if (mmcStatus != CSL_SOK)
		{
			printf ("API: MMC_getCardStatus Failed for Instance %d\n", looper);
			return (mmcStatus);
		}

		/* Deselect the SD card */
		mmcStatus = MMC_deselectCard(mmcsdHandle[looper], &mmcCardObj[looper]);
		if (mmcStatus != CSL_SOK)
		{
			printf ("API: MMC_deselectCard Failed for Instance %d\n", looper);
			return (mmcStatus);
		}

		/* Clear the MMCSD card response registers */
		mmcStatus = MMC_clearResponse(mmcsdHandle[looper]);
		if (mmcStatus != CSL_SOK)
		{
			printf ("API: MMC_clearResponse Failed for Instance %d\n", looper);
			return (mmcStatus);
		}

		/* Send CMD0 to the SD card */
		mmcStatus = MMC_sendCmd(mmcsdHandle[looper], 0x00, 0x00, CSL_MMCSD_EVENT_EOFCMD);
		if (mmcStatus != CSL_SOK)
		{
			printf ("API: MMC_sendCmd Failed for Instance %d\n", looper);
			return (mmcStatus);
		}

		/* Compare the MMC read and write buffers */
		for (count = 0; count < (BUFFER_MAX_SIZE / 2); count++)
		{
			if (pReadBuff[count] != pWriteBuff[count])
			{
				printf ("\nBuffer miss matched at position %d for Instance %d\n", count, looper);
				return (CSL_ESYS_FAIL);
			}
		}

		printf ("\nSD Card Read & Write Buffer Matched for Instance %d\n\n", looper);

	}

	/* Close the MMCSD module */
	for (looper = 0; looper < NO_OF_MMCSD_INSTANCES; looper++)
	{
		mmcStatus = MMC_close(mmcsdHandle[looper]);
		if (mmcStatus != CSL_SOK)
		{
			printf ("API: MMC_close Failed for Instance %d\n", looper);
			return (mmcStatus);
		}
		else
		{
			printf ("API: MMC_close Successful for Instance %d\n", looper);
		}
	}


	return (CSL_SOK);
#else
	printf ("MMCSD Multiple Instance Example is not Supported on current Platform\n\n");

	return (CSL_TEST_FAILED);
#endif /* #ifdef CHIP_C5517 */
}

/**
 *  \brief    Function to calculate the memory clock rate
 *
 * This function computes the memory clock rate value depending on the
 * CPU frequency. This value is used as clock divider value for
 * calling the API MMC_sendOpCond(). Value of the clock rate computed
 * by this function will change depending on the system clock value
 * and MMC maximum clock value defined by macro 'CSL_SD_CLOCK_MAX_KHZ'.
 * Minimum clock rate value returned by this function is 0 and
 * maximum clock rate value returned by this function is 255.
 * Clock derived using the clock rate returned by this API will be
 * the nearest value to 'CSL_SD_CLOCK_MAX_KHZ'.
 *
 *  \param    none
 *
 *  \return   MMC clock rate value
 */
Uint16 computeClkRate(void)
{
	Uint32    sysClock;
	Uint32    remainder;
	Uint32    memMaxClk;
	Uint16    clkRate;

	sysClock  = 0;
	remainder = 0;
	memMaxClk = CSL_SD_CLOCK_MAX_KHZ;
	clkRate   = 0;

	/* Get the clock value at which CPU is running */
	sysClock = getSysClk();

	if (sysClock > memMaxClk)
	{
		if (memMaxClk != 0)
		{
			clkRate   = sysClock / memMaxClk;
			remainder = sysClock % memMaxClk;

            /*
             * If the remainder is not equal to 0, increment clock rate to make
             * sure that memory clock value is less than the value of
             * 'CSL_SD_CLOCK_MAX_KHZ'.
             */
			if (remainder != 0)
			{
				clkRate++;
			}

            /*
             * memory clock divider '(2 * (CLKRT + 1)' will always
             * be an even number. Increment the clock rate in case of
             * clock rate is not an even number
             */
			if (clkRate%2 != 0)
			{
				clkRate++;
			}

			/*
			 * AT this point 'clkRate' holds the value of (2 * (CLKRT + 1).
			 * Get the value of CLKRT.
			 */
			clkRate = clkRate/2;
			clkRate = clkRate - 1;

			/*
			 * If the clock rate is more than the maximum allowed clock rate
			 * set the value of clock rate to maximum value.
			 * This case will become true only when the value of
			 * 'CSL_SD_CLOCK_MAX_KHZ' is less than the minimum possible
			 * memory clock that can be generated at a particular CPU clock.
			 *
			 */
			if (clkRate > CSL_MMC_MAX_CLOCK_RATE)
			{
				clkRate = CSL_MMC_MAX_CLOCK_RATE;
			}
		}
		else
		{
			clkRate = CSL_MMC_MAX_CLOCK_RATE;
		}
	}

	return (clkRate);
}

