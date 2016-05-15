 Example test code to verify the Multiple Instance of CSL GPT module
 functionality

 GPT MULTIPLE INSTANCE TEST

 TEST DESCRIPTION:
 		This test code verifies the functionality of the multiple instances of
  CSL GPT (General Purpose Timer) module. GPT module of C5535/C5515/C5517 DSP
  is used to provide interrupts to the CPU to schedule periodic tasks or a
  delayed task. GPT can be configured with a counter and pre-scaler divider
  value. When the GPT is started counter will be decremented to 0. The rate at
  which this count happens can be controlled by the prescaler divider value.
  GPT clock is derived by dividing the CPU clock with the pre-scaler divider
  value.
 
  This test verifies the multiple instances of the GPT module by checking
  whether the GTP is decrementing the counter or not. Each instance of the GPT
  is configured with a count value 0xFFFF and pre-scaler divider value 256.
  GPT is started and counter value is read. After few cycles of delay GPT
  counter value is read again. Both the counter values are compared to verify
  whether the second count value is less than the first counter value or not.
 
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5535, C5515
  AND C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
 
  TEST PROCEDURE:
   1. Open the CCS and connect the target (C5535/C5515 EVM or C5517 EVM))
   2. Open the project "CSL_GPT_MultiInstances_Example_Out.pjt" and build it
   3. Load the program on to the target
   4. Set the PLL frequency to 12.288MHz
   5. Run the program and observe the test result
 
  TEST RESULT:
   All the CSL APIs should return success
   Timer count value comparison should be successful