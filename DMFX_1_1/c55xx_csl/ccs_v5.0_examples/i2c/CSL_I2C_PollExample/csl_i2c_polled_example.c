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

/** @file csl_i2c_polled_example.c
 *
 *  @brief I2C functional layer polled mode example source file
 *
 *
 * \page    page5  I2C EXAMPLE DOCUMENTATION
 *
 * \section I2C1   I2C EXAMPLE1 - POLL MODE TEST
 *
 * \subsection I2C1x    TEST DESCRIPTION:
 *		This test verifies the operation of CSL I2C module in polled mode.
 *
 * During the test 64 bytes of data is written to EEPROM page and is read back
 * using the CSL I2C APIs. I2C module will be configured to master mode, 7-bit
 * addressing and 10KHz bus frequency using I2C_setup() API. EEPROM page with
 * address 0x0000 is written using the I2C_write() API. Data buffer 'gI2cWrBuf'
 * is used to store the data to be written to EEPROM page. Address of the page
 * to be written is stored in the first two bytes of the I2C write buffer.
 * So the data length passed to the write API should be 66 bytes. Address of
 * the slave device (0x50 for EEPROM) should be passed to the write API.
 *
 * Reading the data from the EEPROM page will be done in two steps. In first
 * step address of the EEPROM page to be read is written using I2C_write API.
 * This operation will be same as data write operation except that the length
 * of the data will be 2 bytes. After writing the EEPROM page address data
 * should be read from that EEPROM page using I2C_read() API. Data read from
 * the EEPROM page is stored in the buffer 'gI2cRdBuf'. Slave device address
 * and data length to be read should be passed to the I2C_read() API. Here the
 * data length should be 64 bytes. After successful completion of the write and
 * read operations data buffers are compared to verify the success of the
 * operations.
 *
 * I2C operates at all the system clock frequencies. Value of the system clock
 * at which CPU is running during the test should be passed to the I2C_setup()
 * function. This will be used to calculate I2C clock and prescaler values.
 * 'CSL_I2C_SYS_CLK' macro is defined with different system clock values.
 * Depending on the system clock value corresponding value definition of the
 * macro should be uncommented and re-build the project for proper operation
 * of the test.
 *
 * Both read and write APIs are synchronous and returns to the application only
 * after completing the data transfer. A small delay is required after each data
 * operation to allow the device to get ready for the next operation.
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5515 AND
 * C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 *
 * \subsection I2C1y    TEST PROCEDURE:
 *  @li Open the CCS and connect the target (C5515 EVM or C5517 EVM)
 *  @li Open the project "CSL_I2C_PollExample_Out.pjt" and build it
 *  @li Load the program on to the target
 *  @li Set the PLL frequency to 12.288MHz
 *  @li Run the program and observe the test result
 *  @li Repeat the test at the following PLL frequencies
 *      C5515: 60MHz and 100MHz
 *      C5517: 60MHz, 100MHz, 150MHz and 200MHz
 *  @li Repeat the test in Release mode
 *
 * \subsection I2C1z    TEST RESULT:
 *  @li All the CSL APIs should return success
 *  @li Read and write data buffer comparison should be successful.
 *
 * ============================================================================
 */

/* ============================================================================
 * Revision History
 * ================
 * 16-Oct-2008 Created
 * 10-Jul-2012 Added C5517 Compatibility
 * ============================================================================
 */

#include <csl_i2c.h>
#include <stdio.h>
#include <csl_general.h>
#include "csl_gpio.h"
#include "csl_ioport.h"

#define CSL_I2C_TEST_PASSED      (0)
#define CSL_I2C_TEST_FAILED      (1)

#define CSL_I2C_PAGE_SIZE        (32)
#define CSL_I2C_DATA_SIZE        (3*CSL_I2C_PAGE_SIZE)
#define CSL_EEPROM_ADDR_SIZE     (2)
#define CSL_I2C_OWN_ADDR         (0x2F)
//#define CSL_I2C_SYS_CLK          (12.228)
//#define CSL_I2C_SYS_CLK          (40)
//#define CSL_I2C_SYS_CLK          (60)
//#define CSL_I2C_SYS_CLK          (75)
#define CSL_I2C_SYS_CLK          (100)
#define CSL_I2C_BUS_FREQ         (400)
#define CSL_I2C_EEPROM_ADDR		 (0x50)
#define CSL_I2C_CODEC_ADDR		 (0x18)

Uint16  gI2cWrBuf[CSL_I2C_PAGE_SIZE + CSL_EEPROM_ADDR_SIZE];
Uint16  gI2cRdBuf[CSL_I2C_PAGE_SIZE];

CSL_I2cSetup     i2cSetup;
CSL_I2cConfig    i2cConfig;

/**
 *  \brief  Tests I2C polled mode operation
 *
 *  \param  none
 *
 *  \return Test result
 */
CSL_Status CSL_i2cPolledTest(void);

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
	CSL_Status    result;

	printf("I2C POLLED MODE TEST!\n\n");

	result =  CSL_i2cPolledTest();

	if(result == CSL_I2C_TEST_PASSED)
	{
		printf("\nI2C POLLED MODE TEST PASSED!!\n");
	}
	else
	{
		printf("\nI2C POLLED MODE TEST FAILED!!\n");
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
 *  \brief  Tests I2C polled mode operation
 *
 *  \param  none
 *
 *  \return Test result
 */
CSL_Status  CSL_i2cPolledTest(void)
{
	CSL_Status         status;
	CSL_Status         result;
	Uint16             startStop;
	volatile Uint16    looper;
    volatile int i;
    Uint32 gpioIoDir, gpioIoDat;

	result = CSL_I2C_TEST_FAILED;

    /* Clock gate all peripherals */
    CSL_SYSCTRL_REGS->PCGCR1 = 0x7FFF;
    CSL_SYSCTRL_REGS->PCGCR2 = 0x007F;
    /* SP0 Mode 2 (GP[5:0]) -- GPIO02/GPIO04 for debug  */
    CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_SP0MODE, MODE2);
    /* SP1 Mode 2 (GP[11:6]) */
    CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_SP1MODE, MODE2);
    /* PP Mode 1 (SPI, GPIO[17:12], UART, and I2S2) -- note this allows UART */
    CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_PPMODE, MODE1);
    gpioIoDir = (((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN0) |
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN1) |
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN2) |
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN3) |
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN4) |
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN5) |
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN6) |
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN7) |
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN8) |
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN9) |
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN10)|
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN11);

    CSL_FINS((*GPIO_DIR0_ADDR), GPIO_DIR0, gpioIoDir);
    CSL_FINS((*GPIO_DIR1_ADDR), GPIO_DIR1, gpioIoDir>>16);
    gpioIoDat = (((Uint32)CSL_GPIO_DIR_OUTPUT)<< CSL_GPIO_PIN0) |
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<< CSL_GPIO_PIN5) |
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<< CSL_GPIO_PIN11);
    CSL_FINS((*GPIO_DOUT0_ADDR), GPIO_DOUT0, gpioIoDat);
    CSL_FINS((*GPIO_DOUT1_ADDR), GPIO_DOUT1, gpioIoDat>>16);
    // disable all interrupts (IER0 and IER1)
    *(volatile ioport unsigned int *)(0x0000) = 0x0000;
    *(volatile ioport unsigned int *)(0x0045) = 0x0000;

    // clear all interrupts (IFR0 and IFR1)
    *(volatile ioport unsigned int *)(0x0001) = 0xFFFF;
    *(volatile ioport unsigned int *)(0x0046) = 0xFFFF;

    // enable all peripherials PCGCR1 PCGCR2
    *(volatile ioport unsigned int *)(0x1c02) = 0;
    *(volatile ioport unsigned int *)(0x1c03) = 0;		//0x0004

    // peripheral reset PSRCR PRCR
    *(volatile ioport unsigned int *)(0x1c04) = 0x0004;	//0x0020
    *(volatile ioport unsigned int *)(0x1c05) = 0x00BF;
    // some delay
    for (i=0; i<0xFFF; i++);

    // clear all interrupts (IFR0 and IFR1)
    *(volatile ioport unsigned int *)(0x0001) = 0xFFFF;
    *(volatile ioport unsigned int *)(0x0046) = 0xFFFF;

    /* Initialize I2C module */
	status = I2C_init(CSL_I2C0);
	if(status != CSL_SOK)
	{
		printf("I2C Init Failed!!\n");
		return(result);
	}

	/* Setup I2C module */
	i2cSetup.addrMode    = CSL_I2C_ADDR_7BIT;
	i2cSetup.bitCount    = CSL_I2C_BC_8BITS;
	i2cSetup.loopBack    = CSL_I2C_LOOPBACK_DISABLE;
	i2cSetup.freeMode    = CSL_I2C_FREEMODE_DISABLE;
	i2cSetup.repeatMode  = CSL_I2C_REPEATMODE_DISABLE;
	i2cSetup.ownAddr     = CSL_I2C_OWN_ADDR;
	i2cSetup.sysInputClk = CSL_I2C_SYS_CLK;
	i2cSetup.i2cBusFreq  = CSL_I2C_BUS_FREQ;
	startStop            = ((CSL_I2C_START) | (CSL_I2C_STOP));

	status = I2C_setup(&i2cSetup);
	if(status != CSL_SOK)
	{
		printf("I2C Setup Failed!!\n");
		return(result);
	}
	for(i = 0; i < CSL_I2C_DATA_SIZE/CSL_I2C_PAGE_SIZE; i++){
		printf("Page %d\n", i);

		/* Assign the EEPROM page address */
		gI2cWrBuf[0] = (CSL_I2C_PAGE_SIZE*i)>>8;
		gI2cWrBuf[1] = (CSL_I2C_PAGE_SIZE*i)&0xFF;

		for(looper = 0; looper < CSL_I2C_PAGE_SIZE; looper++)
		{
			gI2cWrBuf[looper + CSL_EEPROM_ADDR_SIZE] = i*CSL_I2C_PAGE_SIZE + looper;
			gI2cRdBuf[looper] = 0x0000;
		}


		/* Write data */
		status = I2C_write(gI2cWrBuf, (CSL_I2C_PAGE_SIZE + CSL_EEPROM_ADDR_SIZE),
									   CSL_I2C_EEPROM_ADDR, TRUE, startStop,
									   CSL_I2C_MAX_TIMEOUT);
		if(status != CSL_SOK)
		{
			printf("I2C Write Failed!!\n");
			return(result);
		}

		printf("I2C Write Complete\n");

		/* Give some delay */
		for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++){;}

		/* Read data */
		status = I2C_read(gI2cRdBuf, CSL_I2C_PAGE_SIZE, CSL_I2C_EEPROM_ADDR,
						  gI2cWrBuf, CSL_EEPROM_ADDR_SIZE, TRUE,
						  startStop, CSL_I2C_MAX_TIMEOUT, FALSE);
		if(status != CSL_SOK)
		{
			printf("I2C Read Failed!!\n");
			return(result);
		}

		printf("I2C Read Complete\n");

		/* Compare the buffers */
		for(looper = 0; looper < CSL_I2C_PAGE_SIZE; looper++)
		{
			if(gI2cWrBuf[looper + CSL_EEPROM_ADDR_SIZE] != gI2cRdBuf[looper])
			{
				printf("Read Write Buffers Does not Match!!\n");
				return(result);
			}
		}

		if(looper == CSL_I2C_PAGE_SIZE)
		{
			printf("Read Write Buffers Match!!\n");
		}
	}

	result = CSL_I2C_TEST_PASSED;
	return(result);
}


