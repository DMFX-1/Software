  Test code to verify the DMA word swap during I2C data transfers
 
  I2C DMA WORD SWAP TEST
 
  TEST DESCRIPTION:
 		This test verifies the operation of CSL I2C module with write in
  one operating mode and read in other operating mode. This test is conducted
  in two phases in first phase I2C write will be done in DMA mode and read
  will be done in polled mode. In second phase I2C write will be done in
  polled mode and read will be done in DMA mode. This test is to prove that
  the DMA word swap has no effect on the I2C data operations.
 
  On C5515 DSP DMA swaps the words in the source buffer before transferring
  it to the destination. No data mismatch is observed when both write and
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
 
  During the first phase of the test 64 bytes of data is written to EEPROM
  page in DMA mode and read back in polled mode. I2C module will be
  configured to master mode, 7-bit addressing 10KHz bus frequency using
  I2C_config() API. Slave address, data length and data command should be
  passed to the I2C_config API. Data buffer 'gI2cDmaWrBuf' is used to store
  the data to be written to EEPROM page. I2C data transfer is started by
  setting the start bit and DMA data transfer is started using DMA_Start() API.
  DMA data transfer completion is detected using DMA_getStatus() API.
  This completes the I2C write in DMA mode. For reading the EEPROM page in
  polled mode address of the page will be written to the EEPROM in DMA mode.
  I2C is configured for read operation using I2C_config() API. EEPROM page is
  read by using the I2C_read() API. Data buffer 'gI2cPollRdBuf' is used to
  store the data read from the EEPROM page. This completes the I2C read
  operation in polled mode. After successful completion of write and read
  operations gI2cDmaWrBuf and gI2cPollRdBuf are compared for data
  verification.
 
  During the second phase of the test 64 bytes of data is written to EEPROM
  page in polled mode and read back in DMA mode. I2C module will be
  configured to master mode, 7-bit addressing 10KHz bus frequency using
  I2C_setup() API. Data buffer 'gI2cPollWrBuf' is used to store the data to
  be written to EEPROM page. Data is written to the EEPROM page using
  I2C_write() API. This completes the I2C write in polled mode. For reading
  the EEPROM page in DMA mode address of the page will be written to the
  EEPROM in polled mode using I2C_write() API. I2C is configured for read
  operation using I2C_config() API. Data buffer 'gI2cDmaRdBuf' is used to
  store the data read from the EEPROM page. I2C data receive is started by
  setting the start bit and DMA data transfer is started using DMA_Start()
  API. DMA data transfer completion is detected using DMA_getStatus() API.
  This completes the I2C read operation in DMA mode. After successful
  completion of write and read operations gI2cPollWrBuf and gI2cDmaRdBuf
  are compared for data verification.
 
  Small amount of delay is required after each data operation to allow the
  I2C device to get ready for next data operation. Data buffers allocated
  should be of type Uint16 for polled mode operation of type Uint32 for
  DMA mode of operation.
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5515 AND
  C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
 
  TEST PROCEDURE:
   1. Open the CCS and connect the target (C5515 EVM or C5517 EVM)
   2. Open the project "CSL_I2C_DmaWordSwapExapmle_Out.pjt" and build it
   3. Load the program on to the target
   4. Set the PLL frequency to 12.288MHz
   5. Run the program and observe the test result
 
  TEST RESULT:
    All the CSL APIs should return success
    Read and write data buffer comparison should be successful.