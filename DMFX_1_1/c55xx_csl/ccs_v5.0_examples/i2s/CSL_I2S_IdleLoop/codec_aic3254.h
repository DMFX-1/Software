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

#ifndef _CODEC_AIC3204_H_
#define _CODEC_AIC3204_H_

#include "tistdtypes.h"

#include "cslr_gpio.h"

#include "csl_i2c.h"
#include "csl_gpio.h"

CSL_Status AIC3254_init(Uint16 sampRatePb, Uint16 sampRateRec, Uint32 i2cInClk);
Bool Set_Mute_State(Bool flag);
Bool Adjust_Volume(Int16 volume, Uint16 channel);
CSL_Status AIC3254_Write(Uint16 regAddr, Uint16 regData, pI2cHandle hi2c);
CSL_Status AIC3254_Write_Two(Uint16 regAddr1, Uint16 regData1, Uint16 regAddr2, Uint16 regData2, pI2cHandle hi2c);
CSL_Status AIC3254_Read(Uint16 regAddr, Uint16 *Data, pI2cHandle  hi2c);
CSL_Status EEPROM_Write(Uint16 regAddr, Uint16 regData, pI2cHandle hi2c);

#endif /* _CODEC_AIC3204_H_ */
