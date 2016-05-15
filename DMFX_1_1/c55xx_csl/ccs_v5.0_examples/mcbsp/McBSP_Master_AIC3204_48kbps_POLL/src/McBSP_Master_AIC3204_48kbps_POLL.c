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

/** @file McBSP_Master_AIC3204_48kbps_POLL.c
 *
 *  @brief Test code to verify the interaction between McBSP and the audio codec
 *  in Polled Mode
 *
 * \page    page9  McBSP EXAMPLE DOCUMENTATION
 *
 * \section McBSP2   McBSP EXAMPLE2 - Audio polling mode
 *
 * \subsection McBSP2x    TEST DESCRIPTION:
 * This test verifies the operation of the C5517 McBSP (Multichannel Buffered
 * Serial Port) module in polling mode along with the AIC3204.
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSION C5517.
 * MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 *
 * \subsection McBSP2y    TEST PROCEDURE:
 *  @li Open the CCS and connect the target (C5517 EVM)
 *  @li Open the project "CSL_McBSP_Master_AIC3204_48kbps_POLL.pjt" and build it
 *  @li Load the program on to the target
 *  @li Set the PLL frequency to 12.288MHz
 *  @li Connect the audio source to J9 (stereo in 1)
 *  @li Connect the headphones to J4 (headphone)
 *  @li Run the program, start playing a song, and observe the test result
 *  @li Repeat the test at PLL frequencies 60, 100, 150MHz and 200 MHz
 *  @li Repeat the test in Release mode
 *
 * \subsection I2C2z    TEST RESULT:
 *  @li The song played on the audio source should play clearly through the EVM
 *
 * ============================================================================
 */

/* ============================================================================
 * Revision History
 * ================
 * 10-Jul-2012 Added C5517 Documentation
 * ============================================================================
 */

#include "csl_general.h"
#include "stdio.h"
//#include "evm5515.h"
#include "tistdtypes.h"
#include <log.h>
#include <tsk.h>
#include <csl_mcbsp.h>

#define CSL_MCBSP_BUF_SIZE (1u)

extern CSL_McbspHandle  hMcbsp;

/** Buffer to store the Audio data that is read from the Input port of the
    Audio Codec */
Uint32 gmcbspReadBuf[CSL_MCBSP_BUF_SIZE]={0};

/** Semaphore used to indicate that the Data is read from the input port of the
    codec */
extern SEM_Obj readInputData;
/** Semaphore used to indicate that the Data is sent to the output port of the
    codec */
extern SEM_Obj sendOutputData;

/** Global Variable to indicate that the McBSP is configured successfully, and
    the transfer of the audio samples can be started to/from the codec */
volatile Int16 startReceiving = 0;

extern void csl_c5517evm_u44(void);
extern Int16 aic3204_test( );

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  Testing Function                                                        *
 *                                                                          *
 * ------------------------------------------------------------------------ */
void TEST_execute( Int16 ( *funchandle )( ), char *testname, Int16 testid )
{
    Int16 status;

    /* Display test ID */
    printf( "%02d  Testing %s...\n", testid, testname );

    /* Call test function */
    status = funchandle( );

    /* Check for test fail */
    if ( status != 0 )
    {
        /* Print error message */
        printf( "     FAIL... error code %d... quitting\n", status );

        /* Software Breakpoint to Code Composer */
    }
    else
    {
        /* Print error message */
        printf( "%s Test PASSED\n",testname );
    }
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  main( )                                                                 *
 *                                                                          *
 * ------------------------------------------------------------------------ */
void main( void )
{
    /* Initialize BSL */
#if (defined(CHIP_C5517))
    EVM5515_init( );
    printf( "\n***McBSP Codec Test***\n\n" );
    TEST_execute( aic3204_test, "AIC3204", 1 );

    printf( "\n***ALL Tests Passed***\n" );
#else
	printf( "\n***Test not supported by your chip selection***\n" );
#endif
}

/**
 * \brief Task to start reading Audio Data samples from the Input port of the
 * audio codec
 *
 * \param NONE
 *
 * \return NONE
 */
void startReceivingData()
{
	Uint32 data;

#if (defined(CHIP_C5517))
	while (startReceiving)
	{
		SEM_pend(&readInputData, SYS_FOREVER);

		while ((hMcbsp->Regs->SPCRL & 0x0002) == 0) ;

		data = (((Uint32)hMcbsp->Regs->DRRU) << 16);
		data |= hMcbsp->Regs->DRRL;

		gmcbspReadBuf[0] = data;

		SEM_post(&sendOutputData);
	}
#endif  /* #if (defined(CHIP_C5517)) */
}

/**
 * \brief Task to start sending Audio Data samples to the Output port of the
 * audio codec
 *
 * \param NONE
 *
 * \return NONE
 */
void startSendingData()
{
#if (defined(CHIP_C5517))
	while(1)
	{
		SEM_post(&readInputData);

		SEM_pend(&sendOutputData, SYS_FOREVER);

		while ((hMcbsp->Regs->SPCRU & 0x0002) == 0) ;

		hMcbsp->Regs->DXRL =  (gmcbspReadBuf[0] & 0xFFFF);
		hMcbsp->Regs->DXRU = ((gmcbspReadBuf[0] >> 16) & 0xFFFF);
	}
#endif  /* #if (defined(CHIP_C5517)) */
}
