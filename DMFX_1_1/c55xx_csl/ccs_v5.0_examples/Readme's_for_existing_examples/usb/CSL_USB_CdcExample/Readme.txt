  USB CDC example source file
 
  USB CDC TEST
 
  TEST DESCRIPTION:
  This test is to verify the operation of the CSL USB and CDC ACM module. This test runs
  in interrupt mode. USB interrupts are configured and ISR is registered using
  CSL INTC module. After initializing and configuring the USB module, the test program waits
  on a while loop. When there is any input from the USB host application
  USB ISR is triggered and the requested operation is performed inside the ISR.
 
  This USB CDC test can be verified by any terminal program (such as HyperTerminal) which can
  send or receive data via the virtual COM port for the USB CDC. The attached INF
  file (C5515_CDC_ACM.inf) is required. The INF file only be installed for
  the C5515 USB CDC device for the first time, when the using driver wizard of the windriver.
  This installation is required only when running this example for the first
  time.
 
  NOTE: IN THE CURRENT CODE EP1 IN IS CONFIGURED FOR COMMUNICATION CLASS INTERFACE
  EP2 OUT (BULK OUT) AND EP3 (BULK IN) IN ARE CONFIGURED FOR DATA CLASS INTERFACE
  THOSE CAN BE CHANGED IN CSL_CDC.H
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5535, C5515,
  AND C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
 
  TEST PROCEDURE:
  1. Connect the C5535/15/17 EVM with a PC via USB cable
  2. Open the CCS and connect the target (C5535/C5515/C5517 EVM)
  3. Open, build and load the USB program to the target
  4. Set the PLL frequency to 100MHz
  5. Run the program loaded on to the target
  6. Install the inf file for the USB device (Only when running for first time)
  7. After the proper USB enumeration, check the Control Panel-->System-->Hardware-->
     Device Manager-->Ports (COM & LPT) looking for C5515_CDC_ACM_device. A COM port will
     be assigned to the CDC ACM device, says COM16
  8. Run the HyperTerminal. Create a new connection to COM16
  9. Configure the new connection to the desired baud rate, parity, number of bits etc.
 10. The test program is acting as an echo device, whatever you input in the HyperTerminal,
     it will be echo them back. The user input to the HyperTerminal will be sent to the
     C5515 EVM via CSL_CDC_BULK_OUT_EP (EP2 OUT) and the data output from C5515 EVM to the
     will be sent to HyperTerminal via USB CSL_CDC_BULK_IN_EP (EP3 IN). You can change the
     actual port number in csl_cdc.h
 
  TEST RESULT:
   USB CDC ACM device should be detected by the host PC and should be accessible
   through a terminal application (HyperTerminal)
   Data typed in using keyborad and data displayed on the terminal window should match