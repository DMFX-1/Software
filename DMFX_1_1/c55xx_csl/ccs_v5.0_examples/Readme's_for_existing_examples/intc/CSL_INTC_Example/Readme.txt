  INTC sample for SAR
 
  INTC EXAMPLE
 
  TEST DESCRIPTION:
 		This test verifies the functionality of CSL INTC module. INTC module
  is used to configure and enable/disable the interrupts available on the
  C5515/C5517 DSP.
 
  This test verifies the INTC module using CSL SAR. SAR module on C5515/C5517
  DSP is used to convert the voltage generated by the key pad available on the
  C5515/C5517 EVM to digital values. During the test C5515/C5517 EVM key pad
  button voltage is read using interrupts. SAR interrupts are configured and
  ISR is registered using INTC module APIs. SAR is configured and the data
  conversion is started using SAR_startConversion(). Interrupt is generated
  when there is a valid value in the SAR data register. Digital value is read
  from the SAR data register using the Interrupt Service Routine and is
  displayed in the CCS 'stdout' window. Generation of SAR interrupt will prove
  the functionality of INTC module.
 
  Digital values corresponding to the button voltage will vary slightly
  depending on the system clock value. Below are the range of values for
  each button available on the C5515/C5517 EVM key pad.
 
   No button pushed: 0x3fb - 0x3ff
   SHIFT:    0x230 - 0x238
   STOP:     0x0
   RWD:      0x20f - 0x216
   MENU:     0x367 - 0x376
   DN:       0x2bc - 0x2c7
   SEL/PLAY: 0x1d3 - 0x1d8
   UP:       0x24f - 0x258
   REC:      0xd5  - 0xd8
   FWD:      0x163 - 0x167
   MODE:     0x30b - 0x318
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5515 AND
  C5517. MAKE SURE THAT PROPER PROPER CHIP VERSION MACRO SHOULD IS DEFINED
  IN THE FILE c55xx_csl\inc\csl_general.h.
 
  TEST PROCEDURE:
   1. Open the CCS and connect the target (C5515 EVM or C5517)
   2. Open the project "CSL_INTC_Example.pjt" and build it
   3. Load the program on to the target
   4. Set the PLL frequency to 12.288MHz
   5. Press any one of the buttons present on C5515/C5517 EVM key pad.
   6. Run the program and observe the test result
 
  TEST RESULT:
    All the CSL APIs should return success
    Digital value should read in the ISR and displayed in the CCS "stdout"
    window