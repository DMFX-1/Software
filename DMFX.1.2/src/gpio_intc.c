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
#include <pal_osWait.h>
#include "csl_gpio_intc.h"
#include "DMFX2_CSL_BIOS_cfg.h"


/* Global Structure Declaration */
CSL_GpioObj     		GpioObj;
extern CSL_GpioObj     *hGpio;   // defined on gpio_control.c
Uint16 					NavigatorButton = 0;

//GPIO interrupt initialization not used
void gpio_int_init(void)
{
	/* Clear any pending Interrupt */
    IRQ_clear(GPIO_EVENT);
     /* Enabling Interrupt */
    IRQ_enable(GPIO_EVENT);
    IRQ_globalEnable();
}

void gpio_int_test(void) {
	Uint16 i = 0;

	for(i = 0; i < 64; i++) {
		switch(NavigatorButton)
		{
		case(DOWN_BUTTON):
			LOG_printf(&trace, "DOWN button pressed");		// DOWN Button action comes here
			NavigatorButton = 0;
			break;
		case(LEFT_BUTTON):
			LOG_printf(&trace, "LEFT button pressed");		// LEFT Button action comes here
			NavigatorButton = 0;
			break;
		case(CENTER_BUTTON):
			LOG_printf(&trace, "CENTER button pressed");	// Center Button action comes here
			NavigatorButton = 0;
			break;
		case(RIGHT_BUTTON):
			LOG_printf(&trace, "RIGHT button pressed");		// RIGHT Button action comes here
			NavigatorButton = 0;
			break;
		case(UP_BUTTON):
			LOG_printf(&trace, "UP button pressed");		// UP Button action comes here
			NavigatorButton = 0;
			break;
		}
	    PAL_osWaitMsecs(100);
	}
}
void gpio_int_deinit(void)
{
	IRQ_globalDisable();
	/* Disable all the interrupts */
	IRQ_disableAll();

}

/**
 *  \brief  GPIO Interrupt Service Routine
 *
 *  \param  none
 *
 *  \return none
 */
void gpioISR(void)
{

   	CSL_Status status;

    /* Check for GPIO Interrupt Flag Register */
	//C2-DOWN
	if((1 == GPIO_statusBit(hGpio,CSL_GPIO_PIN13,&status)))
    {
        /* Clear GPIO Interrupt Flag Register */
        GPIO_clearInt(hGpio,CSL_GPIO_PIN13);
        NavigatorButton = DOWN_BUTTON;
    }
	//C2-LEFT
	else if((1 == GPIO_statusBit(hGpio,CSL_GPIO_PIN14,&status)))
    {
        /* Clear GPIO Interrupt Flag Register */
        GPIO_clearInt(hGpio,CSL_GPIO_PIN14);
        NavigatorButton = LEFT_BUTTON;
    }
	//C2-CENTER
	else if((1 == GPIO_statusBit(hGpio,CSL_GPIO_PIN15,&status)))
    {
        /* Clear GPIO Interrupt Flag Register */
        GPIO_clearInt(hGpio,CSL_GPIO_PIN15);
        NavigatorButton = CENTER_BUTTON;
    }
	//C2-RIGHT
	else if((1 == GPIO_statusBit(hGpio,CSL_GPIO_PIN16,&status)))
    {
        /* Clear GPIO Interrupt Flag Register */
        GPIO_clearInt(hGpio,CSL_GPIO_PIN16);
        NavigatorButton = RIGHT_BUTTON;
    }
	//C2-UP
	else if((1 == GPIO_statusBit(hGpio,CSL_GPIO_PIN17,&status)))
    {
        /* Clear GPIO Interrupt Flag Register */
        GPIO_clearInt(hGpio,CSL_GPIO_PIN17);
        NavigatorButton = UP_BUTTON;
    }
}
