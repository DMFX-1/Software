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
* file: cslr_tim_1.h
*
* Brief: This file contains the Register Description for tim
*
*********************************************************************/
#ifndef _CSLR_TIM_1_H_
#define _CSLR_TIM_1_H_

#include <cslr.h>

#include <tistdtypes.h>


/* Minimum unit = 2 bytes */

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct  {
    volatile Uint16 TCR;
    volatile Uint16 RSVD0;
    volatile Uint16 TIMPRD1;
    volatile Uint16 TIMPRD2;
    volatile Uint16 TIMCNT1;
    volatile Uint16 TIMCNT2;
    volatile Uint16 TINSR;
} CSL_TimRegs;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* TCR */

#define CSL_TIM_TCR_TIMEN_MASK           (0x8000u)
#define CSL_TIM_TCR_TIMEN_SHIFT          (0x000Fu)
#define CSL_TIM_TCR_TIMEN_RESETVAL       (0x0000u)
/*----TIMEN Tokens----*/
#define CSL_TIM_TCR_TIMEN_DISABLE        (0x0000u)
#define CSL_TIM_TCR_TIMEN_ENABLE         (0x0001u)

#define CSL_TIM_TCR_TIMRST_MASK          (0x4000u)
#define CSL_TIM_TCR_TIMRST_SHIFT         (0x000Eu)
#define CSL_TIM_TCR_TIMRST_RESETVAL      (0x0000u)
/*----TIMRST Tokens----*/
#define CSL_TIM_TCR_TIMRST_CLEAR         (0x0000u)
#define CSL_TIM_TCR_TIMRST_SET           (0x0001u)

#define CSL_TIM_TCR_RSV_MASK             (0x3FC0u)
#define CSL_TIM_TCR_RSV_SHIFT            (0x0006u)
#define CSL_TIM_TCR_RSV_RESETVAL         (0x0000u)

#define CSL_TIM_TCR_PSCDIV_MASK          (0x003Cu)
#define CSL_TIM_TCR_PSCDIV_SHIFT         (0x0002u)
#define CSL_TIM_TCR_PSCDIV_RESETVAL      (0x0000u)

#define CSL_TIM_TCR_AUTORELOAD_MASK      (0x0002u)
#define CSL_TIM_TCR_AUTORELOAD_SHIFT     (0x0001u)
#define CSL_TIM_TCR_AUTORELOAD_RESETVAL  (0x0000u)
/*----AUTORELOAD Tokens----*/
#define CSL_TIM_TCR_AUTORELOAD_DISABLE   (0x0000u)
#define CSL_TIM_TCR_AUTORELOAD_ENABLE    (0x0001u)

#define CSL_TIM_TCR_START_MASK           (0x0001u)
#define CSL_TIM_TCR_START_SHIFT          (0x0000u)
#define CSL_TIM_TCR_START_RESETVAL       (0x0000u)

#define CSL_TIM_TCR_RESETVAL             (0x0000u)

/* TIMPRD1 */

#define CSL_TIM_TIMPRD1_PRD1_MASK        (0xFFFFu)
#define CSL_TIM_TIMPRD1_PRD1_SHIFT       (0x0000u)
#define CSL_TIM_TIMPRD1_PRD1_RESETVAL    (0x0000u)

#define CSL_TIM_TIMPRD1_RESETVAL         (0x0000u)

/* TIMPRD2 */

#define CSL_TIM_TIMPRD2_PRD2_MASK        (0xFFFFu)
#define CSL_TIM_TIMPRD2_PRD2_SHIFT       (0x0000u)
#define CSL_TIM_TIMPRD2_PRD2_RESETVAL    (0x0000u)

#define CSL_TIM_TIMPRD2_RESETVAL         (0x0000u)

/* TIMCNT1 */

#define CSL_TIM_TIMCNT1_TIM1_MASK        (0xFFFFu)
#define CSL_TIM_TIMCNT1_TIM1_SHIFT       (0x0000u)
#define CSL_TIM_TIMCNT1_TIM1_RESETVAL    (0x0000u)

#define CSL_TIM_TIMCNT1_RESETVAL         (0x0000u)

/* TIMCNT2 */

#define CSL_TIM_TIMCNT2_TIM2_MASK        (0xFFFFu)
#define CSL_TIM_TIMCNT2_TIM2_SHIFT       (0x0000u)
#define CSL_TIM_TIMCNT2_TIM2_RESETVAL    (0x0000u)

#define CSL_TIM_TIMCNT2_RESETVAL         (0x0000u)

/* TINSR */

#define CSL_TIM_TINSR_RSV_MASK           (0xFFF8u)
#define CSL_TIM_TINSR_RSV_SHIFT          (0x0003u)
#define CSL_TIM_TINSR_RSV_RESETVAL       (0x0000u)

#define CSL_TIM_TINSR_TINSEL_MASK        (0x0007u)
#define CSL_TIM_TINSR_TINSEL_SHIFT       (0x0000u)
#define CSL_TIM_TINSR_TINSEL_RESETVAL    (0x0000u)
/*----TINSEL Tokens----*/
#define CSL_TIM_TINSR_TINSEL_PLLOUT      (0x0000u)
#define CSL_TIM_TINSR_TINSEL_GPIO0       (0x0001u)
#define CSL_TIM_TINSR_TINSEL_GPIO6       (0x0002u)
#define CSL_TIM_TINSR_TINSEL_GPIO12      (0x0003u)
#define CSL_TIM_TINSR_TINSEL_GPIO18      (0x0004u)
#define CSL_TIM_TINSR_TINSEL_GPIO24      (0x0005u)
#define CSL_TIM_TINSR_TINSEL_USBCLK      (0x0006u)
#define CSL_TIM_TINSR_TINSEL_PLLIN       (0x0007u)

#define CSL_TIM_TINSR_RESETVAL           (0x0000u)

#endif
