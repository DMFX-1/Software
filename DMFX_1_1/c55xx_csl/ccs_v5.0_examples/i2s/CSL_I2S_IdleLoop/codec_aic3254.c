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

#include "codec_aic3254.h"
#include "sample_rate.h"

#define ENABLE_RECORD

#define I2C_OWN_ADDR            (0x2F)
#define I2C_CODEC_ADDR          (0x18)
#define I2C_EEPROM_ADDR         (0x50)
#define I2C_BUS_FREQ            (400)   // I2C 400Khz

extern int pll_mhz;

extern pI2cHandle i2cHandle;
pI2cHandle    hi2c = NULL;

/*
 * Mute control for codec output
 * TRUE = Mute codec output
 * FALSE = UnMute codec output
 ***********************************************************************/
Bool Set_Mute_State(Bool flag)
{
    CSL_Status    result = CSL_SOK;
    Bool retval;

    retval = TRUE;

    // write 0 to page register to select page 0
    result = AIC3254_Write(0, 0, hi2c);
    if (result != CSL_SOK)
    {
        retval = FALSE;
    }
    else
    {
        if (flag == TRUE)
        {
            //mute output
            result = AIC3254_Write(64,0xd,hi2c);
            if (result != CSL_SOK)
            {
                retval = FALSE;
            }
        }
        else
        {
            //unmute output
            result = AIC3254_Write(64,0x1,hi2c);
            if (result != CSL_SOK)
            {
                retval = FALSE;
            }
        }
    }
#if 1
    // write 1 to page register to select page 1 - prepare for next headset volume change
    result = AIC3254_Write(0, 1, hi2c);
    if (result != CSL_SOK)
    {
        retval = FALSE;
    }
#endif
    return retval;
}

#define HEADPHONE_DRIVER_GAIN_MUTE_ENABLE  0x40    // bit6 =1 mute headphone driver
#define VOLUME_STEP_SIZE                   256
#define VOLUME_TABLE_MAX_GAIN_INDEX        29      // headphone gain setting = 29 -> 29 dB gain
#define VOLUME_TABLE_MAX_ATTNEUATION_INDEX 35      // headphone gain setting = 0x3A -> -6dB gain
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
    CSL_Status    result = CSL_SOK;
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
            if (gain > VOLUME_TABLE_MAX_ATTNEUATION_INDEX)
            {
                // set to max attenuation
                gain = VOLUME_TABLE_MAX_ATTNEUATION_INDEX;
            }
        }

    }

    if (channel == 0)
    {
        //adjust volume setting of left headphone
        result = AIC3254_Write(0x10,volume_table[gain],hi2c);
        if (result != CSL_SOK)
        {
            return result;
        }
    }
    else
    {
        //adjust volume setting of right headphone
        result = AIC3254_Write(0x11,volume_table[gain],hi2c);
        if (result != CSL_SOK)
        {
            return result;
        }
    }
    return TRUE;
}

#define I2S2_SRGR          *(volatile ioport Uint16*)(0x2A04)
#define I2S2_ICMR          *(volatile ioport Uint16*)(0x2A14)
#define I2S2_CR            *(volatile ioport Uint16*)(0x2A00)

#define I2S0_SRGR          *(volatile ioport Uint16*)(0x2804)
#define I2S0_ICMR          *(volatile ioport Uint16*)(0x2814)
#define I2S0_CR            *(volatile ioport Uint16*)(0x2800)

CSL_I2cSetup     i2cSetup;

CSL_Status AIC3254_init(
    Uint16 sampRatePb,
    Uint16 sampRateRec,
    Uint32 i2cInClk)
{
    CSL_Status result = CSL_SOK;
    volatile Uint32 looper;

    /* Reset AIC3254 */
    /* NOTE: Assumes EBSR and GPIO are set correctly before function is called */
#if (defined(CHIP_C5517))
    CSL_GPIO_REGS->IODATAOUT1 = 0x0000; /* reset active low */
#else
    CSL_GPIO_REGS->IOOUTDATA1 = 0x0000; /* reset active low */
#endif

    for(looper=0; looper<1000; looper++ )
        asm("    nop");

#if (defined(CHIP_C5517))
    CSL_GPIO_REGS->IODATAOUT1 = 0x0400;
#else
    CSL_GPIO_REGS->IOOUTDATA1 = 0x0400;
#endif

    // I2C hardware initialization
    result = I2C_init(CSL_I2C0);
    if (result!=CSL_SOK)
    {
        return result;
    }
    // I2C setup
    i2cSetup.addrMode    = CSL_I2C_ADDR_7BIT;
    i2cSetup.bitCount    = CSL_I2C_BC_8BITS;
    i2cSetup.loopBack    = CSL_I2C_LOOPBACK_DISABLE;
    i2cSetup.freeMode    = CSL_I2C_FREEMODE_DISABLE;
    i2cSetup.repeatMode  = CSL_I2C_REPEATMODE_DISABLE;
    i2cSetup.ownAddr     = I2C_OWN_ADDR;
    i2cSetup.sysInputClk = i2cInClk;
    i2cSetup.i2cBusFreq  = I2C_BUS_FREQ;

    result = I2C_setup(&i2cSetup);
    if(result != CSL_SOK)
    {
        return (result);
    }

    // validate the I2c handle
    hi2c = i2cHandle;

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *   Configure AIC3204                                                      *
 *      CODEC_CLKIN = PLL CLK                                               *
 *      CODEC_CLKIN = NADC * MADC * AOSR * ADCFS                            *
 *      PLL_CLK     = PLL_CLKIN * R * J.D / P                               *
 *      PLL_CLKIN   = BCLK = 1.536MHz                                       *
 *      ADCFS       = 48KHz                                                 *
 *                                                                          *
 * ------------------------------------------------------------------------ */

    result = AIC3254_Write(  0, 0x00, hi2c );      // Select page 0
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write(  1, 0x01, hi2c );      // Reset codec
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write(  0, 0x01, hi2c );      // Select page 1
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write(  1, 0x08, hi2c );      // Disable crude AVDD generation from DVDD
    if (result != CSL_SOK)
    {
        return result;
    }

#ifdef C5535_EZDSP
    result = AIC3254_Write(  2, 0x01, hi2c );      // Disable Analog Blocks
    if (result != CSL_SOK)
    {
        return result;
    }
#else
    result = AIC3254_Write(  2, 0x00, hi2c );      // Enable Analog Blocks
    if (result != CSL_SOK)
    {
        return result;
    }
#endif

    // PLL and Clocks config and Power Up
    result = AIC3254_Write(  0, 0x00, hi2c );      // Select page 0
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write( 27, 0x0D, hi2c );      // BCLK and WCLK is set as i/p to AIC3204(Slave)
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write(  4, 0x03, hi2c );      // PLL setting: PLLCLK <- BCLK and CODEC_CLKIN <-PLL CLK
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write(  6, 0x07, hi2c );      // PLL setting: J = 32
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write(  7, 0, hi2c );         // PLL setting: HI_BYTE(D)
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write(  8, 0, hi2c );         // PLL setting: LO_BYTE(D)
    if (result != CSL_SOK)
    {
        return result;
    }

    // For 48 KHz sampling
    result = AIC3254_Write(  5, 0x91, hi2c );      // PLL setting: Power up PLL, P=1 and R=2
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write( 13, 0x00, hi2c );      // Hi_Byte(DOSR) for DOSR = 128 decimal or 0x0080 DAC oversamppling
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write( 14, 0x7E, hi2c );      // Lo_Byte(DOSR) for DOSR = 128 decimal or 0x0080
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write( 20, 0x7E, hi2c );      // AOSR for AOSR = 128 decimal or 0x0080 for decimation filters 1 to 6
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write( 30, 0xB0, hi2c );      // BCLK value
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write( 11, 0x87, hi2c );      // Power up NDAC and set NDAC value to 4
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write( 12, 0x82, hi2c );      // Power up MDAC and set MDAC value to 2
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write( 18, 0x84, hi2c );      // Power up NADC and set NADC value to 4
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write( 19, 0x82, hi2c );      // Power up MADC and set MADC value to 2
    if (result != CSL_SOK)
    {
        return result;
    }

    // DAC ROUTING and Power Up
    result = AIC3254_Write(  0, 0x01, hi2c );      // Select page 1
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write( 20, 0x25, hi2c );      // LDAC AFIR routed to HPL
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write( 10, 0x00, hi2c );      // RDAC AFIR routed to HPR
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write( 3, 0x08, hi2c );      // Power up NADC and set NADC value to 4
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write( 4, 0x08, hi2c );      // Power up MADC and set MADC value to 2
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write( 12, 0x08, hi2c );      // LDAC AFIR routed to HPL
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write( 13, 0x08, hi2c );      // RDAC AFIR routed to HPR
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write( 14, 0x08, hi2c );      // LDAC AFIR routed to HPL
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write( 15, 0x08, hi2c );      // RDAC AFIR routed to HPR
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write(  0, 0x00, hi2c );      // Select page 0
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write( 60, 0x07, hi2c );      // Left vol=right vol
    if (result != CSL_SOK)
    {
        return result;
    }


    result = AIC3254_Write( 64, 0x02, hi2c );      // Left vol=right vol
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write( 65, 0x2F, hi2c );      // Left DAC gain to 0dB VOL; Right tracks Left
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write( 66, 0x2F, hi2c );      // Left DAC gain to 0dB VOL; Right tracks Left
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write( 63, 0xd4, hi2c );      // Power up left,right data paths and set channel
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write(  0, 0x01, hi2c );      // Select page 1
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write( 16, 0x00, hi2c );      // Unmute HPL , 0dB gain
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write( 17, 0x00, hi2c );      // Unmute HPR , 0dB gain
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write(  9, 0x30, hi2c );      // Power up HPL,HPR
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write(  0, 0x00, hi2c );      // Select page 0
    if (result != CSL_SOK)
    {
        return result;
    }

    for (looper = 0; looper < 500; looper++)
    {
        ; // Wait
    }


    // ADC ROUTING and Power Up
    result = AIC3254_Write(  0, 0x01, hi2c );      // Select page 1
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write( 52, 0x0C, hi2c );      // STEREO 1 Jack
    if (result != CSL_SOK)
    {
        return result;
    }

                                   // IN2_L to LADC_P through 40 kohm
    result = AIC3254_Write( 55, 0x0C, hi2c );      // IN2_R to RADC_P through 40 kohmm
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write( 54, 0x03, hi2c );      // CM_1 (common mode) to LADC_M through 40 kohm
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write( 57, 0xC0, hi2c );      // CM_1 (common mode) to RADC_M through 40 kohm
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write( 59, 0x00, hi2c );      // MIC_PGA_L unmute
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write( 60, 0x00, hi2c );      // MIC_PGA_R unmute
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write(  0, 0x00, hi2c );      // Select page 0
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write( 81, 0xc0, hi2c );      // Powerup Left and Right ADC
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write( 82, 0x00, hi2c );      // Unmute Left and Right ADC
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write( 0,  0x00, hi2c );
    if (result != CSL_SOK)
    {
        return result;
    }

    result = AIC3254_Write( 61,  0x02, hi2c );
    if (result != CSL_SOK)
    {
        return result;
    }

    for (looper = 0; looper < 200; looper++)
    {
        ; // Wait
    }

#if (!defined(CHIP_C5517))
    /* I2S settings */
    I2S2_SRGR = 0x0015;
    I2S2_ICMR = 0x0028;    // Enable interrupts
    I2S2_CR   |= 0x8012;    // 16-bit word, Master, enable I2C
#else
    I2S0_SRGR = 0x0015;
    I2S0_ICMR = 0x0028;    // Enable interrupts
    I2S0_CR   |= 0x8012;    // 16-bit word, Master, enable I2C

#endif

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
 *  \return CSL_SOK - if successful, else suitable error code
 */
CSL_Status AIC3254_Write(Uint16 regAddr, Uint16 regData, pI2cHandle hi2c)
{
    CSL_Status    status;
    Uint16        writeCount;
    volatile Uint16 looper;
    Uint16        writeBuff[2];
    Uint16        readBuff[2];
    Uint16 startStop = ((CSL_I2C_START) | (CSL_I2C_STOP));

    status = CSL_ESYS_INVPARAMS;

    if(hi2c != NULL)
    {
        writeCount  =  2;
        /* Initialize the buffer          */
        /* First byte is Register Address */
        /* Second byte is register data   */
        writeBuff[0] = (regAddr & 0x00FF);
        writeBuff[1] = (regData & 0x00FF);

        /* Write the data */
        status = I2C_write(writeBuff,
                        writeCount,
                        I2C_CODEC_ADDR,
                        TRUE,
                        startStop,
                        CSL_I2C_MAX_TIMEOUT);
        if (status!=CSL_SOK)
        {
            return status;
        }

        /* Give some delay */
        for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++){;}

#if 1
        // read the data
        status = I2C_read(readBuff,
                        1,
                        I2C_CODEC_ADDR,
                        &regAddr,
                        1,
                        TRUE,
                        startStop,
                        CSL_I2C_MAX_TIMEOUT,
                        FALSE);
        if (status!=CSL_SOK)
        {
            return status;
        }

        if ((readBuff[0]!=writeBuff[1])&&(regAddr>=2))
            status |= (-1);
#endif
    }

    return status;
}

CSL_Status AIC3254_Write_Two(Uint16 regAddr1, Uint16 regData1, Uint16 regAddr2, Uint16 regData2, pI2cHandle hi2c)
{
    CSL_Status    status;
    Uint16        writeCount;
    volatile Uint16 looper;
    Uint16        writeBuff1[2];
    Uint16        writeBuff2[2];
    Uint16        readBuff1[2];
    Uint16        readBuff2[2];
    Uint16 startStop = ((CSL_I2C_START) | (CSL_I2C_STOP));

    status = CSL_ESYS_INVPARAMS;

    if(hi2c != NULL)
    {
        writeCount  =  2;
        /* Initialize the buffer          */
        /* First byte is Register Address */
        /* Second byte is register data   */
        writeBuff1[0] = (regAddr1 & 0x00FF);
        writeBuff1[1] = (regData1 & 0x00FF);

        /* Write the data */
        status = I2C_write(writeBuff1,
                        writeCount,
                        I2C_CODEC_ADDR,
                        TRUE,
                        startStop,
                        CSL_I2C_MAX_TIMEOUT);
        if (status!=CSL_SOK)
        {
            return status;
        }

        /* Give some delay */
        for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++){;}

        writeBuff2[0] = (regAddr2 & 0x00FF);
        writeBuff2[1] = (regData2 & 0x00FF);

        /* Write the data */
        status |= I2C_write(writeBuff2,
                        writeCount,
                        I2C_CODEC_ADDR,
                        TRUE,
                        startStop,
                        CSL_I2C_MAX_TIMEOUT);
        if (status!=CSL_SOK)
        {
            return status;
        }

        /* Give some delay */
        for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++){;}

#if 1
        // read the data
        status = I2C_read(readBuff1,
                        1,
                        I2C_CODEC_ADDR,
                        &regAddr1,
                        1,
                        TRUE,
                        startStop,
                        CSL_I2C_MAX_TIMEOUT,
                        FALSE);
        if (status!=CSL_SOK)
        {
            return status;
        }

        /* Give some delay */
        for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++){;}

        if ((readBuff1[0]!=writeBuff1[1])&&(regAddr1>=2))
            status |= (-1);

        // read the data
        status = I2C_read(readBuff2,
                        1,
                        I2C_CODEC_ADDR,
                        &regAddr2,
                        1,
                        TRUE,
                        startStop,
                        CSL_I2C_MAX_TIMEOUT,
                        FALSE);
        if (status!=CSL_SOK)
        {
            return status;
        }

        /* Give some delay */
        for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++){;}

        if ((readBuff2[0]!=writeBuff2[1])&&(regAddr2>=2))
            status |= (-1);
#endif
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
 *  \return CSL_SOK - if successful, else suitable error code
 */
CSL_Status AIC3254_Read(Uint16 regAddr, Uint16 *data, pI2cHandle  hi2c)
{
    CSL_Status status  = CSL_ESYS_INVPARAMS;
    Uint16 readCount = 1;
    volatile Uint16 looper;
    Uint16 readBuff[1];
    Uint16 startStop = ((CSL_I2C_START) | (CSL_I2C_STOP));

    regAddr = (regAddr & 0x00FF);

    if(hi2c)
    {
        // read the data
        status = I2C_read(readBuff,
                        readCount,
                        I2C_CODEC_ADDR,
                        &regAddr,
                        1,
                        TRUE,
                        startStop,
                        CSL_I2C_MAX_TIMEOUT,
                        FALSE);
        if (status!=CSL_SOK)
        {
            return status;
        }

        *data = readBuff[0];

        return status;
    } else
    {
        // illegal I2C handle
        return CSL_ESYS_BADHANDLE;
    }
}

CSL_Status EEPROM_Write(Uint16 regAddr, Uint16 regData, pI2cHandle hi2c)
{
    CSL_Status    status;
    Uint16        writeCount;
    volatile Uint16 looper;
    Uint16        writeBuff[4];
    Uint16        readBuff[4];
    Uint16 startStop = ((CSL_I2C_START) | (CSL_I2C_STOP));

    status = CSL_ESYS_INVPARAMS;

    if(hi2c != NULL)
    {
        writeCount  =  4;
        /* Initialize the buffer          */
        /* First two bytes are Register Address */
        /* Second two bytes are register data   */
        writeBuff[0] = (regAddr & 0x00FF);
        writeBuff[1] = ((regAddr>>8) & 0x00FF);
        writeBuff[2] = (regData & 0x00FF);
        writeBuff[3] = ((regData>>8) & 0x00FF);
        readBuff[0] = (regAddr & 0x00FF);
        readBuff[1] = ((regAddr>>8) & 0x00FF);
        readBuff[2] = 0;
        readBuff[3] = 0;

        /* Write the data */
        status = I2C_write(writeBuff,
                        writeCount,
                        I2C_EEPROM_ADDR,
                        TRUE,
                        startStop,
                        CSL_I2C_MAX_TIMEOUT);
        if (status!=CSL_SOK)
        {
            return status;
        }

        // read the data
        status = I2C_read(&readBuff[2],
                        2,
                        I2C_EEPROM_ADDR,
                        &readBuff[0],
                        1,
                        TRUE,
                        startStop,
                        CSL_I2C_MAX_TIMEOUT,
                        FALSE);
        if (status!=CSL_SOK)
        {
            return status;
        }

        if ((readBuff[2]!=writeBuff[2])||(readBuff[3]!=writeBuff[3]))
            status |= (-1);
    }

    return status;
}


