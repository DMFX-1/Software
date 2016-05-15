  UART example to test the Interrupt mode functionality of CSL UART
 
  UART INTERRUPT MODE TEST
 
  TEST DESCRIPTION:
 		This test is to verify the CSL UART module operation in interrupt mode.
  This test code communicates with the HyperTerminal on the host PC through
  UART module on C5535 DSP operating in interrupt mode. UART peripheral is
  configured by the test code to the following values.
  Baud Rate - 2400.
  Data bits - 8.
  Parity - None.
  Stop bits - 1.
  Flow control - None.
  HyperTerminal on the host PC should be configured with the same values to
  to proper communication with the C5535/C5515/C5517 EVM. C5535/C5515/C5517 EVM should be
  connected to the host PC using an RS232 cable and HyperTerminal on the host
  PC should be opened and connected.This test works at all the PLL frequencies.
 
  This test code reads characters entered by the user on the HypeTerminal
  continuously one character at a time. For each character entered on the
  HyperTerminal, URAT generates a receive interrupt and the character is
  read in the Rx ISR. Symbol '$' needs to be entered on the HyperTerminal to
  terminate the test.
  Upon running the test, details of the test will be displayed on the
  HyperTerminal. A message will be displayed prompting to enter a character.
  Character entered on the HyperTerminal will be read by the test code and
  and echoed back to the HyperTerminal until the symbol '$' is entered on the
  HyperTerminal. Messages displayed to direct the user will be transmitted to
  the HyperTerminal using Polled mode.
 
  
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

  
 
  Manual inspection is required to verify the success of each step.
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5535, C5515
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
   7. Open the project "CSL_UART_IntExample.pjt" and build it
   8. Load the program on to the target
   9. Set the PLL frequency to 12.288MHz
  10.  Run the program and observe the test result
 
  TEST RESULT:
    All the CSL APIs should return success
    Characters entered on the HyperTerminal should be read and echoed back
    to the HyperTerminal properly.
    Data sent by the UART should be displayed on the HyperTerminal properly