  CSL MMCSD eMMC Card Speccific API tests for CSL MMCSD Module
 
  MMCSD eMMC CARD SPECIFIC APIs TEST
 
  TEST DESCRIPTION:
 		This test code verifies the functionality of the APIs specific to eMMC
  card using the CSL MMCSD module. This Functionality test includes detecting,
  initializing, configuring the eMMC card using CSL MMCSD module.
 
  During the test MMCSD module will be initialized and opened in polled mode.
  The eMMC card specific APIs such as MMC_enableBootPartitionAccess(),
  MMC_enableBootPartitionAccess(), MMC_getCardExtCsd(),
  MMC_disableBootPartitionAccess(), MMC_bootPartitionEnable(),
  MMC_bootPartitionDisable(), MMC_getBootPartitionSize(), MMC_setBootBusWidth
  and MMC_setEMMCBusWidth() are tested. For testing each of these APIs,
  we use MMC_getCardExtCsd() API to verify whether the configurations are set
  or not.
 
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
 
  @verbatim
   memory clock = (system clock) / (2  (CLKRT + 1)
      - memory clock is clock for the memory card
      - system clock is clock at which CPU us running
      - CLKRT is value of clock rate configured in clock control register
   @endverbatim
 
  As per this equation memory clock that can be generated is always less than
  or equal to half of the system clock value. Value of 'CLKRT' can range
  from 0 to 255. Maximum and minimum memory clock values that can be generated
  at a particular CPU frequency are limited by the minimum and maximum value
  of the memory clock rate (CLKRT).
 
  NOTE: THIS TEST WORKS WITH ONLY eMMC CARD.
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5515 AND
  C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
 
  TEST PROCEDURE:
   1. Insert "eMMC" card into the MMC/SD socket(J9) on the C5515 EVM
   or J16 on the C5517 EVM
   2. Open the CCS and connect the target (C5515/C5517 EVM)
   3. Open the project "CSL_MMCSD_extAPIs_Example_Out.pjt" and build it
   4. Load the program on to the target
   5. Set the PLL frequency to 12.288MHz
   6. Run the program and observe the test result
 
  TEST RESULT:
    All the CSL APIs should return success