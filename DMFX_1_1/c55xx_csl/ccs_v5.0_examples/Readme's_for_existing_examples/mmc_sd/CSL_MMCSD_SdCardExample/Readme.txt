  MMCSD example to test Secure Digital(SD) card operation in POLL mode
 
  SD CARD POLL MODE TEST
 
  TEST DESCRIPTION:
 		This test code verifies the functionality of the CSL MMCSD module with
  Secure Digital(SD) card. This Functionality test includes detecting,
  initializing, configuring, writing, reading the SD card using CSL MMCSD
  module. All these steps are carried out in sequence by different CSL APIs.
  After the card detection phase test code verifies whether the detected card
  is SD or not. Test code stops execution and returns with error incase of
  card is not SD. After the successful detection of the SD card, it is
  initialized and configured. Two data buffers 'gSdWriteBuff' and 'gSdReadBuff'
  are used for SD card write and read operations respectively. SD card write
  buffer is initialized to values 0 to 255 and is written to SD card sector
  zero using MMC_write API. Same SD card sector is read using MMC_read API and
  the data read is stored in the SD card read buffer. Data in the buffers
  'gSdWriteBuff' and 'gSdReadBuff' is compared to check whether SD card read
  and write operations are successful or not.
  This test code uses POLLED mode to read and write the SD card data.
 
  Maximum value of the clock at which memory data transaction takes place
  can be controlled using the macro 'CSL_SD_CLOCK_MAX_KHZ'.
  Depending on the clock at which CPU is running, SD clock will be configured
  to the possible value that is nearest to the value defined by this macro.
  SD clock will be configured to a value less than or equal to but not greater
  than the value defined by this macro. Changing this macro value
  will automatically change the memory clock divider value.
  memory clock will be generated from the system clock based on equation
 

   memory clock = (system clock) / (2  (CLKRT + 1)
      - memory clock is clock for the memory card
      - system clock is clock at which CPU us running
      - CLKRT is value of clock rate configured in clock control register
 
  As per this equation memory clock that can be generated is always less than
  or equal to half of the system clock value. Value of 'CLKRT' can range
  from 0 to 255. Maximum and minimum memory clock values that can be generated
  at a particular CPU frequency are limited by the minimum and maximum value
  of the memory clock rate (CLKRT).
 
  NOTE: THIS TEST IS INTENDED TO BE USED WITH SD CARD. IT FAILS WITH MMC.
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5535, C5515
  AND C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
 
  TEST PROCEDURE:
   1. Insert "SD" card into the MMC/SD socket(J9) on the C5535/C5515 EVM
   or J16 on the C5517 EVM
   2. Open the CCS and connect the target (C5535/C5515/C5517 EVM)
   3. Open the project "CSL_MMCSD_MmcCardExample_Out.pjt" and build it
   4. Load the program on to the target
   5. Set the PLL frequency to 12.288MHz
   6. Run the program and observe the test result

  TEST RESULT:
    All the CSL APIs should return success
    Data in the read and write buffers should match