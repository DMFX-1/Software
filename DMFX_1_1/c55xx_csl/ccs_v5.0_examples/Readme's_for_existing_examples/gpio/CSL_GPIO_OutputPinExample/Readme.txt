  Test code to verify the output functionality of GPIO module
 
  GPIO OUTPUT PIN TEST

  TEST DESCRIPTION:
 		This example code tests the GPIO(General Purpose Input Output) pin
  functionality as output pin. This example also verifies configuring the
  CSL GPIO module.
  C5535/C5515/C5517 DSPs have 32 GPIO pins which can be configured as input
  or output. Output functionality of the GPIO pin 0 is verified by this test.
  During this test Pin0 is configured as output pin. A value is written to
  pin0 and verified by reading it back.
  Second part of this test code verifies configuring the CSL GPIO module.
  During this test GPIO module is configured using GPIO_config() CSL API.
  Configured values are verified by reading them using GPIO_getConfig() CSL API.
  Test results are displayed in the CCS "Stdout" window.
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5535, C5515
  AND C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
 
  TEST PROCEDURE:
   1. Open the CCS and connect the target (C5535/C5515 EVM or C5517 EVM)
   2. Open the project "CSL_GPIO_OutputPinExample.pjt" and build it
   3. Load the program on to the target
   4. Set the PLL frequency to 12.288MHz
   5. Run the program and observe the test result
 
  TEST RESULT:
    All the CSL APIs should return success
    Value written to the GPIO pin configured as output pin(Pin0) should be
    read successfully
    CSL GPIO module should be configured successfully. Configuration values
    read using GPIO_getConfig API should match with the actual configured
    values