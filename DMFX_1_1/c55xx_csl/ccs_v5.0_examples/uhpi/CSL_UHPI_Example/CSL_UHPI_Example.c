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

/** @file csl_uhpi_example.c
 *
 *  @brief UHPI functional layer sample source file
 *
 *
 * \page    page19   UHPI EXAMPLE DOCUMENTATION
 *
 * \section UHPI   UHPI EXAMPLE - MEMORY ACCESS
 *
 * \subsection UHPIx    TEST DESCRIPTION:
 *		This test code verifies the functionality of CSL UHPI module. The UHPI module
 * on the C5517 DSP is used to communicate with MSP430, where the MSP430 is the master.
 * This test works together with the CSL_UHPI_MSP430_Example.out
 *
 * THe C5517 DSP is set up as the slave device in the UHPI transfer. It then waits for the
 * data transfer.
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSION C5517.
 * MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 * # define CHIP_C5517
 *
 * \subsection UHPIy    TEST PROCEDURE:
 *  @li Open the CCS and connect the target (C5517 EVM-Master board)
 *  @li Open the project "CSL_UHPI_Example.pjt" and build it
 *  @li Load the program on to the target
 *  @li Once this test is running, run CSL_UHPI_MSP430_Example.out on the MSP430
 * through a different Code Composer window.
 *  @li Once the MSP430 part of the test is finished running, halt this program.
 *  @li Verify the data on memory/DATA @ 0x8000. It should be:
 *  @verbatim
 	0x00008000	_stack, __edata, edata, __data, .data
 	0x00008000	0x0000	0xFFFF	0x0001
 	0x00008003	0xFFFE	0x0002	0xFFFD
 	0x00008006	0x0003	0xFFFC	0x0004
 	0x00008009	0xFFFB	0x0005	0xFFFA
 	0x0000800C	0x0006	0xFFF9	0x0007
 	0x0000800F	0xFFF8	0x0008	0xFFF7
 	0x00008012	0x0009	0xFFF6	0x000A
 	0x00008015	0xFFF5	0x000B	0xFFF4
 	0x00008018	0x000C	0xFFF3	0x000D
 	0x0000801B	0xFFF2	0x000E	0xFFF1
 	0x0000801E	0x000F	0xFFF0	0x0010
 	0x00008021	0xFFEF	0x0011	0xFFEE
 	...etc.
 *  @endverbatim
 *  @li Repeat the test at the following PLL frequencies
 *      C5517: 60MHz, 100MHz, 150MHz and 200MHz
 *  @li Repeat the test in Release mode
 *
 * \subsection UHPIz    TEST RESULT:
 *  @li The data memory address data should match the values above.
 *
 *  If on board buffers latched reset the board & Run the program,Use the correct GEL file for CPU configuration
 */

/* ============================================================================
 * Revision History
 * ================
 * 21-Nov-2011 Created
 * 29-Jun-2012 Documentation and comments added
 * ============================================================================
 */

#include <stdio.h>
#include "tistdtypes.h"

#define ST1_55 *((volatile unsigned *)(0x0003)) //CPU register ST1_55
#define EBSR *((ioport volatile unsigned *)(0x1C00)) //EBSR register for pin mux
#define PCGCRU *((ioport volatile unsigned *)(0x1C03)) //clock gating control, bit0=HPI
#define PSRCR *((ioport volatile unsigned *)(0x1C04)) //software reset counter
#define PRCR *((ioport volatile unsigned *)(0x1C05)) //reset control register
#define CLKSTOPL *((ioport volatile unsigned *)(0x1C3A)) //clock stop control
#define HPICFG *((ioport volatile unsigned *)(0x1C4E)) //HPI configur register, new for 5517
#define PWREMU_MGMT *((ioport volatile unsigned *)(0x2E04)) //power manager and emulation
#define GPIO_ENL *((ioport volatile unsigned *)(0x2E0C)) //HPI configur register, new for 5517
#define HPICL *((ioport volatile unsigned *)(0x2E30)) //HPI configur register, new for 5517

#define FAST     100000 //flash at High frequency
#define SLOW     1000000 //flashing at Low frequency
#define BLINK    10      //number of times that the LED will blink
///#define RTCINTFL 0x1920 //RTC Interrupt Flag Register

///volatile Uint16 wakeup_counter = 0;

void main (void)
{

  volatile Int32 i = 0, j = 0, delay = 0, blink_time = 0;

  delay = SLOW;
//  delay = FAST;
  blink_time = BLINK;


  EBSR &= 0x8FFF; //set pin mux to select the HPI
  PCGCRU = 0x0000; //clock gating control
  for (i=0; i<20; i++) {__asm("	NOP");}

  CLKSTOPL &= 0xFBFF; //clear bit-10 to enable HPI
  PSRCR = 0x000A; //reset control counter
  PRCR = 0x0008; //reset HPI
  for (i=0; i<20; i++) {__asm("	NOP");}

  PWREMU_MGMT = 0x0002;
  HPICFG = 0x0000;   //16bit multiplex mode only
  for (i=0; i<20; i++) {__asm("	NOP");}

  GPIO_ENL = 0x0000; //enable HPI mode
  HPICL = 0x0004;


  while (1) {  //forever loop

    //Toggle the output periodically
    for (j = 0; j < blink_time; j++)
    {
      //set XF to HIGH
      ST1_55 |= 0x2000;
      for (i = 0; i < delay; i++);

      //set XF to LOW
      ST1_55 &= 0xDFFF;
      for (i = 0; i < delay; i++);
    }

  }//end of forever loop

} //end of main

