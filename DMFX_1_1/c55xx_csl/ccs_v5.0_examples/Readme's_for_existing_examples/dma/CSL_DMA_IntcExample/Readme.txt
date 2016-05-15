Test for all the DMA channels in interrupt mode.

DMA INTERRUPT MODE TEST

TEST DESCRIPTION:
 	   	This test verifies operation of the CSL DMA module in interrupt mode.
 C5535/C5515 and C5517 DSPs have four DMA Engines and each DMA engine has
 four channels for the data transfers. Total 16 channels can be configured
 and used for the data transfer simultaneously. DMA can be used to transfer
 data with in the memory and between the memory and peripherals with out
 having the intervention of CPU.

 During the test DMA functionality is verified by transferring the data
 between two buffers allocated in memory of C5535/C5515/C5517 DSP. Data in the
 source buffer 'dmaSRCBuff' is copied into the destination buffer
 'dmaDESTBuff'. DMA interrupts are configured and DMA ISR is registered
 using CSL INTC module. DMA ISR will be triggered by the DMA transfer
 completion interrupt. DMA is configured with proper source and destination
 address and data length using DMA_config() API. Configured values are read
 back using DMA_getConfig() API and are verified with the original values.
 DMA data transfer is triggered using DMA_start() API. After starting the
 DMA data transfer execution of the test will wait for the occurrence of
 DMA transfer interrupt. This is done by checking a global variable which
 will be updated by the DMA ISR. After the transfer completion data in the
 source and destination buffers is compared. The same test procedure is
 repeated on all the 16 DMA channels.

 NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5535, C5515, AND
 C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 c55xx_csl\inc\csl_general.h.
 
 TEST PROCEDURE:
  1. Open the CCS and connect the target
  2. Open the project "CSL_DMA_IntcExample.pjt" and build it
  3. Load the program on to the target
  4. Set the PLL frequency to 12.288MHz
  5. Run the program and observe the test result
  
 TEST RESULT:
   All the CSL APIs should return success
   DMA configuration values should be read back and verified successfully
   DMA transfer should be successful and source and destination data should match on
   all the 16 DMA channels

