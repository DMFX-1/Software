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

/** @file C5000_HPI_main.c
 *
 *  @brief MSP430 slave program to test UHPI module on C517 EVM
 *
 *  Path: \(CSLPATH)\ src
 */

/* ============================================================================
 * Revision History
 * ================
 * 10-Aug-2011 Created
 * 12-Jul-2012 Added new license header
 * ============================================================================
 */
#include "msp430f5529.h"
#include "set_msp430_25mhz.h"
#include "csl_uhpiAccess.h"

#define HPIC_SETTING  0x0002
#define HPIA_SETTING  0x00024000
#define HPIA_BOOT_ADR 0x00033FFD //WordAddress = 0x27FFA/0x27FFB
#define HPIA_BOOT_SGN 0x00033FFE //WordAddress = 0x27FFC/0x27FFD
#define TRANS_LENGTH  64
#define ONE_32B       0xAABBCCDD
#define BOOT_SIGN     0x1234ABCD

void main(void)
{
  volatile int error_counter=0;
  long unsigned int j;
  long unsigned int HPID_data[64];
  long unsigned int HPID_readback[64];
  volatile unsigned int temp16b_m;
  volatile long unsigned int temp32b_m;
  volatile unsigned char temp8;

  WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer

  //set the MSP430 running at 25MHz
  SET_MSP430_25MHZ();

  #if 0
  //-------------For debug only-----------------
  P1DIR = 0xFF; // P1 = output port(D0-D7)
  P2DIR = 0xFF; // P2 = output port(D8-D15)
  P6DIR = 0xFF; //
  PJDIR = 0xC;
  P6OUT = 0x30; //P6.7(HBE0)=0; P6.6(HBE1)=0; P6.5(unused); P6.4(HDS1)=1; P6.3(HPI_HR_NW)=0; P6.2(HHWIL)=0; P6.1(unused); P6.0(MSP430_SPI_SEL)=0->SPI_flash selected

  while((PJIN&0x2)==0x2){};//continuous loop if HINT has not been received yet

  temp16b_m = HPIC_read();
  HPIC_write(0x0006); //function call
  temp16b_m = HPIC_read();
  while(1); //stop here for debug only
  //--------------------------------------------
  #endif

  //Initialization
  P3DIR = 0x10; //Set P3.4 to output, this is an unused pin, for debug only

  //Initialize HPI related port
  P6DIR = 0xFF; //Set P6 to output direction
  P6OUT = 0x30; //P6.7(HBE0)=0; P6.6(HBE1)=0; P6.5(unused); P6.4(HDS1)=1; P6.3(HPI_HR_NW)=0; P6.2(HHWIL)=0; P6.1(unused); P6.0(MSP430_SPI_SEL)=0->SPI_flash selected
  PJDIR = 0xC;  //[HCNTL0, HCNTL1, HPI_HINT, HPI_HDRY] = [OUT, OUT, IN, IN]

  //write the C5525 boot address to a predefine location
  HPIA_write(HPIA_BOOT_ADR); //function call
  temp32b_m = HPIA_read();
  if (temp32b_m != HPIA_BOOT_ADR){
    error_counter++;
  }

  HPID_write_one(ONE_32B);
  temp32b_m = HPID_read_one();
  if (temp32b_m != ONE_32B){
    error_counter++;
  }

  //write the 32-bit boot signature word
  HPIA_write(HPIA_BOOT_SGN); //function call
  temp32b_m = HPIA_read();
  if (temp32b_m != HPIA_BOOT_SGN){
    error_counter++;
  }

  HPID_write_one(BOOT_SIGN);
  temp32b_m = HPID_read_one();
  if (temp32b_m != BOOT_SIGN){
    error_counter++;
  }

  //----auto increment test----
  //initialize HPID data array with test patterns
  for (j = 0; j < TRANS_LENGTH; j++)
  {
    HPID_data[j] = j + ((~j)<<16);
  }

  //clean HPID data readback array with "0"
  for (j = 0; j < TRANS_LENGTH; j++)
  {
    HPID_readback[j] = 0x00000000;
  }

  //setup address for both auto write and read test
  HPIA_write(HPIA_SETTING); //function call
  temp32b_m = HPIA_read();
  if (temp32b_m != HPIA_SETTING){
    error_counter++;
  }

  HPID_write_auto(TRANS_LENGTH, HPID_data); //function call
  HPID_read_auto(TRANS_LENGTH, HPID_readback); //function call

  //comparing the read-back data with the sent data
  for (j = 0; j < TRANS_LENGTH; j++){
    if (HPID_readback[j] != HPID_data[j]){
      error_counter++;
    }
  }

  //test read/write of HPIC
  //-------------------------------------------
  //option for boot test: wait here if HINT has not been received yet,
  //if DSP is set as HPI boot mode, after reseting DSP, the HINT should be sent by DSP
  ///while((PJIN&0x2)==0x2){}; //check the HPI_HINT pin
  //-------------------------------------------

  temp16b_m = HPIC_read();
  temp16b_m = temp16b_m | 0x0006;
  HPIC_write(temp16b_m); //function call
  temp16b_m = HPIC_read();


  for (;;)
  {
    ///volatile unsigned int i;   // volatile to prevent optimization

    P3OUT ^= 0x10;               // Toggle P3.4 using exclusive-OR
    ///for (k = 0; k < 1000; k++);  //wli: software delay

    ///i = 100;                  // SW Delay
    ///do i--;
    ///while (i != 0);
  }
}
