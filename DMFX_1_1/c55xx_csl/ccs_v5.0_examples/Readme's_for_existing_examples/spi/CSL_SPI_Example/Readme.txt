  SPI sample
  
  SPI POLL MODE TEST
 
  TEST DESCRIPTION:
 		This test code verifies the functionality of CSL SPI module. SPI module
  on the C5535/C5515 DSP(Based on the EVM) is used to communicate with SPI EEPROM and audio code.This test code verifies the SPI functionality using SPI EEPROM.

  During the test an EEPROM page will be written and read back using CSL SPI
  module. CSL SPI module will be configured using SPI_config() API. EEPROM
  status is checked to confirm that the device is not busy. Write pin is
  enabled on the EEPROM for SPI data write operation. Write command is sent
  to the SPI. Data is written to the  EEPROM page using SPI_dataTransaction()
  API. After successful completion of the write operation read command is
  sent to the SPI. Data from the EEPROM page is read using
  SPI_dataTransaction() API. Write and read buffers are compared for the data
  verification. A small amount of delay is required after each data operation
  to allow the device get ready for next data operation.
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5535 & C5515 BASED on EVM.
  MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
 
  TEST PROCEDURE:
   1. Open the CCS and connect the target (C5535/C5515 EVM)
   2. Open the project "CSL_SPI_Example_Out.pjt" and build it
   3. Load the program on to the target
   4. Set the PLL frequency to 12.288MHz
   5. Run the program and observe the test result
 
  TEST RESULT:
    All the CSL APIs should return success
    Data in the read and write buffers should match.