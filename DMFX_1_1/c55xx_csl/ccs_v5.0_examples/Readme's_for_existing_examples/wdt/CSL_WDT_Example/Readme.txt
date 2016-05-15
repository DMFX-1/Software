  WDT functional layer sample source file
  
  WDT EXAMPLE -  CPU RESET TEST
 
  TEST DESCRIPTION:
  		This example code verifies the functionality of watchdog timer of the
  C5535/C5515/C5517 DSP. Watchdog timer functionality is to prevent the system from
  locking up if the software becomes trapped in loops with no controlled exit.
  To determine that the software is operating properly, the watchdog timer
  requires a special service sequence to be executed periodically.
  Without this periodic servicing, the watchdog timer counter reaches zero and
  times out. Consequently, an active low pulse will be asserted on the
  watchdog output that is connected to the local hardware reset. Thus, forcing
  a reset of the CPU.
 
  This test code configures the CSL WDT module and start the timer.
  Timer is serviced for 256 times to verify whether the timer is running
  properly. Each time the timer is serviced the timer counter should be reset
  and timer should re-start the counting. Timer count is read and displayed on
  the CCS "Stdout" window after servicing the counter.
  After servicing the counter for 256 times test code will stop servicing the
  timer and keeps reading and displaying the timer count. Timer count will
  go on decreasing and CPU will be reset when timer count reaches to zero.
  After CPU reset test code stops running and target will be disconnected.
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5535, C5515
  AND C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
 
  TEST PROCEDURE:
   1. Open the CCS and connect the target (C5535/C5515/C5517 EVM)
   2. Open the project "CSL_WDT_Example_Out.pjt" and build it
   3. Load the program on to the target
   4. Set the PLL frequency to 12.288MHz
   5. Run the program and observe the test result
   
  TEST RESULT:
    All the CSL APIs should return success
    Watchdog timer should keep running and reset the counter as long as
    timer is serviced by the test code
    Watchdog timer should decrement the counter and reset the CPU at
    count 0 after test code stops servicing the watchdog timer. Target
    should be disconnected at the End of the Test.