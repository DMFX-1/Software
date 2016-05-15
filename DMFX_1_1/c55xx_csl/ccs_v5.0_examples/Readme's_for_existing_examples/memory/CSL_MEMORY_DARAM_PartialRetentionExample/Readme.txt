  DARAM partial memory retention mode test example source file
 
  MEMORY DARAM PARTIAL RETENTION MODE TEST
 
  TEST DESCRIPTION:
 		This test verifies the partial memory retention mode feature of the
  C5515/C5517 DARAM. Internal memory of the C5515/C5517 DSP can be placed into a low
  power memory retention while retaining the content stored in the memory.
  This low power mode is activated through the Sleep Mode Memory Control.
  When the memory is placed in this mode, no accesses can occur.
  When the memory retention mode is enabled, inline diodes will be added
  to reduce the local power supply to the on-chip RAM. Partial memory
  retention mode allows to enable sleep mode for a DARAM bank independent
  of other DARAM or SARAM memory banks.
 
  During test, a data buffer allocated in the DARAM bank0 is initialized with
  data. CSL memory module is initialized and partial memory retention mode
  for DARAM bank 0 is enabled using the CSL API MEM_enablePartialRetentionMode().
  Data in other DARAM banks is accessed to demonstrate that enabling partial
  retention mode for one memory bank will not effect rest of the banks.
  Memory retention mode will be disabled after few CPU cycles by using the
  CSL API MEM_disablePartialRetentionMode(). Data in the DARAM0 buffer is
  verified after few CPU cycles of delay. Buffer should contain the data with
  which it was initialized before enabling the memory retention mode.
  This proves that the data stored in the DARAM0 is retained when it is placed
  into memory retention mode and is accessible after disabling the memory
  retention mode. Same test is repeated for other DARAM banks.
 
  NOTE: DURING MEMORY RETENTION MODE TEST FOR DARAM ALL THE PROGRAM SECTIONS
  SHOULD BE PLACED IN THE SARAM. ACCESSING THE DATA IN THE RETENTION MODE
  ENABLED DARAM BANK BY ANY MEANS(CCS WATCH WINDOW, MEMORY WINDOW OR PLACING
  THE MOUSE POINTER ON THE DARAM DATA BUFFER TO VIEW THE VALUE) WILL CORRUPT
  ALL THE DATA IN THAT BANK.
 
  NOTE: PARTIAL MEMORY RETENTION MODE IS SUPPORTED ONLY ON CHIP 5515/5517. THIS
  TEST IS NOT VALID FOR CHIP 5505. MAKE SURE THAT PROPER CHIP VERSION MACRO
  IS DEFINED IN THE FILE c55xx_csl\inc\csl_general.h.
 
  TEST PROCEDURE:
   1. Open the CCS and connect the target (C5515/C5517 EVM)
   2. Open the project "CSL_MEMORY_DARAM_PartialRetentionExample_Out.pjt"
      and build it
   3. Load the program on to the target
   4. Set the PLL frequency to 12.288MHz
   5. Run the program and observe the test result

  TEST RESULT:
    All CSL APIs should return success
    DARAM buffer data verification after disabling the memory retention
    mode should be successful