  LCDC example to verify the DMA mode(Without Interrupts) of operation
 
  LCD DMA POLL TEST WITHOUT INTERUPTS
 
  TEST DESCRIPTION:
      This test code demonstrates DMA mode of operation of the CSL LCD module.
  DMA module of LCD operates with out using the interrupts. During this test
  LCD panel will be filled with two colors; Blue and Red. Upper half of the
  LCD will be colored Blue and lower half of the LCD will be colored Red.
  Filling the colors will be done in two steps. In the first step test code
  configures CSL LCD module and DMA to fill upper half of the LCD with Blue
  color. In the second step test code configures CSL LCD module and DMA to
  fill lower half of the LCD with Red color. DMA used for LCD data transfer
  is internal to the LCD controller and is not the DMA of C5515 DSP.
  This example uses 256 color mode.
 
  NOTE: Data buffer used by the LCD in DMA mode should be allocated in the
  SARAM. This test code places the 'gLcdBuf' in SARAM using pragma macro.
 
   LCD display after running the test will be as shown below, when
   the LCD is viewed with JTAG port on the left hand side and Power port on the
   right hand side.

     ====================
     ====================
     == BBBBBBBBBBBBBB ==
     == BBBBBBBBBBBBBB ==
     == BBBBBBBBBBBBBB ==
     == RRRRRRRRRRRRRR ==
     == RRRRRRRRRRRRRR ==
     == RRRRRRRRRRRRRR ==
     ====================
     ====================

     'B' - Part of LCD colored in Blue
     'R' - Part of LCD colored in Red

  Manual inspection is required to determine the success/failure of the test.
  Execution status of each API will be displayed on the CCS stdout window.
 
  TEST PROCEDURE:
   1. Open the CCS and connect the target (C5515 EVM)
   2. Open the project "CSL_LCDC_DmaPolledExample_Out.pjt" and build it
   3. Load the program on to the target
   4. Set the PLL frequency to 12.288MHz
   5. Run the program and observe the test result
 
  TEST RESULT:
   All the CSL APIs should return success.
   Upper Half of the LCD panel should be filled with BLUE color
   Lower Half of the LCD panel should be filled with RED color