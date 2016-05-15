 NAND DMA mode test file to check the work around for DMA word swap

 EMIF NAND DMA WORD SWAP TEST

 TEST DESCRIPTION:
 		This test is to verify the software work around for DMA hardware
 word swap. On C5515 DSP DMA swaps the words in the source buffer before
 transferring it to the destination. No data mismatch is observed when the
 write and read operations are done in DMA mode as the word swap occurs in
 both the operations. There will be data mismatch if data write is in DMA
 mode and read is in polling mode or vice versa. To ensure that the data will
 be written to memory properly in DMA mode, words in the write buffer are
 swapped by software. During DMA transfer DMA hardware again will do a word
 swap which will bring the data buffer back to original values. Similarly
 a word swap is required for read buffer after read operation in DMA mode.
 To verify this work around this test will do one data operation in DMA mode
 and other data operation in polled mode and compares the data buffers.

 On C5515 and C5517 this test is to verify that the DMA hardware word swap is
 eliminated and no software work around is required.

 This test is having two parts. In the first part NAND page is written
 in DMA mode and read in polled mode. CSL DMA module will be initialized
 and configured before starting the NAND data transfer. Words in the NAND
 write buffer will be swapped as a work around for DMA hardware word swap.
 NAND module will be initialized, configured, page address is set and write
 command is sent to NAND device. NAND data width should be set to 16 bits
 while using DMA for data transfer. DMA data transfer will be started using
 DMA_start() API. DMA will transfer configured bytes of data to NAND.
 This completes the NAND page write operation in DMA mode. For read operation
 NAND page address will be set and read command will be sent to the NAND
 device. NAND page will be read using NAND_readNBytes() API. This completes
 the NAND read operation in polled mode. NAND write and read buffers are
 compared for data verification.

 In the second part of the test NAND page will be written in polled mode and
 read in DMA mode. CSL DMA module will be initialized and configured for
 NAND read operation. CSL NAND module will be initialized, configured, page
 address is set and write command is sent to the NAND device. NAND page will
 be written using NAND_writeNBytes API. This completes the NAND write
 operation in polled mode. For read operation NAND page address will be set
 and read command will be sent to the NAND device. NAND data width should
 be set to 16 bits while using DMA for data read. DMA data read will be
 started using DMA_start() API. DMA reads configured bytes of data from NAND.
 This completes the NAND page read operation in DMA mode. NAND write and read
 buffers are compared for data verification.

 NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5515 AND
 C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 c55xx_csl\inc\csl_general.h.

 TEST PROCEDURE:
  1. Open the CCS and connect the target (C5515/C5517 EVM)
  2. Open the project "CSL_NAND_DmaWordSwapExample_Out.pjt" and build it
  3. Load the program on to the target
  4. Set the PLL frequency to 12.288MHz
  5. Run the program and observe the test result

 TEST RESULT:
   All the CSL APIs should return success
   Read and write data buffer comparison should be successful during both
   phases of the test.