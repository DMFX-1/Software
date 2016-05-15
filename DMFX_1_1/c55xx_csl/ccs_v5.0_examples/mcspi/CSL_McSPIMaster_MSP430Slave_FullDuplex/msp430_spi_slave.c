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

//******************************************************************************
//   MSP430F552x Demo - USCI_A0, SPI 3-Wire Master Incremented Data
//
//   Description: SPI master talks to SPI slave using 3-wire mode. Incrementing
//   data is sent by the master starting at 0x01. Received data is expected to
//   be same as the previous transmission.  USCI RX ISR is used to handle
//   communication with the CPU, normally in LPM0. If high, P1.0 indicates
//   valid data reception.  Because all execution after LPM0 is in ISRs,
//   initialization waits for DCO to stabilize against ACLK.
//   ACLK = ~32.768kHz, MCLK = SMCLK = DCO ~ 1048kHz.  BRCLK = SMCLK/2
//
//   Use with SPI Slave Data Echo code example.  If slave is in debug mode, P1.1
//   slave reset signal conflicts with slave's JTAG; to work around, use IAR's
//   "Release JTAG on Go" on slave device.  If breakpoints are set in
//   slave RX ISR, master must stopped also to avoid overrunning slave
//   RXBUF.
//
//                   MSP430F552x
//                 -----------------
//             /|\|                 |
//              | |                 |
//              --|RST          P1.0|-> LED
//                |                 |
//                |             P3.4|-> Data Out (UCA0SIMO)
//                |                 |
//                |             P3.5|<- Data In (UCA0SOMI)
//                |                 |
//  Slave reset <-|P1.1         P3.0|-> Serial Clock Out (UCA0CLK)
//
//
//   Bhargavi Nisarga
//   Texas Instruments Inc.
//   April 2009
//   Built with CCSv4 and IAR Embedded Workbench Version: 4.21

//Code modified for USCB0 module
//p3.0
//p3.1
//p3.2
//p3.3
//******************************************************************************

#include <msp430f5529.h>
#include <stdio.h>

// not defined in header SFR_8BIT(UCB0MCTL);                           /* USCI B0 Modulation Control */



unsigned char MST_Data,SLV_Data;
unsigned char temp;
unsigned int delay;

unsigned int Rx_Data[50],index;

void main(void)
{
  volatile unsigned int i;

  index = 0;
  WDTCTL = WDTPW+WDTHOLD;                   // Stop watchdog timer



    //Clock configurations


 P7DIR |= BIT7;                            // MCLK set out to pins
 P7SEL |= BIT7;


  UCSCTL3 |= SELREF_2;                      // Set DCO FLL reference = REFO
  UCSCTL4 |= SELA_2;                        // Set ACLK = REFO

  __bis_SR_register(SCG0);                  // Disable the FLL control loop
  UCSCTL0 = 0x0000;                         // Set lowest possible DCOx, MODx
  UCSCTL1 = DCORSEL_5;                      // Select DCO range 24MHz operation
  UCSCTL2 = FLLD_1 + 374;                   // Set DCO Multiplier for 12MHz
                                            // (N + 1) * FLLRef = Fdco
                                            // (374 + 1) * 32768 = 12MHz
                                            // Set FLL Div = fDCOCLK/2
  __bic_SR_register(SCG0);                  // Enable the FLL control loop

  // Worst-case settling time for the DCO when the DCO range bits have been
  // changed is n x 32 x 32 x f_MCLK / f_FLL_reference. See UCS chapter in 5xx
  // UG for optimization.
  // 32 x 32 x 12 MHz / 32,768 Hz = 375000 = MCLK cycles for DCO to settle
  __delay_cycles(375000);


  // Loop until XT1,XT2 & DCO fault flag is cleared
  do
  {
    UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
                                            // Clear XT2,XT1,DCO fault flags
    SFRIFG1 &= ~OFIFG;                      // Clear fault flags
  }while (SFRIFG1&OFIFG);                   // Test oscillator fault flag



   printf("\r\nTest start\r\n");

   printf("\r\n Clock configured to 12 Mhz \r\n");



  P6DIR |= 0x01;                            // Set P1.0-2 to output direction
  P6OUT |= 0x01;                            // Set P6.0 for LED

  P6OUT |= 0x01;                            // high B2 port active

  P6OUT &= (~0x01);                            // low B1 port active

  P6OUT |= 0x01;                            // high B2 port active



  P3SEL |= BIT0+BIT1+BIT2;             // P3.0,1,2, option select
  P3SEL &= (~BIT3);    //cs

  P3DIR |= 0x08;
//  P3OUT |= 0x08;    //make cs high

//  P3OUT &= (~0x08);    //make cs high



  UCB0CTL1 |= UCSWRST;                      // **Put state machine in reset**
  UCB0CTL0 |= UCSYNC+UCCKPL+UCMSB ;    // 3-pin, 8-bit SPI master  UCMST+
                                            // Clock polarity high, MSB
  UCB0CTL1 |= UCSSEL_2;                     // SMCLK
  UCB0BR0 = 0x9;                           // /2  //9 was fine for rx
  UCB0BR1 = 0x0;                              //
//  UCB0MCTL = 0;                            // No modulation   //  **new syntex
  UCB0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  UCB0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
  UCB0IE |= UCTXIE;                         // Enable USCI_A0 RX interrupt

  for(i=500;i>0;i--);                        // Wait for slave to initialize

  MST_Data = 0x01;                          // Initialize data values
  SLV_Data = 0x00;                          //

  while (!(UCB0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
  UCB0TXBUF = MST_Data;                     // Transmit first character

/*
while(1){
	  if(UCB0IFG&UCRXIFG){
	     Rx_Data[index]= UCB0RXBUF;
	     index++;
	     if(index>49)
	     index = 0 ;
	  }
}
*/


 /*
while(1){

	if(UCB0IFG&UCTXIFG){
		 UCB0TXBUF = MST_Data;
		 MST_Data++;
		 for(delay = 20; delay>0; delay--);
	}
}
*/
 __bis_SR_register(LPM0_bits + GIE);       // CPU off, enable interrupts


}

#pragma vector=USCI_B0_VECTOR

__interrupt void USCI_B0_ISR(void)
{
  volatile unsigned int i;

  switch(__even_in_range(UCB0IV,4))
  {
    case 0: break;                          // Vector 0 - no interrupt
    case 2:                                 // Vector 2 - RXIFG
      while (!(UCB0IFG&UCTXIFG));           // USCI_A0 TX buffer ready?
//	  while (!(UCB0IFG&UCRXIFG));           // USCI_A0 TX buffer ready?
	MST_Data = UCB0RXBUF;
    UCB0TXBUF = MST_Data;                 // Send next value

  //    UCB0TXBUF = 0x88;                 // Send next value

      for(i = 64000; i>0; i--);                // Add time between transmissions to
                                            // make sure slave can process information
      break;
    case 4: break;                          // Vector 4 - TXIFG
    default: break;
  }

}
