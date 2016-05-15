  Test code to verify the interaction between McBSP and the audio codec
  
  McBSP Audio polling mode
 
  TEST DESCRIPTION:
  This test verifies the operation of the C5517 McBSP (Multichannel Buffered
  Serial Port) module in polling mode along with the AIC3204.
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSION C5517.
  MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
 
  TEST PROCEDURE:
   1. Open the CCS and connect the target (C5517 EVM)
   2. Open the project "CSL_McBSP_Master_AIC3204_48kbps_POLL.pjt" and build it
   3. Load the program on to the target
   4. Set the PLL frequency to 12.288MHz
   5. Connect the audio source to P8 (stereo in 1)
   6. Connect the headphones to P8 (headphone)
   7. Run the program, start playing a song, and observe the test result
   8. Run the test at PLL frequencies 40, 75, 100, or 150MHz
 
  TEST RESULT:
   The song played on the audio source should play clearly through the EVM