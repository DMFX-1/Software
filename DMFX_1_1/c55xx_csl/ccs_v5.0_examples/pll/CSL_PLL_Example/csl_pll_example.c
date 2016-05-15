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

/** @file csl_pll_example.c
 *
 *  @brief PLL functional layer sample source file
 *
 *
 * \page    page14  CSL PLL EXAMPLE DOCUMENTATION
 *
 * \section PLL   PLL EXAMPLE
 *
 * \subsection PLLx    TEST DESCRIPTION:
 *		This test code verifies the functionality of CSL PLL module. PLL module
 * on the C5535/C5515/C5517 DSP is used to generate clock for CPU and peripherals.
 * A 32KHz input clock is supplied to the PLL using which different system clock
 * values are generated. Different system clock values are generated by
 * configuring the PLL to different divider values.
 *
 * During the test PLL module will be configured to the 60MHz clock frequency
 * using PLL_config() API. Configured values are read back and verified using
 * PLL_getConfig() APIs. All the values should match the configured values
 * except the test lock mon value which will reflect on the registers only
 * after the PLL is up. Values read from the PLL are displayed on the CCS
 * "stdout" window. Manual inspection is required to verify the test success.
 *
 * C5515 DSP PLL register bit fileds are little different than that of C5535 DSP.
 * Use the 'PLL_Config' values defined 12.288MHz - 120MHz to verify PLL
 * configuration.
 * C5517 DSP PLL register bit fields are different, too.
 * Use the 'PLL_Config' values defined 12.288MHz-200MHz to verify PLL configuration
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5535, C5515
 * AND C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 *
 * \subsection PLLy    TEST PROCEDURE:
 *  @li Open the CCS and connect the target (C5535/C5515/C5517 EVM)
 *  @li Open the project "CSL_PLL_Example.pjt" and build it
 *  @li Load the program on to the target
 *  @li Run the program and observe the test result
 *  @li Repeat the test at the following PLL frequencies
 *      C5515: 60MHz and 100MHz
 *      C5517: 60MHz, 100MHz, 150MHz and 200MHz
 *      C5535 eZdsp: 60MHz and 100MHz
 *  @li Repeat the test in Release mode
 *
 * \subsection PLLz    TEST RESULT:
 *  @li All the CSL APIs should return success
 *  @li Configuration values read from the PLL should match with the actual
 *      configured values except the test lock mon value.
 *
 */

/* ============================================================================
 * Revision History
 * ================
 * 15-Sept-2008 Created
 * 20-Jul-2012 Added C5517 Compatibility
 * ============================================================================
 */

#include<stdio.h>
#include "csl_pll.h"
#include "csl_general.h"
#include "csl_pllAux.h"
#include "cslr_sysctrl.h"
PLL_Obj pllObj;
PLL_Config pllCfg1;

PLL_Handle hPll;

#if (defined(CHIP_C5505_C5515) || defined(CHIP_C5504_C5514))

PLL_Config pllCfg_12p288MHz = {0x8173, 0x8000, 0x0806, 0x0000};
PLL_Config pllCfg_40MHz     = {0x8988, 0x8000, 0x0806, 0x0201};
PLL_Config pllCfg_60MHz     = {0x8724, 0x8000, 0x0806, 0x0000};
PLL_Config pllCfg_75MHz     = {0x88ED, 0x8000, 0x0806, 0x0000};
PLL_Config pllCfg_100MHz    = {0x8BE8, 0x8000, 0x0806, 0x0000};
PLL_Config pllCfg_120MHz    = {0x8E4A, 0x8000, 0x0806, 0x0000};

#elif defined(CHIP_C5517)

PLL_Config pllCfg_25MHz 	= {0x28AB, 0x0003, 0x0000, 0x0024};
PLL_Config pllCfg_50MHz 	= {0x3100, 0x0003, 0x0000,  0x0022};
PLL_Config pllCfg_100MHz 	= {0x41AB, 0x0003, 0x0000,  0x0000};
PLL_Config pllCfg_150MHz 	= {0x3100, 0x0003, 0x0000,  0x0020};
PLL_Config pllCfg_175MHz 	= {0x3956, 0x0003, 0x0000,  0x0020};
PLL_Config pllCfg_200MHz 	= {0x41AB, 0x0003, 0x0000, 0x0020};

#else

PLL_Config pllCfg_12p288MHz = {0x82ED, 0x8000, 0x0806, 0x0200};
PLL_Config pllCfg_40MHz     = {0x8262, 0x8000, 0x0806, 0x0300};
PLL_Config pllCfg_60MHz     = {0x81C8, 0xB000, 0x0806, 0x0000};
PLL_Config pllCfg_75MHz     = {0x823B, 0x9000, 0x0806, 0x0000};
PLL_Config pllCfg_100MHz    = {0x82FA, 0x8000, 0x0806, 0x0000};
PLL_Config pllCfg_120MHz    = {0x8392, 0xA000, 0x0806, 0x0000};

#endif

PLL_Config *pConfigInfo;

#define CSL_TEST_FAILED         (1)
#define CSL_TEST_PASSED         (0)

CSL_Status pll_sample()
{
    CSL_Status status;

    status = PLL_init(&pllObj, CSL_PLL_INST_0);
    if(CSL_SOK != status)
    {
       printf("PLL init failed \n");
       return (status);
    }

	hPll = (PLL_Handle)(&pllObj);

	PLL_reset(hPll);

   /* Configure the PLL for 100MHz */
   pConfigInfo = &pllCfg_100MHz;

   status = PLL_config (hPll, pConfigInfo);
   if(CSL_SOK != status)
   {
       printf("PLL config failed\n");
       return(status);
   }

	status = PLL_getConfig(hPll, &pllCfg1);
    if(status != CSL_SOK)
	{
	    printf("TEST FAILED: PLL get config... Failed.\n");
		printf ("Reason: PLL_getConfig failed. [status = 0x%x].\n", status);
		return(status);
	}

    printf("REGISTER --- CONFIG VALUES\n");
#if (defined(CHIP_C5517))
    printf("%04x --- %04x\n",pllCfg1.PLLMR,hPll->pllConfig->PLLMR);
    printf("%04x --- %04x\n",pllCfg1.PLLICR,hPll->pllConfig->PLLICR);
    printf("%04x --- %04x\n",pllCfg1.PLLODR,hPll->pllConfig->PLLODR);
#else
    printf("%04x --- %04x\n",pllCfg1.PLLCNTL1,hPll->pllConfig->PLLCNTL1);
    printf("%04x --- %04x Test Lock Mon will get set after PLL is up\n",
           pllCfg1.PLLCNTL2,hPll->pllConfig->PLLCNTL2);
    printf("%04x --- %04x\n",pllCfg1.PLLINCNTL,hPll->pllConfig->PLLINCNTL);
    printf("%04x --- %04x\n",pllCfg1.PLLOUTCNTL,hPll->pllConfig->PLLOUTCNTL);
#endif
   status = PLL_bypass(hPll);
   if(CSL_SOK != status)
   {
       printf("PLL bypass failed:%d\n",CSL_ESYS_BADHANDLE);
       return(status);
   }

   status = PLL_enable(hPll);
   if(CSL_SOK != status)
   {
       printf("PLL enable failed:%d\n",CSL_ESYS_BADHANDLE);
       return(status);
   }

   return(CSL_TEST_PASSED);
}

   /////INSTRUMENTATION FOR BATCH TESTING -- Part 1 --
   /////  Define PaSs_StAtE variable for catching errors as program executes.
   /////  Define PaSs flag for holding final pass/fail result at program completion.
        volatile Int16 PaSs_StAtE = 0x0001; // Init to 1. Reset to 0 at any monitored execution error.
        volatile Int16 PaSs = 0x0000; // Init to 0.  Updated later with PaSs_StAtE when and if
   /////                                  program flow reaches expected exit point(s).
   /////
void main(void)
{
    CSL_Status status;

	printf("CSL PLL TEST\n\n");

    status = pll_sample();

	asm("	nop");
    if(status == CSL_TEST_PASSED)
	{
        printf("\nCSL PLL TEST PASSED\n");
	}
    else
	{
        printf("\nCSL PLL TEST FAILED\n");
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

