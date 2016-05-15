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

/** @file csl_mmcsd_extAPIs_Example.c
 *
 *  @brief CSL MMCSD eMMC Card Speccific API tests for CSL MMCSD Module
 *
 *
 * \page    page12  MMCSD EXAMPLE DOCUMENTATION
 *
 * \section MMC7   MMCSD EXAMPLE7 - eMMC CARD SPECIFIC APIs TEST
 *
 * \subsection MMC7x    TEST DESCRIPTION:
 *		This test code verifies the functionality of the APIs specific to eMMC
 * card using the CSL MMCSD module. This Functionality test includes detecting,
 * initializing, configuring the eMMC card using CSL MMCSD module.
 *
 * During the test MMCSD module will be initialized and opened in polled mode.
 * The eMMC card specific APIs such as MMC_enableBootPartitionAccess(),
 * MMC_enableBootPartitionAccess(), MMC_getCardExtCsd(),
 * MMC_disableBootPartitionAccess(), MMC_bootPartitionEnable(),
 * MMC_bootPartitionDisable(), MMC_getBootPartitionSize(), MMC_setBootBusWidth
 * and MMC_setEMMCBusWidth() are tested. For testing each of these APIs,
 * we use MMC_getCardExtCsd() API to verify whether the configurations are set
 * or not.
 *
 * Maximum value of the clock at which memory data transaction takes place
 * can be controlled using the macros 'CSL_SD_CLOCK_MAX_KHZ' and
 * 'CSL_MMC_CLOCK_MAX_KHZ' for SD and MMC cards respectively.
 * Depending on the clock at which CPU is running, memory clock will be
 * configured to the possible value that is nearest to the value defined
 * by these macros. memory clock will be configured to a value less than
 * or equal to but not greater than the value defined by these macros.
 * Changing these macro values will automatically change the memory clock
 * divider value.
 * memory clock will be generated from the system clock based on equation
 *
 * @verbatim
   memory clock = (system clock) / (2 * (CLKRT + 1)
      - memory clock is clock for the memory card
      - system clock is clock at which CPU us running
      - CLKRT is value of clock rate configured in clock control register
   @endverbatim
 *
 * As per this equation memory clock that can be generated is always less than
 * or equal to half of the system clock value. Value of 'CLKRT' can range
 * from 0 to 255. Maximum and minimum memory clock values that can be generated
 * at a particular CPU frequency are limited by the minimum and maximum value
 * of the memory clock rate (CLKRT).
 *
 * NOTE: THIS TEST WORKS WITH ONLY eMMC CARD.
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5515 AND
 * C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 *
 * \subsection MMC7y    TEST PROCEDURE:
 *  @li Insert "eMMC" card into the MMC/SD socket(J9) on the C5515 EVM
 *  or J16 on the C5517 EVM
 *  @li Open the CCS and connect the target (C5515/C5517 EVM)
 *  @li Open the project "CSL_MMCSD_extAPIs_Example_Out.pjt" and build it
 *  @li Load the program on to the target
 *  @li Set the PLL frequency to 12.288MHz
 *  @li Run the program and observe the test result
 *  @li Repeat the test at the following PLL frequencies
 *      C5515: 60MHz and 100MHz
 *      C5517: 60MHz, 100MHz, 150MHz and 200MHz
 *  @li Repeat the test in Release mode
 *
 * \subsection MMC7z    TEST RESULT:
 *  @li All the CSL APIs should return success
 *
 */

/* ============================================================================
 * Revision History
 * ================
 * 24-Jan-2013 Created
 * ============================================================================
 */

#include <csl_mmcsd.h>
#include <csl_intc.h>
#include <csl_general.h>
#include <stdio.h>
#include "csl_sysctrl.h"
#include "csl_pll.h"

/* Macros used to calculate system clock from PLL configurations             */
#define CSL_PLL_DIV_000    (0)
#define CSL_PLL_DIV_001    (1u)
#define CSL_PLL_DIV_002    (2u)
#define CSL_PLL_DIV_003    (3u)
#define CSL_PLL_DIV_004    (4u)
#define CSL_PLL_DIV_005    (5u)
#define CSL_PLL_DIV_006    (6u)
#define CSL_PLL_DIV_007    (7u)
#define CSL_PLL_CLOCKIN    (32768u)

#define PLL_CNTL1        *(ioport volatile unsigned *)0x1C20
#define PLL_CNTL2        *(ioport volatile unsigned *)0x1C21
#define PLL_CNTL3        *(ioport volatile unsigned *)0x1C22
#define PLL_CNTL4        *(ioport volatile unsigned *)0x1C23
/*
 * Macro to define MMC clock maximum value in KHz. Depending on the clock
 * at which CPU is running, memory clock will be configured to the possible
 * value that is nearest to the value defined by this macro. MMC clock will
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
 * As per this equation mmc clock that can be generated is always less than
 * or equal to half of the system clock value. Value of 'CLKRT' can range
 * from 0 to 255. Maximum and minimum mmc clock values that can be generated
 * at a particular frequency are limited by the minimum and maximum value
 * of the memory clock rate (CLKRT).
 *
 * NOTE: MMC SPECIFICATION DEFINED MAXIMUM CLOCK VALUE IS 20MHZ FOR NORMAL MMC
 * CARDS. VALUE OF THE MACRO 'CSL_MMC_CLOCK_MAX_KHZ' SHOULD NOT CROSS THIS
 * MAXIMUM LIMITS.
 */
#define CSL_MMC_CLOCK_MAX_KHZ      (5000u)

/* MMCSD Data structures                                                     */
CSL_MMCControllerObj 	pMmcsdContObj;
CSL_MmcsdHandle 		mmcsdHandle;
CSL_MMCCardObj			mmcCardObj;
CSL_MMCCardIdObj 		mmcCardIdObj;
CSL_MMCCardCsdObj 		mmcCardCsdObj;
CSL_MMCCallBackObj      callBckFun;
CSL_MMCConfig 			mmcConfig;
CSL_MMCSetupNative		setupNative;

/* CSL DMA data structures */
CSL_DMA_Handle        dmaWrHandle;
CSL_DMA_Handle        dmaRdHandle;
CSL_DMA_ChannelObj    dmaWrChanObj;
CSL_DMA_ChannelObj    dmaRdChanObj;
CSL_DMA_Handle        dmaHandle;

CSL_MMCCardExtCsdObj    emmcCard_ExtCSD_Values;

/**
 *  \brief  Tests eMMC card operation in POLLED mode
 *
 *   This function configures the MMCSD module in POLLED mode and
 *   verifies the extended APIs supprted n=by eMMC Card. Function returns
 *   failure incase of no emmc card is detected.
 *
 *  \param  none
 *
 *  \return Test result
 */
CSL_Status CSL_emmcApisTest(void);

/**
 *  \brief  Function to initialize and configure SD card
 *
 *  \param  opMode   - Operating Mode of MMCSD; POLL/DMA
 *
 *  \return Test result
 */
CSL_Status configSdCard (CSL_MMCSDOpMode    opMode);

/**
 *  \brief    Function to calculate the memory clock rate
 *
 * This function computes the memory clock rate value depending on the
 * CPU frequency. This value is used as clock divider value for
 * calling the API MMC_sendOpCond(). Value of the clock rate computed
 * by this function will change depending on the system clock value
 * and MMC maximum clock value defined by macro 'CSL_MMC_CLOCK_MAX_KHZ'.
 * Minimum clock rate value returned by this function is 0 and
 * maximum clock rate value returned by this function is 255.
 * Clock derived using the clock rate returned by this API will be
 * the nearest value to 'CSL_MMC_CLOCK_MAX_KHZ'.
 *
 *  \param    none
 *
 *  \return   MMC clock rate value
 */
Uint16 computeClkRate(void);

/**
 *  \brief  main function
 *
 *   This function calls the eMMC test function and displays
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

	printf("\nCSL MMCSD - eMMC CARD EXTENDED APIs TEST!\n\n");

	status = CSL_emmcApisTest();
	if(status != CSL_SOK)
	{
		printf("\nCSL MMCSD - EMMC CARD EXTENDED APIs TEST FAILED!!\n");
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 2 --
   /////  Reseting PaSs_StAtE to 0 if error detected here.
        PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
   /////
	}
	else
	{
		printf("\nCSL MMCSD - eMMC CARD EXTENDED APIs TEST PASSED!!\n");
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
 *  \brief  Tests eMMC card operation in POLLED mode
 *
 *   This function configures the MMCSD module in POLLED mode and
 *   verifies the extended APIs supprted n=by eMMC Card. Function returns
 *   failure incase of no emmc card is detected.
 *
 *  \param  none
 *
 *  \return Test result
 */
CSL_Status CSL_emmcApisTest(void)
{
	CSL_Status	mmcStatus;
	Uint32      partitionSize;

	mmcStatus = configSdCard(CSL_MMCSD_OPMODE_POLLED);
	if(mmcStatus != CSL_SOK)
	{
		printf("API: configSdCard Failed\n");
		return(mmcStatus);
	}

    mmcStatus = MMC_getCardExtCsd(mmcsdHandle, &emmcCard_ExtCSD_Values);
    if(mmcStatus != CSL_SOK)
    {
        printf("MMC_getCardExtCsd Failed\n");
        exit(mmcStatus);
    }

    if (emmcCard_ExtCSD_Values.extCsdVer < 2)
    {
        printf ("eMMC Card Not Detected\n");
        return (-1);
    }
    else
    {
        printf("eMMC Card Detected!\n\n");
    }

	/* Enable access for boot partition 1 */
    mmcStatus = MMC_enableBootPartitionAccess(mmcsdHandle, 1);
    if(mmcStatus != CSL_SOK)
    {
        printf("MMC_enableBootPartitionAccess Failed\n");
        exit(mmcStatus);
    }

	mmcStatus = MMC_getCardExtCsd(mmcsdHandle, &emmcCard_ExtCSD_Values);
	if(mmcStatus != CSL_SOK)
    {
        printf("MMC_getCardExtCsd Failed\n");
        exit(mmcStatus);
    }

	if (emmcCard_ExtCSD_Values.bootCfg != 1)
	{
		printf("MMC_enableBootPartitionAccess Failed, Partition Access not enabled\n");
        exit(mmcStatus);
	}

	printf("MMC_enableBootPartitionAccess API Passed\n\n");

	/* Disable access for boot partition 1 */
    mmcStatus = MMC_disableBootPartitionAccess(mmcsdHandle);
    if(mmcStatus != CSL_SOK)
    {
        printf("MMC_disableBootPartitionAccess Failed\n");
        exit(mmcStatus);
    }

	mmcStatus = MMC_getCardExtCsd(mmcsdHandle, &emmcCard_ExtCSD_Values);
	if(mmcStatus != CSL_SOK)
    {
        printf("MMC_getCardExtCsd Failed\n");
        exit(mmcStatus);
    }

	if (emmcCard_ExtCSD_Values.bootCfg != 0)
	{
		printf("MMC_disableBootPartitionAccess Failed, Partition Access not disabled\n");
        exit(mmcStatus);
	}

	printf("MMC_disableBootPartitionAccess API Passed\n\n");

	/* Enable boot partition 1 */
    mmcStatus = MMC_bootPartitionEnable(mmcsdHandle, 1, 0);
    if(mmcStatus != CSL_SOK)
    {
        printf("MMC_bootPartitionEnable Failed\n");
        exit(mmcStatus);
    }

	mmcStatus = MMC_getCardExtCsd(mmcsdHandle, &emmcCard_ExtCSD_Values);
	if(mmcStatus != CSL_SOK)
    {
        printf("MMC_getCardExtCsd Failed\n");
        exit(mmcStatus);
    }

	if (emmcCard_ExtCSD_Values.bootCfg != 8)
	{
		printf("MMC_bootPartitionEnable Failed, Partition Access not enabled\n");
        exit(mmcStatus);
	}

	printf("MMC_bootPartitionEnable API Passed\n\n");

	/* Disable boot partition 1 */
    mmcStatus = MMC_bootPartitionDisable(mmcsdHandle);
    if(mmcStatus != CSL_SOK)
    {
        printf("MMC_bootPartitionDisable Failed\n");
        exit(mmcStatus);
    }

	mmcStatus = MMC_getCardExtCsd(mmcsdHandle, &emmcCard_ExtCSD_Values);
	if(mmcStatus != CSL_SOK)
    {
        printf("MMC_getCardExtCsd Failed\n");
        exit(mmcStatus);
    }

	if (emmcCard_ExtCSD_Values.bootCfg != 0)
	{
		printf("MMC_bootPartitionDisable Failed, Partition Access not enabled\n");
        exit(mmcStatus);
	}

	printf("MMC_bootPartitionDisable API Passed\n\n");

	partitionSize = MMC_getBootPartitionSize(mmcsdHandle);
	if(partitionSize == 0)
    {
        printf("MC_getBootPartitionSize Failed\n");
        exit(mmcStatus);
    }

    printf("MMC_getBootPartitionSize API Passed\n");
    printf("Boot Partition size is: %ld\n\n", partitionSize);

	mmcStatus = MMC_setBootBusWidth(mmcsdHandle, (CSL_MmcsdBootBusWidth)0, 1);
	if(mmcStatus != CSL_SOK)
	{
		printf("API: MMC_setBootBusWidth Failed\n");
		return(mmcStatus);
	}

	mmcStatus = MMC_getCardExtCsd(mmcsdHandle, &emmcCard_ExtCSD_Values);
	if(mmcStatus != CSL_SOK)
    {
        printf("MMC_getCardExtCsd Failed\n");
        exit(mmcStatus);
    }

    printf("MMC_setBootBusWidth API Passed\n");
    printf("Boot Bus Width is: %d\n\n", emmcCard_ExtCSD_Values.bootBusWidth);

	mmcStatus = MMC_setEMMCBusWidth(mmcsdHandle, 1);
	if(mmcStatus != CSL_SOK)
	{
		printf("API: MMC_setEMMCBusWidth Failed\n");
		return(mmcStatus);
	}

	mmcStatus = MMC_getCardExtCsd(mmcsdHandle, &emmcCard_ExtCSD_Values);
	if(mmcStatus != CSL_SOK)
    {
        printf("MMC_getCardExtCsd Failed\n");
        exit(mmcStatus);
    }

    printf("MMC_setEMMCBusWidth API Passed\n");
    printf("Boot Bus Width is: %d\n\n", emmcCard_ExtCSD_Values.busWidthMode);

#if 0
	/* Enable boot mode */
    mmcStatus = MMC_enableBootMode(mmcsdHandle);
    if(mmcStatus != CSL_SOK)
    {
        printf("MMC_enableBootMode Failed\n");
        exit(mmcStatus);
    }

	mmcStatus = MMC_getCardExtCsd(mmcsdHandle, &emmcCard_ExtCSD_Values);
	if(mmcStatus != CSL_SOK)
    {
        printf("MMC_getCardExtCsd Failed\n");
        exit(mmcStatus);
    }

	if (emmcCard_ExtCSD_Values.bootCfg != 8)
	{
		printf("MMC_enableBootMode Failed, Partition Access not enabled\n");
        exit(mmcStatus);
	}

	printf("MMC_enableBootMode API Passed\n\n");
#endif

	/* Close the MMCSD module */
	mmcStatus = MMC_close(mmcsdHandle);
	if(mmcStatus != CSL_SOK)
	{
		printf("API: MMC_close Failed\n");
		return(mmcStatus);
	}
	else
	{
		printf("API: MMC_close Successful\n");
	}

	return(CSL_SOK);
}


/**
 *  \brief    Function to calculate the memory clock rate
 *
 * This function computes the memory clock rate value depending on the
 * CPU frequency. This value is used as clock divider value for
 * calling the API MMC_sendOpCond(). Value of the clock rate computed
 * by this function will change depending on the system clock value
 * and MMC maximum clock value defined by macro 'CSL_MMC_CLOCK_MAX_KHZ'.
 * Minimum clock rate value returned by this function is 0 and
 * maximum clock rate value returned by this function is 255.
 * Clock derived using the clock rate returned by this API will be
 * the nearest value to 'CSL_MMC_CLOCK_MAX_KHZ'.
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
	memMaxClk = CSL_MMC_CLOCK_MAX_KHZ;
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
             * 'CSL_MMC_CLOCK_MAX_KHZ'.
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
			 * 'CSL_MMC_CLOCK_MAX_KHZ' is less than the minimum possible
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

/**
 *  \brief  Function to initialize and configure SD card
 *
 *  \param  opMode   - Operating Mode of MMCSD; POLL/DMA
 *
 *  \return Test result
 */
CSL_Status configSdCard (CSL_MMCSDOpMode    opMode)
{
	CSL_Status     status;
	Uint16		   actCard;
	Uint16         clockDiv;
	CSL_CardType   cardType;

	cardType  = CSL_CARD_NONE;

	/* Get the clock divider value for the current CPU frequency */
	clockDiv = computeClkRate();

	/* Initialize MMCSD CSL module */
	status = MMC_init();

    status = SYS_setEBSR(CSL_EBSR_FIELD_SP0MODE,
                         CSL_EBSR_SP0MODE_0);
    status |= SYS_setEBSR(CSL_EBSR_FIELD_SP1MODE,
                         CSL_EBSR_SP1MODE_0);
    if(CSL_SOK != status)
    {
        printf("SYS_setEBSR failed\n");
        return (status);
    }

	/* Open MMCSD CSL module */
#ifdef C5515_EZDSP
	mmcsdHandle = MMC_open(&pMmcsdContObj, CSL_MMCSD1_INST,
						   opMode, &status);
#else
	mmcsdHandle = MMC_open(&pMmcsdContObj, CSL_MMCSD0_INST,
						   opMode, &status);
#endif
	if(mmcsdHandle == NULL)
	{
		printf("MMC_open Failed\n");
		return (status);
	}

	/* Configure the DMA in case of operating mode is set to DMA */
	if(opMode == CSL_MMCSD_OPMODE_DMA)
	{
		/* Initialize Dma */
		status = DMA_init();
		if (status != CSL_SOK)
		{
			printf("DMA_init Failed!\n");
			return(status);
		}

		/* Open Dma channel for MMCSD write */
		dmaWrHandle = DMA_open(CSL_DMA_CHAN0, &dmaWrChanObj, &status);
		if((dmaWrHandle == NULL) || (status != CSL_SOK))
		{
			printf("DMA_open for MMCSD Write Failed!\n");
			return(status);
		}

		/* Open Dma channel for MMCSD read */
		dmaRdHandle = DMA_open(CSL_DMA_CHAN1, &dmaRdChanObj, &status);
		if((dmaRdHandle == NULL) || (status != CSL_SOK))
		{
			printf("DMA_open for MMCSD Read Failed!\n");
			return(status);
		}

		/* Set the DMA handle for eMMC read */
		status = MMC_setDmaHandle(mmcsdHandle, dmaWrHandle, dmaRdHandle);
		if(status != CSL_SOK)
		{
			printf("API: MMC_setDmaHandle for MMCSD Failed\n");
			return(status);
		}
	}

	/* Reset the SD card */
	status = MMC_sendGoIdle(mmcsdHandle);
	if(status != CSL_SOK)
	{
		printf("MMC_sendGoIdle Failed\n");
		return (status);
	}

	/* Check for the card */
    status = MMC_selectCard(mmcsdHandle, &mmcCardObj);
	if((status == CSL_ESYS_BADHANDLE) ||
	   (status == CSL_ESYS_INVPARAMS))
	{
		printf("MMC_selectCard Failed\n");
		return (status);
	}

	/* Verify whether the eMMC card is detected or not */
	if(mmcCardObj.cardType == CSL_MMC_CARD)
	{
		cardType = CSL_MMC_CARD;

		/* Send the eMMC card identification Data */
		status = MMC_sendAllCID(mmcsdHandle, &mmcCardIdObj);
		if(status != CSL_SOK)
		{
			printf("API: MMC_sendAllCID Failed\n");
			return(status);
		}

		/* Set the eMMC Relative Card Address */
		status = MMC_setRca(mmcsdHandle, &mmcCardObj, 0x0001);
		if(status != CSL_SOK)
		{
			printf("API: MMC_setRca Failed\n");
			return(status);
		}

		/* Read the eMMC Card Specific Data */
		status = MMC_getCardCsd(mmcsdHandle, &mmcCardCsdObj);
		if(status != CSL_SOK)
		{
			printf("API: MMC_getCardCsd Failed\n");
			return(status);
		}

		/* Get the clock divider value for the current CPU frequency */
		clockDiv = computeClkRate();
	}
	else
	{
		printf("No eMMC Card detected\n");
		printf("Please Insert eMMC Card\n");
		return (CSL_ESYS_FAIL);
	}

	/* Set the card type in internal data structures */
	status = MMC_setCardType(&mmcCardObj, cardType);
	if(status != CSL_SOK)
	{
		printf("MMC_setCardType Failed\n");
		return (status);
	}

	/* Set the card pointer in internal data structures */
	status = MMC_setCardPtr(mmcsdHandle, &mmcCardObj);
	if(status != CSL_SOK)
	{
		printf("MMC_setCardPtr Failed\n");
		return (status);
	}

	/* Get the number of cards */
	status = MMC_getNumberOfCards(mmcsdHandle, &actCard);
	if(status != CSL_SOK)
	{
		printf("MMC_getNumberOfCards Failed\n");
		return (status);
	}

	/* Set clock for read-write access */
    status = MMC_sendOpCond(mmcsdHandle, clockDiv);
	if(status != CSL_SOK)
	{
		printf("MMC_sendOpCond Failed\n");
		return (status);
	}

	/* Set Endian mode for read and write operations */
  	status = MMC_setEndianMode(mmcsdHandle, CSL_MMCSD_ENDIAN_LITTLE,
  	                           CSL_MMCSD_ENDIAN_LITTLE);
	if(status != CSL_SOK)
	{
		printf("MMC_setEndianMode Failed\n");
		return(status);
	}

	/* Set block length for the memory card
	 * For high capacity cards setting the block length will have
	 * no effect
	 */
	status = MMC_setBlockLength(mmcsdHandle, CSL_MMCSD_BLOCK_LENGTH);
	if(status != CSL_SOK)
	{
		printf("MMC_setBlockLength Failed\n");
		return(status);
	}

	return (status);
}

