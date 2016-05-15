   MSP430F552x Demo - USCI_A0, SPI 3-Wire Master Incremented Data

   Description: SPI master talks to SPI slave using 3-wire mode. Incrementing
   data is sent by the master starting at 0x01. Received data is expected to
   be same as the previous transmission.  USCI RX ISR is used to handle
   communication with the CPU, normally in LPM0. If high, P1.0 indicates
   valid data reception.  Because all execution after LPM0 is in ISRs,
   initialization waits for DCO to stabilize against ACLK.
   ACLK = ~32.768kHz, MCLK = SMCLK = DCO ~ 1048kHz.  BRCLK = SMCLK/2

   Use with SPI Slave Data Echo code example.  If slave is in debug mode, P1.1
   slave reset signal conflicts with slave's JTAG; to work around, use IAR's
   "Release JTAG on Go" on slave device.  If breakpoints are set in
   slave RX ISR, master must stopped also to avoid overrunning slave
   RXBUF.

                   MSP430F552x
                 -----------------
             /|\|                 |
              | |                 |
              --|RST          P1.0|-> LED
                |                 |
                |             P3.4|-> Data Out (UCA0SIMO)
                |                 |
                |             P3.5|<- Data In (UCA0SOMI)
                |                 |
  Slave reset <-|P1.1         P3.0|-> Serial Clock Out (UCA0CLK)