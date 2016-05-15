  I2C dma mode example

  I2C EXAMPLE DMA MODE TEST
 
  TEST DESCRIPTION:
 		This test verifies the operation of CSL I2C module in DMA mode.
  In DMA mode of operation CSL DMA module is used to transfer the data
  between CPU memory and I2C registers.
 
  During the test 64 bytes of data is written to EEPROM page and is read back
  using the DMA module. CSL DMA module needs to be initialized and configured
  for I2C read and write operations before configuring the I2C module.
  On C5515 DSP DMA swaps the words in the source buffer before transferring
  it to the destination. No data mismatch is observed When both write and
  read operations are done in DMA mode as the word swap occurs in both the
  operations. There will be data mismatch if data write is in DMA mode and
  read is in polling mode or vice versa. In case of I2C this word swap does
  not take effect due to 8 bit data access of the I2C hardware.
  I2C module generates a DMA event for each byte of data transfer but DMA
  transfers 4 bytes for each I2C event. To synchronize the I2C events
  and DMA transfers data is stored in 32 buffer with only 8 bits having valid
  data and other bits will have zeros. I2C will be configured for N bytes
  and DMA will be configured for 4*N bytes of data transfers.
  Due this specific buffer allocation and configuration, only one byte will
  be valid data out of four bytes transferred by DMA for each I2C event.
  This DMA word swap is eliminated in the chip version C5515.
 
  Due to big endianness of the C5515/C5517 DSP a value 0x00000001 assigned
  to 32 bit buffer will be stored in memory as 0x0000 and 0x0001.
  During data transfer DMA swaps the words and writes 0x0001 in lower 16 bit
  address and 0x0000 in upper 16 bits address. The value 0x0000 written in
  upper 16 bit address will be ignored by I2C since Tx and Rx registers are
  only 16 bit wide.
 
  I2C module will be configured to master mode, 7-bit addressing 10KHz bus
  frequency using I2C_config() API. Slave address, data length and data
  command should be passed to the I2C_config API. Data buffer 'gI2cWrBuf' is
  used to store the data to be written to EEPROM page. Address of the page to
  be written is stored in the first two bytes of the I2C write buffer. So the
  data length passed to the I2C_config API should be 66 bytes. I2C start bit
  is set to one to enable the I2C data transfer DMA event generation. DMA data
  transfer on the channel configured for I2C transmit will be started by using
  DMA_start() API. DMA data transfer completion is detected using
  DMA_getStatus() API. This completes the I2C write operation.
 
  Reading the data from the EEPROM page will be done in two steps. In first
  step address of the EEPROM page to be read is written to the EEPROM.
  This operation will be same as data write operation except that the length
  of the data will be 2 bytes. After writing the EEPROM page address I2C
  should be configured for read operation using I2C_config() API. Configuring
  the I2C for read or write depends on the value sent to the I2C command
  register. I2C start bit is set to one to enable the I2C data receive DMA
  event generation. DMA data transfer on the channel configured for I2C
  receive will be started by using DMA_start() API. DMA data transfer
  completion is detected using DMA_getStatus() API. This completes the
  I2C read operation. After successful completion of the write and read
  operations write and read buffers are compared for data verification.
 
  Small amount of delay is required after each data operation to  allow the
  I2C device to get ready for next data operation. In DMA mode of operation
  I2C data buffers should be of type Uint32.
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5515, C5515
  AND C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
 
  TEST PROCEDURE:
   1. Open the CCS and connect the target (C5515 EVM or C5517 EVM)
   2. Open the project "CSL_I2C_DmaExample_Out.pjt" and build it
   3. Load the program on to the target
   4. Set the PLL frequency to 12.288MHz
   5. Run the program and observe the test result
    
  TEST RESULT:
    All the CSL APIs should return success
    Read and write data buffer comparison should be successful.