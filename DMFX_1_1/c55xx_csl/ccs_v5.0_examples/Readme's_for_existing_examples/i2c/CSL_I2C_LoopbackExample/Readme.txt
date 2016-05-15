  I2C loopback test example

  I2C LOOPBACK MODE TEST
 
  TEST DESCRIPTION:
 		This test verifies the operation of CSL I2C module in loopback mode.
  In loopback mode I2C module does not communicate with any slave device.
  Internally I2C transmit register will be connected to the I2C receive
  register. Any data copied to the I2C transmitted data will be transmitted
  to the I2C receive registers. Loopback mode is helpful to verify the I2C
  data and shift registers and internals data paths.
 
  During the test one byte of data is written to the I2C transmit register and
  the same is read from the I2C receive register. I2C module will be configured
  to loopback mode, 7-bit addressing and 10KHz bus frequency using I2C_config()
  API. Data buffer 'gI2cWrBuf' is used to store the data to be written to I2C
  transmit register. Data buffer 'gI2cRdBuf' is used to store the data read
  from the I2C receive register. After configuring the I2C module one byte of
  data is written to I2C using a macro 'CSL_I2C0_WRITEREG' and one byte of
  data is read from I2C using 'CSL_I2C0_READREG'. A small delay is required
  between write and read operation to allow the data get loopbacked. This
  write-read operation is repeated for 64 bytes. After tranferring the 64 bytes
  read and write buffers are compared for data verification.
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5535, C5515
  AND C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
 
  TEST PROCEDURE:
   1. Open the CCS and connect the target (C5535/C5515 EVM or C5517 EVM)
   2. Open the project "CSL_I2C_LoopbackExample_Out.pjt" and build it
   3. Load the program on to the target
   4. Set the PLL frequency to 12.288MHz
   5. Run the program and observe the test result
 
  TEST RESULT:
    All the CSL APIs should return success
    Read and write data buffer comparison should be successful.