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

#ifndef _SAMPLE_RATE_H_
#define _SAMPLE_RATE_H_

#define RATE_1KHZ               ( 1000 )

/* Audio sampling rates */
#define SAMP_RATE_8KHZ          ( 0x1F40 )
#define SAMP_RATE_16KHZ         ( 0x3E80 )
#define SAMP_RATE_22_05KHZ      ( 0x5622 )
#define SAMP_RATE_24KHZ         ( 0x5DC0 )
#define SAMP_RATE_32KHZ         ( 0x7D00 )
#define SAMP_RATE_44_1KHZ       ( 0xAC44 )
#define SAMP_RATE_48KHZ         ( 0xBB80 )

/* Audio sampling rates -- samples/msec. */
#define SAMPS_PER_MSEC_8KHZ     ( SAMP_RATE_8KHZ / RATE_1KHZ )
#define SAMPS_PER_MSEC_16KHZ    ( SAMP_RATE_16KHZ / RATE_1KHZ )
#define SAMPS_PER_MSEC_22_05KHZ ( SAMP_RATE_22_05KHZ / RATE_1KHZ )
#define SAMPS_PER_MSEC_24KHZ    ( SAMP_RATE_24KHZ / RATE_1KHZ )
#define SAMPS_PER_MSEC_32KHZ    ( SAMP_RATE_32KHZ / RATE_1KHZ )
#define SAMPS_PER_MSEC_44_1KHZ  ( SAMP_RATE_44_1KHZ / RATE_1KHZ )
#define SAMPS_PER_MSEC_48KHZ    ( SAMP_RATE_48KHZ / RATE_1KHZ )

#endif /* _SAMPLE_RATE_H_ */
