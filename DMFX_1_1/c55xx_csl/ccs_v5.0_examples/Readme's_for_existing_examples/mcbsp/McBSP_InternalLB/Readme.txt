  Test code to verify the internal loopback functionality of McBSP module
 
  McBSP INTERNAL LOOPBACK TEST
 
  TEST DESCRIPTION:
  This test verifies the operation of the C5517 McBSP (Multichannel Buffered
  Serial Port) module in loopback mode.
  In loopback mode the McBSP module does not communicate with any slave device.
  Internally the McBSP transmit register will be connected to the McBSP receive
  register. Any data copied to the McBSP transmitted data will be transmitted
  to the McBSP receive registers. Loopback mode is helpful to verify the McBSP
  data and shift registers and internals data paths.
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSION C5517.
  MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
 
  TEST PROCEDURE:
   1. Open the CCS and connect the target (C5517 EVM)
   2. Open the project "CSL_McBSP_InternalLB.pjt" and build it
   3. Load the program on to the target
   4. Set the PLL frequency to 12.288MHz
   5. Run the program and observe the test result
 
  TEST RESULT:
    All the CSL APIs should return success
    Read and write data buffer comparison should be successful.