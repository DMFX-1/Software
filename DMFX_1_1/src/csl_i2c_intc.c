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

#include "DMFX1.h"
#include "csl_i2c.h"
#include "csl_intc.h"
#include <csl_general.h>
#include "lcdST7565.h"
#include "DMFX1_CSL_BIOS_cfg.h"
#include "DMFX_debug.h"
#include <clk.h>
#include "dsp_fx.h"
#include "csl_i2c_intc.h"
#include "equalizer.h"

#define HYSTERESIS		   0x3 // 3dB

extern Uint16 FxCmd[FXCMD_SIZE];  // Initialized with a memset function in runtime
extern pI2cHandle    i2cHandle;
extern const Uint16  volume_table[];
#if 0
extern void VECSTART(void);
#endif

#ifdef I2C_MASTER_TX
Uint16  gI2cWrBuf[CSL_I2C_DATA_SIZE + CSL_EEPROM_ADDR_SIZE];
#endif
#ifdef I2C_SLAVE_RX
Uint16  gI2cRdBuf[CSL_I2C_DATA_SIZE + CSL_EEPROM_ADDR_SIZE];
#endif

volatile Uint16    dataLength;
Uint16             i2cTxCount;
Uint16             i2cRxCount;
Uint16             i2cErrInRx;
Uint16             stopDetected = FALSE;
Uint16 			   hp_vol = 0;
Uint16 			   lout_vol = 0;
Uint16 			   ana_gain = 0;
Uint16 			   compress = 0;
Uint16 			   equalize = 0;
Uint16			   distortion = 0;
Uint16 			   threshold;
Uint16 			   hold;
Uint16 			   att_dec;

// I2c_Intc_SlaveRx task executes forever
void I2c_Intc_SlaveRx(void)
{
	//char   FxChanges;
	Uint16 i = 0;
    Int16 gain;

#ifdef STS_ENABLE
	STS_add(&STS_I2c_Intc_SlaveRx, 0); // debug
#endif
	/* Assign the EEPROM page address */
#ifdef I2C_MASTER_TX
	gI2cWrBuf[0] = 0x0;
	gI2cWrBuf[1] = 0x0;
#endif
#ifdef STS_ENABLE
	STS_reset(&STS_I2c_Intc_SlaveRx); // debug
#endif
	/* Initialize data buffers */
	for(i = 0; i < CSL_I2C_DATA_SIZE+CSL_EEPROM_ADDR_SIZE; i++)
	{
#ifdef I2C_MASTER_TX
		gI2cWrBuf[i + CSL_EEPROM_ADDR_SIZE] = 0x0000;
#endif
#ifdef I2C_SLAVE_RX
		gI2cRdBuf[i] = 0x0000;
#endif
	}
	LOG_printf(&trace, "I2c_Intc_SlaveRx");
	while(1)
	{
	    TSK_sleep(1000);		// A delay is required to let the I2C operation end
		I2c_Intc_Init();
		//FxChanges = 0;
		I2c_Intc_reInit();
		//Synchronize both DSPs
		// Signal I2C ready to DSP-2 on INT0
        CSL_FINST(CSL_CPU_REGS->ST1_55, CPU_ST1_55_XF, OFF);
		// Wait for XF=0 on DSP-2
        SEM_pend(&SEM_Int0, SYS_FOREVER);
		/* Set the start bit */
		//CSL_I2C_SETSTART();

#ifdef STS_ENABLE
		STS_set(&STS_I2c_Intc_SlaveRx, CLK_gethtime()); // debug
#endif
		/* Wait for the I2C transfer to complete */
        SEM_pend(&SEM_I2c_Intc, SYS_FOREVER);
		// Signal I2C ready to DSP-2 on INT0
        CSL_FINST(CSL_CPU_REGS->ST1_55, CPU_ST1_55_XF, ON);
	    //IRQ_disable(I2C_EVENT);
		for(i = 0; i < CSL_I2C_DATA_SIZE; i++)
		{
			//FxCmd[i] = gI2cRdBuf[i+2];
			if(FxCmd[i] != gI2cRdBuf[i+2]) {
				//FxChanges = 1;
				FxCmd[i] = gI2cRdBuf[i+2];
#if (DEBUG>=10)
				LOG_printf(&trace, "I2c_Intc_SlaveRx Task: FxCmd[%d] = %d", i, FxCmd[i]);
#endif
			}
		}
		if((FxCmd[COMPRESSOR])||(FxCmd[COMPRESSOR] != compress))
		{
			compress = FxCmd[COMPRESSOR];
        	I2c_Codec_Init();
			DRC();
		}
		if((FxCmd[EQUALIZER])||(FxCmd[EQUALIZER] != equalize))
		{
			equalize = FxCmd[EQUALIZER];
        	I2c_Codec_Init();
			equalizer();
		}
#if 0
		if(FxCmd[DISTORTION] != distortion)
		{
			distortion = FxCmd[DISTORTION];
			Codec_Write(0x0,0x01); // Select page 1
			if (distortion)
			{
				if(FxCmd[TYPE] == FUZZ)
				{
					//adjust volume setting of left Line Out
					Codec_Write(0x3B,13);// gain: amplification: 0-29, Attenuation: 30-35, Mute=36
					//adjust volume setting of right Line Out
					Codec_Write(0x3C,13);
				}
				else
				{
					//adjust volume setting of left Line Out
					Codec_Write(0x3B,0);// gain: amplification: 0-29, Attenuation: 30-35, Mute=36
					//adjust volume setting of right Line Out
					Codec_Write(0x3C,0);
				}
			}
			else
			{
				//adjust volume setting of left Line Out
				Codec_Write(0x3B,45);// gain: amplification: 0-29, Attenuation: 30-35, Mute=36
				//adjust volume setting of right Line Out
				Codec_Write(0x3C,45);
			}
		}
#endif
		if (FxCmd[HP_VOL]!= hp_vol)
		{
    		hp_vol = FxCmd[HP_VOL];
        	gain = FxCmd[HP_VOL]/7;  // convert 0-255 to 0-36
        	gain -=6;
        	if (gain < -6) gain = -6;
        	else if (gain > 29) gain = 29;
        	gain &= 0x3F;
#if (DEBUG>=10)
        	LOG_printf(&trace, "Headphones Vol gain = %d", gain);
#endif
        	I2c_Codec_Init();
    		Codec_Write(0x0,0x01); // Select page 1
    		//adjust volume setting of left headphone
    		Codec_Write(0x10,gain);// gain: amplification: 0-29, Attenuation: 30-35, Mute=36
    		//adjust volume setting of right headphone
    		Codec_Write(0x11,gain);
    	}
		if (FxCmd[LOUT_VOL]!= lout_vol)
		{
    		lout_vol = FxCmd[LOUT_VOL];
        	gain = FxCmd[LOUT_VOL]/7;  // convert 0-255 to 0-36
        	gain -=6;
        	if (gain < -6) gain = -6;
        	else if (gain > 29) gain = 29;
        	gain &= 0x3F;
#if (DEBUG>=10)
        	LOG_printf(&trace, "LineOut Vol gain = %d", gain);
#endif
        	I2c_Codec_Init();
    		Codec_Write(0x0,0x01); // Select page 1
    		//adjust volume setting of left Line Out
    		Codec_Write(0x12,gain);// gain: amplification: 0-29, Attenuation: 30-35, Mute=36
    		//adjust volume setting of right Line Out
    		Codec_Write(0x13,gain);
    	}
		if (FxCmd[ANA_GAIN]!= ana_gain)
		{
    		ana_gain = FxCmd[ANA_GAIN];
        	gain = (FxCmd[ANA_GAIN]*3)>>3;  // convert 0-255 to 0-95
        	if (gain > 95) gain = 95;
#if (DEBUG>=10)
        	LOG_printf(&trace, "MICPGA analog gain = %d", gain/2);
#endif
        	//gain |= 0x80;
        	I2c_Codec_Init();
    		Codec_Write(0x0,0x01); // Select page 1
    		//adjust volume setting of left Line Out
    		Codec_Write(0x3B,gain);// gain: amplification: 0-29, Attenuation: 30-35, Mute=36
    		//adjust volume setting of right Line Out
    		Codec_Write(0x3C,gain);
    	}
#ifdef STS_ENABLE
		STS_delta(&STS_I2c_Intc_SlaveRx, CLK_gethtime()); // debug
#endif
#if (DEBUG>=10)
		LOG_printf(&trace, "I2c_Intc_SlaveRx Task: FxChanges = %d", i, FxChanges);
#endif
	}
}

CSL_Status DRC()
{
	Uint16 WrBuffer[2][25] = {
		{
			0x34,
			0x7F, 0xAB, 0x00, 0x00, 
			0x80, 0x55, 0x00, 0x00, 
			0x7F, 0x56, 0x00, 0x00,
			0x00, 0x11, 0x00, 0x00, 
			0x00, 0x11, 0x00, 0x00,
			0x7F, 0xDE, 0x00, 0x00, 
		},
		{
			0x34,
			0x7F, 0xF7, 0x00, 0x00,
			0x80, 0x09, 0x00, 0x00,
			0x7F, 0xEF, 0x00, 0x00,
			0x00, 0x11, 0x00, 0x00,
			0x00, 0x11, 0x00, 0x00,
			0x7F, 0xDE, 0x00, 0x00,
		}
	};
	volatile Uint16 looper;
	CSL_Status  status;
	
	if (!compress)
	{
	    Codec_Write(0x00, 0x2C); // select Page 44
	    Codec_Write(0x01, 0x05); // Enable DAC adaptive filtering

	    Codec_Write(0x00, 0x2E); // select Page 46 to write HPF, LPF coefficients
		/* HPF */
		/*
		 * HPF default values
		 */
		status = I2C_write( &WrBuffer[0][0],
							25,
							CSL_I2C_CODEC_ADDR,
							TRUE,
							((CSL_I2C_START) | (CSL_I2C_STOP)),
							CSL_I2C_MAX_TIMEOUT);
		if(status != CSL_SOK)
		{
			LOG_printf(&trace, "I2C Write Failed!!\n");
			return(status);
		}
	    Codec_Write(0x00, 0x2C); // select Page 44
	    Codec_Write(0x01, 0x01); // Disable DAC adaptive filtering
	}
	else
	{
	    Codec_Write(0x00, 0x2C); // select Page 44
	    Codec_Write(0x01, 0x05); // Enable DAC adaptive filtering
	    // convert 0-255 (8-bits) to 0-7 (3-bits) shifting right 5-bits and shift left 2-bits
	    // or shift left 3-bits and mask with 0x1 1100 = 0x1C
		threshold = 0x60|((FxCmd[CMP_LEVEL]>>3)&0x1C)|HYSTERESIS;
		//threshold stores AGC left/right channel enable (bits 6,5), threshold (bits 4-2) and hysteresis (bits 1-0)
		Codec_Write(0x44, threshold);
		// hold is stored in bits 6-3: convert 8-bits to 7-bits and mask with 0x0111 1000 = 0x78
		hold = (FxCmd[CMP_SUSTAIN]>>1)&0x78;
		Codec_Write(0x45,hold);
		// attack is stored in bits 7-4: mask with 0x1111 0000 = 0xF0
		att_dec = FxCmd[CMP_ATTACK]&0xF0;
		// decay is stored in bits 3-0: shift right 4-bits
		att_dec  |= FxCmd[CMP_DECAY]>>4;
		Codec_Write(0x46,att_dec); // Make DRC attack and Decay as required
	    Codec_Write(0x00, 0x2E); // select Page 46
		/* HPF */
		/*
		 * HPF default values
		 * 	C71 = 0x7FF700
		 *	C72 = 0x100900
		 *	C73 = 0x7FEF00
		 */
		status = I2C_write( &WrBuffer[1][0],
							25,
							CSL_I2C_CODEC_ADDR,
							TRUE,
							((CSL_I2C_START) | (CSL_I2C_STOP)),
							CSL_I2C_MAX_TIMEOUT);
		if(status != CSL_SOK)
		{
			LOG_printf(&trace, "I2C Write Failed!!\n");
			return(status);
		}

	}

	// write 1 to page register to select page 1 - prepare for next headset volume change
	Codec_Write(0, 1);
	return(status);
}

void AGC()
{
	Codec_Write( 0x00, 0x00);   // select page 0
#if 0
	Codec_Write( 0x3D, 0x02);   // activate PRB_R2
#endif
	Codec_Write( 0x57, 0xAF );  // Set Hysteresis = 4dB, Noise Threshold = -75dB
	Codec_Write( 0x58, 0x24 );  // Maximum Gain = 18dB
	Codec_Write( 0x59, 0x39 );  // Attack time = 20ms
	Codec_Write( 0x5A, 0xB8 );  // Decay time = 500ms
	Codec_Write( 0x5B, 0x0B );  // Noise Debounce time = 0ms
	Codec_Write( 0x5C, 0x00 );  // Signal Debounce time = 100ms
	Codec_Write( 0x56, 0xC0 );  // Enable Left AGC, gain = -5.5dB

	Codec_Write( 0x00, 0x00 );  // Select page 0
	Codec_Write( 0x5F, 0xAF );  // Set Hysteresis = 4dB, Noise Threshold = -75dB
	Codec_Write( 0x60, 0x24 );  // Maximum Gain = 18dB
	Codec_Write( 0x61, 0x39 );  // Attack time = 20ms
	Codec_Write( 0x62, 0xB8 );  // Decay time = 500ms
	Codec_Write( 0x63, 0x0B );  // Noise Debounce time = 0ms
	Codec_Write( 0x64, 0x00 );  // Signal Debounce time = 100ms
	Codec_Write( 0x5E, 0xC0 );  // Enable Right AGC, gain = -5.5dB

	// write 1 to page register to select page 1 - prepare for next headset volume change
	Codec_Write(0, 1);
}


CSL_Status Codec_Write(Uint16 regAddr, Uint16 regValue)
{
	CSL_Status  status;
	Uint16      WrBuffer[2];
	volatile Uint16    looper;

	WrBuffer[0] = regAddr;
	WrBuffer[1] = regValue;

	/* Write data */
	status = I2C_write( WrBuffer,
						2,
						CSL_I2C_CODEC_ADDR,
						TRUE,
						((CSL_I2C_START) | (CSL_I2C_STOP)),
						CSL_I2C_MAX_TIMEOUT);
	if(status != CSL_SOK)
	{
		LOG_printf(&trace, "I2C Write Failed!!\n");
		return(status);
	}
#if DEBUG>=9
	LOG_printf(&trace, "I2C Write Complete\n");
#endif
	/* Give some delay */
	for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++){;}
	return (status);
}

CSL_Status Codec_Read(Uint16 regAddr, Uint16 *data)
{
	CSL_Status status;

    regAddr = (regAddr & 0x00FF);

    status = I2C_read(data,
    		 	 	  1,
    		 	 	  CSL_I2C_CODEC_ADDR,
    		 	 	  &regAddr,
    		 	 	  1,
    		 	 	  TRUE,
    		 	 	  ((CSL_I2C_START) | (CSL_I2C_STOP)),
    		 	 	  CSL_I2C_MAX_TIMEOUT,
    		 	 	  FALSE
                    );
	if(status != CSL_SOK)
	{
		LOG_printf(&trace, "I2C Read Failed!!\n");
		return(status);
	}
    return status;
}

CSL_Status  I2c_Codec_Init(void)
{
	CSL_Status         status;
	CSL_Status         result;
	CSL_I2cSetup       i2cSetup;

	result = CSL_I2C_TEST_FAILED;

	/* Setup I2C module */
	i2cSetup.addrMode    = CSL_I2C_ADDR_7BIT;
	i2cSetup.bitCount    = CSL_I2C_BC_8BITS;
	i2cSetup.loopBack    = CSL_I2C_LOOPBACK_DISABLE;
	i2cSetup.freeMode    = CSL_I2C_FREEMODE_DISABLE;
	i2cSetup.repeatMode  = CSL_I2C_REPEATMODE_DISABLE;
	i2cSetup.ownAddr     = CSL_I2C_OWN_ADDR;
	i2cSetup.sysInputClk = CSL_I2C_SYS_CLK;
	i2cSetup.i2cBusFreq  = CSL_I2C_BUS_FREQ;

	status = I2C_setup(&i2cSetup);
	if(status != CSL_SOK)
	{
		LOG_printf(&trace, "I2C Setup Failed!!\n");
		return(result);
	}
	result = CSL_I2C_TEST_PASSED;
	return(result);
}

/**
 *  \brief  Tests I2C interrupt mode operation
 *
 *  \param  none
 *
 *  \return Test result
 */
CSL_Status  I2c_Intc_Init(void)
{
	CSL_Status         status;
	CSL_Status         result;
	CSL_I2cConfig      i2cConfig;
	CSL_I2cConfig      i2cGetConfig;
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

	//-------------------------------------------------------------------------
	/* Configure I2C module for Slave read */
	i2cConfig.icoar  = CSL_I2C_OWN_ADDR;		// CSL_I2C_ICOAR_DEFVAL
	i2cConfig.icimr  = CSL_I2C_ICIMR_DEFVAL;
	i2cConfig.icclkl = CSL_I2C_ICCLK_VAL;	// For 350kHz replace by 0xC => (12000/350/2 - 5
	i2cConfig.icclkh = CSL_I2C_ICCLK_VAL;	// For 350kHz replace by 0xC => (12000/350/2 - 5
	i2cConfig.iccnt  = CSL_I2C_DATA_SIZE + CSL_EEPROM_ADDR_SIZE; //
	i2cConfig.icsar  = CSL_I2C_DEVICE_ADDR;		//CSL_I2C_ICSAR_DEFVAL
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
	/* Enable I2C Rx Ready Event */
	status = I2C_eventEnable(CSL_I2C_EVENT_ICRRDY);
	if(status != CSL_SOK)
	{
		LOG_printf(&trace, "I2C Event enable Failed!!\n");
		return(result);
	}

	/* Enable Stop condition detected Event */
	status = I2C_eventEnable(CSL_I2C_EVENT_SCD);
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

CSL_Status  I2c_Intc_reInit(void)
{
	CSL_Status         result;

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
CSL_Status I2c_Intc_Deinit(void)
{
	CSL_Status         status;
	CSL_Status         result;

	result = CSL_I2C_TEST_FAILED;
	/* Disable I2C Rx Ready Event */
	status = I2C_eventDisable(CSL_I2C_EVENT_ICRRDY);
	if(status != CSL_SOK)
	{
		LOG_printf(&trace, "I2C Rx Ready Event disable Failed!!\n");
		return(result);
	}

	/* Disable I2C Stop condition detected Event */
	status = I2C_eventDisable(CSL_I2C_EVENT_SCD);
	if(status != CSL_SOK)
	{
		LOG_printf(&trace, "I2C Stop condition Event disable Failed!!\n");
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
	/* Compare the buffers */
	for(looper = 0; looper < CSL_EEPROM_ADDR_SIZE; looper++)
	{
		if(0 != gI2cRdBuf[looper])
		{
			LOG_printf(&trace, "Read Address Does not Match!!\n");
			return(result);
		}
	}
	for(looper = 0; looper < CSL_I2C_DATA_SIZE; looper++)
	{
		if(looper != gI2cRdBuf[looper+2])
		{
			LOG_printf(&trace, "Read Data Buffers Does not Match!!\n");
			return(result);
		}
	}

	if(looper == CSL_I2C_DATA_SIZE)
	{
		LOG_printf(&trace, "Read Write Buffers Match!!\n");
	}
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
	i2cErrInRx = TRUE;
#if (DEBUG>=10)
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
	if (--dataLength == 0)
	   SEM_post(&SEM_I2c_Intc);
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
	LOG_printf(&trace, "CSL_i2cScdCallback: I2C Address as slave callback");
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
