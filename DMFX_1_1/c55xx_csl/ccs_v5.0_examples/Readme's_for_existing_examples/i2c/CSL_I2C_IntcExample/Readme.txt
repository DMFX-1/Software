 I2C interrupt mode example

 I2C INTERRUPT MODE TEST
 
 TEST DESCRIPTION:
 		This test verifies the operation of CSL I2C module in interrupt mode.
 
 During the test 64 bytes of data is written to EEPROM page and is read back
 using the CSL I2C APIs and I2C data callback functions. Interrupts are used
 to indicate the device ready status for a byte of data transmit or receive
 operation. CSL INTC module should be configured and I2C ISR should be
 registered before starting the data transfer. I2C callback functions which
 are called from the Interrupt Service Routine should be configured using
 I2C_setCallback() API. I2C module will be configured to master mode, 7-bit
 addressing and 10KHz bus frequency using I2C_config() API. I2C own address,
 data length, clock values, data command should be sent to the I2C config API.
 Data buffer 'gI2cWrBuf' is used to store the data to be written to EEPROM
 page. Address of the page to be written is stored in the first two bytes of
 the I2C write buffer. So the data length sent to the config API should be
 66 bytes. after configuring the I2C module, interrupts for I2C transmit and
 No Acknowledgement error will be enabled and I2C data transfer will be
 triggered by setting the start bit. I2C will start generating the transmit
 interrupts. One byte of data will be copied to the I2C data transmit
 register in the I2C transmit callback function for each interrupt generated.
 After completing the transfer of 66 bytes I2C interrupts will be disabled.
 This completes the EEPROM page write.
 
 Reading the data from the EEPROM page will be done in two steps. In first
 step address of the EEPROM page to be read is written to the EEPROM.
 This operation will be same as data write operation except that the length
 of the data will be 2 bytes. After writing the EEPROM page address I2C will
 be configured for data read using I2C_config() API. I2C receive ready
 interrupt is enabled and I2C data reception is started by setting the start
 bit. I2C starts generating read interrupts. I2C read callback function will
 read one byte of data from I2C read register for each interrupt generated.
 Data read from the EEPROM page is stored in the buffer 'gI2cRdBuf'. After
 reading 64 bytes of data I2C interrupts will be disabled. This completes
 I2C read operation. After successful completion of the write and read
 operations data buffers are compared to verify the success of the operations.
 
 A small delay is required after each data operation to allow the device get
 ready for next data transfer operation.
 
 NOTE: TEST SHOULD NOT BE RUN BY SINGLE STEPPING AFTER ENABLING THE I2C
 INTERRUPTS. DOING SO WILL LEAD TO IMPROPER HANDLING OF THE INTERRUPTS
 AND TEST FAILURE.
 
 NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5515 AND
 C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 c55xx_csl\inc\csl_general.h.
 
 TEST PROCEDURE:
  1. Open the CCS and connect the target (C5515 EVM or C5517 EVM)
  2. Open the project "CSL_I2C_IntcExample_Out.pjt" and build it
  3. Load the program on to the target
  4. Set the PLL frequency to 12.288MHz
  5. Run the program and observe the test result
 
 TEST RESULT:
   All the CSL APIs should return success
   Read and write buffer comparison should be successful.