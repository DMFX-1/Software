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

/** @file gpio_Input_Pin_Example.c
 *
 *  @brief Test code to verify the input functionality of GPIO module
 *
 * \page    page3  GPIO EXAMPLE DOCUMENTATION
 *
 * \section GPIO2   GPIO EXAMPLE2 - INPUT PIN TEST
 *
 * \subsection GPIO2x    TEST DESCRIPTION:
 *		This example code tests the GPIO(General Purpose Input Output) pin
 * functionality as input pin.
 * C5515/C5517 DSPs have 32 GPIO pins which can be configured as input or
 * output. GPIO pin 11 is configured as input pin by this test code.
 * For verifying the input functionality, a source is required which sends data
 * to the GPIO pin configured as input pin. GPIO pin 4 is configured as output
 * pin and is used as data source for pin 11. These two pins should be connected
 * with a jumper on the EVM to facilitate the data loop back. Interrupts are
 * enabled for the GPIO input pin to indicate the high signal on the input pin.
 * A data value 1 is written to the GPIO pin4 which is loop backed to the pin11
 * and is read in the ISR. Test will be successful when the GPIO interrupt for
 * pin11 is generated. Value read from the pin11 is also compared with the value
 * written to the pin4 to announce the success of the test.
 *
 * NOTE: PIN 1(GPIO_4) AND 2(GPIO_11) OF J14 ON C5517 EVM SHOULD BE CONNECTED
 * WITH JUMPER BEFORE RUNNING THIS TEST CODE.
 *
 * NOTE: PIN 1(GPIO_4) AND 2(GPIO_11) OF J13 ON C5515 EVM SHOULD BE CONNECTED
 * WITH JUMPER BEFORE RUNNING THIS TEST CODE.

 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5515 AND
 * C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 *
 * \subsection GPIO2y    TEST PROCEDURE:
 *  @li Open the CCS and connect the target
 *  @li Open the project "CSL_GPIO_InputPinExample.pjt" and build it
 *  @li Load the program on to the target
 *  @li Set the PLL frequency to 12.288MHz
 *  @li Run the program and observe the test result
 *  @li Repeat the test at the following PLL frequencies
 *      C5515: 60MHz and 100MHz
 *      C5517: 60MHz, 100MHz, 150MHz and 200MHz
 *  @li Repeat the test in Release mode
 *
 * \subsection GPIO2z    TEST RESULT:
 *  @li All the CSL APIs should return success
 *  @li An Interrupt should be generated for GPIO Pin11
 *  @li Value read from the GPIO pin 11 should match with the value written to
 *      GPIO pin 4.
 *
 */

/* ============================================================================
 * Revision History
 * ================
 * 26-Sep-2008 - Created
 * 28-Jun-2009 - Added Documentation
 * 10-Jul-2012 - Added C5517 Compatibility
 * ============================================================================
 */

#include "csl_gpio.h"
#include "csl_intc.h"
#include <stdio.h>
#include <csl_general.h>
#include "csl_sysctrl.h"

#define CSL_TEST_FAILED    (-1)
#define CSL_TEST_PASSED    (0)


extern CSL_Status ioExpander_Setup(void);
extern CSL_Status ioExpander_Read(Uint16 port, Uint16 pin, Uint16 *rValue);
extern CSL_Status ioExpander_Write(Uint16 port, Uint16 pin, Uint16 value);

/* Global Structure Declaration */
CSL_GpioObj     GpioObj;
CSL_GpioObj     *hGpio;
volatile int    i = 0;
Uint16          readVal = 0;
Uint16			random = 0;
int				a=0;
Uint16          GPIO_Test = 0;
CSL_Status 		status;

/* Reference the start of the interrupt vector table */
/* This symbol is defined in file vectors.asm       */
extern void VECSTART(void);

/*---------Function prototypes---------*/
/**
 *  \brief  GPIO Interrupt Service Routine
 *
 *  \param  none
 *
 *  \return none
 */
interrupt void gpioISR(void);

/**
 *  \brief  Function to test the functionality of GPIO as input pin
 *
 *  This function configures GPIO pin 11 as input pin and pin 4 as output pin
 *  A data value is written to pin4 and is read from pin11. Both the values are
 *  compared and the result is returned to the main function.
 *
 * Connect GPIO 4 and GPIO11 on the C5515 EVM with a jumper,
 * Pin 1 of J22 is GPIO 4 and Pin 2 of J22 is GPIO11
 *
 *  \param  none
 *
 *  \return Test result
 */
int gpio_input_int_test(void);


/**
 *  \brief  main function
 *
 *   This function calls GPIO test functions and displays the test
 *   result.
 *
 *  \param  none
 *
 *  \return none
 */
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 1 --
   /////  Define PaSs_StAtE variable for catching errors as program executes.
   /////  Define PaSs flag for holding final pass/fail result at program completion.
        volatile Int16 PaSs_StAtE = 0x0001; // Init to 1. Reset to 0 at any monitored execution error.
        volatile Int16 PaSs = 0x0000; // Init to 0.  Updated later with PaSs_StAtE when and if
   /////                                  program flow reaches expected exit point(s).
   /////
void main(void)
{
    int result;

    printf("CSL GPIO Input Pin Test!\n\n");
#ifdef CHIP_C5517
	status = ioExpander_Setup();
	status = ioExpander_Write(1, 0, 1);
	status = ioExpander_Write(1, 4, 0);
	status = ioExpander_Write(1, 5, 0);
	//status = ioExpander_Write(0, 1, 1);
	//if(CSL_SOK != status)
		//printf("Select AIC_McBSP_MODE to 1 Failed \n");
#endif
    /* To test GPIO channel (pin) as an input channel in interrupt mode	*/
    result = gpio_input_int_test();
    if(CSL_TEST_PASSED == result)
    {
        printf("\nCSL GPIO Input Pin Test Passed!!\n");
    }
	else
	{
		printf("\nCSL GPIO Input Pin Test Failed!!\n");
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 2 --
   /////  Reseting PaSs_StAtE to 0 if error detected here.
        PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
   /////
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
 *  \brief  Function to test the functionality of GPIO as input pin
 *
 *  This function configures GPIO pin 11 as input pin and pin 4 as output pin
 *  A data value is written to pin4 and is read from pin11. Both the values are
 *  compared and the result is returned to the main function.
 *
 * Connect GPIO 4 and GPIO11 on the C5515 EVM with a jumper,
 * Pin 1 of J22 is GPIO 4 and Pin 2 of J22 is GPIO11 - C5515 EVM
 * Pin 1 of J14 is GPIO 4 and Pin 2 of J14 is GPIO11 - C5517 EVM
 *
 *  \param  none
 *
 *  \return Test result
 */
int gpio_input_int_test(void)
{
    CSL_Status           status;
    CSL_GpioPinConfig    config;
	volatile Uint32 	 loop;

	printf("Make Sure that Pin 1 and 2 of J13 on C5515 EVM or J14 on C5517 EVM are connected with Jumper\n\n");

    /* Pin muxing for GPIO 4 and 11 Pin */
//    CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_SP0MODE, MODE2);
//    CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_SP1MODE, MODE2);

    status =  SYS_setEBSR(CSL_EBSR_FIELD_SP0MODE,
                          CSL_EBSR_SP0MODE_2);
    status |= SYS_setEBSR(CSL_EBSR_FIELD_SP1MODE,
                          CSL_EBSR_SP1MODE_2);
    if(CSL_SOK != status)
    {
        printf("SYS_setEBSR failed\n");
        return(CSL_TEST_FAILED);
    }

    /* Disable CPU interrupt */
    IRQ_globalDisable();

	/* Clear any pending interrupts */
	IRQ_clearAll();

	/* Disable all the interrupts */
	IRQ_disableAll();

    /* Initialize Interrupt Vector table */
    IRQ_setVecs((Uint32)(&VECSTART));

	/* Open GPIO module */
    hGpio = GPIO_open(&GpioObj,&status);
    if((NULL == hGpio) || (CSL_SOK != status))
    {
        printf("GPIO_open failed\n");
        return(CSL_TEST_FAILED);
    }
    else
    {
		printf("GPIO_open Successful\n");
	}

	/* Reset the GPIO module */
    GPIO_reset(hGpio);
	GPIO_Test = 0;
	i = 0;
	a = 0;
	random = 1;
	printf("Waiting...\n");
	for (loop=0; loop<0x5F5E10; loop++){}
	/* GPIO_config API to make PIN4 as output pin */
	config.pinNum    = CSL_GPIO_PIN4;
    config.direction = CSL_GPIO_DIR_OUTPUT;
    config.trigger   = CSL_GPIO_TRIG_CLEAR_EDGE;
    status = GPIO_configBit(hGpio,&config);
	if(CSL_SOK != status)
	{
		printf("test failed - GPIO_configBit\n");
		return(CSL_TEST_FAILED);
	}
	else
	{
		printf("GPIO PIN4 is configured as Output Pin\n");
	}

    /** test GPIO_config API to make PIN11 as i/p */
	config.pinNum    = CSL_GPIO_PIN11;
    config.direction = CSL_GPIO_DIR_INPUT;
    config.trigger   = CSL_GPIO_TRIG_RISING_EDGE;
    status = GPIO_configBit(hGpio,&config);
	if(CSL_SOK != status)
	{
		printf("test failed - GPIO_configBit\n");
		return(CSL_TEST_FAILED);
	}
	else
	{
		printf("GPIO PIN11 is configured as Input Pin\n");
	}

	/* Enable GPIO interrupts */
    status = GPIO_enableInt(hGpio,CSL_GPIO_PIN11);
	if(CSL_SOK != status)
	{
		printf("test failed- GPIO_enableInt\n");
		return(CSL_TEST_FAILED);
	}

	/* Clear any pending Interrupt */
    IRQ_clear(GPIO_EVENT);

    IRQ_plug(GPIO_EVENT,&gpioISR);

     /* Enabling Interrupt */
    IRQ_enable(GPIO_EVENT);
    IRQ_globalEnable();

    /* drive GPIO4 to 1 */
	status = GPIO_write(hGpio,CSL_GPIO_PIN4,1);
	if(CSL_SOK != status)
	{
		printf("GPIO_write failed\n");
		return CSL_TEST_FAILED;
	}

    /* wait for Interrupt */
    while(i!=1);

	IRQ_globalDisable();

	/* Disable all the interrupts */
	IRQ_disableAll();

    printf("GPIO ISR is called\n");

    if(readVal == 1)
    {
		printf("Reading Value from GPIO Pin11 is successful\n");
	}
	else
	{
		printf("Reading Value from GPIO Pin11 is Failed\n");
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 2 --
   /////  Reseting PaSs_StAtE to 0 if error detected here.
        PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
   /////
	}

	return(CSL_TEST_PASSED);
}

/**
 *  \brief  GPIO Interrupt Service Routine
 *
 *  \param  none
 *
 *  \return none
 */
interrupt void gpioISR(void)
{

   	CSL_Status status;
	i = 0;

    /* Check for GPIO Interrupt Flag Register */
	if((1 == GPIO_statusBit(hGpio,CSL_GPIO_PIN11,&status)))
    {
        /* Clear GPIO Interrupt Flag Register */
        GPIO_clearInt(hGpio,CSL_GPIO_PIN11);
    }

    IRQ_disable(GPIO_EVENT);

    /* Read data on pin 11 */
    status = GPIO_read(hGpio, CSL_GPIO_PIN11, &readVal);

    i++;
}
