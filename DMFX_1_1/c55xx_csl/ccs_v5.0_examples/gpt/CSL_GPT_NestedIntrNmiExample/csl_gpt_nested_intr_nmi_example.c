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

 /* @file csl_gpt0_nested_intr_nmi_example.c
 *
 *  @brief Example test code to verify the CSL GPT module's nested interrupt functionality
 */

/** \page    page4  GPT EXAMPLE DOCUMENTATION
 *
 * \section GPT2   GPT EXAMPLE 2 - Nested interrupt functionality of TIMERS
 *
 * \subsection GPT2x    TEST DESCRIPTION:
 *		This test code verifies the functionality of all the timers.
 * GPTx module of C5517 DSP is used to Non Maskable Interrupt also. The normal TINT
 * interrupts to the CPU to schedule periodic tasks or a delayed task.But the NMI which will
 * have the higher priority above all interrupts and cannot be masked out.
 * In this example TIMER0 is configured for producing NMI and TIMER2,TIMER for TINT.
 * The TIMER2 is given a less count value than TIMER1 and TIMER0. The example demonstrates
 * a condition in which TIMER2 ISR hits first, while inside ISR, NMI triggers and same time
 * or in NMI ISR, TIMER1 TINT triggers. After executing NMI, it should return to TIMER2
 * and then service TIMER1.
 *
 * In this example, the print logs shows the order in which ISR's executed.
 *
 * GPT can be configured with a counter and pre-scaler divider value. When the
 * GPT is started counter will be decremented to 0. The rate at which this
 * count happens can be controlled by the prescaler divider value. GPT clock
 * is derived by dividing the CPU clock with the pre-scaler divider value.
 *
 * This test is having two parts. First part of the test verifies whether the
 * GPT is decrementing the counter or not. GPT is configured with a count value
 * 0xFFFF and pre-scaler divider value 256. GPT is started and counter value is
 * read. After few cycles of delay GPT counter value is read again. Both the
 * counter values are compared to verify whether the second count value is less
 * than the first counter value or not.
 *
 * During second part of the test timer count rate of the GPT is verified.
 * As the GPT clock is derived from the CPU clock based on the pre-scaler
 * value it possible to verify the GPT rate with respect to CPU clock rate.
 * GPT pre-scaler value is set to divide by 4. So the counter runs at a speed
 * that is 1/4th of the CPU clock. Clock at which the CPU is running during the
 * test is calculated using the function getSysClk(). This function returns
 * the value of system clock in KHz. Value of the system clock in KHz
 * represents the CPU clock cycles for one millisecond. GPT count value is set
 * to 1/4th of the CPU clock cycles for one millisecond. With this setup GPT
 * should take one millisecond to count down the counter value to 0. In other
 * way clock cycles executed by the CPU from the time GPT is started to the
 * time counter value reaches 0 should be equal to the number of cycles that
 * are executed by the CPU in one millisecond.GPT is configured to generate
 * interrupt when the count value reaches to 0. GPT is started and the CPU
 * clock cycles are counted in while loop. This loop increments a global
 * variable 'cpuCycleCount' until the GPT generate time-out interrupt.
 * This while loop execution is taking 12 CPU cycles. So the total CPU cycles
 * executed will be 12*cpuCycleCount. At the end of the test this CPU cycles
 * calculated are compared with the CPU clock cycles that will be executed in
 * one millisecond. The calculated CPU cycles can vary ±1% from the actual
 * CPU cycles. If this condition is satisfied GPT is running at the configured
 * rate.
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5517.
 * MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 *
 * \subsection GPT2y    TEST PROCEDURE:
 *  @li Open the CCS and connect the target (C5517 EVM)
 *  @li Open the project "CSL_GPT0_Extclk_GPIO0_Example.pjt" and build it
 *  @li Load the program on to the target
 *  @li Set the PLL frequency to 100MHz
 *  @li Repeat the test at PLL frequencies 60MHz, 100MHz, 150MHz and 200MHz
 *  @li Run the program and observe the test result
 *  @li Repeat the test in Release mode
 *
 * \subsection GPT2z    TEST RESULT:
 *  @li All the CSL APIs should return success
 *  @li Order of ISR hit of Timers in print log should be correct.
 *
 *
 *
 */

/* ============================================================================
 * Revision History
 * ================
 * 28-Oct-2011 Created
 * 30-Jul-2012 Added C5517 Compatibility
 * ============================================================================
 */

#include <stdio.h>
#include "csl_gpt.h"
#include "csl_intc.h"
#include <csl_general.h>

#define CSL_TEST_FAILED    (1u)
#define CSL_TEST_PASSED    (0)

#define CSL_PLL_DIV_000    (0)
#define CSL_PLL_DIV_001    (1u)
#define CSL_PLL_DIV_002    (2u)
#define CSL_PLL_DIV_003    (3u)
#define CSL_PLL_DIV_004    (4u)
#define CSL_PLL_DIV_005    (5u)
#define CSL_PLL_DIV_006    (6u)
#define CSL_PLL_DIV_007    (7u)

#define CSL_PLL_CLOCKIN    (32768u)

//Control Registers for PLL
#define PLL_CNTL1        *(ioport volatile unsigned *)0x1C20
#define PLL_CNTL2        *(ioport volatile unsigned *)0x1C21
#define PLL_CNTL3        *(ioport volatile unsigned *)0x1C22
#define PLL_CNTL4        *(ioport volatile unsigned *)0x1C23

#define Timer_IntrSelReg *(ioport volatile unsigned *)0x1C3E

extern void VECSTART(void);
Uint32    cpuCycleCount = 0;
Uint32    sysClk;
volatile Uint16    hitIsr = 0;
volatile unsigned char continue_flag=0;

volatile unsigned char ISR_FLOW[4]={0xFF,0xFF,0xFF,0xFF};
volatile int idx = 0;
volatile unsigned char REG_Value=0;
volatile unsigned char *ptr=ISR_FLOW;

/**
 *  \brief  GPT Count Rate Verification test function
 *
 * This function verifies whether timer is running at the configured
 * rate or not. This function configures and enables the GPT interrupts
 * to indicate the expiry of the timer count. Test counts number of
 * cycles executed by the CPU after starting the timer till the expiry
 * of the timer. These cycles are used to verify the WDT count rate.
 * It is assumed that calculated CPU cycles will be with in the range
 * ±1% actual CPU clock cycles. Test will be successful if the calculated
 * CPU cycles fall with in this range.
 *
 * NOTE: Changing the PLL setting in the middle (After getSysClk() call)
 *       of the test will result in test failure.
 *
 *  \param  none
 *
 *  \return CSL_TEST_PASSED  - Success
 *          CSL_TEST_FAILED  - Failure
 */
Int16 CSL_gptIntrTest(void);

/**
 *  \brief  Function to calculate the system clock
 *
 *  \param    none
 *
 *  \return   System clock value in KHz
 */
Uint32 getSysClk(void);

/*GPT Interrupt Service Routine*/
interrupt void gptIsr(void);

/*GPT Non Maskable Interrupt Service Routine*/
interrupt void nmiIsr(void);

/*GPT1 Interrupt Service code*/
void gpt1Isr(void);

/*GPT2 Interrupt Service code*/
void gpt2Isr(void);
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 1 --
   /////  Define PaSs_StAtE variable for catching errors as program executes.
   /////  Define PaSs flag for holding final pass/fail result at program completion.
        volatile Int16 PaSs_StAtE = 0x0001; // Init to 1. Reset to 0 at any monitored execution error.
        volatile Int16 PaSs = 0x0000; // Init to 0.  Updated later with PaSs_StAtE when and if
   /////                                  program flow reaches expected exit point(s).
   /////

CSL_Handle    hGpt2= NULL;

void main(void)
{
	Int16	result;

	printf("General Purpose Timers Interrupt test!\n\n");

#if (defined (CHIP_C5517))
	/* Test to verify the Timer Interrupts */
	result = CSL_gptIntrTest();
	if(CSL_TEST_FAILED == result)
	{
		printf("TIMER0 NMI INTERRUPT VERIFICATION TEST FAILED!!\n");
		PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
	}
	else
	{
		printf("TIMER0 NMI INTERRUPT VERIFICATION TEST PASSED!!\n");
	}
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 3 --
   /////  At program exit, copy "PaSs_StAtE" into "PaSs".
   PaSs = PaSs_StAtE; //If flow gets here, override PaSs' initial 0 with
   /////                   // pass/fail value determined during program execution.
   /////  Note:  Program should next exit to C$$EXIT and halt, where DSS, under
  /////   control of a host PC script, will read and record the PaSs' value.
  /////
#else
    printf("Non-Maskable Interrupt NOT SUPPORTED by the Platform\n");
    printf("Hence, cannot run this example\n");
#endif
}



Int16 CSL_gptIntrTest(void)
{
	CSL_Handle    hGpt0,hGpt1;//,hGpt2;
	CSL_Status 	  status;
	CSL_Config 	  hwConfig;
	CSL_GptObj	  gptObj0,gptObj1,gptObj2;
	int i=0;

	hitIsr   = FALSE;
	status   = 0;

	/* Get the System clock value at which CPU is currently running */
	sysClk = getSysClk();

	/* Open the CSL GPT modules */
	hGpt0 = GPT_open (GPT_0, &gptObj0, &status);
	if((NULL == hGpt0) || (CSL_SOK != status))
	{
		printf("GPT0 Open Failed\n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf("GPT0 Open Successful\n");
	}

	hGpt1 = GPT_open (GPT_1, &gptObj1, &status);
	if((NULL == hGpt1) || (CSL_SOK != status))
	{
		printf("GPT1 Open Failed\n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf("GPT1 Open Successful\n");
	}

	hGpt2 = GPT_open (GPT_2, &gptObj2, &status);
	if((NULL == hGpt2) || (CSL_SOK != status))
	{
		printf("GPT2 Open Failed\n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf("GPT2 Open Successful\n");
	}

	/* Reset the GPT modules */
	status = GPT_reset(hGpt0);
	if(CSL_SOK != status)
	{
		printf("GPT0 Reset Failed\n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf("GPT0 Reset Successful\n");
	}

	status = GPT_reset(hGpt1);
	if(CSL_SOK != status)
	{
		printf("GPT1 Reset Failed\n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf("GPT1 Reset Successful\n");
	}

	status = GPT_reset(hGpt2);
	if(CSL_SOK != status)
	{
		printf("GPT2 Reset Failed\n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf("GPT2 Reset Successful\n");
	}


	Timer_IntrSelReg = 0x0001;//To route the timer0 isr to NMI

	/* Clear any pending interrupts */
	IRQ_clearAll();

	/* Disable all the interrupts */
	IRQ_disableAll();

	IRQ_setVecs((Uint32)(&VECSTART));

	IRQ_plug(TINT_EVENT, &gptIsr);
	IRQ_enable(TINT_EVENT);

	IRQ_plug(NMI_EVENT, &nmiIsr);
	IRQ_enable(NMI_EVENT);


	/* Configuring the GPT0 */
	hwConfig.autoLoad 	 = GPT_AUTO_DISABLE;
	hwConfig.ctrlTim 	 = GPT_TIMER_ENABLE;
	hwConfig.preScaleDiv = GPT_PRE_SC_DIV_8;
	hwConfig.prdLow 	 = 0x0000;
	hwConfig.prdHigh 	 = 0x0002;

	/* Configure the GPT0 module */
	status =  GPT_config(hGpt0, &hwConfig);
	if(CSL_SOK != status)
	{
		printf("GPT0 Config Failed\n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf("GPT0 Config Successful\n");
	}

	/* Configuring the GPT1 */
	hwConfig.autoLoad 	 = GPT_AUTO_DISABLE;
	hwConfig.ctrlTim 	 = GPT_TIMER_ENABLE;
	hwConfig.preScaleDiv = GPT_PRE_SC_DIV_8;
	hwConfig.prdLow 	 = 0x0000;
	hwConfig.prdHigh 	 = 0x0002;

	/* Configure the GPT1 module */
	status =  GPT_config(hGpt1, &hwConfig);
	if(CSL_SOK != status)
	{
		printf("GPT1 Config Failed\n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf("GPT1 Config Successful\n");
	}

	/* Configuring the GPT2 */
	hwConfig.autoLoad 	 = GPT_AUTO_DISABLE;
	hwConfig.ctrlTim 	 = GPT_TIMER_ENABLE;
	hwConfig.preScaleDiv = GPT_PRE_SC_DIV_8;
	hwConfig.prdLow 	 = 0xFFFF;
	hwConfig.prdHigh 	 = 0x0001;

	/* Configure the GPT2 module */
	status =  GPT_config(hGpt2, &hwConfig);
	if(CSL_SOK != status)
	{
		printf("GPT2 Config Failed\n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf("GPT2 Config Successful\n");
	}

	/* Enable CPU Interrupts */
	IRQ_globalEnable();

	REG_Value=0;
	/* Start the Timer2 */
	GPT_start(hGpt2);

	/* Start the Timer1 */
	GPT_start(hGpt1);

	/* Start the Timer0 */
	GPT_start(hGpt0);


	//while(!nmi_gpt0); //Check all the timers are generating the interrupts
	while(!continue_flag);
    printf("\r\nNo. of TINT ISR hit = %d\n\n", hitIsr);
	for(i=0;i<4;++i)
	{
		printf(" %d.TIMER%c\n",i+1,ISR_FLOW[i]);
	}

	printf("\r\nTIMER1 Flag of TIAFR read in NMI ISR = %x\r\n",(REG_Value&0x02)?1:0);
	printf("\r\nTimer0 produced the NMI while executing TIMER2 ISR!\r\n");
	//Execute the below code if you want to stop timers after some time.
	/* Disable the CPU interrupts */
	IRQ_globalDisable();

	/* Clear any pending interrupts */
	IRQ_clearAll();

	/* Disable all the interrupts */
	IRQ_disableAll();

	/* Stop the Timer0 */
	status = GPT_stop(hGpt0);
	if(CSL_SOK != status)
	{
		printf("GPT0 Stop Failed \n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf("GPT0 Stop Successful\n");
	}

	/* Stop the Timer1 */
	status = GPT_stop(hGpt1);
	if(CSL_SOK != status)
	{
		printf("GPT1 Stop Failed \n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf("GPT1 Stop Successful\n");
	}

	/* Stop the Timer2 */
	status = GPT_stop(hGpt2);
	if(CSL_SOK != status)
	{
		printf("GPT2 Stop Failed \n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf("GPT2 Stop Successful\n");
	}



	status = GPT_reset(hGpt0);
	status = GPT_reset(hGpt1);
	status = GPT_reset(hGpt2);

	/* Close The GPT0 Module */
	status = GPT_close(hGpt0);
	if(CSL_SOK != status)
	{
		printf("GPT0 Close Failed\n");
		return (CSL_TEST_FAILED);
	}

	/* Close The GPT1 Module */
	status = GPT_close(hGpt1);
	if(CSL_SOK != status)
	{
		printf("GPT1 Close Failed\n");
		return (CSL_TEST_FAILED);
	}

	/* Close The GPT2 Module */
	status = GPT_close(hGpt2);
	if(CSL_SOK != status)
	{
		printf("GPT2 Close Failed\n");
		return (CSL_TEST_FAILED);
	}

    if(('2'==ISR_FLOW[0])&&('0'==ISR_FLOW[1])&&('4'==ISR_FLOW[2])&&('1'==ISR_FLOW[3]))
	{
	 	return (CSL_TEST_PASSED);
    }
    else
	{
		return (CSL_TEST_FAILED);
	}
}

/**
 *  \brief  GPT Interrupt Service Routine
 *
 *  \param  none
 *
 *  \return none
 */
interrupt void gptIsr(void)
{
	volatile unsigned char TIAFR_VAL=0;
	hitIsr++;

	TIAFR_VAL = CSL_SYSCTRL_REGS->TIAFR;
    IRQ_clear(TINT_EVENT);

	if((TIAFR_VAL&0x02) == 0x02)
	{
	 gpt1Isr();
	}
	if((TIAFR_VAL&0x04) == 0x04)
	{
	 gpt2Isr();
	}

}


/**
 *  \brief  NMI Interrupt Service Routine
 *
 *  \param  none
 *
 *  \return none
 */
interrupt void nmiIsr(void)
{
    REG_Value = CSL_SYSCTRL_REGS->TIAFR ;
    CSL_SYSCTRL_REGS->TIAFR = 0x01;
	//*ptr++='0';
	ISR_FLOW[idx] = '0';
	idx++;
}

/**
 *  \brief  GPT1 Interrupt Service code
 *
 *  \param  none
 *
 *  \return none
 */
void gpt1Isr(void)
{
    /* Clear Timer Interrupt Aggregation Flag Register (TIAFR) */
    CSL_SYSCTRL_REGS->TIAFR = 0x02;
	/* Start the Timer2 */
	continue_flag=1;
	//*ptr++='1';
	ISR_FLOW[idx] = '1';
	idx++;
}

/**
 *  \brief  GPT2 Interrupt Service code
 *
 *  \param  none
 *
 *  \return none
 */
void gpt2Isr(void)
{
    //unsigned char i=0;
    volatile Uint32 i;
    //*ptr++='2';
	ISR_FLOW[idx] = '2';
	idx++;
    /* Clear Timer Interrupt Aggregation Flag Register (TIAFR) */
    CSL_SYSCTRL_REGS->TIAFR = 0x04;
	for(i=0;i<100000;i++);
    //*ptr++='2';
	ISR_FLOW[idx] = '4';
	idx++;
}


/**
 *  \brief  Function to calculate the clock at which system is running
 *
 *  \param    none
 *
 *  \return   System clock value in KHz
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
	return(sysClk/1000);
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
	return(sysClk);
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
	pllVS = CSL_FEXT(CSL_SYSCTRL_REGS->CGICR, SYS_CGICR_ML);

	pllRD = CSL_FEXT(CSL_SYSCTRL_REGS->CGICR, SYS_CGICR_RDRATIO);
	pllVO = CSL_FEXT(CSL_SYSCTRL_REGS->CGOCR, SYS_CGOCR_ODRATIO);

	pllRDBypass = CSL_FEXT(CSL_SYSCTRL_REGS->CGICR, SYS_CGICR_RDBYPASS);
	pllOutDiv   = CSL_FEXT(CSL_SYSCTRL_REGS->CGOCR, SYS_CGOCR_OUTDIVEN);
	pllOutDiv2  = CSL_FEXT(CSL_SYSCTRL_REGS->CGOCR, SYS_CGOCR_OUTDIV2BYPASS);

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
	return(sysClk/1000);
}

#endif

