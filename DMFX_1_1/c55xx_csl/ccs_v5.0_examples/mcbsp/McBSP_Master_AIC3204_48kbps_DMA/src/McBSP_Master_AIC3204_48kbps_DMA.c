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

/** @file McBSP_Master_AIC3204_48kbps_DMA.c
 *
 *  @brief Test code to verify the interaction between McBSP and the audio codec
 *  in DMA Mode
 *
 * \page    page9  McBSP EXAMPLE DOCUMENTATION
 *
 * \section McBSP3   McBSP EXAMPLE3 - Audio DMA mode
 *
 * \subsection McBSP3x    TEST DESCRIPTION:
 * This test verifies the operation of the C5517 McBSP (Multichannel Buffered
 * Serial Port) module in DMA mode along with the AIC3204.
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSION C5517.
 * MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 *
 * \subsection McBSP3y    TEST PROCEDURE:
 *  @li Open the CCS and connect the target (C5517 EVM)
 *  @li Open the project "CSL_McBSP_Master_AIC3204_48kbps_DMA.pjt" and build it
 *  @li Load the program on to the target
 *  @li Set the PLL frequency to 12.288MHz
 *  @li Connect the audio source to J9 (stereo in 1)
 *  @li Connect the headphones to J4 (headphone)
 *  @li Run the program, start playing a song, and observe the test result
 *  @li Repeat the test at PLL frequencies 60, 100, 150MHz and 200 MHz
 *  @li Repeat the test in Release mode
 *
 * \subsection I2C3z    TEST RESULT:
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
#include "evm5515.h"

extern void csl_c5517evm_u44(void);
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
        SW_BREAKPOINT;
    }
    else
    {
        /* Print error message */
        printf( "%s Test PASSED\n",testname );
    }
}

extern Int16 aic3204_test( );

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
    SW_BREAKPOINT;
#else
	printf( "\n***Test not supported by your chip selection***\n" );
#endif
}
