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

/** @file csl_dma_SdramExample.c
 *
 *  @brief Test the SDRAM memory transfers using DMA in Polled Mode
 *
 *
 * \page    page2  DMA EXAMPLE DOCUMENTATION
 *
 * \section DMA5   DMA EXAMPLE1 - POLLED MODE TEST
 *
 * \subsection DMA5x  TEST DESCRIPTION:
 * 		This test verifies the memory transfers on SDRAM using the CSL DMA
 * module, by Polling on the transfer complete bit status.
 *
 * C5535/C5515/C5517 DSPs have four DMA Engines and each DMA engine has
 * four channels for the data transfers. Total 16 channels can be configured
 * and used for the data transfer simultaneously. DMA can be used to transfer
 * data with in the memory and between the memory and peripherals with out
 * having intervention of the CPU.
 *
 * During the test, memory is allocated for two buffers in SDRAM of
 * C5535/C5515/C5517 DSP using Linker Command File. Data in the
 * source buffer 'dmaSRCBuff' is copied into the destination buffer
 * 'dmaDESTBuff', using the DMA Engine 4. DMA is configured with proper source
 * and destination address and data length using DMA_config() API. DMA data
 * transfer is triggered using DMA_start() API. DMA transfer completion is
 * detected by polling on the DMA start bit using DMA_getStatus() API.
 * After the transfer completion, data in the source and destination buffer
 * is compared. The same test procedure is repeated on all the DMA channels of
 * DMA Engine 4 (Channel 12 to Channel 16).
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5535, C5515
 * AND C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 *
 * \subsection DMA5y TEST PROCEDURE:
 *  @li Open the CCS and connect the target (C5535/C5515 EVM or C5517 EVM)
 *  @li Open the project "CSL_DMA_SDRAM_Example.pjt" and build it
 *  @li Load the program on to the target
 *  @li Set the PLL frequency to 12.288MHz
 *  @li Run the program and observe the test result
 *  @li Repeat the test at the following PLL frequencies
 *      C5515: 60MHz and 100MHz
 *      C5517: 60MHz, 100MHz, 150MHz and 200MHz
 *  @li Repeat the test in Release mode
 *
 * \subsection DMA5z TEST RESULT:
 *  @li All the CSL APIs should return success
 *  @li DMA transfer should be successful and source and destination data should
 *      match on the DMA channel 12 to channel 16
 *
 * =============================================================================
 */

/* ============================================================================
 * Revision History
 * ================
 * 04-Feb-2013 Created
 * ============================================================================
 */

#include <stdio.h>
#include <csl_general.h>
#include "csl_dma.h"
#include "csl_gpio.h"
#include "csl_sysctrl.h"
#include "csl_pll.h"

#define CSL_DMA_BUFFER_SIZE    (1024u)
#define VALUE_TO_WRITE_TO_BUFFER (0x1234)

/** Declaration of the Source buffer, Memory for this buffer will be allocated
    on the SDRAM using the Linker Command File */
#pragma DATA_SECTION(dmaSRCBuff, "DMA_SRC_BUFFER")
Uint16 dmaSRCBuff[CSL_DMA_BUFFER_SIZE];

/** Declaration of the Desrtination buffer, Memory for this buffer will be
    allocated on the SDRAM using the Linker Command File */
#pragma DATA_SECTION(dmaDESTBuff, "DMA_DEST_BUFFER")
Uint16 dmaDESTBuff[CSL_DMA_BUFFER_SIZE];

CSL_DMA_Handle 		dmaHandle;
CSL_DMA_Config 		dmaConfig;

/**
 * \brief API to configure the GPIO pins, used to Initialize SDRAM
 *
 * \param NONE
 *
 * \return Error Status
 */
CSL_Status configure_gpio();

/**
 * \brief API Initialize the SDRAM
 *
 * \param NONE
 *
 * \return Error Status
 */
CSL_Status initialize_sdram();

   /////INSTRUMENTATION FOR BATCH TESTING -- Part 1 --
   /////  Define PaSs_StAtE variable for catching errors as program executes.
   /////  Define PaSs flag for holding final pass/fail result at program completion.
        volatile Int16 PaSs_StAtE = 0x0001; // Init to 1. Reset to 0 at any monitored execution error.
        volatile Int16 PaSs = 0x0000; // Init to 0.  Updated later with PaSs_StAtE when and if
   /////                                  program flow reaches expected exit point(s).
   /////
void main(void)
{
	CSL_DMA_ChannelObj  dmaObj;
	CSL_Status 			status;
	Uint16   			chanNumber;
	Uint16   			looper;

	printf("\nSDRAM Memory Transfer Using DMA TEST!\n\n");

	status = CSL_SOK;

	status = initialize_sdram();
	if (CSL_SOK != status)
	{
		printf("SDRAM Initialization Failed\n");
		exit (0);
	}
	else
	{
		printf("SDRAM Initialization Successful\n");
	}

#if (defined(CHIP_C5505_C5515) || defined(CHIP_C5504_C5514) || defined(CHIP_C5517))
	dmaConfig.pingPongMode = CSL_DMA_PING_PONG_DISABLE;
#endif

	dmaConfig.autoMode     = CSL_DMA_AUTORELOAD_DISABLE;
	dmaConfig.burstLen     = CSL_DMA_TXBURST_8WORD;
	dmaConfig.trigger      = CSL_DMA_SOFTWARE_TRIGGER;
	dmaConfig.dmaEvt       = CSL_DMA_EVT_NONE;
	dmaConfig.dmaInt       = CSL_DMA_INTERRUPT_DISABLE;
	dmaConfig.chanDir      = CSL_DMA_READ;
	dmaConfig.trfType      = CSL_DMA_TRANSFER_MEMORY;
	dmaConfig.dataLen      = CSL_DMA_BUFFER_SIZE * 2;
	dmaConfig.srcAddr      = (Uint32)dmaSRCBuff;
	dmaConfig.destAddr     = (Uint32)dmaDESTBuff;

    status = DMA_init();
    if (status != CSL_SOK)
    {
        printf("DMA_init() Failed \n");
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 2 --
   /////  Reseting PaSs_StAtE to 0 if error detected here.
        PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
   /////
    }
    else
    {
		/* Test SDRAM memory transfers with DMA Engine 4 */
		for(chanNumber = 12; chanNumber < CSL_DMA_CHAN_MAX; chanNumber++)
		{
			printf("\nSDRAM memory Transfers using DMA Channel No : %d \t", chanNumber);

			for(looper = 0; looper < CSL_DMA_BUFFER_SIZE; looper++)
			{
				dmaSRCBuff[looper]  = VALUE_TO_WRITE_TO_BUFFER + (chanNumber % 12);
				dmaDESTBuff[looper] = 0x0000;
			}

			/* Open the DMA channel */
			dmaHandle = DMA_open((CSL_DMAChanNum)chanNumber,&dmaObj, &status);
			if (dmaHandle == NULL)
			{
				printf("\nDMA_open() Failed \n");
				break;
			}

			/* Configure the DMA channel */
			status = DMA_config(dmaHandle, &dmaConfig);
			if (status != CSL_SOK)
			{
				printf("\nDMA_config() Failed \n");
				break;
			}

			/* Start the DMA Transfer */
			status = DMA_start(dmaHandle);
			if (status != CSL_SOK)
			{
				printf("\nDMA_start() Failed \n");
				break;
			}

			/* Poll for the Transfer Completion */
			while(DMA_getStatus(dmaHandle));

			/* Reset the DMA channel */
			status = DMA_reset(dmaHandle);
			if (status != CSL_SOK)
			{
				printf("\nDMA_close() Failed \n");
				break;
			}

			/* Close the DMA channel */
			status = DMA_close(dmaHandle);
			if (status != CSL_SOK)
			{
				printf("\nDMA_reset() Failed \n");
				break;
			}

			/* Compare the Source and Destination Buffers */
			for(looper = 0; looper < CSL_DMA_BUFFER_SIZE; looper++)
			{
				if ((VALUE_TO_WRITE_TO_BUFFER + (chanNumber % 12)) != dmaDESTBuff[looper])
				{
					printf("\nBuffer miss matched at position %d\n", looper);
					break;
				}
			}

			/* Print the comparison result */
			if(looper == CSL_DMA_BUFFER_SIZE)
			{
				printf("SUCCESSFUL");
			}
			else
			{
				printf("Test FAILED with Channel No: %d", chanNumber);
			}
		}

		if(chanNumber == 16)
		{
			printf("\n\n SDRAM Memory Transfer Using DMA TEST PASSED!!\n");
		}
		else
		{
			printf("\n\n SDRAM Memory Transfer Using DMA TEST FAILED!!\n");
	   /////INSTRUMENTATION FOR BATCH TESTING -- Part 2 --
	   /////  Reseting PaSs_StAtE to 0 if error detected here.
			PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
	   /////
		}
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
 * \brief API Initialize the SDRAM
 *
 * \param NONE
 *
 * \return Error Status
 */
CSL_Status initialize_sdram()
{
	CSL_Status        status;
	volatile int      looper;
	CSL_SysRegsOvly   sysRegs;
	CSL_EmifRegsOvly  emifRegs;
	Uint32            sysClock;

	sysRegs  = (CSL_SysRegsOvly)CSL_SYSCTRL_REGS;
	emifRegs = (CSL_EmifRegsOvly)CSL_EMIF_REGS;

	CSL_FINST(sysRegs->PCGCR1, SYS_PCGCR1_EMIFCG, ACTIVE);

	status = configure_gpio();
	if (CSL_SOK != status)
	{
		printf ("Configuring GPIO Failed\n");
		return (status);
	}

	sysRegs->CCR1 = 0x0001;

	for(looper = 0; looper < 20000; looper++);

	sysRegs->PSRCR = 0x0400;
	sysRegs->PRCR  = 0x0002;

	for(looper = 0; looper < 20000; looper++);

	sysRegs->ESCR = 0x0000;

	emifRegs->SDTIMR1 = 0x3510;

#if (defined(CHIP_C5517))
	emifRegs->SDTIMR2 = 0x4225;
#else
	emifRegs->RSVD8[0] = 0x4225;
#endif

	emifRegs->SDCR1 = 0x4720;
	emifRegs->SDCR2 = 0x0031;

	sysClock = getSysClk();
	sysClock /= 1000;

	/* EMIF Clock cannot be more than 100MHz, so setting the EMIF to operate at
	   half of the CPU clock */
	if (sysClock >= 100)
	{
		sysRegs->ECDR &= 0xFFFE;
	}

	for(looper = 0; looper < 2048; looper++);

	return (status);
}

/**
 * \brief API to configure the GPIO pins, used to Initialize SDRAM
 *
 * \param NONE
 *
 * \return Error Status
 */
CSL_Status configure_gpio()
{
	CSL_Status         status;
    CSL_GpioPinConfig  config;
    Uint16             writeVal;
	CSL_GpioObj        gpioObj;
	CSL_GpioObj        *hGpio;

	writeVal = 1;

	status = SYS_setEBSR(CSL_EBSR_FIELD_PPMODE,
	                     CSL_EBSR_PPMODE_2);
    if (CSL_SOK != status)
    {
        printf("SYS_setEBSR failed\n");
        return (status);
    }

    /* Open GPIO module */
    hGpio = GPIO_open(&gpioObj, &status);
    if((NULL == hGpio) || (CSL_SOK != status))
    {
        printf("GPIO_open failed\n");
        return (1);
    }

    /* Reset all the pins */
    GPIO_reset(hGpio);

    /* Configure GPIO pin 0 as output pin */
    config.pinNum    = CSL_GPIO_PIN0;
    config.direction = CSL_GPIO_DIR_OUTPUT;
    config.trigger   = CSL_GPIO_TRIG_CLEAR_EDGE;

    status = GPIO_configBit(hGpio, &config);
    if(CSL_SOK != status)
    {
        printf("GPIO_configBit failed\n");
        return (status);
    }

    /* Write 1 to output pin */
    status = GPIO_write(hGpio, CSL_GPIO_PIN0, writeVal);
    if(CSL_SOK != status)
    {
        printf("GPIO_write Failed\n");
        return (status);
    }

    /* Close the GPIO module */
    status = GPIO_close(hGpio);
    if(CSL_SOK != status)
    {
        printf("GPIO_close failed\n");
    }

	return (status);
}
