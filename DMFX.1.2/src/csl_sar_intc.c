/*****************************************************************************/
/*                                                                           */
/* FILENAME                                                                  */
/*   csl_sar_intc.c                                                       	 */
/*                                                                           */
/* DESCRIPTION                                                               */
/* 	    Implements SAR interrupt based functions for digital potentiometers  */
/* and chromatic tuner.          											 */
/* Initializes SAR DSP channels for potentiometers and tuner                 */
/* Modifies dsplib channel start conversion by including SAR channel         */
/* Modifies SAR ADC parameter setting function to set NHV bit                */
/* It includes sar interrupt managed by BIOS HWI. SAR interrupt works        */
/* differently for digital potentiometer or tuner                            */
/*                                                                           */
/* NOTES                                                                     */
/*	 This function is based on 	Texas Instruments csl_sar_IntcExample.c		 */
/*																			 */
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
 * ============================================================================*/
/* REVISION                                                                  */
/*   Revision: 1.00                                                          */
/*   Author  : Diego de la Cruz                                              */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* HISTORY                                                                   */
/*   Revision 1.00                                                           */
/*   05/03/2016															     */
/*                                                                           */
/*****************************************************************************/
/* csl_sar_intc()                                                            */
/*****************************************************************************/
/* Inclusion of header files */
#include "gpio_control.h"
#include "csl_gpio.h"
#include <csl_sar.h>
#include <csl_intc.h>
#include <csl_general.h>
#include <pal_osWait.h>
#include <csl_sar_intc.h>
#include <DMFX_debug.h>
#include "DMFX2_CSL_BIOS_cfg.h"
#include <clk.h>
#include "tuner.h"
#include "DMFX2.h"
#include "dsp_fx.h"

#define  NS_FILT	1000L
#define  NS_LOW_THR 100

/* Global Structure Declaration*/
CSL_SarHandleObj SarObj;            /* SAR object structure */
CSL_SarHandleObj *SarHandle;        /* SAR handle           */
Uint16           SarData[4] = {0, 0, 0, 0};        /* SAR Read Buffer      */
Uint16 			 chanNo = 2;						// SAR Channel number 0-3

extern Uint16 FxCmd[FXCMD_SIZE];
extern Int16 TunerBuf[2][TUNER_SIZE];  //stores Tuner SAR samples
char			 		tune_on = 0;
unsigned short int 		TuneIdx = 0;
unsigned volatile char 	TunePingPong = 0;
volatile char			NoiseSup_on = 0;
#if DEBUG>=10
unsigned short int NSBuffer[TUNER_SIZE] = {0};
unsigned short int 		NSIdx = 0;
#endif

// Initializes and configures SAR block for digital potentiometers using Channels 2-5 (GPAIN0-3 CH2-5)
// using SAR ADC sampling frequency of 100Hz and single conversion
// each conversion is managed by LCD function lcd_printdisplay
// it enables SAR interrupt
int	sar_pot_init(void)
{
    Bool flag = 1;
    int result;
    /* Testing of SAR A/D Keypad Voltage Measurement */
    CSL_Status    status;
    CSL_SarChSetup param;
    result = -1;

    tune_on = 0;
    /* Initialize the SAR module */
    status = SAR_init();
    if(status != CSL_SOK)
    {
        LOG_printf(&trace, "SAR Init Failed!!");
        return (result) ;
    }

    /* Open SAR channel */
	status = SAR_chanOpen(&SarObj,CSL_SAR_CHAN_2);
	SarHandle = &SarObj;
	if(status != CSL_SOK)
	{
		LOG_printf(&trace, "SAR_chanOpen Failed!!");
		return result;
	}

	/* Initialize channel */
	status = SAR_chanInit(SarHandle);
	if(status != CSL_SOK)
	{
		LOG_printf(&trace, "SAR_chanInit %d Failed!!");
		return(result);
	}

    /* Clear any pending Interrupt */
    IRQ_clear(SAR_EVENT);
    IRQ_test(SAR_EVENT,&flag);
    /* Register the ISR */
    //IRQ_plug(SAR_EVENT,&sarISR);  // ISR registered on DSP/BIO config and controlled by HWI dispatcher

	param.OpMode =  CSL_SAR_POLLING;		// CSL_SAR_POLLING clears STATUSMASK
	param.MultiCh = CSL_SAR_NO_DISCHARGE;
	param.RefVoltage = CSL_SAR_REF_VIN;
#if 1
	param.SysClkDiv = 155; 		// 100MHz / 156 = 640kHz / 32-cycles/conversion => fs = 20kHz
#else
	param.SysClkDiv = 31249; 	// 100MHz / 31250 = 3.2kHz / 32-bit/sample => fs = 100Hz;
#endif
	/* Configuration for SAR module */
	status = SAR_chanSetup(SarHandle,&param);
	if(status != CSL_SOK)
	{
		LOG_printf(&trace, "SAR_chanConfig Failed!!");
		return(result);
	}

	/* Set channel cycle set */
	status = SAR_chanCycSet(SarHandle,CSL_SAR_SINGLE_CONVERSION);
	if(status != CSL_SOK)
	{
		LOG_printf(&trace, "SAR_chanCycSet Failed!!");
		return(result);
	}
	/* set ADC Measurement parameters */
	status = SAR_A2DSetParam(SarHandle);
	if(status != CSL_SOK)
	{
		LOG_printf(&trace, "SAR_A2DMeasParamSet Failed!!");
		return(result);
	}

    /* Clear pending Interrupt event */
    IRQ_clear(SAR_EVENT);
    /* Enabling Interrupt */
    IRQ_enable(SAR_EVENT);

#if 0
    IRQ_globalEnable();

    /* Set channel and start the conversion */
    status = SAR_SetCh_StrtCnv(SarHandle, 0);
    if(status != CSL_SOK)
    {
        LOG_printf(&trace, "SAR_startConversion Failed!!");
        return(result);
    }
#endif
	// SAR Initialization ends here
    return(result);
}

// Reconfigures SAR block for use tuner
// using SAR ADC sampling frequency of 20kHz and continuous conversion
// it disables SAR interrupt on entry and enables it on exit
// it initiates continuous ADC conversion
int	sar_tune_init(void)
{
    int result;
    CSL_Status    status;
    CSL_SarChSetup param;

    result = -1;
    tune_on = 1;
    IRQ_disable(SAR_EVENT);
    /* Stop the conversion */
    status = SAR_stopConversion(SarHandle);
    if(status != CSL_SOK)
    {
        LOG_printf(&trace, "SAR_stopConversion Failed!!");
    }
	/* Clear any pending interrupts */
    param.OpMode =  CSL_SAR_POLLING;		// CSL_SAR_POLLING clears STATUSMASK
	param.MultiCh = CSL_SAR_NO_DISCHARGE;
	param.RefVoltage = CSL_SAR_REF_VIN;
	param.SysClkDiv = 155; // 100MHz / 156 = 640kHz / 32-cycles/conversion => fs = 20kHz
	/* Configuration for SAR module */
	status = SAR_chanSetup(SarHandle,&param);
	if(status != CSL_SOK)
	{
		LOG_printf(&trace, "SAR_chanConfig Tune Failed!!");
		return(result);
	}

	/* Set channel cycle set */
	status = SAR_chanCycSet(SarHandle,CSL_SAR_CONTINUOUS_CONVERSION);	//CSL_SAR_SINGLE_CONVERSION
	if(status != CSL_SOK)
	{
		LOG_printf(&trace, "SAR_chanCycSet Tune Failed!!");
		return(result);
	}
    /* Clear pending Interrupt event */
    IRQ_clear(SAR_EVENT);
    /* Enabling Interrupt */
    IRQ_enable(SAR_EVENT);
    /* Set channel and start the conversion */
    status = SAR_SetCh_StrtCnv(SarHandle, 0);
    if(status != CSL_SOK)
    {
        LOG_printf(&trace, "SAR_startConversion Tune Failed!!");
        return(result);
    }
	// SAR Initialization ends here
    return(result);
}

// Reconfigures SAR for potentiometer reading
// using SAR ADC sampling frequency of 100Hz and single conversion
// it disables SAR interrupt on entry and enables it on exit
// tuning LEDs are cleared
int	sar_tune_deinit(void)
{
    int result;
    CSL_Status    status;
    CSL_SarChSetup param;

    result = -1;
    tune_on = 0;
	/* Clear any pending Interrupt */
	IRQ_disable(SAR_EVENT);
	CLR_LED3;
	CLR_LED4;
	CLR_LED5;
    /* Stop the conversion */
    status = SAR_stopConversion(SarHandle);
    if(status != CSL_SOK)
    {
        LOG_printf(&trace, "SAR_stopConversion Failed!!");
    }
	/* Clear any pending interrupts */

	param.OpMode =  CSL_SAR_POLLING;		// CSL_SAR_POLLING clears STATUSMASK
	param.MultiCh = CSL_SAR_NO_DISCHARGE;
	param.RefVoltage = CSL_SAR_REF_VIN;
	param.SysClkDiv = 31249; // 100MHz / 31250 = 3.2kHz / 32-bit/sample => fs = 100Hz;
	/* Configuration for SAR module */
	status = SAR_chanSetup(SarHandle,&param);
	if(status != CSL_SOK)
	{
		LOG_printf(&trace, "SAR_chanConfig Failed!!");
		return(result);
	}

	/* Set channel cycle set */
	status = SAR_chanCycSet(SarHandle,CSL_SAR_SINGLE_CONVERSION);
	if(status != CSL_SOK)
	{
		LOG_printf(&trace, "SAR_chanCycSet Failed!!");
		return(result);
	}
	/* Clear pending Interrupt event */
	IRQ_clear(SAR_EVENT);
	/* Enabling Interrupt */
	IRQ_enable(SAR_EVENT);
	// SAR Re-Initialization ends here
	return(result);
}
// SAR potentiometer voltage reading routine
// Used for debugging, not called on normal use
void sar_test_pot_voltage ()
{
    int i = 0, j = 0;

	/* ISR runs for 64 times */
    for(i = 0; i < 64; i++)
    {
        IRQ_globalDisable();

    	for(j = 0; j < POTENTIOMETER; j++)
    		LOG_printf(&trace, "SAR channel = %d, data = %d", j, SarData[j]);
		SarData[chanNo] = 0xFF;
	    IRQ_globalEnable();
	    TSK_sleep(100);
    }
}

// Uninitilizes SAR block disabling interrupts and closing SAR opened channel
int sar_pot_deinit()
{
    CSL_Status    status;
    int result = -1;

    // SAR disabling starts here
    IRQ_disable(SAR_EVENT);

    /* Stop the conversion */
    status = SAR_stopConversion(SarHandle);
    if(status != CSL_SOK)
    {
        LOG_printf(&trace, "SAR_stopConversion Failed!!");
    }
	/* Clear any pending interrupts */
	IRQ_clearAll();

	/* Disable all the interrupts */
	IRQ_disableAll();

    //Disable interrupt
    IRQ_globalDisable();

    /* Close the channel */
    status = SAR_chanClose(SarHandle);
    if(status != CSL_SOK)
    {
        LOG_printf(&trace, "SAR_chanClose Failed!!");
        return(result);
    }

    /* Deinit */
    status = SAR_deInit();
    if(status != CSL_SOK)
    {
        LOG_printf(&trace, "SAR_deInit Failed!!");
        return(result);
    }
    result = 0;
    return(result);
}

// ISR to read ADC data
// interrupt key word not used: interrupt is managed by HWI dispatcher on BIOS
// Different behavior for potentiometer or tuner
// It adapts different reading from channel 2 (GPAIN0 - CH2)
// Inverts SAR potentiometer data due to HW bug (pot pins inverted)
// For tuner use, converts a positive number to a zero DC offset signed sample
// It manages Tuner pingpong buffer once half buffer is full
void sarISR(void)
{
	Uint16 ReadChannel;
	signed short int temp;
	unsigned short int low_th, hi_th;
	static unsigned short int PwrLevel = 0;
	static unsigned short int oldPwrLevel = 0;

	// Noise Suppression is implemented within the SAR interrupt
	temp = SarHandle->baseAddr->SARDATA;
	ReadChannel = ((temp & 0x7000)>>12)-2;
	temp &= 0x0FFF;
	SarData[ReadChannel] = temp;

	switch(ReadChannel)
	{
	case 0:
		TunerBuf[TunePingPong][TuneIdx++] = SarData[0]-356;
		if(TuneIdx == TUNER_SIZE) {
			TuneIdx = 0;
			TunePingPong ^= 1;
		}
		break;
	case 1:
		if(NoiseSup_on)
		{
			if(FxCmd[NS_AUTO] == 1)
			{
				low_th = NS_LOW_THR;
				hi_th  = (short)((18022L*NS_LOW_THR)>>14);
			}
			else
			{
				low_th = FxCmd[NS_THRESHOLD];	// low threshold level = 0 - 255
				// Hi Threshold for hysteresis
				hi_th  = (short)((18022L*(long)FxCmd[NS_THRESHOLD])>>14);	// hi threshold level = 0 - 280 = 1.1*(0-255) = (0-255)*18022/16384)
			}
			if(SarData[1] >= 512)
				temp = SarData[1] - 512;
			else
				temp = 512 - SarData[1];
			PwrLevel = ((NS_FILT*(long)temp)>>15) + (((32767L-NS_FILT)*PwrLevel)>>15);
			if ((PwrLevel < low_th)&&(oldPwrLevel >=low_th))
				MUTE;		// Mute only affects Line Out, not Headphones. Noise Cancel only works on Line Out
			else if ((PwrLevel > hi_th)&&(oldPwrLevel <= hi_th))
				UNMUTE;		// Mute only affects Line Out, not Headphones
	#if DEBUG>=10
			NSBuffer[NSIdx++] = PwrLevel;
			if(NSIdx == TUNER_SIZE) NSIdx = 0;
	#endif
			oldPwrLevel = PwrLevel;
			SAR_SetCh_StrtCnv(SarHandle, 1);	// start the next conversion of Noise Suppression channel 1 (GPAIN1 - CH3)
		}
		break;
	case 2:
#if SAR_BUG
#if (POTENTIOMETER==4)
		if (chanNo == 2)
			SarData[2] = (1023 - ((SarData[2]*89)>>6))>>2;  // scale channel 2 (GPAIN0-CH2) from 725 to 255 (8-bits)
		else
			SarData[chanNo] = (1023 - SarData[chanNo])>>2;  // scale channel 3-5 (GPAIN1-3-CH3-5) from 1023 to 255 (8-bits)
#else
			SarData[2] = (1023 - SarData[2])>>2;  // scale channel from 1023 to 255 (8-bits)
#endif
#else
		if (chanNo == 2)
			SarData[2] = ((SarData[2]*89)>>6)>>2;  // scale channel 2 from 725 to 255 (8-bits)
#endif
#if DEBUG>=10
		if(chanNo != ReadChannel) {
			LOG_printf(&trace, "ISR: SAR data channel %d != %d", ReadChannel, chanNo);
		}
		LOG_printf(&trace, "ISR: SAR channel = %d, data = %d", chanNo, SarData[chanNo]);
#endif
		break;
	}
	IRQ_clear(SAR_EVENT);
#if 0  //SAR single conversions moved to LcdMenu
	if(chanNo == (POTENTIOMETER-1))	// Increases circularly channel number 2-5 (GPAIN0-3 CH 2-5)
	{
		chanNo = 0;
	}
	else
		chanNo += 1;
#if (POTENTIOMETER<=2)
		SAR_SetCh_StrtCnv(SarHandle, chanNo);	// Starts next SAR single conversion and sets new channel
#else
		SAR_SetCh_StrtCnv(SarHandle, chanNo);	// Starts next SAR single conversion and sets new channel
#endif
#endif
}

// SAR ADC set parameters AVDD measure disable, GNDON disable, NoHV enable, Half bit disable
CSL_Status SAR_A2DSetParam (
SAR_Handle                   hSar
)
{
    CSL_Status status;
    if(NULL == hSar)
    {
        return CSL_ESYS_BADHANDLE;
    }
	/* Enabling SAR A/D AVddMeas bit */
	CSL_FINST (hSar->baseAddr->SARPINCTRL ,ANACTRL_SARPINCTRL_AVDDMEAS,
		CLEAR);
	/* Disabling SAR A/D GndSwOn bit */
	CSL_FINST (hSar->baseAddr->SARPINCTRL ,ANACTRL_SARPINCTRL_GNDON,
		CLEAR);
	// Sets NoHV bit
    hSar->baseAddr->SARPINCTRL = ((hSar->baseAddr->SARPINCTRL) & ~0x4) | 0x0004;
	/* Disabling SAR A/D Half bit */
	CSL_FINST (hSar->baseAddr->SARPINCTRL, ANACTRL_SARPINCTRL_HALF,
		CLEAR);

	status = CSL_SOK;
    return status;
}

// Set SAR ADC channel and starts single conversion
CSL_Status SAR_SetCh_StrtCnv (
SAR_Handle                   hSar,
Uint16		                 channel
)
{

    if(NULL == hSar)
    {
        return CSL_ESYS_BADHANDLE;
    }
    // CSL_SAR_REGS->
	CSL_FINS (hSar->baseAddr->SARCTRL, ANACTRL_SARCTRL_CHSEL, channel+2);  // converts channels 0-3 to (GPAIN0-3 CH2-5)
    /* Start SAR A/D conversion */
    CSL_FINST (hSar->baseAddr->SARCTRL, ANACTRL_SARCTRL_ADCSTRT, SET);

    return CSL_SOK;
}
