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

/*
 *  AIC3204 Test
 *
 */

#define AIC3204_I2C_ADDR 0x18
#include "evm5515.h"
#include "evm5515_gpio.h"
#include "evm5515_i2c.h"
#include "stdio.h"
#include "csl_general.h"

extern Int16 aic3204_tone_headphone( );
extern Int16 aic3204_tone_stereo_out( );
extern Int16 aic3204_loop_stereo_in1( );
extern Int16 aic3204_loop_stereo_in2( );
extern Int16 aic3204_loop_mic_in( );
/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _AIC3204_rget( regnum, regval )                                         *
 *                                                                          *
 *      Return value of codec register regnum                               *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 AIC3204_rget(  Uint16 regnum, Uint16* regval )
{
    Int16 retcode = 0;
    Uint8 cmd[2];

    cmd[0] = regnum & 0x007F;       // 7-bit Device Address
    cmd[1] = 0;

    retcode |= EVM5515_I2C_write( AIC3204_I2C_ADDR, cmd, 1 );
    retcode |= EVM5515_I2C_read( AIC3204_I2C_ADDR, cmd, 1 );

    *regval = cmd[0];
    EVM5515_wait( 10 );
    return retcode;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _AIC3204_rset( regnum, regval )                                         *
 *                                                                          *
 *      Set codec register regnum to value regval                           *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 AIC3204_rset( Uint16 regnum, Uint16 regval )
{
    Uint8 cmd[2];
    cmd[0] = regnum & 0x007F;       // 7-bit Register Address
    cmd[1] = regval;                // 8-bit Register Data

    return EVM5515_I2C_write( AIC3204_I2C_ADDR, cmd, 2 );
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  aic3204_test( )                                                           *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 aic3204_test( )
{
	/* Configure Parallel Port */
    SYS_EXBUSSEL &= ~0x7000;     // Set parallel port to 000
    //SYS_EXBUSSEL |=  0x1000;   // Configure Parallel Port for I2S2
    /* Configure Serial Port */
	SYS_EXBUSSEL &= ~0x0F00;   //
	SYS_EXBUSSEL |=  0x0B00;   // Serial Port 1 mode 3 (McBSP and GP[5:4]).
	//EVM5515_GPIO_init();
	//EVM5515_GPIO_setDirection(GPIO10, GPIO_OUT);
	//EVM5515_GPIO_setOutput( GPIO10, 1 );    // Take AIC3201 chip out of reset
	EVM5515_I2C_init( );                    // Initialize I2C

    /* Codec tests */
   /* printf( " -> 1 KHz Tone on Headphone.\n" );
    if ( aic3204_tone_headphone( ) )
        return 1;

    EVM5515_wait( 100 );  // Wait
    printf( " -> 1 KHz Tone on Stereo OUT.\n" );
    if ( aic3204_tone_stereo_out( ) )
        return 1;

    EVM5515_wait( 100 );  // Wait
    printf( "<-> Audio Loopback from Stereo IN 2 --> to Stereo OUT\n" );
    if ( aic3204_loop_stereo_in2( ) )
        return 1;

    EVM5515_wait( 100 );  // Wait
    printf( "<-> Microphone --> to HP\n" );
    if ( aic3204_loop_mic_in( ) )
       return 1;
    */

    EVM5515_wait( 100 );  // Wait
    printf( "<-> Audio Loopback from Stereo IN 1 -->CODEC-->C5525(McBSP)-->CODEC--> HP\n" );
#if (defined(CHIP_C5517))
    if ( aic3204_loop_stereo_in1( ) )
        return 1;


	EVM5515_GPIO_setOutput( GPIO26, 0 );
    return 0;
#else
	return 0;
#endif
}
