/*
 * $$$MODULE_NAME codec_aic3254.c
 *
 * $$$MODULE_DESC codec_aic3254.c
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/**
 *  \file   codec_aic3254.c
 *
 *  \brief  codec configuration function
 *
 *   This file contains the APIs for codec(AIC3254) read and write using I2C
 *
 *  (C) Copyright 2005, Texas Instruments, Inc
 *
 *  \author     PR Mistral
 *
 *  \version    1.0
 *
 */

#include "psp_i2s.h"
#include "psp_i2c.h"
#include "dda_i2c.h"
#include "psp_common.h"
#include "codec_aic3254.h"
#include "app_globals.h"
#include "csl_general.h"
#include "DMFX1_CSL_BIOS_cfg.h"

PSP_Handle    hi2c = NULL;

PSP_Result Beep_Generator(Int16 freq, Int16 len)
{
	PSP_Result result;

	/* Beep generator */
	AIC3254_Write( 0x00, 0x00, hi2c);  /* select page 0 */
    AIC3254_Write( 0x3C, 0x19 ,hi2c);  // Select DAC signal processing block PRB_P25
	/* beep length = floor(fs*len) */
	AIC3254_Write( 0x49, 0x00, hi2c);  /* beep length 1 */
	AIC3254_Write( 0x4A, 0x5D, hi2c);  /* beep length 2 */
	AIC3254_Write( 0x4B, 0xC0, hi2c);  /* beep length 3 */
	/* sine = round(32768, sin(2*pi()*fin/fs)) */
	AIC3254_Write( 0x4C, 0x12, hi2c);  /* sine 01 */
	AIC3254_Write( 0x4D, 0x5E, hi2c);  // sine 02
	/* cosine = round(32768, cos(2*pi()*fin/fs)) */
	AIC3254_Write( 0x4E, 0x7E, hi2c);  /* cosine 01 */
	AIC3254_Write( 0x4F, 0xAD, hi2c);  /* cosine 02 */
	AIC3254_Write( 0x48, 0x0A, hi2c);  // beep gain right -2dB
	AIC3254_Write( 0x47, 0x8A, hi2c);  // beep gain left -2dB

	// write 1 to page register to select page 1 - prepare for next headset volume change
	result = AIC3254_Write(0, 1, hi2c);

	if (result != PSP_SOK)
	{
		return FALSE;
	}
	return result;
}
/*
 * Mute control for codec output
 * TRUE = Mute codec output
 * FALSE = UnMute codec output
 ***********************************************************************/
Bool Set_Mute_State(Bool flag)
{
    PSP_Result    result = PSP_SOK;
    Bool retval;

    retval = TRUE;

    // write 0 to page register to select page 0
    result = AIC3254_Write(0, 0, hi2c);
    if (result != PSP_SOK) 
    {
        retval = FALSE;
    }
    else
    {
        if (flag == TRUE)
        {
            //mute output
            result = AIC3254_Write(64,0xd,hi2c);
            if (result != PSP_SOK) 
            {
                retval = FALSE;
            }
        }
        else
        {
            //unmute output
            result = AIC3254_Write(64,0x1,hi2c);
            if (result != PSP_SOK) 
            {
                retval = FALSE;
            }
        }
    }
#if 1
    // write 1 to page register to select page 1 - prepare for next headset volume change
    result = AIC3254_Write(0, 1, hi2c);
    if (result != PSP_SOK) 
    {
        retval = FALSE;
    }
#endif
    return retval;
}

#define HEADPHONE_DRIVER_GAIN_MUTE_ENABLE  0x40    // bit6 =1 mute headphone driver
#define VOLUME_STEP_SIZE                   256
#define VOLUME_TABLE_MAX_GAIN_INDEX        29      // headphone gain setting = 29 -> 29 dB gain
#define VOLUME_TABLE_MAX_ATTENUATION_INDEX 35      // headphone gain setting = 0x3A -> -6dB gain
#define USB_MAX_ATTENUATION_VALUE          -32768
#define VOLUME_TABLE_MUTE_HEADPHONE_INDEX  36      // headphone gain setting = 0x7B set gain to -5dB with headphone driver muted

// table has both gain and attenuation settings for headphone output of the codec.
// 0 : no gain/no attenuation, gain : 1 - 29, attenuation : 0x3F - 0x3A, muted: 0x7B
const Uint16 volume_table[] =  {
    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,
    0x3F,0x3E,0x3D,0x3C,0x3B,0x3A,(0x3B | HEADPHONE_DRIVER_GAIN_MUTE_ENABLE)
};

/*
 * Change gain setting of headphone output of codec
 * volume = gain setting received from USB
 * channel = 0:left channel, =1 right channel
 ***********************************************************************/
Bool Adjust_Volume(Int16 volume, Uint16 channel)
{
    PSP_Result    result = PSP_SOK;
    Uint16        gain;

    // goto max attentuation
    if (volume == USB_MAX_ATTENUATION_VALUE)
    {
        // the max attenuation for the headpphone  is -6dB so we will mute the headphone driver
        // and set the codec gain to the lowest value(-5dB) that allows the headphone driver
        // to be muted. any volume change other than the max attenuation will turn off the
        // headphone driver mute
        gain = VOLUME_TABLE_MUTE_HEADPHONE_INDEX;
    }
    else if (volume >= 0)
    {
        // determine gain index
        gain = volume/VOLUME_STEP_SIZE;

        // check range
        if(gain > VOLUME_TABLE_MAX_GAIN_INDEX)
        {
            // set to max gain
            gain = VOLUME_TABLE_MAX_GAIN_INDEX;
        }
    }
    else
    {
        // determine attenuation index
        gain = (-volume)/VOLUME_STEP_SIZE;
        if (gain !=0)
        {
            //index from start of attentuation values in table
            gain += VOLUME_TABLE_MAX_GAIN_INDEX;
            if (gain > VOLUME_TABLE_MAX_ATTENUATION_INDEX)
            {
                // set to max attenuation
                gain = VOLUME_TABLE_MAX_ATTENUATION_INDEX;
            }
        }

    }

    if (channel == 0)
    {
        //adjust volume setting of left headphone
        result = AIC3254_Write(0x10,volume_table[gain],hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
    }
    else
    {
        //adjust volume setting of right headphone
        result = AIC3254_Write(0x11,volume_table[gain],hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
    }
    return TRUE;
}

/*
 * Change gain setting of headphone output of codec
 * volume = gain setting received from USB
 * channel = 0:left channel, =1 right channel
 ***********************************************************************/
Bool Change_Volume(Int16 volume, Uint16 channel)
{
    PSP_Result    result = PSP_SOK;
    Uint16        gain;
	static Int16  voldb = 2;

    if (volume > 0)
    {
        // determine gain index
        voldb += 1;

        // check range
        if(voldb > VOLUME_TABLE_MAX_GAIN_INDEX)
        {
            // set to max gain
            voldb = VOLUME_TABLE_MAX_GAIN_INDEX;
        }
        gain = voldb;
    }
    else if (volume < 0) 
    {
        // determine attenuation index
        if (voldb == -6)
        {
	        // the max attenuation for the headpphone  is -6dB so we will mute the headphone driver
	        // and set the codec gain to the lowest value(-5dB) that allows the headphone driver
	        // to be muted. any volume change other than the max attenuation will turn off the
	        // headphone driver mute
	        gain = VOLUME_TABLE_MUTE_HEADPHONE_INDEX;
        }
        else if (voldb <= 0) {
	        voldb -= 1;
            //index from start of attentuation values in table
            gain = VOLUME_TABLE_MAX_GAIN_INDEX - voldb;
        }
        else { 
	        voldb -= 1;
        	gain = voldb;
        }
    }
    else  
	    gain = VOLUME_TABLE_MUTE_HEADPHONE_INDEX;

    if (channel == 0)
    {
        //adjust volume setting of left headphone
        result = AIC3254_Write(0x10,volume_table[gain],hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
    }
    else
    {
        //adjust volume setting of right headphone
        result = AIC3254_Write(0x11,volume_table[gain],hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
    }
    return TRUE;
}

PSP_Result AIC3254_init(void)
{
    PSP_Result result = PSP_SOK;
    volatile Uint16 looper;

    /* Reset AIC3254 */
    /* NOTE: Assumes EBSR and GPIO are set correctly before function is called */
    /*CSL_FINS((*GPIO_DOUT0_ADDR), GPIO_DOUT0, 0x0000); // reset active low
    for(looper=0; looper<10; looper++ )
        asm("    nop");
    CSL_FINS((*GPIO_DOUT0_ADDR), GPIO_DOUT0, 0x0400);*/

#ifdef C5535_EZDSP
    hi2c = I2C_Init(0x2f, 100000);
#else
    hi2c = I2C_Init(0x2f, 10000);
#endif
    if (hi2c)
    {
        result = AIC3254_Write(0, 0, hi2c); // write 0 to page register to select page 0
        if (result != PSP_SOK) 
        {
            return result;
        }

        result = AIC3254_Write(1, 1, hi2c); // reset codec
        if (result != PSP_SOK) 
        {
            return result;
        }

        /* Select the PLL input and CODEC_CLKIN */
        /* PLL input is assumed as 12MHz */
        result = AIC3254_Write(4, 0x03, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }

        /*
          PLL_CLK = (PLL_CLKIN * R * J.D)/P
          DAC_FS = PLL_CLKIN/(NDAC * MDAC * DOSR)
          ADC_FS = PLL_CLKIN/(NADC * MADC * AOSR)
          DAC_CLK = PLL_CLK/NDAC
          BCLK = DAC_CLK/BDIV_CLKIN

          DAC_FS, BCLK:
              48 kHz: P=1, R=1, J=7, D=1680 (0x690) 1754 (0x6da)
                      NDAC=2, MDAC=7, DOSR=128
                      BCLK = 28
                      PLL_CLK = (12e6 * 1 * 7.1754)/1 = 86104800
                      DAC_FS = PLL_CLK/(2 * 7 * 128) = 48049.55
                      BCLK = PLL_CLK/NDAC/BCLK = 86104800/2/28 = 1537585.71
              16 kHz: P=1, R=1, J=7, D=1754 (0x6da)
                      NDAC=6, MDAC=7, DOSR=128
                      BCLK = 28
                      PLL_CLK = (12e6 * 1 * 7.1754)/1 = 86104800
                      DAC_FS = PLL_CLK/(6 * 7 * 128) = 16016.52
                      BCLK = PLL_CLK/NDAC/BCLK = 28701600/2/28 = 512528.57
          ADC_FS:
              48 kHz: P=1, R=1, J=7, D=1754 (0x6da)
                      NADC=2, MADC=7, AOSR=128
                      ADC_FS = PLL_CLK/(2 * 7 * 128) = 48049.55
              16 kHz: P=1, R=1, J=7, D=1754 (0x6da)
                      NADC=6, MADC=7, AOSR=128
                      ADC_FS = PLL_CLK/(2 * 7 * 128) = 16016.52
        */

        #ifdef SAMPLE_RATE_TX_48kHz
        // Power up the PLL and set P = 1 & R = 1
        result = AIC3254_Write(5, 0x91, hi2c); // 48khz
        #endif

        #ifdef SAMPLE_RATE_TX_44_1kHz
        /* Power up the PLL and set P = 6 & R = 1 */
        result = AIC3254_Write(5, 0xE1, hi2c);
        #endif

        #ifdef SAMPLE_RATE_TX_32kHz
        // Power up the PLL and set P = 3 & R = 1
        result = AIC3254_Write(5, 0xB1, hi2c); // 32khz
        #endif

        #ifdef SAMPLE_RATE_TX_16kHz
        // Power up the PLL and set P = 1 & R = 1
        result = AIC3254_Write(5, 0x91, hi2c); // 16khz
        #endif

        if (result != PSP_SOK) 
        {
            return result;
        }

        #ifdef SAMPLE_RATE_TX_48kHz
        // Set J value to 7
        result = AIC3254_Write(6, 0x07, hi2c); // 48khz
        #endif

        #ifdef SAMPLE_RATE_TX_44_1kHz
        /* Set J value to 8 */
        result = AIC3254_Write(6, 0x08, hi2c);
        //result = AIC3254_Write(6, 0x07, hi2c);
        #endif

        #ifdef SAMPLE_RATE_TX_32kHz
        // Set J value to 8
        result = AIC3254_Write(6, 0x08, hi2c); // 32khz
        #endif

        #ifdef SAMPLE_RATE_TX_16kHz
        // Set J value to 7
        result = AIC3254_Write(6, 0x07, hi2c); // 16khz
        #endif

        if (result != PSP_SOK) 
        {
            return result;
        }

        //
        // CODEC_CLKIN = 12MHz *(R * J.D)/P = 12*1*7/1=84MHz
        // CODEC_CLKIN = 12MHz *(R * J.D)/P = 12*1*7.168/1=86.016MHz
        //

        #ifdef SAMPLE_RATE_TX_48kHz
        // Set D value= 0x690 => (MSB) = 0x6
        result = AIC3254_Write(7, 0x6, hi2c); // 48khz
        //result = AIC3254_Write(7, 0x0, hi2c); // 48khz
        #endif

        #ifdef SAMPLE_RATE_TX_44_1kHz
        /* Set D value(MSB) = 0x12 */
        result = AIC3254_Write(7, 0x12, hi2c);
        //result = AIC3254_Write(7, 0x0, hi2c);
        #endif

        #ifdef SAMPLE_RATE_TX_32kHz
        // Set D value(MSB) = 0x12
        result = AIC3254_Write(7, 0x12, hi2c); // 32khz
        #endif

        #ifdef SAMPLE_RATE_TX_16kHz
        // Set D value(MSB) = 0x06
        result = AIC3254_Write(7, 0x6, hi2c); // 16khz
        #endif

        if (result != PSP_SOK) 
        {
            return result;
        }

        #ifdef SAMPLE_RATE_TX_48kHz
        // Set D value = 0x690 => (LSB) = 0x90
        result = AIC3254_Write(8, 0x90, hi2c); // 48khz ; 0x690 => .1680 => D = 7.1680 => DAC_FS = 48000.00
        //result = AIC3254_Write(8, 0x0, hi2c); // 48khz ; D = 0
        #endif

        #ifdef SAMPLE_RATE_TX_44_1kHz
        /* Set D value(LSB) = 0x40 */
        result = AIC3254_Write(8, 0x40, hi2c);
        //result = AIC3254_Write(8, 0x38, hi2c);
        #endif

        #ifdef SAMPLE_RATE_TX_32kHz
        // Set D value(LSB) = 0x40
        result = AIC3254_Write(8, 0x40, hi2c); // 32khz
        #endif

        #ifdef SAMPLE_RATE_TX_16kHz
        // Set D value(LSB) = 0xda
        result = AIC3254_Write(8, 0xda, hi2c); // 16khz
        #endif

        if (result != PSP_SOK) 
        {
            return result;
        }

        #ifdef SAMPLE_RATE_TX_48kHz
        // Set NDAC to 2 - this along with BCLK N configures BCLK
        //result = AIC3254_Write(11,0x82, hi2c); // 48khz
        result = AIC3254_Write(11,0x87, hi2c); // 48khz
        #endif

        #ifdef SAMPLE_RATE_TX_44_1kHz
        /* Set NDAC to 12 */
        result = AIC3254_Write(11,0x8C, hi2c);
        //result = AIC3254_Write(11,0x82, hi2c);
        //result = AIC3254_Write(11,0x84, hi2c);
        #endif

        #ifdef SAMPLE_RATE_TX_32kHz
        // Set NDAC to 1 - this along with BCLK N configures BCLK
        result = AIC3254_Write(11,0x81, hi2c); // 32khz
        #endif

        #ifdef SAMPLE_RATE_TX_16kHz
        // Set NDAC to 6 - this along with BCLK N configures BCLK
        result = AIC3254_Write(11, 0x86, hi2c); // 16khz
        #endif

        if (result != PSP_SOK) 
        {
            return result;
        }

        #ifdef SAMPLE_RATE_TX_48kHz
        // Set MDAC to 7
        //result = AIC3254_Write(12,0x87, hi2c); // 48khz
        result = AIC3254_Write(12,0x82, hi2c); // 48khz
        #endif

        #ifdef SAMPLE_RATE_TX_44_1kHz
        /* Set MDAC to 2 */
        result = AIC3254_Write(12,0x82, hi2c);
        //result = AIC3254_Write(12,0x98, hi2c);
        //result = AIC3254_Write(12,0x88, hi2c);
        #endif

        #ifdef SAMPLE_RATE_TX_32kHz
        // Set MDAC to 11
        result = AIC3254_Write(12,0x8B, hi2c); // 32khz
        #endif

        #ifdef SAMPLE_RATE_TX_16kHz
        // Set MDAC to 7
        result = AIC3254_Write(12,0x87, hi2c); // 16khz
        #endif

        if (result != PSP_SOK) 
        {
            return result;
        }

        //
        // DAC_FS = (12MHz *(R * J.D)/P)/(NDAC * MDAC * DOSR) = 84000 / 2 / 7 / 125 = 48kHz
        //

        /* Set DAC OSR MSB value to 0 */
        result = AIC3254_Write(13, 0x0, hi2c );
        if (result != PSP_SOK) 
        {
            return result;
        }

        #ifdef SAMPLE_RATE_TX_48kHz
        // Set DAC OSR LSB value to 128
        // This generates the DAC_FS = 48KHz
        result = AIC3254_Write(14, 128, hi2c ); // 48khz
        //result = AIC3254_Write(14, 125, hi2c ); // 48khz
        #endif

        #ifdef SAMPLE_RATE_TX_44_1kHz
        /* Set DAC OSR LSB value to 16 */
        /* This generates the DAC_FS = 44.1KHz */
        result = AIC3254_Write(14, 16, hi2c );
        //result = AIC3254_Write(14, 20, hi2c );
        #endif

        #ifdef SAMPLE_RATE_TX_32kHz
        // Set DAC OSR LSB value to 96
        result = AIC3254_Write(14, 96, hi2c ); // 32khz
        #endif

        #ifdef SAMPLE_RATE_TX_16kHz
        // Set DAC OSR LSB value to 128
        // This generates the DAC_FS = 16KHz
        result = AIC3254_Write(14, 128, hi2c ); // 16khz
        #endif

        if (result != PSP_SOK) 
        {
            return result;
        }

        //
        // BCLK = (12MHz *(R * J.D)/P)/(NDAC * BCLK N) = 86000 / 2 / 28) 1.536
        // BCLK = (12MHz *(R * J.D)/P)/(NDAC * BCLK N) = 84000 / 2 / 28) 1.500
        //
        #ifdef SAMPLE_RATE_TX_48kHz
        // Set BCLK N value to 28
        // This along with NDAC generates the  BCLK = 1.536 MHz
        // This along with NDAC generates the  BCLK = 1.500 MHz
        //result = AIC3254_Write(30,0x9C, hi2c); // 48khz
        result = AIC3254_Write(30,0x88, hi2c); // 48khz
        #endif

        #ifdef SAMPLE_RATE_TX_44_1kHz
        /* Set BCLK N value to 1 */
        /* This generates the  BCLK = 1.4112MHz */
        result = AIC3254_Write(30,0x81, hi2c);
        //result = AIC3254_Write(30,0x8c, hi2c);
        //result = AIC3254_Write(30,0x85, hi2c);
        #endif

        #ifdef SAMPLE_RATE_TX_32kHz
        // Set BCLK N value to 33
        result = AIC3254_Write(30,0xA1, hi2c); // 32khz
        #endif

        #ifdef SAMPLE_RATE_TX_16kHz
        // Set BCLK N value to 28
        // This along with NDAC generates the  BCLK = 512 kHz
        // BCLK = (12MHz *(R * J.D)/P)/(NDAC * BCLK N)
        result = AIC3254_Write(30, 0x9C, hi2c); // 16khz
        #endif

        if (result != PSP_SOK) 
        {
            return result;
        }

        //#ifdef ENABLE_RECORD
        #if defined(SAMPLE_RATE_RX_48kHz) && defined (SAMPLE_RATE_TX_48kHz)
        //
        // Set ADC_FS to 48 kHZ
        //

        // Set NADC to 7
        //result = AIC3254_Write(18,0x82, hi2c); // 48khz
        result = AIC3254_Write(18,0x87, hi2c); // 48khz
        if (result != PSP_SOK) 
        {
            return result;
        }

        // Set MADC to 2
        //result = AIC3254_Write(19,0x87, hi2c); // 48khz
        result = AIC3254_Write(19,0x82, hi2c); // 48khz
        if (result != PSP_SOK) 
        {
            return result;
        }

        // Set ADC OSR LSB value to 128
        // This generates the ADC_FS = 48KHz
        // ADC_FS = (12MHz *(R * J.D)/P)/(NADC * MADC * AOSR) = (12000*1*7.168/1)/(7*2*128) = 48
        result = AIC3254_Write(20, 128, hi2c ); // 48khz
        // ADC_FS = (12MHz *(R * J.D)/P)/(NADC * MADC * AOSR) = (12000*1*7/1)/(7*2*125) = 48
        //result = AIC3254_Write(20, 125, hi2c ); // 48khz
        if (result != PSP_SOK) 
        {
            return result;
        }


        #elif defined(SAMPLE_RATE_RX_16kHz)
        //
        // Set ADC_FS to 16 kHZ
        // DAC_FS drives I2S FS: If DAC_FS = 48 kHz, every 3rd sample picked in recorded data

        // Set NADC to 6
        result = AIC3254_Write(18, 0x86, hi2c); // 16khz
        if (result != PSP_SOK) 
        {
            return result;
        }

        // Set MADC to 7
        result = AIC3254_Write(19, 0x87, hi2c); // 16khz
        if (result != PSP_SOK) 
        {
            return result;
        }

        // Set ADC OSR LSB value to 128
        // This generates the ADC_FS = 16KHz
        // ADC_FS = (12MHz *(R * J.D)/P)/(NADC * MADC * AOSR)
        result = AIC3254_Write(20, 128, hi2c ); // 16khz
        if (result != PSP_SOK) 
        {
            return result;
        }


        #else
        LOG_printf(&trace, "DAC_FS == 16 KHZ && ADC_FS == 48 kHZ NOT SUPPORTED\n");
       
        #endif
        //#endif // ENABLE_RECORD

        //result = AIC3254_Write(27,/*0xC*/0xd, hi2c); // BCLK and WCLK is set as op to AIC3254(Master)
        result = AIC3254_Write(27, 0xc, hi2c); // BCLK and WCLK is set as op to AIC3254(Master)
        if (result != PSP_SOK) 
        {
            return result;
        }

        result = AIC3254_Write(0,1,hi2c);// select page 1
        if (result != PSP_SOK) 
        {
            return result;
        }

#if 0
		// power up Mic Bias using LDO-IN
        result = AIC3254_Write(51,0x48,hi2c);
        // try Mic Bias off
        //result = AIC3254_Write(51,0x8,hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
#endif

        result = AIC3254_Write(0x1,0x8,hi2c);// Disable crude AVDD generation from DVDD
        if (result != PSP_SOK) 
        {
            return result;
        }

#ifdef C5535_EZDSP        
		result = AIC3254_Write(0x2,1,hi2c);// Enable Analog Blocks and internal LDO
#else
        result = AIC3254_Write(0x2,0,hi2c);// Enable Analog Blocks
#endif

        if (result != PSP_SOK) 
        {
            return result;
        }

        // Rpop=25K, Cc=100uF, N=4, t=4*25K*100e-6
        result = AIC3254_Write(0x14,0x21,hi2c);// Depop reg R=6K,t=4RC, soft step time=0ms
        if (result != PSP_SOK) 
        {
            return result;
        }

        result = AIC3254_Write(0xc,0x8,hi2c);// LDAC AFIR routed to HPL
        if (result != PSP_SOK) 
        {
            return result;
        }

        result = AIC3254_Write(0xd,0x8,hi2c);// RDAC AFIR routed to HPR
        if (result != PSP_SOK) 
        {
            return result;
        }

        result = AIC3254_Write(14,0x8,hi2c);// LDAC AFIR routed to LOL
        if (result != PSP_SOK)
        {
            return result;
        }

        result = AIC3254_Write(15,0x8,hi2c);// RDAC AFIR routed to LOR
        if (result != PSP_SOK)
        {
            return result;
        }

        // weakly drive to CM unused inputs IN1-3/R-L
        result = AIC3254_Write(58,0xcc,hi2c);
        if (result != PSP_SOK)
        {
            return result;
        }

        //Route IN2L to LEFT_P with 10K input impedance
        result = AIC3254_Write(52,0x10,hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }

        //Route Common Mode to LEFT_M with impedance of 10K
        //result = AIC3254_Write(54,0xC0,hi2c);
        result = AIC3254_Write(54,0x1,hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }

        //Route IN2R to RIGHT_P with 10K input impedance
        result = AIC3254_Write(55,0x10,hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }

        //Route Common Mode to RIGHT_M with impedance of 10K
        result = AIC3254_Write(57,0x01,hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }

        //Unmute Left MICPGA
        result = AIC3254_Write(59,ADC_GAIN,hi2c); // Gain = 0 dB
        if (result != PSP_SOK) 
        {
            return result;
        }

        //Unmute Right MICPGA
        result = AIC3254_Write(60,ADC_GAIN,hi2c); // Gain = 0 dB
        if (result != PSP_SOK) 
        {
            return result;
        }

        result = AIC3254_Write(0, 0, hi2c); // write 0 to page register to select page 0
        if (result != PSP_SOK) 
        {
            return result;
        }

        result = AIC3254_Write(0x3C,25,hi2c);// Select PRB_P25 DAC: 5 Biquads, DRC, 3D, Beep Generator, Filter A
        if (result != PSP_SOK)
        {
            return result;
        }

        result = AIC3254_Write(0x3D,0x2,hi2c);// Select PRB_R2 ADC: 5 Biquads, 1st order IIR, AGC, Filter A
        if (result != PSP_SOK)
        {
            return result;
        }
        /*
        ticks = PRD_getticks();
        LOG_printf(&trace, "Ticks = %d", ticks);
        AIC3254_Read(78,&testread ,hi2c);
        LOG_printf(&trace, "AIC3204 read reg 78 = 0x%08x", testread);

        ReadFilterCoeffs(hi2c);	// Read Filter coefficients with DAC powered down
        result = AIC3254_Write(0, 0, hi2c); // write 0 to page register to select page 0
        */
        if (result != PSP_SOK)
        {
            return result;
        }

        result = AIC3254_Write(64,0x2,hi2c); // left vol=right vol
        if (result != PSP_SOK) 
        {
            return result;
        }

        result = AIC3254_Write(63,0xd4, hi2c); // power up left,right data paths and set channel
        if (result != PSP_SOK) 
        {
            return result;
        }

        result = AIC3254_Write(64,0xc,hi2c); // left vol=right vol; muted
        if (result != PSP_SOK) 
        {
            return result;
        }

        result = AIC3254_Write(0,1,hi2c);// select page 1
        if (result != PSP_SOK) 
        {
            return result;
        }

        result = AIC3254_Write(0x10,0,hi2c);// unmute HPL , 0dB gain
        if (result != PSP_SOK) 
        {
            return result;
        }

        result = AIC3254_Write(0x11,0,hi2c);// unmute HPR , 0dB gain
        if (result != PSP_SOK) 
        {
            return result;
        }

        result = AIC3254_Write(18,0,hi2c);// unmute LOL , 0dB gain
        if (result != PSP_SOK)
        {
            return result;
        }

        result = AIC3254_Write(19,0,hi2c);// unmute LOR , 0dB gain
        if (result != PSP_SOK)
        {
            return result;
        }

        result = AIC3254_Write(0x9,0x3C,hi2c);// power up HPL,HPR, LOL, LOR
        if (result != PSP_SOK) 
        {
            return result;
        }

        result = AIC3254_Write(0x0,0x0,hi2c);// select page 0
        if (result != PSP_SOK) 
        {
            return result;
        }

        result = AIC3254_Write(0x40,0x2,hi2c);// unmute DAC with right vol=left vol
        if (result != PSP_SOK) 
        {
            return result;
        }

        // Adjusted to avoid saturation on output ???
        result = AIC3254_Write(0x41, 0xF1,hi2c);// set DAC gain to -7.5dB 0xF1
        if (result != PSP_SOK) 
        {
            return result;
        }

        //Powerup left and right ADC
        result = AIC3254_Write(81,0xc0,hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }

        //Unmute left and right ADC
        result = AIC3254_Write(82,0x00,hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }

        //result = AIC3254_Write(83,0x68,hi2c);// Left ADC Digital Volume (-12dB)
        //result = AIC3254_Write(84,0x68,hi2c);// Right ADC Digital Volume (-12dB)
        if (result != PSP_SOK) 
        {
            return result;
        }

        result = AIC3254_Write(0x0,0x1,hi2c);// select page 1
        if (result != PSP_SOK) 
        {
            return result;
        }

        //result = AIC3254_Write(0x10,0,hi2c);// unmute HPL , 0dB gain
        result = AIC3254_Write(0x10,0,hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }

        //result = AIC3254_Write(0x11,0,hi2c);// unmute HPR , 0dB gain
        result = AIC3254_Write(0x11,0,hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }

        result = AIC3254_Write(0x12,0,hi2c);// unmute LOL 0dB gain
        if (result != PSP_SOK) 
        {
            return result;
        }

        result = AIC3254_Write(0x13,0,hi2c);// unmute LOR 0dB gain
        if (result != PSP_SOK) 
        {
            return result;
        }

#if 0 // debug
       // route ADC_FS to WCLK (I2S FS)
        result = AIC3254_Write(33, 0x10, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
#endif

        // write 1 to page register to select page 1 - prepare for next headset volume change
        result = AIC3254_Write(0, 1, hi2c);
        if (result != PSP_SOK) 
        {
            return FALSE;
        }
        return result;
  }
  else
  {
        //printf("\n I2C init error \n");
        LOG_printf(&trace, "\n I2C init error \n");
        return PSP_E_DRIVER_INIT;
  }

}

PSP_Result ReadFilterCoeffs(PSP_Handle hi2c)
{
	Uint16 coeff[2];
	Uint16 hpf[6];
	Uint16 lpf[6];
	PSP_Result result = PSP_E_INVAL_PARAM;

    //AIC3254_Write(0x00, 0x2C, hi2c); //select page 44
    //AIC3254_Write(0x01, 0x05, hi2c); // Enable DAC adaptive filtering

    // HPF
    AIC3254_Write(0x00, 0x2E, hi2c); //select page 46
	result = AIC3254_Read(0x34, &coeff[0], hi2c);
    if (result != PSP_SOK)
    {
        return result;
    }
	AIC3254_Read(0x35, &coeff[1], hi2c);
	hpf[0] = (coeff[0]<<8)|(coeff[1]&0xFF);
	LOG_printf(&trace, "DRC HPF C71 b0h = 0x%08x", hpf[0]);
	AIC3254_Read(0x36, &coeff[0], hi2c);
	AIC3254_Read(0x37, &coeff[1], hi2c);
	hpf[1] = (coeff[0]<<8)|(coeff[1]&0xFF);
	LOG_printf(&trace, "DRC HPF C71 b0l = 0x%08x", hpf[1]);
	AIC3254_Read(0x38, &coeff[0], hi2c);
	AIC3254_Read(0x39, &coeff[1], hi2c);
	hpf[2] = (coeff[0]<<8)|(coeff[1]&0xFF);
	LOG_printf(&trace, "DRC HPF C72 b1h = 0x%08x", hpf[2]);
	AIC3254_Read(0x3A, &coeff[0], hi2c);
	AIC3254_Read(0x3B, &coeff[1], hi2c);
	hpf[3] = (coeff[0]<<8)|(coeff[1]&0xFF);
	LOG_printf(&trace, "DRC HPF C72 b1l = 0x%08x", hpf[3]);
	AIC3254_Read(0x3C, &coeff[0], hi2c);
	AIC3254_Read(0x3D, &coeff[1], hi2c);
	hpf[4] = (coeff[0]<<8)|(coeff[1]&0xFF);
	LOG_printf(&trace, "DRC HPF C73 a1h = 0x%08x", hpf[4]);
	AIC3254_Read(0x3E, &coeff[0], hi2c);
	AIC3254_Read(0x3F, &coeff[1], hi2c);
	hpf[5] = (coeff[0]<<8)|(coeff[1]&0xFF);
	LOG_printf(&trace, "DRC HPF C73 a1l = 0x%08x", hpf[5]);
	// LPF
	AIC3254_Read(0x40, &coeff[0], hi2c);
	AIC3254_Read(0x41, &coeff[1], hi2c);
	lpf[0] = (coeff[0]<<8)|(coeff[1]&0xFF);
	LOG_printf(&trace, "DRC LPF C74 b0h = 0x%08x", lpf[0]);
	AIC3254_Read(0x42, &coeff[0], hi2c);
	AIC3254_Read(0x43, &coeff[1], hi2c);
	lpf[1] = (coeff[0]<<8)|(coeff[1]&0xFF);
	LOG_printf(&trace, "DRC LPF C74 b0l = 0x%08x", lpf[1]);
	AIC3254_Read(0x44, &coeff[0], hi2c);
	AIC3254_Read(0x45, &coeff[1], hi2c);
	lpf[2] = (coeff[0]<<8)|(coeff[1]&0xFF);
	LOG_printf(&trace, "DRC LPF C75 b1h = 0x%08x", lpf[2]);
	AIC3254_Read(0x46, &coeff[0], hi2c);
	AIC3254_Read(0x47, &coeff[1], hi2c);
	lpf[3] = (coeff[0]<<8)|(coeff[1]&0xFF);
	LOG_printf(&trace, "DRC LPF C75 b1l = 0x%08x", lpf[3]);
	AIC3254_Read(0x48, &coeff[0], hi2c);
	AIC3254_Read(0x49, &coeff[1], hi2c);
	lpf[4] = (coeff[0]<<8)|(coeff[1]&0xFF);
	LOG_printf(&trace, "DRC LPF C76 a1h = 0x%08x", lpf[4]);
	AIC3254_Read(0x4A, &coeff[0], hi2c);
	AIC3254_Read(0x4B, &coeff[1], hi2c);
	lpf[5] = (coeff[0]<<8)|(coeff[1]&0xFF);
	LOG_printf(&trace, "DRC LPF C76 a1l = 0x%08x", lpf[5]);
	return result;
}
/**
 *  \brief Codec write function
 *
 *  Function to write a byte of data to a codec register.
 *
 *  \param regAddr  [IN]  Address of the register to write the data
 *  \param regData  [IN]  Data to write into the register
 *
 *  \return PSP_SOK - if successful, else suitable error code
 */
PSP_Result AIC3254_Write(Uint16 regAddr, Uint16 regData, PSP_Handle hi2c)
{
    PSP_Result    status;
    Uint16        writeCount;
    Uint16        writeBuff[2];

    status = PSP_E_INVAL_PARAM;

    //if(hi2c != NULL)
    {
        writeCount  =  2;
        /* Initialize the buffer          */
        /* First byte is Register Address */
        /* Second byte is register data   */
        writeBuff[0] = (regAddr & 0x00FF);
        writeBuff[1] = (regData & 0x00FF);

        /* Write the data */
        status = I2C_Write(hi2c, I2C_CODEC_ADDR, writeCount, writeBuff);
    }

    return status;
}

/**
 *  \brief Codec read function
 *
 *  Function to read a byte of data from a codec register.
 *
 *  \param regAddr  [IN]  Address of the register to read the data
 *  \param data     [IN]  Pointer to the data read from codec register
 *
 *  \return PSP_SOK - if successful, else suitable error code
 */
PSP_Result AIC3254_Read(Uint16 regAddr, Uint16 *data, PSP_Handle  hi2c)
{
    PSP_Result status  = PSP_E_INVAL_PARAM;
    Uint16 readCount = 1;
    Uint16 readBuff[1];

    regAddr = (regAddr & 0x00FF);

   if(hi2c)
     status = I2C_Read(hi2c,
                    I2C_CODEC_ADDR,
                    readCount,
                    regAddr,
                    readBuff);

    if(status == PSP_SOK)
     *data = readBuff[1];

    return status;
}


Uint16   codec_Ioctl(Uint16 regAddr, Uint16 regData, PSP_Handle hi2c)
{
    return PSP_SOK;
}
