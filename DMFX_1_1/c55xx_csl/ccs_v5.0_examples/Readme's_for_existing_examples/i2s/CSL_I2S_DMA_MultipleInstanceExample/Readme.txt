  Test code to verify the CSL I2S multiple instance functionality in
  DMA mode
 
  I2S MULTIPLE INSTANCES TEST IN DMA MODE
 
  TEST DESCRIPTION:
 		This test verifies the operation of CSL I2S module for multiple
  instances, in DMA mode. I2S module on C5535/C5515/C5517 DSP is used to
  exchange the audio data between the audio codec and DSP.
 
  Testing of the I2S with the audio codec is not possible as codec is not
  part of the CSL package. This test verifies the I2S operation in loopback
  mode. For each instance of I2S in loopback mode of operation, data written
  to the I2S data transmit registers will be transmitted to the I2S data
  receive registers. CSL DMA module should be configured for the I2S Tx and
  Rx operations. DMA module is used to transfer the data between CPU memory
  and I2S data registers. I2S is configured to master mode, stereo, 16bit
  data length and to loop back mode. Due to word swap behavior of the DMA
  while transferring the data to/from I2S, I2S write data buffer should be
  word swapped before transferring it to the I2S registers. In case of I2S no
  word swap is required on the data received due internal word swap by the I2S
  in loopback mode. I2S data transfer is enabled and DMA is started using
  DMA_start() API. DMA writes the data to the I2s Tx registers. After
  successful completion of write operation DMA is configured for Rx operation
  and again started to read the data from I2S Rx registers. I2S write and read
  buffers are compared for the data verification. The above procedure is
  carried out for each instance of the I2S, that are supported on the
  platform.
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5535, C5515
  AND C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
 
  TEST PROCEDURE:
   1. Open the CCS and connect the target (C5535/C5515 EVM or C5517 EVM)
   2. Open the project "CSL_I2S_DMA_MultipleInstanceExample_out.pjt" and
       build it
   3. Load the program on to the target
   4. Set the PLL frequency to 12.288MHz
   5. Run the program and observe the test result
 
  TEST RESULT:
    All the CSL APIs should return success
    Read and write data buffer comparison should be successful on all the
    I2S instances, supported by the Platform.