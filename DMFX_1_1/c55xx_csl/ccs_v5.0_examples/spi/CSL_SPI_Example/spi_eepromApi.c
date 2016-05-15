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

#include "spi_eepromApi.h"

#include <stdio.h>
#include "csl_general.h"
#include "csl_spi.h"
#include "csl_sysctrl.h"

#if (!defined(CHIP_C5517))

#define	SPI_FRAME_LENGTH		(1)
#define	SPI_CLK_DIV				(25)

static Uint16 commandBuf[8];


Int16 spi_eeprom_init(CSL_SpiHandle spiHandle)
{
	volatile Int16 looper;
	Int16 		   result;
	SPI_Config	   hwConfig;

	CSL_SYSCTRL_REGS->PSRCR = (Uint16)0x04;
	CSL_SYSCTRL_REGS->PRCR  = (Uint16)0x00FF;

	CSL_SPI_REGS->SPICDR   = (Uint16)0x00;
	CSL_SPI_REGS->SPICCR   = (Uint16)0x00;

	CSL_SPI_REGS->SPIDCR1  = (Uint16)0x00;
	CSL_SPI_REGS->SPIDCR2  = (Uint16)0x00;
	CSL_SPI_REGS->SPICMD1  = (Uint16)0x00;
	CSL_SPI_REGS->SPICMD2  = (Uint16)0x00;
	CSL_SPI_REGS->SPISTAT1 = (Uint16)0x00;
	CSL_SPI_REGS->SPISTAT2 = (Uint16)0x00;
	CSL_SPI_REGS->SPIDR1   = (Uint16)0x00;
	CSL_SPI_REGS->SPIDR2   = (Uint16)0x00;

	for(looper = 0; looper < 100; looper++) {;}

	/* Enable SPI with PPMODE as MODE5 */
//	CSL_SYSCTRL_REGS->EBSR = (Uint16)0x5000;
#if (defined (C5535_EZDSP))
    result = SYS_setEBSR(CSL_EBSR_FIELD_PPMODE,
                         CSL_EBSR_PPMODE_1);
#else
    result = SYS_setEBSR(CSL_EBSR_FIELD_PPMODE,
                         CSL_EBSR_PPMODE_3);
#endif
	if (CSL_SOK != result)
    {
        printf("SYS_setEBSR failed\n");
        return (result);
    }

	CSL_SPI_REGS->SPICCR = (Uint16)(1<<15);

	/** Set the hardware configuration 							*/
	hwConfig.spiClkDiv	= SPI_CLK_DIV;
	hwConfig.wLen		= SPI_WORD_LENGTH_8;
	hwConfig.frLen		= SPI_FRAME_LENGTH;
	hwConfig.wcEnable	= SPI_WORD_IRQ_ENABLE;
	hwConfig.fcEnable	= SPI_FRAME_IRQ_DISABLE;
	hwConfig.csNum		= SPI_CS_NUM_0;
	hwConfig.dataDelay	= SPI_DATA_DLY_0;
	hwConfig.csPol		= SPI_CSP_ACTIVE_LOW;
	hwConfig.clkPol		= SPI_CLKP_LOW_AT_IDLE;
	hwConfig.clkPh		= SPI_CLK_PH_FALL_EDGE;

	result = SPI_config(spiHandle, &hwConfig);

	if(CSL_SOK != result)
	{
		printf ("SPI Instance Configuration Failed\n");
		return (result);
	}
	else
	{
		printf ("SPI Instance Configured successfully\n");
	}

	CSL_SPI_REGS->SPICCR = (Uint16)(1<<15);
	/* Wait for SPI ready */
	while((CSL_SPI_REGS->SPISTAT1 & 0x0001) != 0) {};

    /* Clear Block Protection */
    commandBuf[0] = 0x01; // SPI_CMD_WRSR
    commandBuf[1] = 0xC3;

    CSL_SPI_REGS->SPICMD1 = (Uint16)0x0000 | 2 - 1;
    for (looper = 0; looper < 2; looper++)
    {
		CSL_SPI_REGS->SPIDR2 = (Uint16)commandBuf[looper] << 8;
		CSL_SPI_REGS->SPIDR1 = (Uint16)0x0000;
        CSL_SPI_REGS->SPICMD2 = (Uint16)0x0039;  //  8-bit words, read
        while((CSL_SPI_REGS->SPISTAT1 & 0x0002) == 0) {};
        commandBuf[looper] = CSL_SPI_REGS->SPIDR2 & 0xff;
	}

	while((CSL_SPI_REGS->SPISTAT1 & 0x0001) != 0) {};

    return (result);
}

/* ------------------------------------------------------------------------ *
 *  SPI_EEPROM_read( src, dst, len )                                              *
 * ------------------------------------------------------------------------ */
Int16 SPI_EEPROM_read(CSL_SpiHandle spiHandle, Uint16 src, Uint32 dst, Uint32 length )
{
    Uint16     *dst_ptr;
    CSL_Status result;

	result = CSL_SOK;

	CSL_SPI_REGS->SPICMD1 = (Uint16)0x0000 | 0;

	/* Issue Write Enable command */
	commandBuf[0] = (Uint16)SPI_CMD_WREN;
	result |= SPI_write(spiHandle, commandBuf, 1);

	CSL_SPI_REGS->SPICMD1 = (Uint16)0x0000 | length + 3 - 1;

	/* Send Read command */
    commandBuf[0] = (Uint16)SPI_CMD_READ;
    commandBuf[1] = (Uint16)(src >> 8);
    commandBuf[2] = (Uint16)src;
    result |= SPI_write(spiHandle, commandBuf, 3);

	/* Read the requested EEPROM page */
	dst_ptr = (Uint16 *)dst;
	result |= SPI_read(spiHandle, dst_ptr, length);

	return (result);
}

/* ------------------------------------------------------------------------ *
 *  SPI_EEPROM_write( src, dst, len )                                             *
 * ------------------------------------------------------------------------ */
Int16 SPI_EEPROM_write(CSL_SpiHandle spiHandle, Uint32 src, Uint16 dst, Uint32 length )
{
    Uint16     *src_ptr;
    CSL_Status result;

    result = CSL_SOK;

    /* Establish source */
    src_ptr = (Uint16 *)src;

	CSL_SPI_REGS->SPICMD1 = (Uint16)0x0000 | 0;

	/* Issue Write Enable command */
	commandBuf[0] = (Uint16)SPI_CMD_WREN;
	result |= SPI_write(spiHandle, commandBuf, 1);

	CSL_SPI_REGS->SPICMD1 = (Uint16)0x0000 | length + 3 - 1;

	commandBuf[0] = (Uint16)SPI_CMD_WRITE;
	commandBuf[1] = (Uint16)(dst >> 8);
	commandBuf[2] = (Uint16)dst;
	result |= SPI_write(spiHandle, commandBuf, 3);

	/* Write the Data Buffer to the requested EEPROM page */
	result |= SPI_write(spiHandle, src_ptr, length);

	return (result);
}

#endif /* #if (defined(CHIP_C5517)) */

