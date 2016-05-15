  CSL LCDC Example code to draw different diagrams on LCD
 
  LCD DIAGRAM DISPLAY TEST
 
  TEST DESCRIPTION:
   	This test code demonstrates following functionalities of the CSL LCDC
  module on the C5515 LCD panel.
  - Drawing a Rectangle
  - Drawing a Circle
  - Drawing a Line
  - Copying an Icon
  - Moving an Icon
  Drawing rectangle is done by drawRectangle() API which displays a Red
  color rectangle on the LCD.
  Drawing circle is done by drawCircle() API which displays a blue color
  circle on the LCD.
  Drawing line is done by drawLine() API which displays a white color line
  on the LCD.
  Copying icon is done by iconCopy() API which copies a small portion of image
  from the rectangle and displays on the LCD. This copied icon will be in the
  square shape.
  Moving icon is done by iconMove() API which moves a small portion of image
  from the circle and displays on the LCD. This moved icon will be in the
  rectangular shape. A Black colored rectangular hole will be created on the
  circle due to moving the icon from circle space. This example uses 65k
  color mode.
 
   Position of the images drawn on the LCD screen will be as shown below, when
   the LCD is viewed with JTAG port on the left hand side and Power port on the
   right hand side.

     ================
     =  RRRR  CCCC  =
     =  RRRR  CCCC  =
     =  RRRR  CCCC  =
     =              =
     = LLLLLLLLLLLL =
     =              =
     = IM        IC =
     ================

     'R'  - Position of the Rectangle
     'C'  - Position of the Circle
     'L'  - Position of the Line
     'IM' - Position of moved icon from circle
     'IC' - Position of copied icon from rectangle

  Manual inspection is required to determine the success/failure of the test.
  Execution status of each API will be displayed on the CCS stdout window.
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5515.
  MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
 
  TEST PROCEDURE:
   1. Open the CCS and connect the target (C5515 EVM)
   2. Open the project "CSL_LCDC_DiagramExample_Out.pjt" and build it
   3. Load the program on to the target
   4. Set the PLL frequency to 12.288MHz
   5. Run the program and observe the test result
 
  TEST RESULT:
    All the CSL APIs should return success.
    All the objects displayed on the LCD of the C5515 EVM should be clear,
    with proper color and at proper position as described in the
    TEST DESCRIPTION section.