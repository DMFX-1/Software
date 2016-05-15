  CSL MMCSD interrupt mode test code
 
  MMC/SD CARD INTERRUPT MODE TEST
 
  TEST DESCRIPTION:
 		This test code verifies the functionality of CSL MMCSD module in
  interrupt mode. This Functionality test includes detecting, initializing,
  configuring, writing, reading the SD/MMC card using CSL MMCSD module.
 
  During the test MMCSD module will be initialized and opened in interrupt
  mode. MMCSD data transfer call back function is configured using
  MMC_setDataTransferCallback() API. This callback function is called from
  MMC read and write APIs. When this callback function is invoked, execution
  waits until MMCSD generates an interrupt. After receiving the interrupt
  call back function disables all the MMCSD interrupts and returns the
  value of MMC status register(MMCST0). Interrupts enabled during a particular
  callback will be decided by the CSL APIs.
  After successful detection of the card MMC module will be configured and
  MMC_write() API is called to write the data to the card inserted.
  Write API returns after successful completion of the data write. MMC_read()
  API is called to read the data from the card. Read API returns after
  successful completion of the read operation. Both read and write APIs
  invoke callback function during the data transfers. Write and read buffers
  are compared for data verification.
 
  Maximum value of the clock at which memory data transaction takes place
  can be controlled using the macros 'CSL_SD_CLOCK_MAX_KHZ' and
  'CSL_MMC_CLOCK_MAX_KHZ' for SD and MMC cards respectively.
  Depending on the clock at which CPU is running, memory clock will be
  configured to the possible value that is nearest to the value defined
  by these macros. memory clock will be configured to a value less than
  or equal to but not greater than the value defined by these macros.
  Changing these macro values will automatically change the memory clock
  divider value.
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
 
  NOTE: THIS TEST WORKS WITH SD AND MMC CARD.
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5535, C5515
  AND C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.

NOTE: This example is validated with PQ1 MMC Mobile card on c5515 EVM.
 
  TEST PROCEDURE:
   1. Insert "MMC/SD" card into the MMC/SD socket(J9) on the C5535/C5515 EVM
   or J16 on the C5517 EVM
   2. Open the CCS and connect the target (C5535/C5515/C5517 EVM)
   3. Open the project "CSL_MMCSD_dmaExample_Out.pjt" and build it
   4. Load the program on to the target
   5. Set the PLL frequency to 12.288MHz
   6. Run the program and observe the test result
 
  TEST RESULT:
    All the CSL APIs should return success
    Data in the read and write buffers should match