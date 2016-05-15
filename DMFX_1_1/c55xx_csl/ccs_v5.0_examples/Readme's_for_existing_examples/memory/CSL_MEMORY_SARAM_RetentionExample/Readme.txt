  SARAM memory retention mode test example source file
  
  MEMORY SARAM RETENTION MODE TEST
 
  TEST DESCRIPTION:
 		This test verifies the memory retention mode feature of the C5535/C5515/C5517
  SARAM. Internal memory of the C5535/C5515/C5517 DSP can be placed into a low power
  memory retention while retaining the content stored in the memory. This low
  power mode is activated through the Sleep Mode Memory Control. When the
  memory is placed in this mode, no accesses can occur. When the memory
  retention mode is enabled, inline diodes will be added to reduce the local
  power supply to the on-chip RAM. Memory retention mode can be enabled/disabled
  for the DARAM and SARAM independently.
 
  A data buffer is allocated in the SARAM is initialized with some
  data. CSL memory module is initialized and memory retention mode for
  SARAM is enabled using the CSL API MEM_enableRetentionMode(). Memory
  retention mode will be disabled after few CPU cycles by using the CSL API
  MEM_disableRetentionMode(). Data in the SARAM buffer is verified after
  few CPU cycles of delay. Buffer should contain the data with which it was
  initialized before enabling the memory retention mode. This proves that
  the data stored in the SARAM is retained when it is placed into memory
  retention mode and is accessible after disabling the memory retention mode.
 
  NOTE: DURING MEMORY RETENTION MODE TEST FOR SARAM ALL THE PROGRAM SECTIONS
  SHOULD BE PLACED IN THE DARAM. ACCESSING THE DATA IN THE SARAM BY ANY WAY
  (CCS WATCH WINDOW, MEMORY WINDOW OR PLACING THE MOUSE POINTER ON THE SARAM
  DATA BUFFER TO VIEW THE VALUE) AFTER ENABLING THE MEMORY RETENTION MODE
  WILL CORRUPT ALL THE DATA IN SARAM.
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5535, C5515
  AND C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
 
  TEST PROCEDURE:
   1. Open the CCS and connect the target (C5535/C5515/C5517 EVM)
   2. Open the project "CSL_MEMORY_SARAM_RetentionExample_Out.pjt" and build it
   3. Load the program on to the target
   4. Set the PLL frequency to 12.288MHz
   5. Run the program and observe the test result
 
  TEST RESULT:
    All the CSL APIs should return success
    SARAM buffer data verification after disabling the memory retention
    mode should be successful