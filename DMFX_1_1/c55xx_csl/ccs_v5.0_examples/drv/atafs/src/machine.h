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

/********************************************************************
 * (c)2001, Texas Instruments Inc.                                  *
 * Digital Audio Group                                              *
 *                                                                  *
 * Author: Jason Lin                                                *
 * File: machine.h                                                  *
 * Description:                                                     *
 *  This file should NOT be documented by Doxygen.  It is solely a  *
 *  bunch of machine specific definitions.  It basically makes it   *
 *  simple to port between PC, DSPs, and other micro.               *
 ********************************************************************/
#ifndef _MACHINE_H_
#define _MACHINE_H_

#ifdef __TMS320C55X__

#ifndef _Uint16_
#define _Uint16_
typedef unsigned short  Uint16;
#endif

#ifndef _Uint32_
#define _Uint32_
typedef unsigned long   Uint32;
#endif

#ifndef _Uchar_
#define _Uchar_
typedef unsigned char   Uchar;
#endif
#endif

#ifndef NULL
#define NULL 0
#endif

#endif /* _MACHINE_H_ */

