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
*    mcspi.c
**********************************************************************
*   TMS320C55x code for accessing MCSPI devices on C5525.
**********************************************************************/
#include "soc.h"
#include "sysctrl.h"
#include "mcspi.h"

/*******************************************************************
* void MCSPI_RESET(void)
* input:
*        None
* Output:
*        None
* Return:
*        None;
*Description:
*     This function is to reset the MCSPI module in the Phoenix
********************************************************************/
Uint16 MCSPI_RESET(void)
{
	Uint16 looper;

	// Disable MCSPI module in Idle PCGCR
	//CSL_FINST(CSL_SYSCTRL_REGS->PCGCR1, SYS_PCGCR1_MCSPICG, ACTIVE);
	CSL_SYSCTRL_REGS->PCGCR1 = 0;
	// Disable MCSPI Refernce Clock in Idle PCGCR
	//CSL_FINST(CSL_SYSCTRL_REGS->PCGCR2, SYS_PCGCR2_MCSPIREFCG, ACTIVE);
	CSL_SYSCTRL_REGS->PCGCR2 = 0;

	CSL_SYSCTRL_REGS->CLKSTOP1 = 0;

	// set reset counter value
	CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_SP1MODE, MODE1);

	// set reset counter value
	CSL_FINS(CSL_SYSCTRL_REGS->PSRCR, SYS_PSRCR_COUNT, 0x20);

	// enable mcspi self-clearing
	CSL_FINST(CSL_SYSCTRL_REGS->PRCR, SYS_PRCR_PG1_RST, RST);

	// give some delay for the device to reset
	for(looper = 0; looper < 300; looper++){;}


	return 0;
}

/*******************************************************************
* void MCSPI_INIT(unsigned char clock_divider)
* input:
*        clock_divider     set the clock divider for MCSPI
* Output:
*        None
* Return:
*        None;
*Description:
*     This function is to init the MCSPI module in the Phoenix
********************************************************************/
Uint16 MCSPI_INIT(unsigned char clock_divider)
{
	Uint16 status;
	Uint16 rd_value;

	status = 0;
	// reset the MCSPI
	MCSPI_RESET();

	/*Takes the System Clock and divides it down for the McSPI functional clock(SPIREFCLK).
	0000 = Divide-by-1
	0001 = Divide-by-2
	0010 = Divide-by-4
	0011 = Divide-by-6
	0100 = Divide-by-8
	0101 = Divide-by-10
	0110 = Divide-by-12
	0111 = Divide-by-14
	1000 = Divide-by-16
	1001 = Divide-by-18
	1010 = Divide-by-20
	1011 = Divide-by-22
	1100 = Divide-by-24
	1101 = Divide-by-26
	1110 = Divide-by-28
	1111 = Divide-by-30*/
	CSL_FINS(CSL_SYSCTRL_REGS->MSPIFCDR, SYS_MSPIFCDR_FCDIV, 0);
	/*Takes the System Clock and divides it down for the McSPI functional clock(SPIREFCLK).
	0000 = Divide-by-1
	0001 = Divide-by-2
	0010 = Divide-by-4*/
	CSL_FINS(CSL_SYSCTRL_REGS->MSPIFCDR, SYS_MSPIFCDR_OCDIV, 0);
	// set the MCSPI operation mode to master
	CSL_FINS(CSL_MCSPI_REGS->MODULCTRLL, MCSPI_MODULCTRLL_MS,       CSL_MCSPI_MODULCTRLL_MS_MASTER);
	// set the inital delay to no delay
	CSL_FINS(CSL_MCSPI_REGS->MODULCTRLL, MCSPI_MODULCTRLL_INITDLY,  CSL_MCSPI_MODULCTRLL_INITDLY_NODELAY);
	// set the MCSPI to single channel mode
	CSL_FINS(CSL_MCSPI_REGS->MODULCTRLL, MCSPI_MODULCTRLL_SINGLE,  CSL_MCSPI_MODULCTRLL_SINGLE_SINGLECH);

	// set the MCSPI clock phase to be even
	CSL_FINS(CSL_MCSPI_REGS->CH0CONFL, MCSPI_CH0CONFL_PHA,  CSL_MCSPI_CH0CONFL_PHA_EVEN);
	// set the MCSPI clock polarity to active low
	CSL_FINS(CSL_MCSPI_REGS->CH0CONFL, MCSPI_CH0CONFL_POL,  CSL_MCSPI_CH0CONFL_POL_ACTIVELOW);
	// set the module clock divider
	CSL_FINS(CSL_MCSPI_REGS->CH0CONFL, MCSPI_CH0CONFL_CLKD, clock_divider);
	// set the chip select polarity to active low
	CSL_FINS(CSL_MCSPI_REGS->CH0CONFL, MCSPI_CH0CONFL_EPOL, CSL_MCSPI_CH0CONFL_EPOL_ACTIVELOW);
	// set the word length to 8 bit
	CSL_FINS(CSL_MCSPI_REGS->CH0CONFL, MCSPI_CH0CONFL_WL,   CSL_MCSPI_CH0CONFL_WL_8BIT);
	// set to duplex mode to mode 2 - transmit mode
	CSL_FINS(CSL_MCSPI_REGS->CH0CONFL, MCSPI_CH0CONFL_TRM,  CSL_MCSPI_CH0CONFL_TRM_MODE2);

	// enable data line 1 for reception (no transmission)
	CSL_FINS(CSL_MCSPI_REGS->CH0CONFU, MCSPI_CH0CONFU_DPE1, CSL_MCSPI_CH0CONFU_DPE1_NOTRANSMISSON);
	// enable data line 0 transmission
	CSL_FINS(CSL_MCSPI_REGS->CH0CONFU, MCSPI_CH0CONFU_DPE0, CSL_MCSPI_CH0CONFU_DPE0_DATLINE0);
	// select data line 1 for RX
	CSL_FINS(CSL_MCSPI_REGS->CH0CONFU, MCSPI_CH0CONFU_IS,   CSL_MCSPI_CH0CONFU_IS_LINE1);

	// disable FIFO
	CSL_FINS(CSL_MCSPI_REGS->CH0CONFU, MCSPI_CH0CONFU_FFER, CSL_MCSPI_CH0CONFU_FFER_FIFONOTUSED);
	CSL_FINS(CSL_MCSPI_REGS->CH0CONFU, MCSPI_CH0CONFU_FFEW, CSL_MCSPI_CH0CONFU_FFER_FIFONOTUSED);

	// disable all MCSPI IRQs
	CSL_MCSPI_REGS->IRQENABLEL = 0x0000;
	CSL_MCSPI_REGS->IRQENABLEU = 0x0000;

	// clear CH0STATL
	rd_value = CSL_MCSPI_REGS->CH0STATL;
	CSL_MCSPI_REGS->CH0STATL = rd_value;

	// enable channel 0
	CSL_FINS(CSL_MCSPI_REGS->CH0CTRLL,  MCSPI_CH0CTRLL_EN,  1);

	return (status);
}

/*************************************************************************
*Uint16 MCSPI_READ_WRITE_N_BYTES(unsigned int ch, unsigned short *buffer, unsigned short count,
*                    unsigned short *cmdbuffer, unsigned short cmdcount,
*                                   unsigned short readWrite )
* input:
*        buffer: read/write buffer
*        count:   read/write count
*        cmddbuffer: commandbuffer
*        cmdcount:   command length
*        readWrote:  read/write operation
* Output:
*        None
* Return:
*        None;
*Description:
*     This function is to read/write from SPI module
********************************************************************/
Uint16 MCSPI_READ_WRITE_N_BYTES(unsigned short *buffer, unsigned short count,
               unsigned short *cmdbuffer, unsigned short cmdcount, unsigned short readWrite )
{
	volatile Uint16  bufIndex, bufLen, i, stat;
	Uint16 rd_value;

	// set the force bit
	CSL_FINS(CSL_MCSPI_REGS->CH0CONFU,  MCSPI_CH0CONFU_FORCE,  1);

	if (readWrite==MCSPI_CMD_READ)
	{
		// reading data
		// write read data command first
		bufIndex = 0;
		while(bufIndex<cmdcount)
		{
			// wait for the TX register to be empty
			if (bufIndex==0)
			{
				// TX register empty
				do
				{
					stat = CSL_MCSPI_REGS->CH0STATL;
				}while ((stat&0x0002)!=0x0002);
			} else
			{
				// TX register empty and end of transmission
				do
				{
					stat = CSL_MCSPI_REGS->CH0STATL;
				}while ((stat&0x0006)!=0x0006);
			}
			// write one word
			CSL_MCSPI_REGS->TX0L = (Uint16)(cmdbuffer[bufIndex]);
			CSL_MCSPI_REGS->TX0U = (Uint16)0x0000;
			bufIndex++;
		}

		// read data
		bufIndex = 0;
		// wait for the command transmission to finish
		do
		{
			stat = CSL_MCSPI_REGS->CH0STATL;
		} while ((stat&0x0006)!=0x0006);
		// set to duplex mode to mode 1 receive mode. we should get first clock here
		CSL_FINS(CSL_MCSPI_REGS->CH0CONFL, MCSPI_CH0CONFL_TRM,  CSL_MCSPI_CH0CONFL_TRM_MODE1);
		while(bufIndex<count)
		{
			// wait for RX data ready
			do
			{
				stat = CSL_MCSPI_REGS->CH0STATL;
			} while (!(stat&0x0001));
			// read one word and provide clock for next RX data
			buffer[bufIndex] = (CSL_MCSPI_REGS->RX0L)&0x00FF;
			bufIndex++;
			rd_value = CSL_MCSPI_REGS->RX0U;
		}
		// wait for RX data ready
		do
		{
			stat = CSL_MCSPI_REGS->CH0STATL;
		} while (!(stat&0x0001));
		// clear the force bit
		CSL_FINS(CSL_MCSPI_REGS->CH0CONFU,  MCSPI_CH0CONFU_FORCE,  0);
		// set to duplex mode to mode 2 transmit mode
		CSL_FINS(CSL_MCSPI_REGS->CH0CONFL, MCSPI_CH0CONFL_TRM,  CSL_MCSPI_CH0CONFL_TRM_MODE2);
	} else if (readWrite==MCSPI_CMD_WRITE) // MCSPI read data
	{
		// write command
		bufIndex = 0;
		while(bufIndex < cmdcount)
		{
			// write the command out first
			if (bufIndex==0)
			{
				do
				{
					stat = CSL_MCSPI_REGS->CH0STATL;
				}while ((stat&0x0002)!=0x0002);
			} else
			{
				do
				{
					stat = CSL_MCSPI_REGS->CH0STATL;
				}while ((stat&0x0006)!=0x0006);
			}
			CSL_MCSPI_REGS->TX0L = (Uint16)(cmdbuffer[bufIndex]);
			CSL_MCSPI_REGS->TX0U = (Uint16)0x0000;
			bufIndex++;
		}

		// write data
		bufIndex = 0;
		while(bufIndex < count)
		{
			// write the command out first
			do
			{
				stat = CSL_MCSPI_REGS->CH0STATL;
			}while ((stat&0x0006)!=0x0006);
			CSL_MCSPI_REGS->TX0L = (Uint16)(buffer[bufIndex]);
			CSL_MCSPI_REGS->TX0U = (Uint16)0x0000;
			bufIndex++;
		}

		// wait for the transmission to finish
		do
		{
			stat = CSL_MCSPI_REGS->CH0STATL;
		} while ((stat&0x0006)!=0x0006);
		// clear the force bit
		CSL_FINS(CSL_MCSPI_REGS->CH0CONFU,  MCSPI_CH0CONFU_FORCE,  0);
	} else // MCSPI write data
	{
		// illegal operation
		return (1);
	}

	return (0);
}
