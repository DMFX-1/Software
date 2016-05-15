  Example code to verify EMIF SDRAM polled mode

  EMIF SDRAM POLL MODE TEST
 
  TEST DESCRIPTION:
       Following example code tests the read and write operations of CSL
  EMIF SDRAM module in polled mode. A 1KB data is written and read from
  SDRAM in this test.Data buffer "gsdramWriteBuf" will hold the data to
  be written to SDRAM.Data buffer "gsdramReadBuf" is used to store the
  data read from the SDRAM device. SDRAM address of 0x28000 is written/read
  in the current code. Address can be changed in the write and read operation APIs
 
  During the test CSL EMIF module will be initialized and configured write
  operation. SDRAM configurations are set. Data is written to the SDRAM device
  using CSL API SDRAM_write.
 
  The Powerdown mode and Self refresh modes are enabled and disabled using the
  respective APIs. Then data is read from SDRAM to a buffer and the write buffer
  and read buffer contents are compared. Test is passed if the contents match.
 
  NOTE: This test assumes that
   a. SDRAM is connected to emif on C5515 EVM and on C5517 EVM
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5515 AND
  C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
 
  TEST PROCEDURE:
   1. Open the CCS and connect the target (C5515/C5517 EVM)
   2. Open the project "CSL_EMIF_SDRAM_PollExample_Out.pjt" and build it
   3. Load the program on to the target
   4. Set the PLL frequency to 12.288MHz
   5. Run the program and observe the test result
 
  TEST RESULT:
    All the CSL APIs should return success
    Read and write data buffer comparison should be successful.