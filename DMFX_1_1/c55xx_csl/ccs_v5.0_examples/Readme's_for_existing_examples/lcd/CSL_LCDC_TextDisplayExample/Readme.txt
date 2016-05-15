  LCDC test code to display text on the LCD panel
 
  LCD TEXT DISPLAY TEST
 
  TEST DESCRIPTION:
 		This test code demonstrates displaying a string of characters on the
  LCD panel. Different text is displayed on the LCD panel in two steps.
 
  In the first step two character strings are displayed on the LCD panel
  using DMA mode with text in White color. LCD panel is divided into two
  vertical parts to display the strings in separate planes.
  String "IMAGINATION IS BEGINNING OF CREATION.YOU IMAGINE WHAT YOU DESIRE"
  will be displayed on the left half of the LCD.
  String "YOU WILL WHAT YOU IMAGINE AND AT LAST YOU CREATE WHAT YOU WILL"
  will be displayed on the right half of the LCD.
  This two strings will be displayed for few seconds and then text will be
  cleared line by line. During this step text displayed will be smaller
  in size as the LCD panel is divided into two parts.
 
  In the second step one character string will be displayed on the LCD panel
  using polled mode with text in Red color.
  String "CSL FOR TEXAS INSTRUMENTS C5515 LCD MODULE" will be displayed
  on complete plane of the LCD. During this step text displayed will be bigger
  in size and more clear.
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5515.
  MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
 
  TEST PROCEDURE:
   1. Open the CCS and connect the target (C5515 EVM)
   2. Open the project "CSL_LCDC_TextDisplayExample_Out.pjt" and build it
   3. Load the program on to the target
   4. Set the PLL frequency to 12.288MHz
   5. Run the program and observe the test result
   6. Repeat the test at the following PLL frequencies
 
  TEST RESULT:
    All the CSL APIs should return success.
    String "IMAGINATION IS BEGINNING OF CREATION.YOU IMAGINE WHAT YOU DESIRE"
    should be displayed on the left half of the LCD. String "YOU WILL WHAT
    YOU IMAGINE AND AT LAST YOU CREATE WHAT YOU WILL" will be displayed on
    the right half of the LCD. The strings should be cleared line by line
    after few seconds.
    String "CSL FOR TEXAS INSTRUMENTS C5515 LCD MODULE" should be displayed
    on the LCD panel with text in Red color.