
How to test USB EP Poll example in CSL

This package is designed for testing the bootloading for C55xx USB EP <--> PC. 
We use USB USB EP1 OUT and EP2 IN.


1. Install the C55xx_USB_EP driver on the Host PC (32-bit)

1.1.	Unzip sprabd7b.zip on PC and navigate to C55xx_USB_EP_install.bat and invoke the same
(available @ sprabd7b\C55xx_USB_EP_installation_11.20\C55xx_USB_EP_installation_11.20\redist_32bit)



2. Ensure a usb cable is plugged into port J13 (C5517 USB) on-board C5517 EVM.

3. Ensure none of the other usb cables are connected to the PC.

4. Plug-in another usb cable into port J3 (EMU USB) on-board C5517 EVM.

5. Load and run the USB EP Poll example in CSL.

6. After the USB enumeration, then run c55xx_usb_ep_diag.exe on PC command prompt.

7. Follow the prompt.



Following message is displayed on the PC:
 @verbatim

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

   @endverbatim
 
 Numbers from 1 to 99 should be entered to execute the operation given
     against each number
 
 Choose the option 6 to change the transfer mode to single blocking
 
 For read and write operations, choose option 4 which displays below
     messages
 @verbatim
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
 @endverbatim
 
 All the pipe numbers (end points) available for data transfers with
     supported data direction (IN/OUT) will be listed as shown above.
     In the current version of CSL USB end point example, four end points are
     supported with each end point capable of transferring and receiving
     data
 
 For writing data from host to device, choose an end point number with
     'OUT' direction. Enter the data size to be transferred and enter the
     data bytes to be transferred. Perform at least one write operation to
     an end point before reading data from it.
     Please check below log messages for write operation
 
 @verbatim
	 Enter option: 2

	 Please enter the pipe number (hex): 0x1
	 Please enter the size of the buffer (dec):  4
	 Please enter the input buffer (hex): 0xABCD1234
	 Transferred 4 bytes
 @endverbatim
 
 For reading data from device to host, choose an end point number with
     'IN' direction. Enter the data size to be received. Data size entered
     for reading should be same as the size of data written previously.
     Please check below log messages for read operation
 
 @verbatim
	 Enter option: 1

	 Please enter the pipe number (hex): 0x81
	 Please enter the size of the buffer (dec):  4
	 Transferred 4 bytes
	 AB CD 12 34
 @endverbatim
 
 For verifying the USB operations write few bytes of data to an end point,
     read same number of bytes from that end point and compare the data read
     and data written. Data read should be same as data written for a given
     end point
 
 Data read/write operations for a specific end point are independent of
     other end points. Data can be written to any end point any number of
     times with out effecting data buffers of other end points.
 
 
  Limitations of the test
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
 

 