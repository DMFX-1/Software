 Example test code to verify the CSL GPT module's nested interrupt functionality

 GPT NESTED INTERRUPT

 GPT Nested interrupt functionality of TIMERS
 
 TEST DESCRIPTION:
 		This test code verifies the functionality of all the timers.
  GPTx module of C5517 DSP is used to Non Maskable Interrupt also. The normal TINT
  interrupts to the CPU to schedule periodic tasks or a delayed task.But the NMI which will
  have the higher priority above all interrupts and cannot be masked out.
  In this example TIMER0 is configured for producing NMI and TIMER2,TIMER for TINT.
  The TIMER2 is given a less count value than TIMER1 and TIMER0. The example demonstrates
  a condition in which TIMER2 ISR hits first, while inside ISR, NMI triggers and same time
  or in NMI ISR, TIMER1 TINT triggers. After executing NMI, it should return to TIMER2
  and then service TIMER1.
 
  In this example, the print logs shows the order in which ISR's executed.
 
  GPT can be configured with a counter and pre-scaler divider value. When the
  GPT is started counter will be decremented to 0. The rate at which this
  count happens can be controlled by the prescaler divider value. GPT clock
  is derived by dividing the CPU clock with the pre-scaler divider value.
 
  This test is having two parts. First part of the test verifies whether the
  GPT is decrementing the counter or not. GPT is configured with a count value
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
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5517.
  MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
 
  TEST PROCEDURE:
   1. Open the CCS and connect the target (C5517 EVM)
   2. Open the project "CSL_GPT0_Extclk_GPIO0_Example.pjt" and build it
   3. Load the program on to the target
   4. Set the PLL frequency to 100MHz
   5. Run the program and observe the test result
   
 
  TEST RESULT:
   All the CSL APIs should return success
   Order of ISR hit of Timers in print log should be correct.