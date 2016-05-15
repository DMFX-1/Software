  Test code to verify the CSL I2S functionality in POLL mode
 
  I2S POLL MODE TEST
 
  TEST DESCRIPTION:
 		This test verifies the operation of CSL I2S module in polled mode.
  I2S module on C5535/C5515/C5517 DSP is used to exchange the audio data between
  the audio codec and DSP.
 
  Testing of the I2S with the audio codec is not possible as codec is not
  part of the CSL package. This test verifies the I2S operation in loopback
  mode. In loopback mode of operation data written to the I2S data transmit
  registers will be transmitted to the I2S data receive registers. I2S is
  configured to master mode, stereo, 16bit data length and to loop back mode.
  I2S data transfer is enabled and the data is transferred to the I2S using
  I2S_write() API. After successful completion of I2S write data is read from
  the I2S using I2S_read() API. I2S write and read buffers are compared for
  the data verification.
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5535, C5515
  AND C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
 
  TEST PROCEDURE:
   1. Open the CCS and connect the target (C5535/C5515/C5517 EVM)
   2. Open the project "CSL_I2S_PollExample_out.pjt" and build it
   3. Load the program on to the target
   4. Set the PLL frequency to 12.288MHz
   5. Run the program and observe the test result
 
  TEST RESULT:
   All the CSL APIs should return success
   Read and write data buffer comparison should be successful.