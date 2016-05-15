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
 *  Board Setup
 *
 */

#include "evm5515.h"

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  EVM5515_wait( delay )                                                          *
 *                                                                          *
 *      Wait in a software loop for 'x' delay                               *
 *                                                                          *
 * ------------------------------------------------------------------------ */
void EVM5515_wait( Uint32 delay )
{
    volatile Uint32 i;
    for ( i = 0 ; i < delay ; i++ ){ };
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _waitusec( usec )                                                       *
 *                                                                          *
 *      Wait in a software loop for 'x' microseconds                        *
 *                                                                          *
 * ------------------------------------------------------------------------ */
void EVM5515_waitusec( Uint32 usec )
{
    EVM5515_wait( (Uint32)usec * 8 );
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  EVM5515_init( )                                                      *
 *                                                                          *
 *      Setup board board functions                                         *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVM5515_init( )
{
    /* Enable clocks to all peripherals */
    SYS_PCGCR1 = 0x0000;
    SYS_PCGCR2 = 0x0000;

    return 0;
}
