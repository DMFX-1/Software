 Example code to test the CSL DAT module functionality

 DAT EXAMPLE

 TEST DESCRIPTION:
		This test verifies the operation of the CSL DAT module. DAT module
 functionality is to support MEMORY-MEMORY data transfers using DMA hardware.
 DAT module is useful to initialize/copy large data buffers with out using CPU.

 CSL DAT module supports two basic operations
 1. Fill Data - Filling a data buffer with a given value
 2. Copy Data - Copying one data buffer to another data buffer
 DAT module configures the DMA module and starts the data transfer depending
 on the request sent from the application program.

 During the test data buffer 'srcDARAM' initialized to all zeros is used to
 verify the DAT functionality. This data buffer is divided into three parts
 and each part(512 bytes) is used for different operation. In the first
 step part1 of the data buffer is filled with a value '0xF5' using DAT_fill()
 CSL API. In the second step data in part1 of the data buffer is copied to
 the part2 of the data buffer using DAT_copy() API. In the third step part3
 of the data buffer is filled with a value '0x5' using DAT_fill() API. As the 
 DMA module of C5515/C5517 DSP is having 16 channels, DAT module can support
 data transfers on all the 16 channels. Above test is repeated and verified
 on all the DMA channels. After each operation data buffer is verified to
 detect any errors in the data transfer. DAT/DMA module considers data buffer
 as the 32bit buffer. During fill operation given value is assigned to MSW of
 the 32bit location in the buffer. For verifying the fill operation MSW of the
 32bit location should be compared with the filled value.

 NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5535, C5515, AND
 C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 c55xx_csl\inc\csl_general.h.

 TEST PROCEDURE:
  1. Open the CCS and connect the target (C5535/C5515 EVM or C5517 EVM)
  2. Open the project "CSL_DAT_Example_Out.pjt" and build it
  3. Load the program on to the target
  4. Set the PLL frequency to 12.288MHz
  5. Run the program and observe the test result

 TEST RESULT:
  All the CSL APIs should return success
  Data fill and Data copy operations should be successful on all the 16 channels
