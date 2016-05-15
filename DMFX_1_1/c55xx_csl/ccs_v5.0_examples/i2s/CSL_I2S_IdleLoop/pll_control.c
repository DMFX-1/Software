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

#include "csl_pll.h"
#include "csl_general.h"
#include "csl_pllAux.h"

PLL_Obj pllObj;
PLL_Config pllCfg1;

PLL_Handle hPll;

#if (defined(CHIP_C5517))
const PLL_Config pllCfg_25MHz 		= {0x28AB, 0x0003, 0x0000, 0x0024};
const PLL_Config pllCfg_50MHz 		= {0x3100, 0x0003, 0x0000, 0x0022};
//const PLL_Config pllCfg_100MHz 		= {0x41AB, 0x0003, 0x0000, 0x0000};
const PLL_Config pllCfg_100MHz 		= {0x41AB, 0x0003, 0x0010, 0x0000};
const PLL_Config pllCfg_150MHz 		= {0x3100, 0x0003, 0x0000, 0x0020};
const PLL_Config pllCfg_175MHz 		= {0x3956, 0x0003, 0x0000, 0x0020};
const PLL_Config pllCfg_200MHz 		= {0x41AB, 0x0003, 0x0000, 0x0020};
#else
const PLL_Config pllCfg_40MHz       = {0x8988, 0x8000, 0x0806, 0x0201};
const PLL_Config pllCfg_50MHz       = {0x8BE8, 0x8000, 0x0806, 0x0201};
const PLL_Config pllCfg_60MHz       = {0x8724, 0x8000, 0x0806, 0x0000};
const PLL_Config pllCfg_75MHz       = {0x88ED, 0x8000, 0x0806, 0x0000};
const PLL_Config pllCfg_100MHz      = {0x8BE8, 0x8000, 0x0806, 0x0000};
const PLL_Config pllCfg_120MHz      = {0x8E4A, 0x8000, 0x0806, 0x0000};
#endif

PLL_Config *pConfigInfo;

// Set the PLL to desired MHZ
// mhz:
//    40 - 40Mhz
//    50 - 50Mhz
//    60 - 60Mhz
//    75 - 75Mhz
//    100 - 100Mhz
//    120 - 120Mhz
//    other - 60Mhz
CSL_Status pll_sample(int mhz)
{
    CSL_Status status;
    volatile int i;

    status = PLL_init(&pllObj, CSL_PLL_INST_0);
    if (status != CSL_SOK)
    {
        return status;
    }

    hPll = (PLL_Handle)(&pllObj);

    PLL_reset(hPll);
/*
    status = PLL_bypass(hPll);
    if (status != CSL_SOK)
    {
        return status;
    }
*/
#if (defined(CHIP_C5517))
    /* Configure the PLL */
    switch (mhz)
    {
    case 25:
    	pConfigInfo = (PLL_Config *)&pllCfg_25MHz;
        break;
        
    case 50:
        pConfigInfo = (PLL_Config *)&pllCfg_50MHz;
        break;

    case 100:
        pConfigInfo = (PLL_Config *)&pllCfg_100MHz;
        break;

    case 150:
        pConfigInfo = (PLL_Config *)&pllCfg_150MHz;
        break;
    
    case 175:
        pConfigInfo = (PLL_Config *)&pllCfg_175MHz;
        break;
    
    case 200:
        pConfigInfo = (PLL_Config *)&pllCfg_200MHz;
        break;
    
    default:
        pConfigInfo = (PLL_Config *)&pllCfg_100MHz;
    }

#else
    /* Configure the PLL */
    switch (mhz)
    {
    case 40:
        pConfigInfo = (PLL_Config *)&pllCfg_40MHz;
        break;
        
    case 50:
        pConfigInfo = (PLL_Config *)&pllCfg_50MHz;
        break;

    case 60:
        pConfigInfo = (PLL_Config *)&pllCfg_60MHz;
        break;

    case 75:
        pConfigInfo = (PLL_Config *)&pllCfg_75MHz;
        break;

    case 100:
        pConfigInfo = (PLL_Config *)&pllCfg_100MHz;
        break;

    case 120:
        pConfigInfo = (PLL_Config *)&pllCfg_120MHz;
        break;
    
    default:
        pConfigInfo = (PLL_Config *)&pllCfg_60MHz;
    }
#endif    

    /* Set the PLL using CSL function */
    status = PLL_config (hPll, pConfigInfo);
    if (status != CSL_SOK)
    {
        return(status);
    }

    /* Read the PLL configure back */
    status = PLL_getConfig(hPll, &pllCfg1);
    if (status != CSL_SOK)
    {
        return status;
    }

    /* Wait for PLL to stabilize */
    for (i=0; i<100; i++);

    /* Enable the PLL */
    status = PLL_enable(hPll);
    if (status != CSL_SOK)
    {
        return status;
    }

    return CSL_SOK;
}
