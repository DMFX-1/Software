 Example code to verify the interrupt APIs of CSL EMIF module

 EMIF NAND INTERRUPT MODE

 TEST DESCRIPTION:
      Following example code tests the interrupt related APIs of CSL
 EMIF module. EMIF module interrupts are used to verify the ready status
 of the NAND device and to detect errors occurred during data transfer
 operations.

 During the test four different NAND pages will be written and read using
 CSL EMIF NAND write and read APIs. After each write-read operation Wait-Raise(WR)
 interrupt is read using NAND_intrReadWR() API and Asynchronous Time-out(AT)
 error is read using NAND_intrReadAT() API. Asynchronous Timeout error
 indicates timeout errors occurred during the data transfer operations.
 Wait-Raise interrupt indicates NAND ready status for next data operation.

 During the test NAND CSL module will be initialized and configured and
 NAND WR and AT interrupts are enabled. NAND page is written and read using
 CSL_nandReadWrite() API. This API receives NAND page row and column address
 as parameters. CSL NAND module will be configured for writing the page with
 received row address and column address. NAND page will be written using
 NAND_writeNBytes() API. After successful completion of write operation CSL
 NAND module will be configured for read operation. NAND page will be read
 using NAND_readNBytes() API. After completion of the read operation data
 buffers are compared to verify the read data. EMIF_intrReadAT() API is
 called to check if there is any timeout error during data transfer
 operation. Test exits with error if the AT interrupt value is 1. If no AT
 interrupt is occurred NAND Wait-Raise status will be read using
 EMIF_intrReadWR(). This status will be read until NAND device get ready for
 data transfer. Once the device ready status is detected CSL_nandReadWrite()
 will be called to write and read next NAND page. NAND row address will be
 incremented in each call for this API. This cycle will be repeated for four
 times to write and read four different NAND pages.

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
  a. NAND flash is connected to emif cs4 on the C5515 EVM and the C5517 EVM.
  b. On all three EVMs A11 address bit is connected to ALE signal of NAND
     flash and A12 address bit is connected to CLE signal of NAND flash.
 This example may not work with connections that are different from above
 connections.

 NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5515 AND
 C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 c55xx_csl\inc\csl_general.h.

 TEST PROCEDURE:
  1. Open the CCS and connect the target(C5515/C5517 EVM)
  2. Open the project "CSL_NAND_IntrExample_Out.pjt" and build it
  3. Load the program on to the target
  4. Set the PLL frequency to 12.288MHz
  5. Run the program and observe the test result

 TEST RESULT:
   All the CSL APIs should return success
   No AT interrupt should occur during data transfer operations
   Read and Write data buffer comparison should be successful.