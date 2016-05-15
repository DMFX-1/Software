  RTC compensation test example source file
 
  RTC COMPENSATION TEST
 
  TEST DESCRIPTION:
  		This example code tests the RTC module operation with positive and
  negative compensation settings. The RTC module of C5535/C5515/C5517 DSP can
  compensate for oscillator drift due to temperature by adding or subtracting
  the value of the compensation registers (in milliseconds) to the 32KHz
  counter every hour. RTC module can be configured for positive or negative
  compensation using "rtc oscillator drift compensation register".
 
  When RTC is configured for positive compensation of value 'N', N number of
  milliseconds are added to the millisecond component of time. This positive
  drift in time will occur after change in hours component.After each change
  in hours component, N number of milliseconds are counted and millisecond
  count starts again from 0.
 
  NOTE: RTC Positive compensation does not work for the compensation values
  that are multiple of 10 on the chip C5535. RTC_setCompensation API will
  return error for such compensation values. This issue is not present on
  chip C5515.
 
  When RTC is configured for negative compensation of value 'N', N number of
  milliseconds are removed from the millisecond component of time.
  This negative drift in time will happen after change in hours component.
  After each change in hours component count of milliseconds starts from N
  instead of 0.
 
  This test code configures the RTC for positive compensation and then for
  negative compensation. Positive compensation setting is also verified with
  compensation value which is multiple of ten. After starting the RTC,
  time is read and displayed in the CCS "stdout" window at regular intervals
  of time. Manual inspection is required to observe the drift in RTC time
  after change in hours component. Time and Date will be displayed in the
  format HH:MM:SS:mmmm, DD-MM-YY respectively. 'm' in the time format
  represents millisecond component of the time.
 
  For each compensation setting this test code will start at time 12:59:59:xxxx
  and runs till 13:00:01:xxxx. Drift in time will occur after hours component
  changes to 13 from 12.
 
  
   Small test log of RTC time when configured for positive compensation of
   value 256 is given below. After a change in hours component, milliseconds
   are counted till 256 and count again starts from 0. msec count shown below
   is not exactly 256 and 0 due to delay in read operation.
   Time and Date is : 12:59:59:1020, 16-10-08
   Time and Date is : 13:00:00:0023, 16-10-08
   Time and Date is : 13:00:00:0048, 16-10-08
   Time and Date is : 13:00:00:0076, 16-10-08
   Time and Date is : 13:00:00:0102, 16-10-08
   Time and Date is : 13:00:00:0129, 16-10-08
   Time and Date is : 13:00:00:0156, 16-10-08
   Time and Date is : 13:00:00:0182, 16-10-08
   Time and Date is : 13:00:00:0209, 16-10-08
   Time and Date is : 13:00:00:0236, 16-10-08
   Time and Date is : 13:00:00:0006, 16-10-08   <== Positive Drift in Time
   Time and Date is : 13:00:00:0034, 16-10-08

   Small test log of RTC time when configured for negative compensation of
   value 256 is given below. After a change in hours component, milliseconds
   are counted from 256 instead of 0. msec count shown below
   is not exactly 256 due to delay in read operation.
   Time and Date is : 12:59:59:0989, 16-10-08
   Time and Date is : 12:59:59:1017, 16-10-08
   Time and Date is : 13:00:00:0276, 16-10-08   <== Negative Drift in Time
   Time and Date is : 13:00:00:0304, 16-10-08

   
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5535, C5515
  AND C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
 
  TEST PROCEDURE:
   1. Open the CCS and connect the target (C5535/C5515/C5517 EVM)
   2. Open the project "CSL_RTC_Compensation_Example_Out.pjt" and build it
   3. Load the program on to the target
   4. Set the PLL frequency to 12.288MHz
   5. Run the program and observe the test result
 
  TEST RESULT:
    All the CSL RTC APIs should return success
    A Positive drift in time(msec) should be observed after change in
    hours component of time during the positive compensation test
    A Negative drift in time(msec) should be observed after change in
    hours component of time during the negative compensation test
    RTC_setCompensation API should return failure when tested with a
    positive compensation value which is multiple of ten on chip C5515.
    It should run successfully on chip C5515.