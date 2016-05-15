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
 *  GPIO implementation
 *
 */

#include "evm5515_gpio.h"

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _GPIO_init( )                                                           *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVM5515_GPIO_init()
{
    return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _GPIO_setDirection( number, direction )                                 *
 *                                                                          *
 *      number    <- GPIO#                                                  *
 *      direction <- 0:OUT 1:IN                                             *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVM5515_GPIO_setDirection( Uint16 number, Uint16 direction )
{

    Uint32 bank_id = ( number >> 4);
    Uint32 pin_id  = ( 1 << ( number & 0xF ) );

    if (bank_id == 0)
        if ((direction & 1) == GPIO_IN)
    	    SYS_GPIO_DIR0 &= ~pin_id;
        else
            SYS_GPIO_DIR0 |= pin_id;

    if (bank_id == 1)
        if ((direction & 1) == GPIO_IN)
    	    SYS_GPIO_DIR1 &= ~pin_id;
        else
            SYS_GPIO_DIR1 |= pin_id;

    return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _GPIO_setOutput( number, output )                                       *
 *                                                                          *
 *      number   <- GPIO#                                                   *
 *      value    <- 0:LOW 1:HIGH                                            *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVM5515_GPIO_setOutput( Uint16 number, Uint16 output )
{
    Uint32 bank_id = ( number >> 4 );
    Uint32 pin_id  = ( 1 << ( number & 0xF ) );

    if (bank_id == 0)
        if ((output & 1) == 0)
    	    SYS_GPIO_DATAOUT0 &= ~pin_id;
        else
            SYS_GPIO_DATAOUT0 |= pin_id;

    if (bank_id == 1)
        if ((output & 1) == 0)
    	    SYS_GPIO_DATAOUT1 &= ~pin_id;
        else
            SYS_GPIO_DATAOUT1 |= pin_id;

    return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _GPIO_getInput( number )                                                *
 *                                                                          *
 *      number   <- GPIO#                                                   *
 *                                                                          *
 *      Returns:    0:LOW                                                   *
 *                  1:HIGH                                                  *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVM5515_GPIO_getInput( Uint16 number )
{
    Uint32 input;
    Uint32 bank_id = ( number >> 4 );
    Uint32 pin_id  = ( number & 0xF );

    if (bank_id == 0)
        input = (SYS_GPIO_DATAIN0 >> pin_id) & 1;
    if (bank_id == 1)
        input = (SYS_GPIO_DATAIN1 >> pin_id) & 1;

    return input;
}
