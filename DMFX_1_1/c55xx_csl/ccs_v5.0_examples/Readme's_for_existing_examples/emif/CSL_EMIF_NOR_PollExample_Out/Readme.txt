 Example code to verify EMIF nor polled mode  

 EMIF NOR POLL MODE TEST
 
 TEST DESCRIPTION:
       Following example code tests the read and write operations of CSL
  EMIF module NOR flash in polled mode. A 1KB data is written and read
  from NOR flash in this test.
  Data buffer "gnorWriteBuf" will hold the data to be written to NOR.
  Data buffer "gnorReadBuf" is used to store the data read from the NOR
  device.
 
  During the test CSL EMIF module will be initialized and asynchronous
  configurations are performed.Data is written to the NOR device using
  CSL API NOR_writeNwords.NOR write operation is complete at this point.
 
  After the successful completion of the write operations read command is sent
  to the NOR device. Data is read from the NOR device using CSL API NOR_readNwords.
  NOR read operation is complete at this point.After completing the read
  operation read and written buffers are compared to verify the test result.
 
 
  NOTE: This test assumes that
   a. NOR flash is connected to emif cs2 on C5515 EVM and on C5517 EVM.
  This example may not work with connections that are different from above
  connections.
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5515 AND
  C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
 
 TEST PROCEDURE:
   1. Open the CCS and connect the target (C5515/C5517 EVM)
   2. Open the project "CSL_NOR_PollExample_Out.pjt" and build it
   3. Load the program on to the target
   4. Set the PLL frequency to 12.288MHz
   5. Run the program and observe the test result
 
 TEST RESULT:
    All the CSL APIs should return success
    Read and write data buffer comparison should be successful.