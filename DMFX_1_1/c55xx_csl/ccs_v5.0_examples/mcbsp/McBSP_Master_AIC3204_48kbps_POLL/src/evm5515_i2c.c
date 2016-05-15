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
 *  I2C implementation
 *
 */
#include "evm5515_i2c.h"

Int32 i2c_timeout = 0x0fff;

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _I2C_init( )                                                            *
 *                                                                          *
 *      Enable and initalize the I2C module                                 *
 *      The I2C clk is set to run at 20 KHz                                 *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVM5515_I2C_init( )
{
    I2C_MDR = 0x0400;             // Reset I2C
    I2C_PSC   = 15;               // Config prescaler for 100MHz
    I2C_CLKL  = 25;               // Config clk LOW for 100kHz
    I2C_CLKH  = 25;               // Config clk HIGH for 100kHz

    I2C_MDR   = 0x0420   ;        // Release from reset; Master, Transmitter, 7-bit address

    return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _I2C_close( )                                                           *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVM5515_I2C_close( )
{
    I2C_MDR = 0;                      // Reset I2C
    return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _I2C_reset( )                                                           *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVM5515_I2C_reset( )
{
    EVM5515_I2C_close( );
    EVM5515_I2C_init( );
    return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _I2C_write( i2c_addr, data, len )                                       *
 *                                                                          *
 *      I2C write in Master mode                                            *
 *                                                                          *
 *      i2c_addr    <- I2C slave address                                    *
 *      data        <- I2C data ptr                                         *
 *      len         <- # of bytes to write                                  *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVM5515_I2C_write( Uint16 i2c_addr, Uint8* data, Uint16 len )
{
    Int16 timeout, i;

		//I2C_IER = 0x0000;
        I2C_CNT = len;                    // Set length
        I2C_SAR = i2c_addr;               // Set I2C slave address
        I2C_MDR = MDR_STT                 // Set for Master Write
                  | MDR_TRX
                  | MDR_MST
                  | MDR_IRS
                  | MDR_FREE;

        EVM5515_wait(100);              // Short delay

        for ( i = 0 ; i < len ; i++ )
        {
           I2C_DXR = data[i];            // Write
            timeout = 0x7fff;//i2c_timeout;
            do
            {
                if ( timeout-- < 0  )
                {
                    EVM5515_I2C_reset( );
                    return -1;
                }
            } while ( ( I2C_STR & STR_XRDY ) == 0 );// Wait for Tx Ready
        }

        I2C_MDR |= MDR_STP;             // Generate STOP

		EVM5515_waitusec(1000);

        return 0;

}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _I2C_read( i2c_addr, data, len )                                        *
 *                                                                          *
 *      I2C read in Master mode                                             *
 *                                                                          *
 *      i2c_addr    <- I2C slave address                                    *
 *      data        <- I2C data ptr                                         *
 *      len         <- # of bytes to write                                  *
 *                                                                          *
 *      Returns:    0: PASS                                                 *
 *                 -1: FAIL Timeout                                         *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 EVM5515_I2C_read( Uint16 i2c_addr, Uint8* data, Uint16 len )
{
    Int32 timeout, i;

    I2C_CNT = len;                    // Set length
    I2C_SAR = i2c_addr;               // Set I2C slave address
    I2C_MDR = MDR_STT                 // Set for Master Read
              | MDR_MST
              | MDR_IRS
              | MDR_FREE;

    EVM5515_wait( 10 );            // Short delay

    for ( i = 0 ; i < len ; i++ )
    {
        timeout = i2c_timeout;

        //Wait for Rx Ready
        do
        {
            if ( timeout-- < 0 )
            {
                EVM5515_I2C_reset( );
                return -1;
            }
        } while ( ( I2C_STR & STR_RRDY ) == 0 );// Wait for Rx Ready

        data[i] = I2C_DRR;            // Read
    }

    I2C_MDR |= MDR_STP;               // Generate STOP

	EVM5515_waitusec(10);
    return 0;
}
