 Test for CSL DMA stop API
 
 DMA STOP API TEST
 
 TEST DESCRIPTION:
 		This test verifies the functionality of DMA_stop() API of the CSL DMA
  module. This API is not required during normal mode operation of the DMA
  module. DMA transfer will be stopped automatically after transferring the
  configured length of data in normal mode of operation. DMA_stop() API is
  useful in two cases. One case is when DMA is operating in auto-reload mode
  and it is required to stop the DMA data transfer. Second case is DMA
  transfer is started in normal mode but it is required to abort the data
  transfer before the normal completion. This test verifies the functionality
  of DMA stop API by aborting the data transfer while DMA is transferring the
  data in normal mode of operation.
    During the test DMA will be configured to transfer the data in the source
  buffer 'dmaSRCBuff' to the destination buffer 'dmaDESTBuff' using
  DMA_config() API. Configured values are read back using DMA_getConfig() API
  and are verified with the original values. DMA data transfer is triggered
  using DMA_start() API. DMA data transfer is aborted immediately using
  DMA_stop() API. After stopping the DMA data transfer data in the source and
  destination buffer will be compared. Data in the source and destination
  buffers should match till the point DMA is stopped by using the stop API and
  should mismatch after that point. Position of the buffer mismatch will be
  displayed in the CCS "stdout" window. This position will be different for
  each DMA channel. Same test procedure is repeated on all the 16 DMA channels.
    DMA data length should be large enough and DMA_stop() API should be called
  immediately after calling the DMA_start() API for proper functionality of
  the test. DO NOT RUN THE TEST BY SINGLE STEPPING.
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5535, C5515
  AND C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
 
  TEST PROCEDURE:
   1. Open the CCS and connect the target
   2. Open the project "CSL_DMA_StopAPIExample.pjt" and build it
   3. Load the program on to the target
   4. Set the PLL frequency to 12.288MHz
   5. Run the program and observe the test result

 TEST RESULT:
   All the CSL APIs should return success
   DMA configuration values should be read back and verified successfully
   DMA transfer should be stopped successfully and source and destination
   data should mismatch on all the 16 DMA channels.