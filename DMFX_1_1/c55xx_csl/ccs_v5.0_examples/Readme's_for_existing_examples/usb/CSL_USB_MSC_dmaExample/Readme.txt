   Example code to test the USB MSC operation in DMA mode
 
   NOTE: For Testing MSC module a macro CSL_MSC_TEST needs to be defined
   This includes some code in csl_usbAux.h file which is essential for MSC
   operation and not required for MUSB stand alone testing. define this macro in
   predefined symbols in project build options (Defined in the current usb
   msc example pjt).Semaphores and mail boxes are used in the MSC example code as
   the USB operation is not possible with out OS calls. DSP BIOS is used for this
   purpose. Defining Start transfer and complete transfer call back functions is
   must and MSC module does not work if they are not implemented properly.
   A call back is sent to this functions from MUSB module.
 
  TEST PROCEDURE:
  1. Open the CCS and connect the target (C5535/C5515/C5517 EVM)
  2. Open, build and load the USB program to the target
  3. Set the PLL frequency to 100MHz
  4. Run the program loaded on to the target
  5. A new Storage device should appear in "My Computer". Open the drive and
     do some file transfers.
 
  TEST RESULT:
   USB Storage device should be detected by the host PC and the file
   transfers should be successful
   File transfers done using the example should be as desired, when the
   SD card contents are viewed when connected to the PC using a Card Reader
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5505, C5515,
  AND C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.