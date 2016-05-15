  UHPI sample
 
  UHPI MEMORY ACCESS
 
  TEST DESCRIPTION:
 		This test code verifies the functionality of CSL UHPI module. The UHPI module
  on the C5517 DSP is used to communicate with MSP430, where the MSP430 is the master.
  This test works together with the CSL_UHPI_MSP430_Example.out
 
  THe C5517 DSP is set up as the slave device in the UHPI transfer. It then waits for the
  data transfer.
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSION C5517.
  MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
  # define CHIP_C5517
 
  TEST PROCEDURE:
   1. Open the CCS and connect the target (C5517 EVM-Master board)
   2. Open the project "CSL_UHPI_Example.pjt" and build it
   3. Load the program on to the target
   4. Once this test is running, run CSL_UHPI_MSP430_Example.out on the MSP430
  through a different Code Composer window.
   5. Once the MSP430 part of the test is finished running, halt this program.
   6. Verify the data on memory/DATA @ 0x8000. It should be:
   
 	0x00008000	_stack, __edata, edata, __data, .data
 	0x00008000	0x0000	0xFFFF	0x0001
 	0x00008003	0xFFFE	0x0002	0xFFFD
 	0x00008006	0x0003	0xFFFC	0x0004
 	0x00008009	0xFFFB	0x0005	0xFFFA
 	0x0000800C	0x0006	0xFFF9	0x0007
 	0x0000800F	0xFFF8	0x0008	0xFFF7
 	0x00008012	0x0009	0xFFF6	0x000A
 	0x00008015	0xFFF5	0x000B	0xFFF4
 	0x00008018	0x000C	0xFFF3	0x000D
 	0x0000801B	0xFFF2	0x000E	0xFFF1
 	0x0000801E	0x000F	0xFFF0	0x0010
 	0x00008021	0xFFEF	0x0011	0xFFEE
 	...etc.
   
 
  TEST RESULT:
    The data memory address data should match the values above.
 
    If on board buffers latched reset the board & Run the program,Use the correct GEL file for CPU configuration
 