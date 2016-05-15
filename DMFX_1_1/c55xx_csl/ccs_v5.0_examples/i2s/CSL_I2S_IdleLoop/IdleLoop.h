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

#ifndef IDLELOOP_H_
#define IDLELOOP_H_

/* DSP LDO setting */
//#define DSP_LDO                 ( 105 )
#define DSP_LDO                 ( 130 )

/* PLL output MHz setting */
#define PLL_MHZ                 ( 100 )

#define NUM_SAMP_PER_MS         ( 16 )  // samples per msec
#define NUM_MS_PER_FRAME        ( 20 )  // msec per frame
#define FRAME_LEN               ( NUM_SAMP_PER_MS * NUM_MS_PER_FRAME)   // frame length

#define NUM_FRAMES_OUT_CIRCBUF  ( 10 )  // 10 frames in output circular buffer
#define OUT_CIRCBUF_LEN         ( NUM_SAMP_PER_MS * NUM_MS_PER_FRAME * NUM_FRAMES_OUT_CIRCBUF)  // output circular buffer length

/* Ping pong buffer size, one frame for ping, one frame for pong */
#define I2S_DMA_BUF_LEN         ( FRAME_LEN*2 )

#endif /*IDLELOOP_H_*/
