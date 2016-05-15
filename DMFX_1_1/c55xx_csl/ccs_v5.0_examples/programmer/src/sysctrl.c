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

/*********************************************************************
*    sysctrl.c
**********************************************************************
*   C5505 System-Control routines.
**********************************************************************/
#include "registers.h"
#include "sysctrl.h"

/*******************************************************************
* PeripheralPinMapping
* - Serial0 = Serial-Port-0 pin mapping (PERIPH_BUS_S0_*)
* - Serial1 = Serial-Port-1 pin mapping (PERIPH_BUS_S1_*)
* - Parallel = Parallel-Port pin mapping (PERIPH_BUS_PP_*)
* This function sets the pin-mapping for each peripheral bus.
********************************************************************/
void PeripheralPinMapping(unsigned short Serial0, unsigned short Serial1, unsigned short Parallel)
{
   /* Setup Peripheral (External) Bus Selection */
   IOPORT_REG_PERIPHSEL0 = Serial0 | Serial1 | Parallel;
}

/*******************************************************************
* PeripheralReset
* - PeripheralResets = Peripherals to reset (PERIPH_RESET_*)
* - ResetHoldCycles = # sys-clk cycles to hold reset signal low
* - DelayCycles = # of delay cycles to wait for peripheral-ready
* This function resets the specified peripheral(s).
********************************************************************/
void PeripheralReset(unsigned short PeripheralResets, unsigned short ResetHoldCycles, unsigned long DelayCycles)
{
   int i;

   /* Reset Counter value */
   IOPORT_REG_PER_RSTCOUNT = ResetHoldCycles;

   /* Reset the specifed peripherals */
   IOPORT_REG_PER_RESET = PeripheralResets;

   for (i=0; i<DelayCycles; i++);
}

/*******************************************************************
* PeripheralClkEnableOnly
* - PeripheralClks = PeripheralClks to enable (PERIPH_CLK_*)
* This function enables the specified peripheral-clks ONLY.
* (all other peripheral-clks are disabled).
********************************************************************/
void PeripheralClkEnableOnly(unsigned long PeripheralClks)
{
   /* Enable the specifed peripheral-clocks (disable all others) */
   IOPORT_REG_IDLE_PCGCR_LSW = ~((unsigned short)(PeripheralClks & 0xFFFF));
   IOPORT_REG_IDLE_PCGCR_MSW = ~((unsigned short)((PeripheralClks >> 16) & 0x007F));
}

/*******************************************************************
* PeripheralClkEnable
* - PeripheralClks = PeripheralClks to enable (PERIPH_CLK_*)
* This function enables the specified peripheral-clks.
* (all other peripheral-clks are unchanged).
********************************************************************/
void PeripheralClkEnable(unsigned long PeripheralClks)
{
   /* Enable the specifed peripheral-clocks (clear the disable-bits) */
   IOPORT_REG_IDLE_PCGCR_LSW &= ~((unsigned short)(PeripheralClks & 0xFFFF));
   IOPORT_REG_IDLE_PCGCR_MSW &= ~((unsigned short)((PeripheralClks >> 16) & 0x007F));
}

