  Test code to verify the HID class functionality of the USB
 
  USB HID CLASS TEST
 
  TEST DESCRIPTION:
  This test is to verify the operation of the CSL USB HID module. This test
  initializes the USB HID module, SAR module to read the push button values,
  the test program then waits in a while loop for the user input through push
  buttons. The EVM will act as an HID (mouse) device and the PC will act as
  the Host. Using the push buttons present on the EVM, we can send the mouse
  commands to the Host PC, from EVM.
 
  This test can be verified by pressing the push buttons on the EVM, and
  observing whether the corresponding mouse operation is performed on the
  Host PC.
 
  
   Digital values corresponding to the button voltage will vary slightly
   depending on the system clock value. Below are the range of mouse commands
   corresponding to each push button of the EVM

    UP:   To move the mouse cursor to up from current cursor position
    DN:   To move the mouse cursor to down from current cursor position
    RWD:  To move the mouse cursor to left from current cursor position
    FWD:  To move the mouse cursor to right from current cursor position
    MENU: To send the Right Click command
    MODE: To send the Left Click command
    STOP: To stop the Program

  
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5515 AND
  C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
 
  TEST PROCEDURE:
  1. Connect the C5515/17 EVM with a PC via USB cable
  2. Open the CCS and connect the target (C5515/C5517 EVM)
  3. Open, build and load the USB program to the target
  4. Set the PLL frequency to 100MHz
  5. Run the program loaded on to the target
  6. After the proper USB enumeration, check the Control Panel-->System-->
     Hardware-->Device Manager-->Ports (COM & LPT) looking for Human Interface
     Devices.
  7. The test program is acting as an HID device(mouse), whatever you input
     through the push bottons, it will be sent as mouse commands to the host
     PC.
 
  TEST RESULT:
   USB HID Class device should be detected by the host PC and the user should
   be able to interact to the PC using the EVM as a mouse.
   Push Buttons pressed on the EVM, will be read and the respective mouse
   commands will be sent to the Host PC.
 