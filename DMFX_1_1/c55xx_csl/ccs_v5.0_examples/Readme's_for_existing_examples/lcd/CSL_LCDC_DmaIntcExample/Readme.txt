  LCDC example to verify the DMA mode(Using Interrupts) of operation
 
  LCD DMA INTERRUPT TEST
 
  TEST DESCRIPTION:
      This test code demonstrates DMA mode of operation of the CSL LCD module.
  DMA module of LCD operates using the interrupts.During this test LCD panel
  will be filled with two colors; Blue and Green. Left half of the LCD will be
  colored Blue and right half of the LCD will be colored Green. Filling the
  colors will be done in two steps. In the first step test code configures CSL
  LCD module and DMA to fill left half of the LCD with Blue color. In the
  second step test code configures CSL LCD module and DMA to fill right half of
  the LCD with Green color. DMA used for LCD data transfer is internal to the
  LCD controller and is not the DMA of C5515 DSP. This example uses 256
  color mode.
 
 
  NOTE: Data buffer used by the LCD in DMA mode should be allocated in the
  SARAM. This test code places the 'gLcdBuf' in SARAM using pragma macro.
 
   LCD display after running the test will be as shown below, when
   the LCD is viewed with JTAG port on the left hand side and Power port on the
   right hand side.

     ====================
     ====================
     == BBBBBBBGGGGGGG ==
     == BBBBBBBGGGGGGG ==
     == BBBBBBBGGGGGGG ==
     == BBBBBBBGGGGGGG ==
     == BBBBBBBGGGGGGG ==
     == BBBBBBBGGGGGGG ==
     ====================
     ====================

     'B' - Part of LCD colored in Blue
     'G' - Part of LCD colored in Green

  Manual inspection is required to determine the success/failure of the test.
  Execution status of each API will be displayed on the CCS stdout window.
 
  TEST PROCEDURE:
   1. Open the CCS and connect the target (C5515 EVM)
   2. Open the project "CSL_LCDC_DmaIntcExample_Out.pjt" and build it
   3. Load the program on to the target
   4. Set the PLL frequency to 12.288MHz
   5. Run the program and observe the test result
 
  TEST RESULT:
    All the CSL APIs should return success.
    Left Half of the LCD panel should be filled with BLUE color
    Right Half of the LCD panel should be filled with GREEN color