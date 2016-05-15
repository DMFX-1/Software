  I2C polled mode example

  I2C POLL MODE TEST
 
  TEST DESCRIPTION:
 		This test verifies the operation of CSL I2C module in polled mode.
 
  During the test 64 bytes of data is written to EEPROM page and is read back
  using the CSL I2C APIs. I2C module will be configured to master mode, 7-bit
  addressing and 10KHz bus frequency using I2C_setup() API. EEPROM page with
  address 0x0000 is written using the I2C_write() API. Data buffer 'gI2cWrBuf'
  is used to store the data to be written to EEPROM page. Address of the page
  to be written is stored in the first two bytes of the I2C write buffer.
  So the data length passed to the write API should be 66 bytes. Address of
  the slave device (0x50 for EEPROM) should be passed to the write API.
 
  Reading the data from the EEPROM page will be done in two steps. In first
  step address of the EEPROM page to be read is written using I2C_write API.
  This operation will be same as data write operation except that the length
  of the data will be 2 bytes. After writing the EEPROM page address data
  should be read from that EEPROM page using I2C_read() API. Data read from
  the EEPROM page is stored in the buffer 'gI2cRdBuf'. Slave device address
  and data length to be read should be passed to the I2C_read() API. Here the
  data length should be 64 bytes. After successful completion of the write and
  read operations data buffers are compared to verify the success of the
  operations.
 
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
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5515 AND
  C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
 
  TEST PROCEDURE:
   1. Open the CCS and connect the target (C5515 EVM or C5517 EVM)
   2. Open the project "CSL_I2C_PollExample_Out.pjt" and build it
   3. Load the program on to the target
   4. Set the PLL frequency to 12.288MHz
   5. Run the program and observe the test result
 
  TEST RESULT:
    All the CSL APIs should return success
    Read and write data buffer comparison should be successful.