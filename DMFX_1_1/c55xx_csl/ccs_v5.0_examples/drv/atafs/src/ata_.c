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

/********************************************************************************/
/* $Id: ata_.c,2.4+ $                                                           */
/*                                                                              */
/* This is proprietary information, not to be published -- TI INTERNAL DATA     */
/* Copyright (C) 2000, Texas Instruments, Inc.  All Rights Reserved.            */
/*                                                                              */
/* Author: Susmit Pal                                                           */
/* Date: November 6, 2002                                                       */
/* Modified Date: November 28, 2002                                             */
/* Last Modified Date: February 11, 2003                                        */
/********************************************************************************/

#include "ata.h"
#include "ata_.h"

/* #pragma CODE_SECTION (_AtaReadByte, ".text:_AtaReadByte") */
char    gtByte;
AtaUint32   gdword;

char _AtaReadByte(AtaSector PhySector, AtaState *pAtaDrive, AtaUint16 ByteOffset)
{
    AtaUint16 ll1; char tbyte;

    ll1 = _AtaReadWord(PhySector, pAtaDrive, ByteOffset);
    tbyte = (char) ll1;
    tbyte &= 0xFF;
    gtByte = tbyte; //To watch tbyte. Can be removed.
    return(tbyte);
}

/* #pragma CODE_SECTION (_AtaReadDoubleWord, ".text:_AtaReadDoubleWord") */

AtaUint32 _AtaReadDoubleWord(AtaSector PhySector, AtaState* pAtaDrive, AtaUint16 ByteOffset)
{
    AtaUint32 ll2, dword;
    AtaUint16 ll1;

    ll1 = _AtaReadWord(PhySector, pAtaDrive, ByteOffset);
    dword = (AtaUint32)ll1;
    ll1 = _AtaReadWord(PhySector, pAtaDrive, ByteOffset+2);
    ll2 = (AtaUint32)ll1;
    ll2 = ll2 << 16;
    dword |= ll2;

    gdword = dword;

    return(dword);
}

