/*
 * $$$MODULE_NAME codec_aic3254.h
 *
 * $$$MODULE_DESC codec_aic3254.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

#ifndef _CODEC_AIC3204_H_
#define _CODEC_AIC3204_H_

#include "psp_common.h"

#define I2C_OWN_ADDR            (0x2F)
#define I2C_BUS_FREQ            (10000u)
#define I2C_CODEC_ADDR          (0x18)
#define GAIN_IN_dB         0			// -12dB <= GAIN_IN_dB <= 35.5dB
#define ADC_GAIN		   2*(GAIN_IN_dB+12)

PSP_Result Beep_Generator(Int16 freq, Int16 len);
PSP_Result AIC3254_init(void);
PSP_Result ReadFilterCoeffs(PSP_Handle hi2c);
Bool Set_Mute_State(Bool flag);
Bool Adjust_Volume(Int16 volume, Uint16 channel);
PSP_Result AIC3254_Write(Uint16 regAddr, Uint16 regData, PSP_Handle hi2c);
PSP_Result AIC3254_Read(Uint16 regAddr, Uint16 *Data,PSP_Handle  hi2c);

#endif /* _CODEC_AIC3204_H_ */
