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

/** @file csl_uart_intc_example.c
 *
 *  @brief UART example to test the Interrupt mode functionality of CSL UART
 *
 *
 * \page    page18  CSL UART EXAMPLE DOCUMENTATION
 *
 * \section UART3   UART EXAMPLE3 - INTERRUPT MODE TEST
 *
 * \subsection UART3x    TEST DESCRIPTION:
 *		This test is to verify the CSL UART module operation in interrupt mode.
 * This test code communicates with the HyperTerminal on the host PC through
 * UART module on C5535 DSP operating in interrupt mode. UART peripheral is
 * configured by the test code to the following values.
 * Baud Rate - 2400.
 * Data bits - 8.
 * Parity - None.
 * Stop bits - 1.
 * Flow control - None.
 * HyperTerminal on the host PC should be configured with the same values to
 * to proper communication with the C5535/C5515/C5517 EVM. C5535/C5515/C5517 EVM should be
 * connected to the host PC using an RS232 cable and HyperTerminal on the host
 * PC should be opened and connected.This test works at all the PLL frequencies.
 *
 * This test code reads characters entered by the user on the HypeTerminal
 * continuously one character at a time. For each character entered on the
 * HyperTerminal, URAT generates a receive interrupt and the character is
 * read in the Rx ISR. Symbol '$' needs to be entered on the HyperTerminal to
 * terminate the test.
 * Upon running the test, details of the test will be displayed on the
 * HyperTerminal. A message will be displayed prompting to enter a character.
 * Character entered on the HyperTerminal will be read by the test code and
 * and echoed back to the HyperTerminal until the symbol '$' is entered on the
 * HyperTerminal. Messages displayed to direct the user will be transmitted to
 * the HyperTerminal using Polled mode.
 *
 * @verbatim
   Messages displayed on the HyperTerminal during this test will be as shown
   below.

      ====================================================================
      ==                                                                ==
      ==  UART INTERRUPT TEST!                                          ==
      ==  TEST READS A CHARACTER FROM HYPERTERMINAL CONTINUOUSLY        ==
      ==  ENTER '$' TO END THE TEST                                     ==
      ==                                                                ==
      ==  Enter char:C                                                  ==
      ==  C                                                             ==
      ==  Enter char:S                                                  ==
      ==  S                                                             ==
      ==  Enter char:L                                                  ==
      ==  L                                                             ==
      ==  Enter char:$                                                  ==
      ==  $                                                             ==
      ==                                                                ==
      ==  YOU HAVE ENTERED '$'.                                         ==
      ==  END OF THE TEST!!                                             ==
      ==                                                                ==
      ====================================================================

  @endverbatim
 *
 * Manual inspection is required to verify the success of each step.
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5535, C5515
 * AND C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 *
 * \subsection UART3y    TEST PROCEDURE:
 *  @li Connect the RS232 cable; One end to the UART port on the C5535/C5515/C5517 EVM(J13/J1/J2)
 *      and other  to the COM port of the Windows Xp PC.
 *  @li Open the HyperTerminal on the Host PC. To open the HyperTerminal click
 *      on 'Start->Programs->Accessories->Communications->HyperTerminal’
 *  @li Disconnect the HyperTerminal by selecting menu Call->Disconnect
 *  @li Select Menu 'File->Properties' and click on the Button 'Configure'
 *     \n  Set 'Bits Per Second(Baud Rate)' to 2400.
 *     \n  Set 'Data bits' to 8.
 *     \n  Set 'Parity' to None.
 *     \n  Set 'Stop bits' to 1.
 *     \n  Set 'Flow control' to None.
 *     \n  Click on 'OK' button.
 *     \n  HyperTerminal is configured and ready for communication
 *  @li Connect the HyperTerminal by selecting menu Call->Call.
 *  @li Open the CCS and connect the target (C5535/C5515/C5517 EVM)
 *  @li Open the project "CSL_UART_IntExample.pjt" and build it
 *  @li Load the program on to the target
 *  @li Set the PLL frequency to 12.288MHz
 *  @li Run the program and observe the test result
 *  @li Repeat the test at the following PLL frequencies
 *      C5515: 60MHz and 100MHz
 *      C5517: 60MHz, 100MHz, 150MHz and 200MHz
 *      C5535 eZdsp: 60MHz and 100MHz
 *  @li Repeat the test in Release mode
 *
 * \subsection UART3z    TEST RESULT:
 *  @li All the CSL APIs should return success
 *  @li Characters entered on the HyperTerminal should be read and echoed back
 *      to the HyperTerminal properly.
 *  @li Data sent by the UART should be displayed on the HyperTerminal properly
 *
 */

/* ============================================================================
 * Revision History
 * ================
 * 05-Sept-2008 Created
 * 23-Jul-2012 Added C5517 Compatibility
 * ============================================================================
 */

#include <stdio.h>
#include "csl_uart.h"
#include "csl_uartAux.h"
#include "csl_intc.h"
#include "csl_general.h"
#include "csl_sysctrl.h"

#define CSL_TEST_FAILED         (1U)
#define CSL_TEST_PASSED         (0)
#define	CSL_UART_BUF_LEN    	(4U)

/* Global constants */
/* String length to be received and transmitted */
#define WR_STR_LEN        80
#define RD_STR_LEN        10

#define CSL_PLL_DIV_000    (0)
#define CSL_PLL_DIV_001    (1u)
#define CSL_PLL_DIV_002    (2u)
#define CSL_PLL_DIV_003    (3u)
#define CSL_PLL_DIV_004    (4u)
#define CSL_PLL_DIV_005    (5u)
#define CSL_PLL_DIV_006    (6u)
#define CSL_PLL_DIV_007    (7u)

#define CSL_PLL_CLOCKIN    (32768u)

#define PLL_CNTL1        *(ioport volatile unsigned *)0x1C20
#define PLL_CNTL2        *(ioport volatile unsigned *)0x1C21
#define PLL_CNTL3        *(ioport volatile unsigned *)0x1C22
#define PLL_CNTL4        *(ioport volatile unsigned *)0x1C23

/* Global data definition */
/* UART setup structure */
CSL_UartSetup mySetup =
{
	/* Input clock freq in MHz */
    60000000,
	/* Baud rate */
    2400,
	/* Word length of 8 */
    CSL_UART_WORD8,
	/* To generate 1 stop bit */
    0,
	/* Disable the parity */
    CSL_UART_DISABLE_PARITY,
	/* Enable trigger 14 fifo */
	CSL_UART_FIFO_DMA1_DISABLE_TRIG14,
	/* Loop Back enable */
	CSL_UART_NO_LOOPBACK,
	/* No auto flow control*/
	CSL_UART_NO_AFE ,
	/* No RTS */
	CSL_UART_NO_RTS ,
};

/* CSL UART data structures */
CSL_UartObj uartObj;
CSL_UartHandle hUart;

/* CSL UART data buffers */
Uint16    uartIntcWriteBuff[CSL_UART_BUF_LEN ];
Uint16    uartIntcReadBuff[CSL_UART_BUF_LEN ];
char      buffer1[30] = "\r\nEnter char:";
char      buffer2[10] = {0};
char      buffer3[30] = "\r\n";
Uint16    gcount = 16;
Uint16    mutex = 0;
char      *ptr = buffer1;
Uint16    setbit;
volatile Bool    endOfTest = FALSE;

/* Interrupt vector start address */
extern void VECSTART(void);

/**
 *  \brief  Function to calculate the system clock
 *
 *  \param    none
 *
 *  \return   System clock value in Hz
 */
Uint32 getSysClk(void);

/**
 *  \brief  Interrupt Service Routine to handle UART Line Status Interrupt
 *
 *  \param  none
 *
 *  \return none
 */
void uart_lsiIsr(void)
{
	Uint16 reg;
	reg=hUart->uartRegs->LSR;
	reg= reg;
}

/**
 *  \brief  Interrupt Service Routine to handle UART Character Timeout Interrupt
 *
 *  \param  none
 *
 *  \return none
 */
void uart_ctoIsr(void)
{
	UART_read( hUart,buffer2,0,0);
	ptr = buffer2;
	gcount = 1;
  	UART_eventEnable(hUart,CSL_UART_XMITOR_REG_EMPTY_INTERRUPT);
}

/**
 *  \brief  Interrupt Service Routine to handle UART Transmit Interrupt
 *
 *  \param  none
 *
 *  \return none
 */
void uart_txIsr(void)
{
	if(gcount == 1)
	{
		UART_write(hUart, buffer3, 2, 0);
		UART_write(hUart, ptr, gcount, 0);
		if(*ptr == '$')
		{
			endOfTest = TRUE;
		}
		gcount = 13;
	}

	if(endOfTest == FALSE)
	{
		UART_write( hUart,buffer1,gcount,0);
	}

	UART_eventDisable(hUart, CSL_UART_XMITOR_REG_EMPTY_INTERRUPT);
}

/**
 *  \brief  Interrupt Service Routine to handle UART Receive Interrupt
 *
 *  \param  none
 *
 *  \return none
 */
void uart_rxIsr(void)
{
	UART_read( hUart,buffer2,0,0);
	gcount = 1;
 	UART_eventEnable(hUart,CSL_UART_XMITOR_REG_EMPTY_INTERRUPT);
}

/**
 *  \brief  Interrupt Dispatcher to identify interrupt source
 *
 *  This function identify the type of UART interrupt generated and
 *  calls concerned ISR to handle the interrupt
 *
 *  \param  none
 *
 *  \return none
 */
interrupt void UART_intrDispatch(void)
{
	Uint16 eventId = 0;

	IRQ_disable(UART_EVENT);

	/* Get the event Id which caused interrupt */
	eventId = UART_getEventId(hUart);

	if (((void (*)(void))(hUart->UART_isrDispatchTable[eventId])))
	{
		((void (*)(void))(hUart->UART_isrDispatchTable[eventId]))();
	}

	IRQ_enable(UART_EVENT);

	return;
}

/**
 *  \brief  UART interrupt Test function
 *
 *   This function verifies the UART operation in interrupt mode.
 *   This function runs in an infinite loop to read the characters
 *   from HyperTerminal and echo the characters back to HyperTerminal.
 *
 *  \param  none
 *
 *  \return Test result(Only Failure Case)
 */
CSL_Status uart_IntcSample(void)
{
	CSL_UartIsrAddr    isrAddr;
	CSL_Status         status;
	Uint32            sysClk;

	sysClk = getSysClk();

	mySetup.clkInput = sysClk;

    status = SYS_setEBSR(CSL_EBSR_FIELD_PPMODE,
                         CSL_EBSR_PPMODE_1);
    if(CSL_SOK != status)
    {
        printf("SYS_setEBSR failed\n");
        return(status);
    }

    /* Loop counter and error flag */
    status = UART_init(&uartObj,CSL_UART_INST_0,UART_INTERRUPT);
    if(CSL_SOK != status)
    {
        printf("UART_init failed error code %d\n",status);
        return(status);
    }
	else
	{
		printf("UART_init Successful\n");
	}

    status = SYS_setEBSR(CSL_EBSR_FIELD_PPMODE,
                         CSL_EBSR_PPMODE_1);
    if(CSL_SOK != status)
    {
        printf("SYS_setEBSR failed\n");
        return (status);
    }

    /* Handle created */
    hUart = (CSL_UartHandle)(&uartObj);

    /* Configure UART registers using setup structure */
    status = UART_setup(hUart,&mySetup);
    if(CSL_SOK != status)
    {
        printf("UART_setup failed error code %d\n",status);
        return(status);
    }
	else
	{
		printf("UART_setup Successful\n");
	}

	/* Send the details of the test to HyperTerminal */
   	status = UART_fputs(hUart,"\r\n\nUART INTERRUPT TEST!",0);
    if(CSL_SOK != status)
    {
        printf("UART_fputs failed error code %d\n",status);
        return(status);
    }

   	status = UART_fputs(hUart,"\r\nTEST READS A CHARACTER FROM HYPERTERMINAL CONTINUOUSLY",0);
    if(CSL_SOK != status)
    {
        printf("UART_fputs failed error code %d\n",status);
        return(status);
    }

   	status = UART_fputs(hUart,"\r\nENTER '$' TO END THE TEST\r\n",0);
    if(CSL_SOK != status)
    {
        printf("UART_fputs failed error code %d\n",status);
        return(status);
    }

	/* Configure and Register the UART interrupts */
	isrAddr.rbiAddr  = uart_rxIsr;
	isrAddr.tbeiAddr = uart_txIsr;
	isrAddr.ctoi     = uart_ctoIsr;
	isrAddr.lsiAddr  = uart_lsiIsr;

    /* Disable interrupt */
    IRQ_globalDisable();

    /* Clear any pending interrupts */
	IRQ_clearAll();

	/* Disable all the interrupts */
	IRQ_disableAll();

	IRQ_setVecs((Uint32)(&VECSTART));

	/* Configuring Interrupt */
	IRQ_plug (UART_EVENT, &UART_intrDispatch);

	/* Enabling Interrupt */
	IRQ_enable(UART_EVENT);
	IRQ_globalEnable();

	/* Set the UART callback function */
 	status = UART_setCallback(hUart,&isrAddr);
	if(status != CSL_SOK)
	{
		printf("UART_setCallback Failed\n");
		return(status);
	}

 	/* Enable the UART Events */
	status = UART_eventEnable(hUart, CSL_UART_XMITOR_REG_EMPTY_INTERRUPT);
	if(status != CSL_SOK)
	{
		printf("UART_eventEnable Failed\n");
		return(status);
	}

	status = UART_eventEnable(hUart, CSL_UART_RECVOR_REG_DATA_INTERRUPT);
	if(status != CSL_SOK)
	{
		printf("UART_eventEnable Failed\n");
		return(status);
	}

	status = UART_eventEnable(hUart, CSL_UART_RECVOR_LINE_STATUS_INTERRUPT);
	if(status != CSL_SOK)
	{
		printf("UART_eventEnable Failed\n");
		return(status);
	}

	/* Tests runs until users enters Symbol '$' on the HyperTerminal */
	while(endOfTest == FALSE)
	{

	}

	printf("\nUSER ENTERED '$' on HyperTerminal\n");
	printf("END OF TEST!\n");

	/* Disable UART interrupts */
	IRQ_disable(UART_EVENT);

	/* Disable GLobal Interrupts */
	IRQ_globalDisable();

	/* Send the END OF TEST MESSAGE to HyperTerminal */
   	status = UART_fputs(hUart,"\r\n\nYOU HAVE ENTERED '$'.",0);
    if(CSL_SOK != status)
    {
        printf("UART_fputs failed error code %d\n",status);
        return(status);
    }

   	status = UART_fputs(hUart,"\r\nEND OF THE TEST!!\r\n",0);
    if(CSL_SOK != status)
    {
        printf("UART_fputs failed error code %d\n",status);
        return(status);
    }

    /* Disable interrupt */
    IRQ_globalDisable();

    /* Clear any pending interrupts */
	IRQ_clearAll();

	/* Disable all the interrupts */
	IRQ_disableAll();

	return(CSL_SOK);
}

/**
 *  \brief  Configures Dma
 *
 *  \param  chanNum - Dma channel number
 *
 *  \return Dma handle
 */
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 1 --
   /////  Define PaSs_StAtE variable for catching errors as program executes.
   /////  Define PaSs flag for holding final pass/fail result at program completion.
        volatile Int16 PaSs_StAtE = 0x0001; // Init to 1. Reset to 0 at any monitored execution error.
        volatile Int16 PaSs = 0x0000; // Init to 0.  Updated later with PaSs_StAtE when and if
   /////                                  program flow reaches expected exit point(s).
   /////
void main()
{
	CSL_Status   status;

	printf("CSL UART INTERRUPT MODE TEST!\n\n");
	printf("Test Reads One Character at a time from the HyperTerminal\n");
	printf("Enter the Symbol - '$' on the HyperTerminal to Terminate the Test\n\n");

	status = uart_IntcSample();
	if(status != CSL_SOK)
	{
		printf("\nCSL UART INTERRUPT MODE TEST FAILED!!\n");
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 2 --
   /////  Reseting PaSs_StAtE to 0 if error detected here.
        PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
   /////
	}
	else
	{
		printf("\nCSL UART INTERRUPT MODE TEST COMPLETED!!\n");
	}
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 3 --
   /////  At program exit, copy "PaSs_StAtE" into "PaSs".
        PaSs = PaSs_StAtE; //If flow gets here, override PaSs' initial 0 with
   /////                   // pass/fail value determined during program execution.
   /////  Note:  Program should next exit to C$$EXIT and halt, where DSS, under
   /////   control of a host PC script, will read and record the PaSs' value.
   /////
}

/**
 *  \brief  Function to calculate the clock at which system is running
 *
 *  \param    none
 *
 *  \return   System clock value in Hz
 */
#if (defined(CHIP_C5505_C5515) || defined(CHIP_C5504_C5514))

Uint32 getSysClk(void)
{
	Bool      pllRDBypass;
	Bool      pllOutDiv;
	Uint32    sysClk;
	Uint16    pllM;
	Uint16    pllRD;
	Uint16    pllOD;

	pllM = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR1, SYS_CGCR1_M);

	pllRD = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR2, SYS_CGCR2_RDRATIO);
	pllOD = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR4, SYS_CGCR4_ODRATIO);

	pllRDBypass = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR2, SYS_CGCR2_RDBYPASS);
	pllOutDiv   = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR4, SYS_CGCR4_OUTDIVEN);

	sysClk = CSL_PLL_CLOCKIN;

	if (0 == pllRDBypass)
	{
		sysClk = sysClk/(pllRD + 4);
	}

	sysClk = (sysClk * (pllM + 4));

	if (1 == pllOutDiv)
	{
		sysClk = sysClk/(pllOD + 1);
	}

	/* Return the value of system clock in KHz */
	return(sysClk);
}

#elif (defined(CHIP_C5517))
Uint32 getSysClk(void)
{
	Uint32    sysClk;
	float    Multiplier;
	Uint16    OD;
	Uint16    OD2;
	Uint16    RD, RefClk;
	Uint32	  temp1, temp2, temp3, vco;
	//Uint16 DIV;

	temp2 =  PLL_CNTL2;
	temp3 =  (temp2 & 0x8000) <<1 ;
	temp1 = temp3 + PLL_CNTL1;
	Multiplier = temp1/256 +1;
	RD = (PLL_CNTL2 & 0x003F) ;

	RefClk = 12000/(RD+1);

	vco = Multiplier * (Uint32)RefClk;

	OD = (PLL_CNTL4 & 0x7);

	sysClk = vco/(OD+1);

	OD2 = ((PLL_CNTL4 >> 10) & 0x1F) ;

	if (PLL_CNTL3 & 0x8000)	// PLL Bypass
		sysClk = RefClk;
	else
		sysClk = vco/(OD+1);

	if ((PLL_CNTL4 & 0x0020) == 0)	/* OutDiv2 */
		sysClk = sysClk / ( 2*(OD2+1));

	/* Return the value of system clock in KHz */
	return(sysClk*1000);
}
#else

Uint32 getSysClk(void)
{
	Bool      pllRDBypass;
	Bool      pllOutDiv;
	Bool      pllOutDiv2;
	Uint32    sysClk;
	Uint16    pllVP;
	Uint16    pllVS;
	Uint16    pllRD;
	Uint16    pllVO;
	Uint16    pllDivider;
	Uint32    pllMultiplier;

	pllVP = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR1, SYS_CGCR1_MH);
	pllVS = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR2, SYS_CGCR2_ML);

	pllRD = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR2, SYS_CGCR2_RDRATIO);
	pllVO = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR4, SYS_CGCR4_ODRATIO);

	pllRDBypass = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR2, SYS_CGCR2_RDBYPASS);
	pllOutDiv   = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR4, SYS_CGCR4_OUTDIVEN);
	pllOutDiv2  = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR4, SYS_CGCR4_OUTDIV2BYPASS);

	pllDivider = ((pllOutDiv2) | (pllOutDiv << 1) | (pllRDBypass << 2));

	pllMultiplier = ((Uint32)CSL_PLL_CLOCKIN * ((pllVP << 2) + pllVS + 4));

	switch(pllDivider)
	{
		case CSL_PLL_DIV_000:
		case CSL_PLL_DIV_001:
			sysClk = pllMultiplier / (pllRD + 4);
		break;

		case CSL_PLL_DIV_002:
			sysClk = pllMultiplier / ((pllRD + 4) * (pllVO + 4) * 2);
		break;

		case CSL_PLL_DIV_003:
			sysClk = pllMultiplier / ((pllRD + 4) * 2);
		break;

		case CSL_PLL_DIV_004:
		case CSL_PLL_DIV_005:
			sysClk = pllMultiplier;
		break;

		case CSL_PLL_DIV_006:
			sysClk = pllMultiplier / ((pllVO + 4) * 2);
		break;

		case CSL_PLL_DIV_007:
			sysClk = pllMultiplier / 2;
		break;
	}

	/* Return the value of system clock in KHz */
	return(sysClk);
}

#endif
