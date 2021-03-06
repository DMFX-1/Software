  Test code to verify the SAR functionality in DMA mode
 
  SAR DMA MODE TEST
 
  TEST DESCRIPTION:
 		This test verifies the functionality of CSL SAR(Successive Approximation
  Register) in DMA mode. SAR module on C5515/C5517 DSP is used to convert the
  voltage generated by the key pad available on the C5515/C5517 EVM to digital
  values. This key pad is having 10 button with each button having a different
  digital value corresponding to the voltage generated when it is pressed.
 
  During the test C5515/C5517 key pad button voltage is read and the converted
  digital value is displayed in the CCS "stdout" window. This digital value
  corresponds to the voltage of the button pressed while running the test.
  SAR module is having six channels to read the data. This test configures
  channel four for reading the button voltage in DMA mode. DMA channel should
  configured for SAR data read before starting the SAR data conversion. DMA
  module in this test uses interrupt to indicate the completion of DMA data
  transfer. CSL INTC module should be configured and DMA ISR should registered
  before starting the SAR data conversion. After successful configuration of
  the SAR, DMA data transfer is started using DMA_start() API and SAR voltage
  conversion is started using the CSL API SAR_startConversion(). Digital value
  of the button voltage is read by the DMA module and is copied to the data
  buffer 'readBuffer'. Even though the digital value generated by SAR is
  maximum 10 bits, DMA reads 32 bits from the SAR. Due to word swap nature
  of the DMA, value read from the SAR will be stored in the upper word
  of the data buffer. This value will be displayed in the CCS "stdout" window.
  Digital value is read from SAR and is displayed on CCS "stdout" window
  only one time.
 
  
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
   2. Open the project "CSL_SAR_DmaExample.pjt" and build it
   3. Load the program on to the target
   4. Set the PLL frequency to 12.288MHz
   5. Press any one of the buttons present on C5515/C5517 EVM key pad.
   6. Run the program and observe the test result
 
  TEST RESULT:
    All the CSL APIs should return success
    Digital value corresponding to a button voltage should be within the
    range specified in the test procedure.