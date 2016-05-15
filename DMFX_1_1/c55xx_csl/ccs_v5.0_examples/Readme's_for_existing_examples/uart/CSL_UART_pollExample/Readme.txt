  UART example code to test the Polled mode functionality of CSL UART
  
  UART POLL MODE TEST
 
  TEST DESCRIPTION:
 		This test is to verify the CSL UART module operation in polled mode.
  This test code communicates with the HyperTerminal on the host PC through
  UART module on C5535/C5515 DSP operating in polled mode. UART peripheral is
  configured by the test code to the following values
  Baud Rate - 2400.
  Data bits - 8.
  Parity - None.
  Stop bits - 1.
  Flow control - None.
  HyperTerminal on the host PC should be configured with the same values to
  to proper communication with the C5535/C5515/C5517 EVM. C5535/C5515/C5517 EVM should be
  connected to the host PC using an RS232 cable and HyperTerminal on the host
  PC should be opened and connected.This test works at all the PLL frequencies.
 
  Upon running the test, a message prompting to enter the string size will be
  displayed on HyperTerminal.Enter the string size which is greater than 1 and
  less than 99. String length should be two characters and the numbers below
  10 should be preceded by 0(01,02,03...). After reading the string size
  a message prompting to enter the string will be displayed on HyperTerminal.
  Enter the string which is exactly of the length specified in the previous
  query. Once the string of the specified size is entered, test code will
  stop reading from the HyperTerminal and send message to inform that the
  reading from HyperTerminal has been stopped. After reading the string
  test code will send the same string back to the HyperTerminal.
 
  
   Messages displayed on the HyperTerminal when entered a string
   "CSL UART TESTING" will be as shown below.

     ====================================================================
     ==                                                                ==
     ==  Enter the size of the string(min 01 to max 99)                ==
     ==  >> 16                                                         ==
     ==                                                                ==
     ==  Enter the string:                                             ==
     ==  >> CSL UART TESTING                                           ==
     ==                                                                ==
     ==  Please stop typing, reading already stopped...!!!             ==
     ==                                                                ==
     ==  YOU HAVE ENTERED: CSL UART TESTING                            ==
     ==                                                                ==
     ====================================================================

  
  Manual inspection is required to verify the success of each step.
  All the communication between UART and HyperTerminal will be displayed
  on the CCS "stdout" window.
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5535. C5515
  AND C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
 
  TEST PROCEDURE:
   1. Connect the RS232 cable; One end to the UART port on the C5535/C5515/C5517 EVM(J13/J1/J2)
      and other  to the COM port of the Windows Xp PC.
   2. Open the HyperTerminal on the Host PC. To open the HyperTerminal click
      on 'Start->Programs->Accessories->Communications->HyperTerminal’
   3. Disconnect the HyperTerminal by selecting menu Call->Disconnect
   4. Select Menu 'File->Properties' and click on the Button 'Configure'
       \n  Set 'Bits Per Second(Baud Rate)' to 2400.
       \n  Set 'Data bits' to 8.
       \n  Set 'Parity' to None.
       \n  Set 'Stop bits' to 1.
       \n  Set 'Flow control' to None.
       \n  Click on 'OK' button.
       \n  HyperTerminal is configured and ready for communication
   5. Connect the HyperTerminal by selecting menu Call->Call.
   6. Open the CCS and connect the target (C5535/C5515/C5517 EVM)
   7. Open the project "CSL_UART_pollExample.pjt" and build it
   8. Load the program on to the target
   9. Set the PLL frequency to 12.288MHz
  10. Run the program and observe the test result
 
  TEST RESULT:
   All the CSL APIs should return success
   Data sent from the HyperTerminal should be read by the UART properly
   Data sent by the UART should be displayed on the HyperTerminal properly
 