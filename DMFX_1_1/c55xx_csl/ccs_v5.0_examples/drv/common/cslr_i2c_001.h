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

#ifndef _CSLR__I2C_1_H_
#define _CSLR__I2C_1_H_

#include <cslr.h>

#include <stdtypes.h>
/* Minimum unit = 2 bytes */

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct  {
    volatile Uint16 ICOAR;
    volatile Uint16 RSVD0[3];
    volatile Uint16 ICIMR;
    volatile Uint16 RSVD1[3];
    volatile Uint16 ICSTR;
    volatile Uint16 RSVD2[3];
    volatile Uint16 ICCLKL;
    volatile Uint16 RSVD3[3];
    volatile Uint16 ICCLKH;
    volatile Uint16 RSVD4[3];
    volatile Uint16 ICCNT;
    volatile Uint16 RSVD5[3];
    volatile Uint16 ICDRR;
    volatile Uint16 RSVD6[3];
    volatile Uint16 ICSAR;
    volatile Uint16 RSVD7[3];
    volatile Uint16 ICDXR;
    volatile Uint16 RSVD8[3];
    volatile Uint16 ICMDR;
    volatile Uint16 RSVD9[3];
    volatile Uint16 ICIVR;
    volatile Uint16 RSVD10[3];
    volatile Uint16 ICEMDR;
    volatile Uint16 RSVD11[3];
    volatile Uint16 ICPSC;
    volatile Uint16 RSVD12[3];
    volatile Uint16 ICPID1;
    volatile Uint16 RSVD13[3];
    volatile Uint16 ICPID2;
    volatile Uint16 RSVD14[3];
    volatile Uint16 ICDMAC;
    volatile Uint16 RSVD15[11];
    volatile Uint16 ICPFUNC;
    volatile Uint16 RSVD16[3];
    volatile Uint16 ICPDIR;
    volatile Uint16 RSVD17[3];
    volatile Uint16 ICPDIN;
    volatile Uint16 RSVD18[3];
    volatile Uint16 ICPDOUT;
    volatile Uint16 RSVD19[3];
    volatile Uint16 ICPDSET;
    volatile Uint16 RSVD20[3];
    volatile Uint16 ICPDCLR;
    volatile Uint16 RSVD21[3];
    volatile Uint16 ICPDRV;
    volatile Uint16 RSVD22[3];
    volatile Uint16 ICPPDIS;
    volatile Uint16 RSVD23[3];
    volatile Uint16 ICPPSEL;
    volatile Uint16 RSVD24[3];
    volatile Uint16 ICPSRS;
} CSL_I2cDrvRegs;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* ICOAR */

#define CSL_I2C_ICOAR_OADDR_MASK (0x03FFu)
#define CSL_I2C_ICOAR_OADDR_SHIFT (0x0000u)
#define CSL_I2C_ICOAR_OADDR_RESETVAL (0x0000u)

#define CSL_I2C_ICOAR_RESETVAL (0x0000u)

/* ICIMR */

#define CSL_I2C_ICIMR_AAS_MASK (0x0040u)
#define CSL_I2C_ICIMR_AAS_SHIFT (0x0006u)
#define CSL_I2C_ICIMR_AAS_RESETVAL (0x0000u)
/*----AAS Tokens----*/
#define CSL_I2C_ICIMR_AAS_DISABLE (0x0000u)
#define CSL_I2C_ICIMR_AAS_ENABLE (0x0001u)

#define CSL_I2C_ICIMR_SCD_MASK (0x0020u)
#define CSL_I2C_ICIMR_SCD_SHIFT (0x0005u)
#define CSL_I2C_ICIMR_SCD_RESETVAL (0x0000u)
/*----SCD Tokens----*/
#define CSL_I2C_ICIMR_SCD_DISABLE (0x0000u)
#define CSL_I2C_ICIMR_SCD_ENABLE (0x0001u)

#define CSL_I2C_ICIMR_ICXRDY_MASK (0x0010u)
#define CSL_I2C_ICIMR_ICXRDY_SHIFT (0x0004u)
#define CSL_I2C_ICIMR_ICXRDY_RESETVAL (0x0000u)
/*----ICXRDY Tokens----*/
#define CSL_I2C_ICIMR_ICXRDY_DISABLE (0x0000u)
#define CSL_I2C_ICIMR_ICXRDY_ENABLE (0x0001u)

#define CSL_I2C_ICIMR_ICRRDY_MASK (0x0008u)
#define CSL_I2C_ICIMR_ICRRDY_SHIFT (0x0003u)
#define CSL_I2C_ICIMR_ICRRDY_RESETVAL (0x0000u)
/*----ICRRDY Tokens----*/
#define CSL_I2C_ICIMR_ICRRDY_DISABLE (0x0000u)
#define CSL_I2C_ICIMR_ICRRDY_ENABLE (0x0001u)

#define CSL_I2C_ICIMR_ARDY_MASK (0x0004u)
#define CSL_I2C_ICIMR_ARDY_SHIFT (0x0002u)
#define CSL_I2C_ICIMR_ARDY_RESETVAL (0x0000u)
/*----ARDY Tokens----*/
#define CSL_I2C_ICIMR_ARDY_DISABLE (0x0000u)
#define CSL_I2C_ICIMR_ARDY_ENABLE (0x0001u)

#define CSL_I2C_ICIMR_NACK_MASK (0x0002u)
#define CSL_I2C_ICIMR_NACK_SHIFT (0x0001u)
#define CSL_I2C_ICIMR_NACK_RESETVAL (0x0000u)
/*----NACK Tokens----*/
#define CSL_I2C_ICIMR_NACK_DISABLE (0x0000u)
#define CSL_I2C_ICIMR_NACK_ENABLE (0x0001u)

#define CSL_I2C_ICIMR_AL_MASK (0x0001u)
#define CSL_I2C_ICIMR_AL_SHIFT (0x0000u)
#define CSL_I2C_ICIMR_AL_RESETVAL (0x0000u)
/*----AL Tokens----*/
#define CSL_I2C_ICIMR_AL_DISABLE (0x0000u)
#define CSL_I2C_ICIMR_AL_ENABLE (0x0001u)

#define CSL_I2C_ICIMR_RESETVAL (0x0000u)

/* ICSTR */

#define CSL_I2C_ICSTR_SDIR_MASK (0x4000u)
#define CSL_I2C_ICSTR_SDIR_SHIFT (0x000Eu)
#define CSL_I2C_ICSTR_SDIR_RESETVAL (0x0000u)

#define CSL_I2C_ICSTR_NACKSNT_MASK (0x2000u)
#define CSL_I2C_ICSTR_NACKSNT_SHIFT (0x000Du)
#define CSL_I2C_ICSTR_NACKSNT_RESETVAL (0x0000u)

#define CSL_I2C_ICSTR_BB_MASK (0x1000u)
#define CSL_I2C_ICSTR_BB_SHIFT (0x000Cu)
#define CSL_I2C_ICSTR_BB_RESETVAL (0x0000u)

#define CSL_I2C_ICSTR_RSFULL_MASK (0x0800u)
#define CSL_I2C_ICSTR_RSFULL_SHIFT (0x000Bu)
#define CSL_I2C_ICSTR_RSFULL_RESETVAL (0x0000u)

#define CSL_I2C_ICSTR_XSMT_MASK (0x0400u)
#define CSL_I2C_ICSTR_XSMT_SHIFT (0x000Au)
#define CSL_I2C_ICSTR_XSMT_RESETVAL (0x0001u)

#define CSL_I2C_ICSTR_AAS_MASK (0x0200u)
#define CSL_I2C_ICSTR_AAS_SHIFT (0x0009u)
#define CSL_I2C_ICSTR_AAS_RESETVAL (0x0000u)

#define CSL_I2C_ICSTR_AD0_MASK (0x0100u)
#define CSL_I2C_ICSTR_AD0_SHIFT (0x0008u)
#define CSL_I2C_ICSTR_AD0_RESETVAL (0x0000u)


#define CSL_I2C_ICSTR_SCD_MASK (0x0020u)
#define CSL_I2C_ICSTR_SCD_SHIFT (0x0005u)
#define CSL_I2C_ICSTR_SCD_RESETVAL (0x0000u)

#define CSL_I2C_ICSTR_ICXRDY_MASK (0x0010u)
#define CSL_I2C_ICSTR_ICXRDY_SHIFT (0x0004u)
#define CSL_I2C_ICSTR_ICXRDY_RESETVAL (0x0001u)

#define CSL_I2C_ICSTR_ICRRDY_MASK (0x0008u)
#define CSL_I2C_ICSTR_ICRRDY_SHIFT (0x0003u)
#define CSL_I2C_ICSTR_ICRRDY_RESETVAL (0x0000u)

#define CSL_I2C_ICSTR_ARDY_MASK (0x0004u)
#define CSL_I2C_ICSTR_ARDY_SHIFT (0x0002u)
#define CSL_I2C_ICSTR_ARDY_RESETVAL (0x0000u)

#define CSL_I2C_ICSTR_NACK_MASK (0x0002u)
#define CSL_I2C_ICSTR_NACK_SHIFT (0x0001u)
#define CSL_I2C_ICSTR_NACK_RESETVAL (0x0000u)

#define CSL_I2C_ICSTR_AL_MASK (0x0001u)
#define CSL_I2C_ICSTR_AL_SHIFT (0x0000u)
#define CSL_I2C_ICSTR_AL_RESETVAL (0x0000u)

#define CSL_I2C_ICSTR_RESETVAL (0x0410u)

/* ICCLKL */

#define CSL_I2C_ICCLKL_ICCL_MASK (0xFFFFu)
#define CSL_I2C_ICCLKL_ICCL_SHIFT (0x0000u)
#define CSL_I2C_ICCLKL_ICCL_RESETVAL (0x0000u)

#define CSL_I2C_ICCLKL_RESETVAL (0x0000u)

/* ICCLKH */

#define CSL_I2C_ICCLKH_ICCH_MASK (0xFFFFu)
#define CSL_I2C_ICCLKH_ICCH_SHIFT (0x0000u)
#define CSL_I2C_ICCLKH_ICCH_RESETVAL (0x0000u)

#define CSL_I2C_ICCLKH_RESETVAL (0x0000u)

/* ICCNT */

#define CSL_I2C_ICCNT_ICDC_MASK (0xFFFFu)
#define CSL_I2C_ICCNT_ICDC_SHIFT (0x0000u)
#define CSL_I2C_ICCNT_ICDC_RESETVAL (0x0000u)

#define CSL_I2C_ICCNT_RESETVAL (0x0000u)

/* ICDRR */


#define CSL_I2C_ICDRR_D_MASK (0x00FFu)
#define CSL_I2C_ICDRR_D_SHIFT (0x0000u)
#define CSL_I2C_ICDRR_D_RESETVAL (0x0000u)

#define CSL_I2C_ICDRR_RESETVAL (0x0000u)

/* ICSAR */


#define CSL_I2C_ICSAR_SADDR_MASK (0x03FFu)
#define CSL_I2C_ICSAR_SADDR_SHIFT (0x0000u)
#define CSL_I2C_ICSAR_SADDR_RESETVAL (0x03FFu)

#define CSL_I2C_ICSAR_RESETVAL (0x03FFu)

/* ICDXR */


#define CSL_I2C_ICDXR_D_MASK (0x00FFu)
#define CSL_I2C_ICDXR_D_SHIFT (0x0000u)
#define CSL_I2C_ICDXR_D_RESETVAL (0x0000u)

#define CSL_I2C_ICDXR_RESETVAL (0x0000u)

/* ICMDR */

#define CSL_I2C_ICMDR_NACKMOD_MASK (0x8000u)
#define CSL_I2C_ICMDR_NACKMOD_SHIFT (0x000Fu)
#define CSL_I2C_ICMDR_NACKMOD_RESETVAL (0x0000u)

#define CSL_I2C_ICMDR_FREE_MASK (0x4000u)
#define CSL_I2C_ICMDR_FREE_SHIFT (0x000Eu)
#define CSL_I2C_ICMDR_FREE_RESETVAL (0x0000u)

#define CSL_I2C_ICMDR_STT_MASK (0x2000u)
#define CSL_I2C_ICMDR_STT_SHIFT (0x000Du)
#define CSL_I2C_ICMDR_STT_RESETVAL (0x0000u)


#define CSL_I2C_ICMDR_STP_MASK (0x0800u)
#define CSL_I2C_ICMDR_STP_SHIFT (0x000Bu)
#define CSL_I2C_ICMDR_STP_RESETVAL (0x0000u)

#define CSL_I2C_ICMDR_MST_MASK (0x0400u)
#define CSL_I2C_ICMDR_MST_SHIFT (0x000Au)
#define CSL_I2C_ICMDR_MST_RESETVAL (0x0000u)

#define CSL_I2C_ICMDR_TRX_MASK (0x0200u)
#define CSL_I2C_ICMDR_TRX_SHIFT (0x0009u)
#define CSL_I2C_ICMDR_TRX_RESETVAL (0x0000u)

#define CSL_I2C_ICMDR_XA_MASK (0x0100u)
#define CSL_I2C_ICMDR_XA_SHIFT (0x0008u)
#define CSL_I2C_ICMDR_XA_RESETVAL (0x0000u)

#define CSL_I2C_ICMDR_RM_MASK (0x0080u)
#define CSL_I2C_ICMDR_RM_SHIFT (0x0007u)
#define CSL_I2C_ICMDR_RM_RESETVAL (0x0000u)

#define CSL_I2C_ICMDR_DLB_MASK (0x0040u)
#define CSL_I2C_ICMDR_DLB_SHIFT (0x0006u)
#define CSL_I2C_ICMDR_DLB_RESETVAL (0x0000u)

#define CSL_I2C_ICMDR_IRS_MASK (0x0020u)
#define CSL_I2C_ICMDR_IRS_SHIFT (0x0005u)
#define CSL_I2C_ICMDR_IRS_RESETVAL (0x0000u)

#define CSL_I2C_ICMDR_STB_MASK (0x0010u)
#define CSL_I2C_ICMDR_STB_SHIFT (0x0004u)
#define CSL_I2C_ICMDR_STB_RESETVAL (0x0000u)

#define CSL_I2C_ICMDR_FDF_MASK (0x0008u)
#define CSL_I2C_ICMDR_FDF_SHIFT (0x0003u)
#define CSL_I2C_ICMDR_FDF_RESETVAL (0x0000u)

#define CSL_I2C_ICMDR_BC_MASK (0x0007u)
#define CSL_I2C_ICMDR_BC_SHIFT (0x0000u)
#define CSL_I2C_ICMDR_BC_RESETVAL (0x0000u)

#define CSL_I2C_ICMDR_RESETVAL (0x0000u)

/* ICIVR */


#define CSL_I2C_ICIVR_TESTMD_MASK (0x0F00u)
#define CSL_I2C_ICIVR_TESTMD_SHIFT (0x0008u)
#define CSL_I2C_ICIVR_TESTMD_RESETVAL (0x0000u)


#define CSL_I2C_ICIVR_INTCODE_MASK (0x0007u)
#define CSL_I2C_ICIVR_INTCODE_SHIFT (0x0000u)
#define CSL_I2C_ICIVR_INTCODE_RESETVAL (0x0000u)
/*----INTCODE Tokens----*/
#define CSL_I2C_ICIVR_INTCODE_NONE (0x0000u)
#define CSL_I2C_ICIVR_INTCODE_AL (0x0001u)
#define CSL_I2C_ICIVR_INTCODE_NACK (0x0002u)
#define CSL_I2C_ICIVR_INTCODE_RAR (0x0003u)
#define CSL_I2C_ICIVR_INTCODE_RDR (0x0004u)
#define CSL_I2C_ICIVR_INTCODE_TDR (0x0005u)
#define CSL_I2C_ICIVR_INTCODE_SCD (0x0006u)
#define CSL_I2C_ICIVR_INTCODE_AAS (0x0007u)

#define CSL_I2C_ICIVR_RESETVAL (0x0000u)

/* ICEMDR */


#define CSL_I2C_ICEMDR_IGNACK_MASK (0x0002u)
#define CSL_I2C_ICEMDR_IGNACK_SHIFT (0x0001u)
#define CSL_I2C_ICEMDR_IGNACK_RESETVAL (0x0000u)

#define CSL_I2C_ICEMDR_BCM_MASK (0x0001u)
#define CSL_I2C_ICEMDR_BCM_SHIFT (0x0000u)
#define CSL_I2C_ICEMDR_BCM_RESETVAL (0x0001u)

#define CSL_I2C_ICEMDR_RESETVAL (0x0001u)

/* ICPSC */


#define CSL_I2C_ICPSC_IPSC_MASK (0x00FFu)
#define CSL_I2C_ICPSC_IPSC_SHIFT (0x0000u)
#define CSL_I2C_ICPSC_IPSC_RESETVAL (0x0000u)

#define CSL_I2C_ICPSC_RESETVAL (0x0000u)

/* ICPID1 */

#define CSL_I2C_ICPID1_CLASS_MASK (0xFF00u)
#define CSL_I2C_ICPID1_CLASS_SHIFT (0x0008u)
#define CSL_I2C_ICPID1_CLASS_RESETVAL (0x0001u)

#define CSL_I2C_ICPID1_REVISION_MASK (0x00FFu)
#define CSL_I2C_ICPID1_REVISION_SHIFT (0x0000u)
#define CSL_I2C_ICPID1_REVISION_RESETVAL (0x0006u)

#define CSL_I2C_ICPID1_RESETVAL (0x0106u)

/* ICPID2 */


#define CSL_I2C_ICPID2_TYPE_MASK (0x00FFu)
#define CSL_I2C_ICPID2_TYPE_SHIFT (0x0000u)
#define CSL_I2C_ICPID2_TYPE_RESETVAL (0x0005u)

#define CSL_I2C_ICPID2_RESETVAL (0x0005u)

/* ICDMAC */


#define CSL_I2C_ICDMAC_TXDMAEN_MASK (0x0002u)
#define CSL_I2C_ICDMAC_TXDMAEN_SHIFT (0x0001u)
#define CSL_I2C_ICDMAC_TXDMAEN_RESETVAL (0x0001u)

#define CSL_I2C_ICDMAC_RXDMAEN_MASK (0x0001u)
#define CSL_I2C_ICDMAC_RXDMAEN_SHIFT (0x0000u)
#define CSL_I2C_ICDMAC_RXDMAEN_RESETVAL (0x0001u)

#define CSL_I2C_ICDMAC_RESETVAL (0x0003u)

/* ICPFUNC */


#define CSL_I2C_ICPFUNC_PFUNC_MASK (0x0001u)
#define CSL_I2C_ICPFUNC_PFUNC_SHIFT (0x0000u)
#define CSL_I2C_ICPFUNC_PFUNC_RESETVAL (0x0000u)

#define CSL_I2C_ICPFUNC_RESETVAL (0x0000u)

/* ICPDIR */


#define CSL_I2C_ICPDIR_PDIR1_MASK (0x0002u)
#define CSL_I2C_ICPDIR_PDIR1_SHIFT (0x0001u)
#define CSL_I2C_ICPDIR_PDIR1_RESETVAL (0x0000u)
/*----PDIR1 Tokens----*/
#define CSL_I2C_ICPDIR_PDIR1_IN (0x0000u)
#define CSL_I2C_ICPDIR_PDIR1_OUT (0x0001u)

#define CSL_I2C_ICPDIR_PDIR0_MASK (0x0001u)
#define CSL_I2C_ICPDIR_PDIR0_SHIFT (0x0000u)
#define CSL_I2C_ICPDIR_PDIR0_RESETVAL (0x0000u)
/*----PDIR0 Tokens----*/
#define CSL_I2C_ICPDIR_PDIR0_IN (0x0000u)
#define CSL_I2C_ICPDIR_PDIR0_OUT (0x0001u)

#define CSL_I2C_ICPDIR_RESETVAL (0x0000u)

/* ICPDIN */


#define CSL_I2C_ICPDIN_PDIN1_MASK (0x0002u)
#define CSL_I2C_ICPDIN_PDIN1_SHIFT (0x0001u)
#define CSL_I2C_ICPDIN_PDIN1_RESETVAL (0x0000u)
/*----PDIN1 Tokens----*/
#define CSL_I2C_ICPDIN_PDIN1_LOW (0x0000u)
#define CSL_I2C_ICPDIN_PDIN1_HIGH (0x0001u)

#define CSL_I2C_ICPDIN_PDIN0_MASK (0x0001u)
#define CSL_I2C_ICPDIN_PDIN0_SHIFT (0x0000u)
#define CSL_I2C_ICPDIN_PDIN0_RESETVAL (0x0000u)
/*----PDIN0 Tokens----*/
#define CSL_I2C_ICPDIN_PDIN0_LOW (0x0000u)
#define CSL_I2C_ICPDIN_PDIN0_HIGH (0x0001u)

#define CSL_I2C_ICPDIN_RESETVAL (0x0000u)

/* ICPDOUT */


#define CSL_I2C_ICPDOUT_PDOUT1_MASK (0x0002u)
#define CSL_I2C_ICPDOUT_PDOUT1_SHIFT (0x0001u)
#define CSL_I2C_ICPDOUT_PDOUT1_RESETVAL (0x0000u)
/*----PDOUT1 Tokens----*/
#define CSL_I2C_ICPDOUT_PDOUT1_LOW (0x0000u)
#define CSL_I2C_ICPDOUT_PDOUT1_HIGH (0x0001u)

#define CSL_I2C_ICPDOUT_PDOUT0_MASK (0x0001u)
#define CSL_I2C_ICPDOUT_PDOUT0_SHIFT (0x0000u)
#define CSL_I2C_ICPDOUT_PDOUT0_RESETVAL (0x0000u)
/*----PDOUT0 Tokens----*/
#define CSL_I2C_ICPDOUT_PDOUT0_LOW (0x0000u)
#define CSL_I2C_ICPDOUT_PDOUT0_HIGH (0x0001u)

#define CSL_I2C_ICPDOUT_RESETVAL (0x0000u)

/* ICPDSET */


#define CSL_I2C_ICPDSET_PDSET1_MASK (0x0002u)
#define CSL_I2C_ICPDSET_PDSET1_SHIFT (0x0001u)
#define CSL_I2C_ICPDSET_PDSET1_RESETVAL (0x0000u)
/*----PDSET1 Tokens----*/
#define CSL_I2C_ICPDSET_PDSET1_NONE (0x0000u)
#define CSL_I2C_ICPDSET_PDSET1_SET (0x0001u)

#define CSL_I2C_ICPDSET_PDSET0_MASK (0x0001u)
#define CSL_I2C_ICPDSET_PDSET0_SHIFT (0x0000u)
#define CSL_I2C_ICPDSET_PDSET0_RESETVAL (0x0000u)
/*----PDSET0 Tokens----*/
#define CSL_I2C_ICPDSET_PDSET0_NONE (0x0000u)
#define CSL_I2C_ICPDSET_PDSET0_SET (0x0001u)

#define CSL_I2C_ICPDSET_RESETVAL (0x0000u)

/* ICPDCLR */


#define CSL_I2C_ICPDCLR_PDCLR1_MASK (0x0002u)
#define CSL_I2C_ICPDCLR_PDCLR1_SHIFT (0x0001u)
#define CSL_I2C_ICPDCLR_PDCLR1_RESETVAL (0x0000u)
/*----PDCLR1 Tokens----*/
#define CSL_I2C_ICPDCLR_PDCLR1_NONE (0x0000u)
#define CSL_I2C_ICPDCLR_PDCLR1_RESET (0x0001u)

#define CSL_I2C_ICPDCLR_PDCLR0_MASK (0x0001u)
#define CSL_I2C_ICPDCLR_PDCLR0_SHIFT (0x0000u)
#define CSL_I2C_ICPDCLR_PDCLR0_RESETVAL (0x0000u)
/*----PDCLR0 Tokens----*/
#define CSL_I2C_ICPDCLR_PDCLR0_NONE (0x0000u)
#define CSL_I2C_ICPDCLR_PDCLR0_RESET (0x0001u)

#define CSL_I2C_ICPDCLR_RESETVAL (0x0000u)

/* ICPDRV */


#define CSL_I2C_ICPDRV_PDRV1_MASK (0x0002u)
#define CSL_I2C_ICPDRV_PDRV1_SHIFT (0x0001u)
#define CSL_I2C_ICPDRV_PDRV1_RESETVAL (0x0001u)

#define CSL_I2C_ICPDRV_PDRV0_MASK (0x0001u)
#define CSL_I2C_ICPDRV_PDRV0_SHIFT (0x0000u)
#define CSL_I2C_ICPDRV_PDRV0_RESETVAL (0x0001u)

#define CSL_I2C_ICPDRV_RESETVAL (0x0003u)

/* ICPPDIS */


#define CSL_I2C_ICPPDIS_PPDIS1_MASK (0x0002u)
#define CSL_I2C_ICPPDIS_PPDIS1_SHIFT (0x0001u)
#define CSL_I2C_ICPPDIS_PPDIS1_RESETVAL (0x0001u)

#define CSL_I2C_ICPPDIS_PPDIS0_MASK (0x0001u)
#define CSL_I2C_ICPPDIS_PPDIS0_SHIFT (0x0000u)
#define CSL_I2C_ICPPDIS_PPDIS0_RESETVAL (0x0001u)

#define CSL_I2C_ICPPDIS_RESETVAL (0x0003u)

/* ICPPSEL */


#define CSL_I2C_ICPPSEL_PPSEL1_MASK (0x0002u)
#define CSL_I2C_ICPPSEL_PPSEL1_SHIFT (0x0001u)
#define CSL_I2C_ICPPSEL_PPSEL1_RESETVAL (0x0000u)

#define CSL_I2C_ICPPSEL_PPSEL0_MASK (0x0001u)
#define CSL_I2C_ICPPSEL_PPSEL0_SHIFT (0x0000u)
#define CSL_I2C_ICPPSEL_PPSEL0_RESETVAL (0x0000u)

#define CSL_I2C_ICPPSEL_RESETVAL (0x0000u)

/* ICPSRS */


#define CSL_I2C_ICPSRS_PSRS1_MASK (0x0002u)
#define CSL_I2C_ICPSRS_PSRS1_SHIFT (0x0001u)
#define CSL_I2C_ICPSRS_PSRS1_RESETVAL (0x0000u)

#define CSL_I2C_ICPSRS_PSRS0_MASK (0x0001u)
#define CSL_I2C_ICPSRS_PSRS0_SHIFT (0x0000u)
#define CSL_I2C_ICPSRS_PSRS0_RESETVAL (0x0000u)

#define CSL_I2C_ICPSRS_RESETVAL (0x0000u)

#endif

