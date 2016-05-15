 Test DMA ping-pong mode

 DMA PING-PONG MODE TEST

 TEST DESCRIPTION:
 	   	This test verifies operation of the CSL DMA module in ping-pong mode.
 Ping-Pong mode is a special mode of data transfer in which DMA generates
 a half way interrupt. When ping-pong mode is enabled DMA considers the
 source data buffer as two parts and generates an interrupt after
 transferring half of the data. First half of the buffer is Ping data buffer
 and second half of the buffer is Pong data buffer. These two buffers
 should be allocated in contiguous memory locations or one ping buffer of size
 equal to double the data transfer length should be allocated. Buffer being
 transferred by the DMA currently can be identified by reading
 'Last Transfer Type' bit of the channel transfer control register.
 Ping-Pong mode can be utilized effectively in the interrupt mode of data
 transfer.

 During the test DMA ping-pong functionality is verified by transferring
 the data between four buffers allocated in memory of C5517 DSP. There is
 one Ping set of data buffers and one Pong set of data buffers. DMA is
 configured for ping-pong mode and data transfer is started. Execution will
 wait for the ping and pong data transfer interrupts to occur. Successful
 completion of data transfer will copy Ping source buffer data into Ping
 destination buffer and Pong source buffer data into Pong destination buffer.
 Status of the Ping and Pong set transfer will be displayed in the CCS stdout
 window. Test is repeated for all the 16 DMA channels.

 NOTE: DMA PING-PONG BUFFER MODE IS SUPPORTED ONLY ON CHIPS 5515 AND C5517. THIS TEST HAS
 BEEN DEVELOPED TO WORK WITH CHIPS C5515 AND C5517. MAKE SURE THAT PROPER CHIP VERSION
 MACRO IS DEFINED IN THE FILE c55xx_csl\inc\csl_general.h.

 TEST PROCEDURE:
  1. Open the CCS and connect the target (C5515 EVM or C5517 EVM)
  2. Open the project "CSL_DMA_PingPongExample.pjt" and build it
  3. Load the program on to the target
  4. Set the PLL frequency to 12.288MHz
  5. Run the program and observe the test result

 TEST RESULT:
   All the CSL APIs should return success
   Ping source and destination data should match on all the 16 DMA channels
   Pong source and destination data should match on all the 16 DMA channels

