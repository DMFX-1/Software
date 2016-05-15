  MMCSD-ATAFS interface test code

  MMCSD-ATAFS DIRECTORY CREATION TEST
 
  TEST DESCRIPTION:
 		This test code verifies the functionality of MMCSD-ATAFS interface.
  ATA File System is used to create, write and read files on the SD card.
  Files created by the ATFS can be read from windows PC using a card reader.
 
  During the test, a directory is created on the SD card, a file is created
  in it, 512 bytes of data is written to the file and the same data is
  read back.
 
  CSL MMCSD module should be configured before initializing the file system.
  During thecard detection test code checks for the SD card.If no SD card
  is detected test exits with error. After successful detection and
  configuration of the SD card file system should be initialized using
  ATA_systemInit(). SD card should be formatted to run this test.
  ATA_systemInit() function fails if the card is not formatted. File with
  the given name is created using ATA_create() API. 512 bytes of data is
  written to the file created using ATA_write() API. After successful
  completion of write operation same data is read from the card using
  ATA_readLittleEndian(). Write and read buffers are compared for the data
  verification. The file created and data written can be accessed using a
  SD card reader. This test is executed in both polling and DMA modes.
 
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
 
  NOTE: THIS TEST WORKS WITH ONLY SD CARD. SD CARD SHOULD BE FORMATTED TO
  RUN THIS TEST.
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5505, C5515
  AND C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
 
  TEST PROCEDURE:
   1. Insert formatted "SD" card into the MMC/SD socket(J9) on the C5505/C5515 EVM
   or J16 on C5517 EVM
   2. Open the CCS and connect the target (C5505/C5515/C5517 EVM)
   3. Open the project "CSL_MMCSD_SdCardFSDirExample_Out.pjt" and build it
   4. Load the program on to the target
   5. Set the PLL frequency to 12.288MHz
   6. Run the program and observe the test result

 
  TEST RESULT:
    All the CSL APIs should return success
    Data in the read and write buffers should match.
    Directory and file created should be accessible from a windows PC
    using card reader.