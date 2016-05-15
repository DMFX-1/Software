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

#ifndef _CSLR_RTC_1_H_
#define _CSLR_RTC_1_H_

#include <cslr.h>

/* Minimum unit = 2 bytes */

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct  {
    volatile Uint16 CTRL1;
    volatile Uint16 CTRL2;
    volatile Uint16 RSVD0[2];
    volatile Uint16 MS;
    volatile Uint16 MSALARM;
    volatile Uint16 RSVD1[2];
    volatile Uint16 SEC;
    volatile Uint16 SECALARM;
    volatile Uint16 RSVD2[2];
    volatile Uint16 MINS;
    volatile Uint16 MINALARM;
    volatile Uint16 RSVD3[2];
    volatile Uint16 HRS;
    volatile Uint16 HRALARM;
    volatile Uint16 RSVD4[2];
    volatile Uint16 DAY;
    volatile Uint16 DAYALARM;
    volatile Uint16 RSVD5[2];
    volatile Uint16 MONTH;
    volatile Uint16 MONTHALARM;
    volatile Uint16 RSVD6[2];
    volatile Uint16 YEAR;
    volatile Uint16 YEARALARM;
    volatile Uint16 RSVD7[2];
    volatile Uint16 STATUS1;
    volatile Uint16 STATUS2;
    volatile Uint16 RSVD8[2];
    volatile Uint16 INTCR;
    volatile Uint16 RSVD9[3];
    volatile Uint16 COMP;
    volatile Uint16 RSVD10[3];
    volatile Uint16 OSC;
    volatile Uint16 RSVD11[3];
    volatile Uint16 PMGT;
    volatile Uint16 RSVD12[47];
    volatile Uint16 SCRATC0_LSW;
    volatile Uint16 SCRATCH0_MSW;
    volatile Uint16 RSVD13[2];
    volatile Uint16 SCRATCH1_LSW;
    volatile Uint16 SCRATCH1_MSW;
} CSL_RtcRegs;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* CTRL1 */


#define CSL_RTC_CTRL1_VBUSP_INT_EN_MASK (0x0001u)
#define CSL_RTC_CTRL1_VBUSP_INT_EN_SHIFT (0x0000u)
#define CSL_RTC_CTRL1_VBUSP_INT_EN_RESETVAL (0x0000u)

#define CSL_RTC_CTRL1_RESETVAL (0x0000u)

/* CTRL2 */

#define CSL_RTC_CTRL2_UPDT_TIME_MASK (0x8000u)
#define CSL_RTC_CTRL2_UPDT_TIME_SHIFT (0x000Fu)
#define CSL_RTC_CTRL2_UPDT_TIME_RESETVAL (0x0000u)

#define CSL_RTC_CTRL2_UPDT_ALARM_MASK (0x4000u)
#define CSL_RTC_CTRL2_UPDT_ALARM_SHIFT (0x000Eu)
#define CSL_RTC_CTRL2_UPDT_ALARM_RESETVAL (0x0000u)


#define CSL_RTC_CTRL2_RESETVAL (0x0000u)

/* MS */


#define CSL_RTC_MS_SEC_MASK (0x1FFFu)
#define CSL_RTC_MS_SEC_SHIFT (0x0000u)
#define CSL_RTC_MS_SEC_RESETVAL (0x0000u)

#define CSL_RTC_MS_RESETVAL (0x0000u)

/* MSALARM */


#define CSL_RTC_MSALARM_SEC_ALARM_MASK (0x1FFFu)
#define CSL_RTC_MSALARM_SEC_ALARM_SHIFT (0x0000u)
#define CSL_RTC_MSALARM_SEC_ALARM_RESETVAL (0x0000u)

#define CSL_RTC_MSALARM_RESETVAL (0x0000u)

/* SEC */



#define CSL_RTC_SEC_SEC_MASK (0x007Fu)
#define CSL_RTC_SEC_SEC_SHIFT (0x0000u)
#define CSL_RTC_SEC_SEC_RESETVAL (0x0000u)

#define CSL_RTC_SEC_RESETVAL (0x0000u)

/* SECALARM */



#define CSL_RTC_SECALARM_SEC_ALARM_MASK (0x007Fu)
#define CSL_RTC_SECALARM_SEC_ALARM_SHIFT (0x0000u)
#define CSL_RTC_SECALARM_SEC_ALARM_RESETVAL (0x0000u)

#define CSL_RTC_SECALARM_RESETVAL (0x0000u)

/* MINS */



#define CSL_RTC_MINS_MIN_MASK (0x007Fu)
#define CSL_RTC_MINS_MIN_SHIFT (0x0000u)
#define CSL_RTC_MINS_MIN_RESETVAL (0x0000u)

#define CSL_RTC_MINS_RESETVAL (0x0000u)

/* MINALARM */



#define CSL_RTC_MINALARM_MIN_ALARM_MASK (0x007Fu)
#define CSL_RTC_MINALARM_MIN_ALARM_SHIFT (0x0000u)
#define CSL_RTC_MINALARM_MIN_ALARM_RESETVAL (0x0000u)

#define CSL_RTC_MINALARM_RESETVAL (0x0000u)

/* HRS */


#define CSL_RTC_HRS_HR_MASK (0x003Fu)
#define CSL_RTC_HRS_HR_SHIFT (0x0000u)
#define CSL_RTC_HRS_HR_RESETVAL (0x0000u)

#define CSL_RTC_HRS_RESETVAL (0x0000u)

/* HRALARM */


#define CSL_RTC_HRALARM_HR_ALARM_MASK (0x003Fu)
#define CSL_RTC_HRALARM_HR_ALARM_SHIFT (0x0000u)
#define CSL_RTC_HRALARM_HR_ALARM_RESETVAL (0x0000u)

#define CSL_RTC_HRALARM_RESETVAL (0x0000u)

/* DAY */


#define CSL_RTC_DAY_DAY_MASK (0x003Fu)
#define CSL_RTC_DAY_DAY_SHIFT (0x0000u)
#define CSL_RTC_DAY_DAY_RESETVAL (0x0001u)

#define CSL_RTC_DAY_RESETVAL (0x0001u)

/* DAYALARM */


#define CSL_RTC_DAYALARM_DAY_ALARM_MASK (0x003Fu)
#define CSL_RTC_DAYALARM_DAY_ALARM_SHIFT (0x0000u)
#define CSL_RTC_DAYALARM_DAY_ALARM_RESETVAL (0x0001u)

#define CSL_RTC_DAYALARM_RESETVAL (0x0001u)

/* MONTH */


#define CSL_RTC_MONTH_MONTH_MASK (0x001Fu)
#define CSL_RTC_MONTH_MONTH_SHIFT (0x0000u)
#define CSL_RTC_MONTH_MONTH_RESETVAL (0x0000u)

#define CSL_RTC_MONTH_RESETVAL (0x0000u)

/* MONTHALARM */


#define CSL_RTC_MONTHALARM_MONTH_ALARM_MASK (0x001Fu)
#define CSL_RTC_MONTHALARM_MONTH_ALARM_SHIFT (0x0000u)
#define CSL_RTC_MONTHALARM_MONTH_ALARM_RESETVAL (0x0000u)

#define CSL_RTC_MONTHALARM_RESETVAL (0x0000u)

/* YEAR */


#define CSL_RTC_YEAR_YEAR_MASK (0x00FFu)
#define CSL_RTC_YEAR_YEAR_SHIFT (0x0000u)
#define CSL_RTC_YEAR_YEAR_RESETVAL (0x0000u)

#define CSL_RTC_YEAR_RESETVAL (0x0000u)

/* YEARALARM */


#define CSL_RTC_YEARALARM_YEAR_ALARM_MASK (0x00FFu)
#define CSL_RTC_YEARALARM_YEAR_ALARM_SHIFT (0x0000u)
#define CSL_RTC_YEARALARM_YEAR_ALARM_RESETVAL (0x0000u)

#define CSL_RTC_YEARALARM_RESETVAL (0x0000u)

/* STATUS1 */

#define CSL_RTC_STATUS1_ALARM_MASK (0x8000u)
#define CSL_RTC_STATUS1_ALARM_SHIFT (0x000Fu)
#define CSL_RTC_STATUS1_ALARM_RESETVAL (0x0000u)


#define CSL_RTC_STATUS1_EXT_EVT_MASK (0x0020u)
#define CSL_RTC_STATUS1_EXT_EVT_SHIFT (0x0005u)
#define CSL_RTC_STATUS1_EXT_EVT_RESETVAL (0x0000u)

#define CSL_RTC_STATUS1_DAY_EVT_MASK (0x0010u)
#define CSL_RTC_STATUS1_DAY_EVT_SHIFT (0x0004u)
#define CSL_RTC_STATUS1_DAY_EVT_RESETVAL (0x0000u)

#define CSL_RTC_STATUS1_HR_EVT_MASK (0x0008u)
#define CSL_RTC_STATUS1_HR_EVT_SHIFT (0x0003u)
#define CSL_RTC_STATUS1_HR_EVT_RESETVAL (0x0000u)

#define CSL_RTC_STATUS1_MIN_EVT_MASK (0x0004u)
#define CSL_RTC_STATUS1_MIN_EVT_SHIFT (0x0002u)
#define CSL_RTC_STATUS1_MIN_EVT_RESETVAL (0x0000u)

#define CSL_RTC_STATUS1_SEC_EVT_MASK (0x0002u)
#define CSL_RTC_STATUS1_SEC_EVT_SHIFT (0x0001u)
#define CSL_RTC_STATUS1_SEC_EVT_RESETVAL (0x0000u)

#define CSL_RTC_STATUS1_MS_EVT_MASK (0x0001u)
#define CSL_RTC_STATUS1_MS_EVT_SHIFT (0x0000u)
#define CSL_RTC_STATUS1_MS_EVT_RESETVAL (0x0000u)

#define CSL_RTC_STATUS1_RESETVAL (0x0000u)

/* STATUS2 */




#define CSL_RTC_STATUS2_PWRUP_MASK (0x0001u)
#define CSL_RTC_STATUS2_PWRUP_SHIFT (0x0000u)
#define CSL_RTC_STATUS2_PWRUP_RESETVAL (0x0001u)

#define CSL_RTC_STATUS2_RESETVAL (0x0001u)

/* INTCR */

#define CSL_RTC_INTCR_ALARM_INT_MASK (0x8000u)
#define CSL_RTC_INTCR_ALARM_INT_SHIFT (0x000Fu)
#define CSL_RTC_INTCR_ALARM_INT_RESETVAL (0x0000u)


#define CSL_RTC_INTCR_EXTEVT_INT_MASK (0x0020u)
#define CSL_RTC_INTCR_EXTEVT_INT_SHIFT (0x0005u)
#define CSL_RTC_INTCR_EXTEVT_INT_RESETVAL (0x0000u)

#define CSL_RTC_INTCR_DAYEVT_INT_MASK (0x0010u)
#define CSL_RTC_INTCR_DAYEVT_INT_SHIFT (0x0004u)
#define CSL_RTC_INTCR_DAYEVT_INT_RESETVAL (0x0000u)

#define CSL_RTC_INTCR_HREVT_INT_MASK (0x0008u)
#define CSL_RTC_INTCR_HREVT_INT_SHIFT (0x0003u)
#define CSL_RTC_INTCR_HREVT_INT_RESETVAL (0x0000u)

#define CSL_RTC_INTCR_MINEVT_INT_MASK (0x0004u)
#define CSL_RTC_INTCR_MINEVT_INT_SHIFT (0x0002u)
#define CSL_RTC_INTCR_MINEVT_INT_RESETVAL (0x0000u)

#define CSL_RTC_INTCR_SECEVT_INT_MASK (0x0002u)
#define CSL_RTC_INTCR_SECEVT_INT_SHIFT (0x0001u)
#define CSL_RTC_INTCR_SECEVT_INT_RESETVAL (0x0000u)

#define CSL_RTC_INTCR_MSEVT_INT_MASK (0x0001u)
#define CSL_RTC_INTCR_MSEVT_INT_SHIFT (0x0000u)
#define CSL_RTC_INTCR_MSEVT_INT_RESETVAL (0x0000u)

#define CSL_RTC_INTCR_RESETVAL (0x0000u)

/* COMP */

#define CSL_RTC_COMP_DRIFT_MASK (0x8000u)
#define CSL_RTC_COMP_DRIFT_SHIFT (0x000Fu)
#define CSL_RTC_COMP_DRIFT_RESETVAL (0x0000u)


#define CSL_RTC_COMP_COMP_MASK (0x1FFFu)
#define CSL_RTC_COMP_COMP_SHIFT (0x0000u)
#define CSL_RTC_COMP_COMP_RESETVAL (0x0000u)

#define CSL_RTC_COMP_RESETVAL (0x0000u)

/* OSC */

#define CSL_RTC_OSC_SW_RESET_MASK (0x8000u)
#define CSL_RTC_OSC_SW_RESET_SHIFT (0x000Fu)
#define CSL_RTC_OSC_SW_RESET_RESETVAL (0x0000u)


#define CSL_RTC_OSC_OSC32K_PWRD_MASK (0x0010u)
#define CSL_RTC_OSC_OSC32K_PWRD_SHIFT (0x0004u)
#define CSL_RTC_OSC_OSC32K_PWRD_RESETVAL (0x0000u)

#define CSL_RTC_OSC_RESET_MASK (0x000Fu)
#define CSL_RTC_OSC_RESET_SHIFT (0x0000u)
#define CSL_RTC_OSC_RESET_RESETVAL (0x0008u)

#define CSL_RTC_OSC_RESETVAL (0x0008u)

/* PMGT */


#define CSL_RTC_PMGT_WU_DOUT_MASK (0x0010u)
#define CSL_RTC_PMGT_WU_DOUT_SHIFT (0x0004u)
#define CSL_RTC_PMGT_WU_DOUT_RESETVAL (0x0000u)

#define CSL_RTC_PMGT_WU_DIR_MASK (0x0008u)
#define CSL_RTC_PMGT_WU_DIR_SHIFT (0x0003u)
#define CSL_RTC_PMGT_WU_DIR_RESETVAL (0x0000u)

#define CSL_RTC_PMGT_BG_PD_MASK (0x0004u)
#define CSL_RTC_PMGT_BG_PD_SHIFT (0x0002u)
#define CSL_RTC_PMGT_BG_PD_RESETVAL (0x0000u)

#define CSL_RTC_PMGT_LDO_PD_MASK (0x0002u)
#define CSL_RTC_PMGT_LDO_PD_SHIFT (0x0001u)
#define CSL_RTC_PMGT_LDO_PD_RESETVAL (0x0000u)

#define CSL_RTC_PMGT_CLKOUTEN_MASK (0x0001u)
#define CSL_RTC_PMGT_CLKOUTEN_SHIFT (0x0000u)
#define CSL_RTC_PMGT_CLKOUTEN_RESETVAL (0x0000u)

#define CSL_RTC_PMGT_RESETVAL (0x0000u)

/* SCRATC0_LSW */

#define CSL_RTC_SCRATC0_LSW_LSWSCRATCH0_MASK (0xFFFFu)
#define CSL_RTC_SCRATC0_LSW_LSWSCRATCH0_SHIFT (0x0000u)
#define CSL_RTC_SCRATC0_LSW_LSWSCRATCH0_RESETVAL (0x0000u)

#define CSL_RTC_SCRATC0_LSW_RESETVAL (0x0000u)

/* SCRATCH0_MSW */

#define CSL_RTC_SCRATCH0_MSW_MSWSCRATCH0_MASK (0xFFFFu)
#define CSL_RTC_SCRATCH0_MSW_MSWSCRATCH0_SHIFT (0x0000u)
#define CSL_RTC_SCRATCH0_MSW_MSWSCRATCH0_RESETVAL (0x0000u)

#define CSL_RTC_SCRATCH0_MSW_RESETVAL (0x0000u)

/* SCRATCH1_LSW */

#define CSL_RTC_SCRATCH1_LSW_LSWSCRATCH1_MASK (0xFFFFu)
#define CSL_RTC_SCRATCH1_LSW_LSWSCRATCH1_SHIFT (0x0000u)
#define CSL_RTC_SCRATCH1_LSW_LSWSCRATCH1_RESETVAL (0x0000u)

#define CSL_RTC_SCRATCH1_LSW_RESETVAL (0x0000u)

/* SCRATCH1_MSW */

#define CSL_RTC_SCRATCH1_MSW_MSWSCRATCH1_MASK (0xFFFFu)
#define CSL_RTC_SCRATCH1_MSW_MSWSCRATCH1_SHIFT (0x0000u)
#define CSL_RTC_SCRATCH1_MSW_MSWSCRATCH1_RESETVAL (0x0000u)

#define CSL_RTC_SCRATCH1_MSW_IOINT_MASK (0x0001u)
#define CSL_RTC_SCRATCH1_MSW_IOINT_SHIFT (0x0000u)
#define CSL_RTC_SCRATCH1_MSW_IOINT_RESETVAL (0x0000u)

#define CSL_RTC_SCRATCH1_MSW_RESETVAL (0x0000u)

#endif

