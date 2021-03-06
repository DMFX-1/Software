  Test code to verify the SAR functionality in polled mode
 
  SAR POLL MODE TEST
 
  TEST DESCRIPTION:
 		This test verifies the functionality of CSL SAR(Successive Approximation
  Register) in polled mode. SAR module on C5515/C5517 DSP is used to convert
  the voltage generated by the key pad available on the C5515/C5517 EVM to
  digital values. This key pad is having 10 button with each button having
  a different digital value corresponding to the voltage generated when it is
  pressed.
 
  This test is having two parts. In the first part of the test C5515/C5517
  key pad button voltage is read and the converted digital value is displayed
  on the CCS "stdout". This digital value corresponds to the voltage of the
  button pressed while running the test. SAR module is having six channels to
  read the data. This test configures channel four for reading the button
  voltage in polled mode. After successful configuration of the SAR, voltage
  conversion is started using the CSL API SAR_startConversion(). Digital value
  of the button voltage is read for 40 times and is displayed in the CCS
  "stdout" window. After reading 40 digital values SAR data conversion is
  stopped and CSL SAR module will be closed.
 
  In the second part of the test SAR is configured using SAR_chanConfig() API.
  Configured values are read back using SAR_getConfig() API and are compared
  with the original values. This part of the test also verifies setting the GPO
  direction using SAR_GPODirSet() API. Success of this API call can be
  verified using the SAR GPO control register(SARGPOCTRL) value displayed in
  the CCS "stdout" window.
 
  
   Digital values corresponding to the button voltage will vary slightly
   depending on the system clock value. Below are the range of values for
   each button available on the C5515/C5517 EVM key pad.

    No button pushed: 0x3fb - 0x3ff
    SHIFT:    0x230 - 0x238
    STOP:     0x0
    RWD:      0x20f - 0x216
    MENU:     0x367 - 0x374
    DN:       0x2bc - 0x2c6
    SEL/PLAY: 0x1d3 - 0x1d8
    UP:       0x24f - 0x258
    REC:      0xd5  - 0xd8
    FWD:      0x163 - 0x167
    MODE:     0x30b - 0x316

  
 
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5515 AND
  C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
 
  TEST PROCEDURE:
   1. Open the CCS and connect the target (C5515/C5517 EVM)
   2. Open the project "CSL_SAR_PollExample.pjt" and build it
   3. Load the program on to the target
   4. Set the PLL frequency to 12.288MHz
   5. Press any one of the buttons present on C5515/C5517 EVM key pad.
 
  TEST RESULT:
    All the CSL APIs should return success
    Digital value corresponding to a button voltage should be within the
    range specified in the test procedure.
    SAR configuration values should be read successfully and should match
    with original value.
    Value of the "SARGPOCTRL" register displayed after calling SAR config
    API should be 0x80 indicating that GPO direction set is successful.