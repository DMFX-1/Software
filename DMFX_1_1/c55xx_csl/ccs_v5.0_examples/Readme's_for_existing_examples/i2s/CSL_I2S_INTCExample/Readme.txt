  Test code to verify the CSL I2S functionality in Interrupt mode
 
 
  I2S INTERRUPT MODE TEST
 
  TEST DESCRIPTION:
 		This test verifies the operation of CSL I2S module in interrupt mode.
  I2S module on C5535/C5515/C5517 DSP is used to exchange the audio data between
  the audio codec and DSP.
 
  Testing of the I2S with the audio codec is not possible as codec is not
  part of the CSL package. This test verifies the I2S operation in loopback
  mode. In loopback mode of operation data written to the I2S data transmit
  registers will be transmitted to the I2S data receive registers. CSL INTC
  module should be configured and I2S ISR should be registered before starting
  the data transfer. Separate ISRs are registered for I2S Tx and Rx. Interrupts
  are used to check the Tx/Rx status of the I2S. I2S is configured to master
  mode,stereo, 16bit data length and to loop back mode. When the I2S data
  transfer is enabled I2S starts generating the Tx interrupts. Data is written
  to the I2S Tx registers in the ISR 'i2s_txIsr'. As the I2S is configured in
  loopback mode when the data is written to Tx register it will be transmitted
  to Rx register and Rx interrupt will be generated. Data will be read from the
  I2S Rx registers in the ISR 'i2s_rxIsr'.I2S write and read buffers are
  compared for the data verification.
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5535, C5515
  AND C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
 
  TEST PROCEDURE:
   1. Open the CCS and connect the target (C5535/C5515 EVM or C5517 EVM)
   2. Open the project "CSL_I2S_INTCExample_Out.pjt" and build it
   3. Load the program on to the target
   4. Set the PLL frequency to 12.288MHz
   5. Run the program and observe the test result
 
  TEST RESULT:
    All the CSL APIs should return success
    Read and write data buffer comparison should be successful.