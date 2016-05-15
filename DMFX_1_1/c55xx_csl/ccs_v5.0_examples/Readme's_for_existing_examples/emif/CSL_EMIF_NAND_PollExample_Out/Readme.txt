 Example code to verify EMIF nand polled mode 

 EMIF NAND POLL MODE TEST
 
 TEST DESCRIPTION:
       Following example code tests the read and write operations of CSL
  EMIF module in polled mode. A NAND page is written and read in this test.
  Data buffer "gnandWriteBuf" will hold the data to be written to NAND.
  Data buffer "gnandReadBuf" is used to store the data read from the NAND
  device. NAND page written/read in the current code is having column
  address 0x0000 and row address 0x000040. Page address can be changed by
  using the API NAND_setAddress.
 
  During the test CSL EMIF module will be initialized and configured write
  operation. NAND page address is set and write command is send to the NAND
  device. Data is written to the NAND device using CSL API NAND_writeNBytes.
  NAND ECC is read to check if there are any errors during the write
  operation. NAND write operation is complete at this point.
 
  After the successful completion of the write operations NAND page address
  is set again and read command is send to the NAND device. Data is read from
  the NAND device using CSL API NAND_readNBytes. NAND ECC is read to check if
  there are any errors during the read operation. NAND read operation is
  complete at this point.After completing the read operation read and written
  buffers are compared to verify the test result.
 
  This test code works with both BIG block and SMALL block NAND devices.
  NAND device under test should not be write protected to run this test.
  This test will return with error in the case of NAND device with write
  protection. Proper NAND device should be inserted in the NAND daughter card
  attached to the EVM(Applicable only to the EVMs with external daughter card)
  to run this test. Details of few well known NAND devices are listed in lookup
  table "cslNandIdLookup" to verify the NAND device ID. This test will not
  work with any NAND devices for which details are not available in the lookup
  table.
 
  NOTE: This test assumes that
   a. NAND flash is connected to emif cs4 on C5515 EVM and on C5517 EVM.
   b. On all three EVMs A11 address bit is connected to ALE signal of NAND
      flash and A12 address bit is connected to CLE signal of NAND flash.
  This example may not work with connections that are different from above
  connections.
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5515 AND
  C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
 
  TEST PROCEDURE:
   1. Open the CCS and connect the target (C5515/C5517 EVM)
   2. Open the project "CSL_NAND_PollExample_Out.pjt" and build it
   3. Load the program on to the target
   4. Set the PLL frequency to 12.288MHz
   5. Run the program and observe the test result
 
  TEST RESULT:
    All the CSL APIs should return success
    Read and write data buffer comparison should be successful.
 