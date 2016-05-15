  Example to configure the Audio Codec using I2C in polled mode
 
  I2C CODEC CONFIGURATION

  I2C Codec Configure TEST
 
  TEST DESCRIPTION:
 		This test verifies the operation of configuring AUdio codec using CSL
  I2C module in polled mode.
 
  During the test the Audio Codec registers are configured, by writing some
  value to them using the I2C_write() API provided by the CSL I2C module.
  After the I2C_write() API is successful, the configured codec register
  values are read back using I2C_read() API. Finally the written and the read
  values are compared and validated whether they are same or not. Depending on
  the comparison results, the example code returns success or failure.
 
  I2C operates at all the system clock frequencies. Value of the system clock
  at which CPU is running during the test should be passed to the I2C_setup()
  function. This will be used to calculate I2C clock and prescaler values.
  'CSL_I2C_SYS_CLK' macro is defined with different system clock values.
  Depending on the system clock value corresponding value definition of the
  macro should be uncommented and re-build the project for proper operation
  of the test.
 
  Both read and write APIs are synchronous and returns to the application only
  after completing the data transfer. A small delay is required after each data
  operation to allow the device to get ready for the next operation.
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5535, C5515
  AND C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
 
  TEST PROCEDURE:
   1. Open the CCS and connect the target (C5535/C5515 EVM or C5517 EVM)
   2. Open the project "CSL_I2C_CodecTestExample.pjt" and build it
   3. Load the program on to the target
   4. Set the PLL frequency to 12.288MHz
   5. Run the program and observe the test result
 
   TEST RESULT:
    All the CSL APIs should return success
    Read and write value comparison should be successful.