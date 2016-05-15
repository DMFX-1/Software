 Example test code to verify the CSL GPT module functionality

 GPT TEST
 
 TEST DESCRIPTION:
 		This test code verifies the functionality of the CSL GPT (General
  Purpose Timer) module. GPT module of C5535/C5515/C5517 DSP is used to provide
  interrupts to the CPU to schedule periodic tasks or a delayed task.
  GPT can be configured with a counter and pre-scaler divider value. When the
  GPT is started counter will be decremented to 0. The rate at which this
  count happens can be controlled by the prescaler divider value. GPT clock
  is derived by dividing the CPU clock with the pre-scaler divider value.
 
  This test is having two parts. First part of the test verifies whether the
  GTP is decrementing the counter or not. GPT is configured with a count value
  0xFFFF and pre-scaler divider value 256. GPT is started and counter value is
  read. After few cycles of delay GPT counter value is read again. Both the
  counter values are compared to verify whether the second count value is less
  than the first counter value or not.
 
  During second part of the test timer count rate of the GPT is verified.
  As the GPT clock is derived from the CPU clock based on the pre-scaler
  value it possible to verify the GPT rate with respect to CPU clock rate.
  GPT pre-scaler value is set to divide by 4. So the counter runs at a speed
  that is 1/4th of the CPU clock. Clock at which the CPU is running during the
  test is calculated using the function getSysClk(). This function returns
  the value of system clock in KHz. Value of the system clock in KHz
  represents the CPU clock cycles for one millisecond. GPT count value is set
  to 1/4th of the CPU clock cycles for one millisecond. With this setup GPT
  should take one millisecond to count down the counter value to 0. In other
  way clock cycles executed by the CPU from the time GPT is started to the
  time counter value reaches 0 should be equal to the number of cycles that
  are executed by the CPU in one millisecond.GPT is configured to generate
  interrupt when the count value reaches to 0. GPT is started and the CPU
  clock cycles are counted in while loop. This loop increments a global
  variable 'cpuCycleCount' until the GPT generate time-out interrupt.
  This while loop execution is taking 12 CPU cycles. So the total CPU cycles
  executed will be 12*cpuCycleCount. At the end of the test this CPU cycles
  calculated are compared with the CPU clock cycles that will be executed in
  one millisecond. The calculated CPU cycles can vary ±1% from the actual
  CPU cycles. If this condition is satisfied GPT is running at the configured
  rate.
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5535, C5515
  AND C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
 
  TEST PROCEDURE:
   1. Open the CCS and connect the target (C5535/C5515 EVM or C5517 EVM))
   2. Open the project "CSL_GPTExampale_Out.pjt" and build it
   3. Load the program on to the target
   4. Set the PLL frequency to 12.288MHz
   5. Run the program and observe the test result
 
  TEST RESULT:
    All the CSL APIs should return success
    Timer count value comparison should be successful in the first part of
    the test
    Timer should run at the configured rate in the second part of the test