  Test code to verify the CSL I2S functionality with the Audio Code
  in DMA Interrupt mode
 
 
  I2S IDLE LOOP
 
  TEST DESCRIPTION:
 		This test verifies the operation of CSL I2S module with the Hardware
  Audio Codec in DMA Interrupt mode. I2S module on C5535/C5515/C5517 DSP is
  used to exchange the audio data between the audio codec and DSP.
 
  		Testing of the I2S with the audio codec is done by integrating the I2S,
  I2C and DMA CSL Module, to send a sine wave to the Data input registers of
  the Audio Codec, which inturn will pump this sine wave to the Data Output
  register, and finally to the Stereo Output.
 
 		I2C CSL Module is used to configure the Audio Codec with proper
  configurations. CSL DMA module should be configured for the I2S Tx and Rx
  operations, I2S is configured to Slave mode and the Codec is configured in
  Master Mode. DMA module is used to transfer the data between CPU memory and
  I2S data registers, the I2S Module will inturn transfer the Data to the
  Codec Data Input registers.
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5535, C5515
  AND C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
 
  TEST PROCEDURE:
   1. Open the CCS and connect the target (C5535/C5515 EVM or C5517 EVM)
   2. Open the project "CSL_I2S_IdleLoop_Example_Out.pjt" and build it
   3. Load the program on to the target
   4. Set the PLL frequency to 12.288MHz
   5. Run the program and observe the test result
 
  TEST RESULT:
    All the CSL APIs should return success