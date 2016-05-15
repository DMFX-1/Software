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

/** @file csl_i2c_intc_example.c
 *
 *  @brief I2C functional layer interrupt mode example source file
 *
 *
 * \page    page5  I2C EXAMPLE DOCUMENTATION
 *
 * \section I2C3   I2C EXAMPLE3 - INTERRUPT MODE TEST
 *
 * \subsection I2C3x    TEST DESCRIPTION:
 *		This test verifies the operation of CSL I2C module in interrupt mode.
 *
 * During the test 64 bytes of data is written to EEPROM page and is read back
 * using the CSL I2C APIs and I2C data callback functions. Interrupts are used
 * to indicate the device ready status for a byte of data transmit or receive
 * operation. CSL INTC module should be configured and I2C ISR should be
 * registered before starting the data transfer. I2C callback functions which
 * are called from the Interrupt Service Routine should be configured using
 * I2C_setCallback() API. I2C module will be configured to master mode, 7-bit
 * addressing and 10KHz bus frequency using I2C_config() API. I2C own address,
 * data length, clock values, data command should be sent to the I2C config API.
 * Data buffer 'gI2cWrBuf' is used to store the data to be written to EEPROM
 * page. Address of the page to be written is stored in the first two bytes of
 * the I2C write buffer. So the data length sent to the config API should be
 * 66 bytes. after configuring the I2C module, interrupts for I2C transmit and
 * No Acknowledgement error will be enabled and I2C data transfer will be
 * triggered by setting the start bit. I2C will start generating the transmit
 * interrupts. One byte of data will be copied to the I2C data transmit
 * register in the I2C transmit callback function for each interrupt generated.
 * After completing the transfer of 66 bytes I2C interrupts will be disabled.
 * This completes the EEPROM page write.
 *
 * Reading the data from the EEPROM page will be done in two steps. In first
 * step address of the EEPROM page to be read is written to the EEPROM.
 * This operation will be same as data write operation except that the length
 * of the data will be 2 bytes. After writing the EEPROM page address I2C will
 * be configured for data read using I2C_config() API. I2C receive ready
 * interrupt is enabled and I2C data reception is started by setting the start
 * bit. I2C starts generating read interrupts. I2C read callback function will
 * read one byte of data from I2C read register for each interrupt generated.
 * Data read from the EEPROM page is stored in the buffer 'gI2cRdBuf'. After
 * reading 64 bytes of data I2C interrupts will be disabled. This completes
 * I2C read operation. After successful completion of the write and read
 * operations data buffers are compared to verify the success of the operations.
 *
 * A small delay is required after each data operation to allow the device get
 * ready for next data transfer operation.
 *
 * NOTE: TEST SHOULD NOT BE RUN BY SINGLE STEPPING AFTER ENABLING THE I2C
 * INTERRUPTS. DOING SO WILL LEAD TO IMPROPER HANDLING OF THE INTERRUPTS
 * AND TEST FAILURE.
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5515 AND
 * C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 *
 * \subsection I2C3y   TEST PROCEDURE:
 *  @li Open the CCS and connect the target (C5515 EVM or C5517 EVM)
 *  @li Open the project "CSL_I2C_IntcExample_Out.pjt" and build it
 *  @li Load the program on to the target
 *  @li Set the PLL frequency to 12.288MHz
 *  @li Run the program and observe the test result
 *  @li Repeat the test at the following PLL frequencies
 *      C5515: 60MHz and 100MHz
 *      C5517: 60MHz, 100MHz, 150MHz and 200MHz
 *  @li Repeat the test in Release mode
 *
 * \subsection I2C3z   TEST RESULT:
 *  @li All the CSL APIs should return success
 *  @li Read and write buffer comparison should be successful.
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
#include "DMFX2.h"
#include "csl_i2c.h"
#include "csl_intc.h"
#include <csl_general.h>
#include "lcdST7565.h"
#include "DMFX2_CSL_BIOS_cfg.h"
#include "DMFX_debug.h"
#include "csl_i2c_intc.h"
#include "csl_sar_intc.h"
#include <clk.h>
#include "dsp_fx.h"

extern Uint16 FxCmd[FXCMD_SIZE];  // Initialized with a memset function in runtime
extern pI2cHandle    i2cHandle;
#if 0
extern void VECSTART(void);
#endif

#ifdef I2C_MASTER_TX
Uint16  gI2cWrBuf[CSL_I2C_DATA_SIZE + CSL_EEPROM_ADDR_SIZE];
#endif
#ifdef I2C_SLAVE_RX
Uint16  gI2cRdBuf[CSL_I2C_DATA_SIZE];
#endif

volatile Uint16    dataLength;
Uint16             i2cTxCount;
Uint16             i2cRxCount;
Uint16             i2cErrInTx;
Uint16             stopDetected = FALSE;

// I2c_Intc_MasterTx task executes forever
void I2c_Intc_MasterTx(void)
{
	char FxChanges = 0;
	Uint16 i = 0;
#if 0
	Uint16 semcnt = 0;
#endif

	/* Assign the EEPROM page address */
#ifdef STS_ENABLE
	STS_add(&STS_I2c_Intc_MasterTx, 0); // debug
#endif
#ifdef I2C_MASTER_TX
	gI2cWrBuf[0] = 0x0;
	gI2cWrBuf[1] = 0x0;
#endif
	/* Initialize data buffers */
	for(i = 0; i < CSL_I2C_DATA_SIZE; i++)
	{
#ifdef I2C_MASTER_TX
		gI2cWrBuf[i + CSL_EEPROM_ADDR_SIZE] = 0x0000;
#endif
#ifdef I2C_SLAVE_RX
		gI2cRdBuf[i] = 0x0000;
#endif
	}
	I2c_Intc_Init();
	LOG_printf(&trace, "I2c_Intc_MasterTx");
	while(1)
	{
        TSK_sleep(1000);		// A delay is required to let the I2C operation end
#if 0
		semcnt = SEM_count(&SEM_Menu);
		LOG_printf(&trace, "SEM_Menu pend count = %d", semcnt);
#endif
#ifdef STS_ENABLE
		STS_set(&STS_I2c_Intc_MasterTx, CLK_gethtime()); // debug
#endif
        //SEM_pend(&SEM_Menu, SYS_FOREVER);
		for(i = 0; i < CSL_I2C_DATA_SIZE; i++)
		{
			if(FxCmd[i] !=  gI2cWrBuf[i+2])
			{
				FxChanges = 1;
				gI2cWrBuf[i+2] = FxCmd[i];
			}
		}
		if(FxChanges)
		{
			FxChanges = 0;
			I2c_Intc_reInit();
			//Synchronize both DSPs
			// Wait for XF=0 on DSP-1
	        SEM_pend(&SEM_Int0, SYS_FOREVER);
			// Signal I2C ready to DSP-1 on INT0
	        CSL_FINST(CSL_CPU_REGS->ST1_55, CPU_ST1_55_XF, OFF);
	        for(i = 0; i < 500; i++);   // wait for interrupt and semaphore on DSP-1
			/* Set the start bit */
			CSL_I2C_SETSTART();
#if (DEBUG>=10)
			semcnt = SEM_count(&SEM_I2c_Intc);
			LOG_printf(&trace, "SEM_I2c_Intc pend count = %d", semcnt);
#endif
#ifdef STS_ENABLE
			STS_delta(&STS_I2c_Intc_MasterTx, CLK_gethtime()); // debug
#endif
			/* Wait for the I2C transfer to complete */
	        SEM_pend(&SEM_I2c_Intc, SYS_FOREVER);
	        TSK_sleep(1000);		// A delay is required to let the I2C operation end
			// Signal I2C ready to DSP-1 on INT0
	        CSL_FINST(CSL_CPU_REGS->ST1_55, CPU_ST1_55_XF, ON);
		    //IRQ_disable(I2C_EVENT);
		}
	}
}

/**
 *  \brief  Tests I2C interrupt mode operation
 *
 *  \param  none
 *
 *  \return Test result
 */
CSL_Status I2c_Intc_Init(void)
{
	CSL_Status         status;
	CSL_Status         result;
	volatile Uint16    looper;
	CSL_I2cConfig      i2cConfig;
	CSL_I2cConfig      i2cGetConfig;
#if 0
	CSL_IRQ_Config     config;
#endif
	CSL_I2cIsrAddr     i2cIsrAddr;

	result = CSL_I2C_TEST_FAILED;

	/* Initialize I2C module */
	status = I2C_init(CSL_I2C0);
	if(status != CSL_SOK)
	{
		LOG_printf(&trace, "I2C Init Failed!!\n");
		return(result);
	}

	/* Set the I2C call back function address */
    i2cIsrAddr.alAddr   = CSL_i2cAlCallback;
    i2cIsrAddr.nackAddr = CSL_i2cNackCallback;
    i2cIsrAddr.ardyAddr = CSL_i2cArdyCallback;
    i2cIsrAddr.rrdyAddr = CSL_i2cRxCallback;
    i2cIsrAddr.xrdyAddr = CSL_i2cTxCallback;
    i2cIsrAddr.scdAddr  = CSL_i2cScdCallback;
    i2cIsrAddr.aasAddr  = CSL_i2cAasCallback;

	status = I2C_setCallback(&i2cIsrAddr);
	if(status != CSL_SOK)
	{
		LOG_printf(&trace, "I2C Set callback Failed!!\n");
		return(result);
	}

	/* Configure I2C module for write */
	i2cConfig.icoar  = CSL_I2C_OWN_ADDR;		// CSL_I2C_ICOAR_DEFVAL
	i2cConfig.icimr  = CSL_I2C_ICIMR_DEFVAL;
	i2cConfig.icclkl = CSL_I2C_ICCLK_VAL;
	i2cConfig.icclkh = CSL_I2C_ICCLK_VAL;
	i2cConfig.iccnt  = CSL_I2C_DATA_SIZE + CSL_EEPROM_ADDR_SIZE;
	i2cConfig.icsar  = CSL_I2C_DEVICE_ADDR;		// CSL_I2C_ICSAR_DEFVAL
#ifdef I2C_MASTER_TX
	i2cConfig.icmdr  = CSL_I2C_ICMDR_WRITE_DEFVAL; // Master TX Write mode
#endif
#ifdef I2C_SLAVE_RX
	i2cConfig.icmdr  = 0x0820; 	 // Slave RX Read mode
#endif
	i2cConfig.icemdr = CSL_I2C_ICEMDR_DEFVAL;
	i2cConfig.icpsc  = CSL_I2C_ICPSC_DEFVAL;

	status = I2C_config(&i2cConfig);
	if(status != CSL_SOK)
	{
		LOG_printf(&trace, "I2C Config Failed!!\n");
		return(result);
	}

	/* Read the configured values using I2C_getConfig function */
	status = I2C_getConfig(&i2cGetConfig);
	if(status != CSL_SOK)
	{
	    LOG_printf(&trace, "I2C  Get Config Failed!!\n");
		return(result);
	}

	/* Verify the read configuration values */
	if((i2cConfig.icoar  != i2cGetConfig.icoar)  ||
	   (i2cConfig.icimr  != i2cGetConfig.icimr)  ||
	   (i2cConfig.icclkl != i2cGetConfig.icclkl) ||
	   (i2cConfig.icclkl != i2cGetConfig.icclkh) ||
	   (i2cConfig.iccnt  != i2cGetConfig.iccnt)  ||
	   (i2cConfig.icsar  != i2cGetConfig.icsar)  ||
	   (i2cConfig.icmdr  != i2cGetConfig.icmdr)  ||
	   (i2cConfig.icemdr != i2cGetConfig.icemdr) ||
	   (i2cConfig.icpsc  != i2cGetConfig.icpsc))
    {
	    LOG_printf(&trace, "I2C get config not matching with config values!!\n");
		return(result);
	}
#ifdef I2C_MASTER_TX
	i2cErrInTx = FALSE;
	i2cTxCount = 0;
#endif
#ifdef I2C_SLAVE_RX
	i2cRxCount = 0;
#endif
	dataLength = i2cConfig.iccnt;
	/* Enable I2C NACK Error Event */
	status = I2C_eventEnable(CSL_I2C_EVENT_NACK);
	if(status != CSL_SOK)
	{
		LOG_printf(&trace, "I2C Event enable Failed!!\n");
		return(result);
	}

	/* Enable I2C Tx Ready Event */
	status = I2C_eventEnable(CSL_I2C_EVENT_ICXRDY);
	if(status != CSL_SOK)
	{
		LOG_printf(&trace, "I2C Event enable Failed!!\n");
		return(result);
	}
	/* Enable I2C Interrupts */
	IRQ_clear(I2C_EVENT);
	IRQ_enable(I2C_EVENT);
	result = CSL_I2C_TEST_PASSED;
	return(result);
}

CSL_Status I2c_Intc_MasterTx_Test(void)
{
	CSL_Status         result;

	result = CSL_I2C_TEST_FAILED;
	/* Set the start bit */
	CSL_I2C_SETSTART();

	/* Wait for the I2C transfer to complete */
	while(dataLength != 0)
	{
		if(i2cErrInTx != FALSE)
		{
			LOG_printf(&trace, "I2C NACK ERROR during TX\n");
			return(result);
		}
	}
	result = CSL_I2C_TEST_PASSED;
	return(result);
}
CSL_Status  I2c_Intc_reInit(void)
{
	CSL_Status         result;

	CSL_I2C0_WRITEREG(ICMDR, CSL_I2C_ICMDR_WRITE_DEFVAL); // Master TX Write mode

#ifdef I2C_MASTER_TX
	i2cErrInTx = FALSE;
	i2cTxCount = 0;
#endif
#ifdef I2C_SLAVE_RX
	i2cRxCount = 0;
#endif
	dataLength = CSL_I2C_DATA_SIZE + CSL_EEPROM_ADDR_SIZE; //i2cConfig.iccnt;
	result = CSL_I2C_TEST_PASSED;
	return(result);
}
CSL_Status I2C_Intc_Deinit(void)
{
	CSL_Status         status;
	CSL_Status         result;
#if 0
	volatile Uint16    looper;
#endif

	result = CSL_I2C_TEST_FAILED;
	/* Disable I2C NACK Error Event */
	status = I2C_eventDisable(CSL_I2C_EVENT_NACK);
	if(status != CSL_SOK)
	{
		LOG_printf(&trace, "I2C Event disable Failed!!\n");
		return(result);
	}

	/* Disable I2C Tx Ready Event */
	status = I2C_eventDisable(CSL_I2C_EVENT_ICXRDY);
	if(status != CSL_SOK)
	{
		LOG_printf(&trace, "I2C Tx Ready Event disable Failed!!\n");
		return(result);
	}
	/* Disable CPU Interrupts */
	IRQ_globalDisable();

	/* Clear any pending interrupts */
	IRQ_clearAll();

	/* Disable all peripheral the interrupts */
	IRQ_disableAll();

#ifdef I2C_MASTER_TX
	LOG_printf(&trace, "I2C Write complete\n");
#endif
#ifdef I2C_SLAVE_RX
	LOG_printf(&trace, "I2C Read complete\n");
#endif
#if 0
	/* Give some delay */
	for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++){;}
	for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++){;}

	/* Configure the I2C module for writing EEPROM page address for read operation */
	i2cConfig.icoar  = CSL_I2C_ICOAR_DEFVAL;
	i2cConfig.icimr  = CSL_I2C_ICIMR_DEFVAL;
	i2cConfig.icclkl = CSL_I2C_ICCLK_VAL;
	i2cConfig.icclkh = CSL_I2C_ICCLK_VAL;
	i2cConfig.iccnt  = CSL_EEPROM_ADDR_SIZE;
	i2cConfig.icsar  = CSL_I2C_ICSAR_DEFVAL;
	i2cConfig.icmdr  = CSL_I2C_ICMDR_WRITE_DEFVAL;
	i2cConfig.icemdr = CSL_I2C_ICEMDR_DEFVAL;
	i2cConfig.icpsc  = CSL_I2C_ICPSC_DEFVAL;

	status = I2C_config(&i2cConfig);
	if(status != CSL_SOK)
	{
		LOG_printf(&trace, "I2C Config Failed!!\n");
		return(result);
	}

	/* Read the configured values using I2C_getConfig function */
	status = I2C_getConfig(&i2cGetConfig);
	if(status != CSL_SOK)
	{
	    LOG_printf(&trace, "I2C  Get Config Failed!!\n");
		return(result);
	}

	/* Verify the read configuration values */
	if((i2cConfig.icoar  != i2cGetConfig.icoar)  ||
	   (i2cConfig.icimr  != i2cGetConfig.icimr)  ||
	   (i2cConfig.icclkl != i2cGetConfig.icclkl) ||
	   (i2cConfig.icclkl != i2cGetConfig.icclkh) ||
	   (i2cConfig.iccnt  != i2cGetConfig.iccnt)  ||
	   (i2cConfig.icsar  != i2cGetConfig.icsar)  ||
	   (i2cConfig.icmdr  != i2cGetConfig.icmdr)  ||
	   (i2cConfig.icemdr != i2cGetConfig.icemdr) ||
	   (i2cConfig.icpsc  != i2cGetConfig.icpsc))
    {
	    LOG_printf(&trace, "I2C get config not matching with config values!!\n");
		return(result);
	}

	i2cErrInTx = FALSE;
	i2cTxCount = 0;
	dataLength = i2cConfig.iccnt;

	/* Enable I2C NACK Error Event */
	status = I2C_eventEnable(CSL_I2C_EVENT_NACK);
	if(status != CSL_SOK)
	{
		LOG_printf(&trace, "I2C Event enable Failed!!\n");
		return(result);
	}

	/* Enable I2C Tx Ready Event */
	status = I2C_eventEnable(CSL_I2C_EVENT_ICXRDY);
	if(status != CSL_SOK)
	{
		LOG_printf(&trace, "I2C Event enable Failed!!\n");
		return(result);
	}

	/* Set the start bit */
	CSL_I2C_SETSTART();

	/* Wait for the I2C transfer to complete */
	while(dataLength != 0)
	{
		if(i2cErrInTx != FALSE)
		{
			LOG_printf(&trace, "I2C NACK ERROR during TX\n");
			return(result);
		}
	}

	/* Disable I2C NACK Error Event */
	status = I2C_eventDisable(CSL_I2C_EVENT_NACK);
	if(status != CSL_SOK)
	{
		LOG_printf(&trace, "I2C Event disable Failed!!\n");
		return(result);
	}

	/* Disable I2C Tx Ready Event */
	status = I2C_eventDisable(CSL_I2C_EVENT_ICXRDY);
	if(status != CSL_SOK)
	{
		LOG_printf(&trace, "I2C Event disable Failed!!\n");
		return(result);
	}

	/* Give some delay */
	for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++){;}
	for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++){;}

	LOG_printf(&trace, "I2C Read complete\n");

	LOG_printf(&trace, "Master Tx Write Buffer Test End!!\n");
#endif
	result = CSL_I2C_TEST_PASSED;
	return(result);
}

/**
 *  \brief  I2C Arbitration loss callback
 *
 *  \param  None
 *
 *  \return None
 */
void CSL_i2cAlCallback(void)
{
	;
#if (DEBUG>=10)
	LOG_printf(&trace, "CSL_i2cAlCallback: I2C Arbitration loss callback");
#endif
}

/**
 *  \brief  I2C No acknowledgement callback
 *
 *  \param  None
 *
 *  \return None
 */
void CSL_i2cNackCallback(void)
{
	i2cErrInTx = TRUE;
#if DEBUG>=10
	LOG_printf(&trace, "CSL_i2cNackCallback: I2C No acknowledgement callback");
#endif
}

/**
 *  \brief  I2C Access ready callback
 *
 *  \param  None
 *
 *  \return None
 */
void CSL_i2cArdyCallback(void)
{
	;
#if (DEBUG>=10)
	LOG_printf(&trace, "CSL_i2cArdyCallback: I2C Access ready callback");
#endif
}

/**
 *  \brief  I2C Receive ready callback
 *
 *  \param  None
 *
 *  \return None
 */
void CSL_i2cRxCallback(void)
{
#ifdef I2C_SLAVE_RX
	gI2cRdBuf[i2cRxCount++] = CSL_I2C_0_REGS->ICDRR;
	if(--dataLength == 0)
		SEM_post(&SEM_I2c_Intc);
#if (DEBUG>=10)
	LOG_printf(&trace, "CSL_i2cRxCallback: dataLength = %d", dataLength);
#endif
#endif
}

/**
 *  \brief  I2C Transmit ready callback
 *
 *  \param  None
 *
 *  \return None
 */
void CSL_i2cTxCallback(void)
{
#ifdef I2C_MASTER_TX
	CSL_I2C_0_REGS->ICDXR = gI2cWrBuf[i2cTxCount++];
	if (--dataLength == 0) {
		LOG_printf(&trace, "post SEM_I2c_Intc");
		SEM_post(&SEM_I2c_Intc);
	}
#if (DEBUG>=10)
	LOG_printf(&trace, "CSL_i2cTxCallback: dataLength = %d", dataLength);
#endif
#endif
}

/**
 *  \brief  I2C Stop condition detected callback
 *
 *  \param  None
 *
 *  \return None
 */
void CSL_i2cScdCallback(void)
{
	stopDetected = TRUE;
#if (DEBUG>=10)
	LOG_printf(&trace, "CSL_i2cScdCallback: STOP DETECTED!!");
#endif
}

/**
 *  \brief  I2C Address as slave callback
 *
 *  \param  None
 *
 *  \return None
 */
void CSL_i2cAasCallback(void)
{
	;
#if (DEBUG>=10)
	LOG_printf(&trace, "CSL_i2cAasCallback: I2C Address as slave callback");
#endif
}

/**
 *  \brief  I2C interrupt service routine
 *
 *  \param  None
 *
 *  \return None
 */
void i2c_isr(void)
{
	Uint16    eventId;

	eventId = I2C_getEventId();

	if(eventId != 0)
	{
		i2cHandle->I2C_isrDispatchTable[eventId - 1]();
	}
	IRQ_clear(I2C_EVENT);
#if (DEBUG>=10)
	LOG_printf(&trace, "i2c_isr: eventid = %d", eventId);
#endif
}

void Int0_ISR(void)
{
	IRQ_clear(INT0_EVENT);
#if (DEBUG>=10)
	LOG_printf(&trace, "Int0_ISR");
#endif
	SEM_post(&SEM_Int0);
}
