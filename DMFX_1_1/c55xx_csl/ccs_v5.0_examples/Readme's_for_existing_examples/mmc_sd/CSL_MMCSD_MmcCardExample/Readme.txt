  MMCSD example to test MultiMediaCard(MMC) operation in POLL mode
 
  MMC CARD POLL MODE TEST
 
  TEST DESCRIPTION:
 		This test code verifies the functionality of the CSL MMCSD module with
  MultiMediaCard(MMC). This Functionality test includes detecting, initializing,
  configuring, writing, reading the MMC card using CSL MMCSD module. All these
  steps are carried out in sequence by different CSL APIs. After the card
  detection phase test code verifies whether the detected card is MMC or not.
  Test code stops execution and returns with error incase of card is not MMC.
  After the successful detection of the MMC, it is initialized and configured.
  Two data buffers 'gMmcWriteBuff' and 'gMmcReadBuff' are used for MMC write
  and read operations respectively. MMC write buffer is initialized to values
  0 to 255 and is written to MMC sector zero using MMC_write API.
  Same MMC sector is read using MMC_read API and the data read is stored in the
  MMC read buffer. Data in the buffers 'gMmcWriteBuff' and 'gMmcReadBuff' is
  compared to check if MMC read and write operations are successful or not.
  This test code uses POLLED mode to read and write the MMC data. Functions
  'isr_rcv' and 'isr_txmt' are provided to verify MMC_setCallBack API.
 
  Maximum value of the clock at which memory data transaction takes place
  can be controlled using the macro 'CSL_MMC_CLOCK_MAX_KHZ'.
  Depending on the clock at which CPU is running, memory clock will be
  configured to the possible value that is nearest to the value defined by
  this macro. memory clock will be configured to a value less than or equal
  to but not greater than the value defined by this macro. Changing this
  macro value will automatically change the MMC clock divider value.
  memory clock will be generated from the system clock based on equation
 
   memory clock = (system clock) / (2  (CLKRT + 1)
      - memory clock is clock for the memory card
      - system clock is clock at which CPU us running
      - CLKRT is value of clock rate configured in clock control register
 
  As per this equation mmc clock that can be generated is always less than
  or equal to half of the system clock value. Value of 'CLKRT' can range
  from 0 to 255. Maximum and minimum mmc clock values that can be generated
  at a particular frequency are limited by the minimum and maximum value
  of the memory clock rate (CLKRT).
 
  NOTE: THIS TEST IS INTENDED TO BE USED WITH MMC CARD. IT FAILS WITH SD CARD
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5515 AND
  C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.

 NOTE: This example is validated with PQ1 MMC Mobile card on c5515 EVM.
 
  TEST PROCEDURE:
   1. Insert "MMC" card into the MMC/SD socket(J9) on the C5515 EVM
   or J16 on the C5517 EVM also on C5517 EVM Set the Bootmode setting in SW3[3:0] to 1001 where 1-towards ON, 0-away from ON.
   2. Open the CCS and connect the target (C5515/C5517 EVM)
   3. Open the project "CSL_MMCSD_MmcCardExample_Out.pjt" and build it
   4. Load the program on to the target
   5. Set the PLL frequency to 12.288MHz
   6. Run the program and observe the test result
 
  TEST RESULT:
    All the CSL APIs should return success
    Data in the read and write buffers should match