  Test code to verify the input functionality of GPIO module
 
  GPIO INPUT PIN TEST
 
  TEST DESCRIPTION:
 		This example code tests the GPIO(General Purpose Input Output) pin
  functionality as input pin.
  C5515/C5517 DSPs have 32 GPIO pins which can be configured as input or
  output. GPIO pin 11 is configured as input pin by this test code.
  For verifying the input functionality, a source is required which sends data
  to the GPIO pin configured as input pin. GPIO pin 4 is configured as output
  pin and is used as data source for pin 11. These two pins should be connected
  with a jumper on the EVM to facilitate the data loop back. Interrupts are
  enabled for the GPIO input pin to indicate the high signal on the input pin.
  A data value 1 is written to the GPIO pin4 which is loop backed to the pin11
  and is read in the ISR. Test will be successful when the GPIO interrupt for
  pin11 is generated. Value read from the pin11 is also compared with the value
  written to the pin4 to announce the success of the test.
 
  NOTE: PIN 1(GPIO_4) AND 2(GPIO_11) OF J14 ON C5517 EVM SHOULD BE CONNECTED
  WITH JUMPER BEFORE RUNNING THIS TEST CODE.
 
  NOTE: PIN 1(GPIO_4) AND 2(GPIO_11) OF J13 ON C5515 EVM SHOULD BE CONNECTED
  WITH JUMPER BEFORE RUNNING THIS TEST CODE.

  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5515 AND
  C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
 
  TEST PROCEDURE:
   1. Open the CCS and connect the target
   2. Open the project "CSL_GPIO_InputPinExample.pjt" and build it
   3. Load the program on to the target
   4. Set the PLL frequency to 12.288MHz
   5. Run the program and observe the test result
 
  TEST RESULT:
    All the CSL APIs should return success
    An Interrupt should be generated for GPIO Pin11
    Value read from the GPIO pin 11 should match with the value written to
    GPIO pin 4.