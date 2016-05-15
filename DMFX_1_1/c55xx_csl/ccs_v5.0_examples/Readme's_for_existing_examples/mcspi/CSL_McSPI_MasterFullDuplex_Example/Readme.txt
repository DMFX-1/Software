  McSPI functional layer sample source file
 
  McSPI MASTER FULL DUPLEX TEST
 
  TEST DESCRIPTION:
 		This test code verifies the full duplex functionality of CSL McSPI module. The McSPI module
  on the C5517 DSP is used to communicate with SPI MsP430.
  This test code verifies the McSPI functionality with MsP430 SPI .
 
  CSL McSPI module will be configured using McSPI_config() API.Write,Read function used for data transfer operation.
  After successful completion of the write operation read
  sent to the SPI.Write and read buffers are compared for the data
  verification. A small amount of delay is required after each data operation
  to allow the device get ready for next data operation.
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5517.
  MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
  # define CHIP_C5517
 
  TEST PROCEDURE:
   1. Open the CCS and connect the target (C5517 EVM-Master board)
   2. Open the project "CSL_McSPI_MasterFullDulpex_Example.pjt" and build it
   3. Load the program on to the target
   4. Load the equivalent CSL_McSPIMaster_MSP430Slave_FullDuplex onto the MSP430
   5. Set the PLL frequency to 12.288MHz
   7. Run the program and observe the transmitting data
   8. Run the MSP430 program and observe the receiving data increase in value
    
  TEST RESULT:
    All the CSL APIs should return success
    Run the master program first & then the slave program
    Data in the read and write buffers should match.(or)Data needs to be verified in Slave program buffer
 
   If on board buffers latched reset the board & Run the program,Use the correct GEL file for CPU configuration
 