  USB functional layer polled mode example source file
 
  USB POLL MODE TEST
 
  TEST DESCRIPTION:
  This test is to verify the operation of the CSL USB module. This test runs
  in poll mode. In the poll mode interrupt registers are read continuously
  in a while loop. If there is any interrupt observed it will be served and
  test continues to poll for the next interrupts.
 
  This USB test can be verified by a host USB tool which can send/receive
  data to/from the USB device. This tool should be installed on the host PC
  for evaluating the CSL USB end point examples.
  Please contact TI for more details on the USB host tool
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5535, C5515
  AND C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
 
  TEST PROCEDURE:
  1. Open the CCS and connect the target (C5535/C5515/C5517 EVM)
  2. Open, build and load the USB program to the target
  3. Set the PLL frequency to 100MHz
  4. Run the program loaded on to the target
  5. Run the USB host application. It displays following message


       DeviceAttach: received and accepted attach for vendor id 0x451, product id 0x901
		0, interface 0, device handle 0x00392AD8

		Main Menu (active Dev/Prod/Interface/Alt. Setting: 0x451/0x9010/0/0)
       ----------
       1. Display device configurations
       2. Change interface alternate setting
       3. Reset Pipe
       4. Read/Write from pipes
	   6. Selective Suspend
	   7. Refresh
       99. Exit
       Enter option:

 
  6. Numbers from 1 to 99 should be entered to execute the operation given
     against each number
 
  7. Choose the option 6 to change the transfer mode to single blocking
 
  8. For read and write operations, choose option 4 which displays below
      messages

	 Control pipe:
	   pipe num. 0x0: packet size 64, type Control, dir In & Out, interval 0 (ms)
	 Interface 0
	 Alternate Setting: 0
	   pipe num. 0x81: packet size 512, type Bulk, dir In, interval 0 (ms)
	   pipe num. 0x1: packet size 512, type Bulk, dir Out, interval 0 (ms)
	   pipe num. 0x82: packet size 512, type Bulk, dir In, interval 0 (ms)
	   pipe num. 0x2: packet size 512, type Bulk, dir Out, interval 0 (ms)
	   pipe num. 0x83: packet size 512, type Bulk, dir In, interval 0 (ms)
	   pipe num. 0x3: packet size 512, type Bulk, dir Out, interval 0 (ms)
	   pipe num. 0x84: packet size 512, type Bulk, dir In, interval 0 (ms)
	   pipe num. 0x4: packet size 512, type Bulk, dir Out, interval 0 (ms)

	 Read/Write from/to device's pipes using Streaming Data Transfers
	 ---------------------
	 1.  Read from pipe
	 2.  Write to pipe
	 3.  Listen to pipe (continuous read)
	 4.  Check streaming READ speed
	 5.  Check streaming WRITE speed
	 6.  Switch to Single Blocking Transfers mode
	 99. Main menu
	Enter option:
 
  9. All the pipe numbers (end points) available for data transfers with
     supported data direction (IN/OUT) will be listed as shown above.
     In the current version of CSL USB end point example, four end points are
     supported with each end point capable of transferring and receiving
     data
 
 10. For writing data from host to device, choose an end point number with
     'OUT' direction. Enter the data size to be transferred and enter the
     data bytes to be transferred. Perform at least one write operation to
     an end point before reading data from it.
     Please check below log messages for write operation
 

	 Enter option: 2

	 Please enter the pipe number (hex): 0x1
	 Please enter the size of the buffer (dec):  4
	 Please enter the input buffer (hex): 0xABCD1234
	 Transferred 4 bytes
 
 11. For reading data from device to host, choose an end point number with
     'IN' direction. Enter the data size to be received. Data size entered
     for reading should be same as the size of data written previously.
     Please check below log messages for read operation
 

	 Enter option: 1

	 Please enter the pipe number (hex): 0x81
	 Please enter the size of the buffer (dec):  4
	 Transferred 4 bytes
	 AB CD 12 34
 
 12. For verifying the USB operations write few bytes of data to an end point,
     read same number of bytes from that end point and compare the data read
     and data written. Data read should be same as data written for a given
     end point
 
 13. Data read/write operations for a specific end point are independent of
     other end points. Data can be written to any end point any number of
     times with out effecting data buffers of other end points.
 
 14. Limitations of the test
       This example and the host test code are implemented to show case how to
       configure and use the CSL USB module at end point level, but they are
       not intended to demo full-fledged USB application.
 
       For any endpoint, first operation should be write which can be followed
       by several read operations. Reading data from an end point without
       writing data will result in timeout error for USB host tool
 
       Data size read from an end point should always be equal to the size
       of data written previously to that end point.
 
       Read operation for an end point always returns data written previously
       to that end point
 
 
  TEST RESULT:
   USB device should be detected by the host PC and should be accessible
   through the USB host application
   Data written and data read should match in the host application