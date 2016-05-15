  MMCSD example to test Multiple instances of the Secure Digital(SD)
  card operation in POLL mode
 
  SD CARD MULTIPLE INSTANCES TEST
 
  TEST DESCRIPTION:
 		This test code verifies the multiple instances functionality of the CSL
  MMCSD module with Secure Digital(SD) card. This Functionality test includes
  detecting, initializing, configuring, writing, reading the SD card using CSL
  MMCSD module, for each instance of the MMCSD module supported on C5517 EVM.
  All these steps are carried out in sequence by different CSL APIs, for each
  instance of the MMCSD module on C5517 EVM. After the card detection phase
  test code verifies whether the detected card is SD or not. Test code stops
  execution and returns with error incase of card is not SD. After the
  successful detection of the SD card, it is initialized and configured. Two
  data buffers 'pWriteBuff' and 'pReadBuff' are used for SD card write and
  read operations respectively, for each instance of the MMCSD module. SD card
  write buffer is initialized to values 0 to 255 and is written to SD card
  sector zero using MMC_write API. Same SD card sector is read using MMC_read
  API and the data read is stored in the SD card read buffer. Data in the
  buffers 'pWriteBuff' and 'pReadBuff' is compared to check whether SD card
  read and write operations are successful or not. This test code uses POLLED
  mode to read and write the SD card data.
 
  NOTE: THIS TEST IS INTENDED TO BE USED WITH SD CARD. IT FAILS WITH MMC.
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH ONLY CHIP VERSION C5517.
  MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
 
  TEST PROCEDURE:
   1. Insert "SD" cards into the MMC/SD socket(J16) and socket(J21) on the
   C5517 EVM
   2. Open the CCS and connect the target (C5517 EVM)
   3. Open the project "CSL_MMCSD_SdCard_MultiInstance_Example_Out.pjt" and build it
   4. Load the program on to the target
   5. Set the PLL frequency to 12.288MHz
   6. Run the program and observe the test result
 
  TEST RESULT:
    All the CSL APIs should return success
    Data in the read and write buffers should match, for all the MMCSD
    instances supported on the Platform