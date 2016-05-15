  LCDC example to verify 65k color mode operation
 
  LCD 65K COLOR MODE TEST
 
  TEST DESCRIPTION:
      This test code demonstrates 65k color mode operation of CSL LCD module.
  LCD controller on the EVM supports three color modes; 256, 65k and 262k.
  In 65k color mode each color is represented using two 8 bit data.
  LCD controller on the EVM is interfaced to LCD panel using 8-bit data
  interface. Each pixel data will be sent to the LCD in two 16 bit words,
  each word carrying one 8-bit information of the color. Color information
  is transferred to LCDC registers using internal DMA module.
 
  DMA module of LCD operates using the interrupts.During this test LCD panel
  will be filled with two colors; Red and Green. Upper half of the LCD will be
  filled Red color and lower half of the LCD will be filled with green color.
  Filling the colors will be done in two steps. In the first step test code
  configures CSL LCD module and DMA to fill upper half of the LCD with Red
  color. In the second step test code configures CSL LCD module and DMA to
  fill lower half of the LCD with Green color. DMA used for LCD data transfer
  is internal to the LCD controller and is not the DMA of C5515 DSP.
 
  NOTE: Data buffer used by the LCD in DMA mode should be allocated in the
  SARAM. This test code places the 'gLcdBuf' in SARAM using pragma macro.
 
  NOTE: Macro 'LCD_65K_COLOR_MODE' should be defined to enable the color value
  and other configurations for 65k color mode. This macro was defined in
  project build options.
 
   LCD display after running the test will be as shown below, when
   the LCD is viewed with JTAG port on the left hand side and Power port on the
   right hand side.

     ====================
     ====================
     == RRRRRRRRRRRRRR ==
     == RRRRRRRRRRRRRR ==
     == RRRRRRRRRRRRRR ==
     == GGGGGGGGGGGGGG ==
     == GGGGGGGGGGGGGG ==
     == GGGGGGGGGGGGGG ==
     ====================
     ====================

     'R' - Part of LCD colored in Red
     'G' - Part of LCD colored in Green
   
  Manual inspection is required to determine the success/failure of the test.
  Execution status of each API will be displayed on the CCS stdout window.
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5515.
  MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
 
  TEST PROCEDURE:
   1. Open the CCS and connect the target (C5515 EVM)
   2. Open the project "CSL_LCDC_65kColorModeExample_Out.pjt" and build it
   3. Load the program on to the target
   4. Set the PLL frequency to 12.288MHz
   5. Run the program and observe the test result
 
  TEST RESULT:
    All the CSL APIs should return success.
    Upper Half of the LCD panel should be filled with RED color
    Lower Half of the LCD panel should be filled with GREEN color