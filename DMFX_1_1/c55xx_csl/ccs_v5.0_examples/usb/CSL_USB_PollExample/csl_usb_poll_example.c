/* ============================================================================
 * Copyright (c) 2008-2012 Texas Instruments Incorporated.
 * Except for those rights granted to you in your license from TI, all rights
 * reserved.
 *
 * Software License Agreement
 * Texas Instruments (TI) is supplying this software for use solely and
 * exclusively on TI devices. The software is owned by TI and/or its suppliers,
 * and is protected under applicable patent and copyright laws.  You may not
 * combine this software with any open-source software if such combination would
 * cause this software to become subject to any of the license terms applicable
 * to such open source software.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
 * NO WARRANTIES APPLY TO THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY.
 * EXAMPLES OF EXCLUDED WARRANTIES ARE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE AND WARRANTIES OF NON-INFRINGEMENT,
 * BUT ALL OTHER WARRANTY EXCLUSIONS ALSO APPLY. FURTHERMORE, TI SHALL NOT,
 * UNDER ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, CONSEQUENTIAL
 * OR PUNITIVE DAMAGES, FOR ANY REASON WHATSOEVER.
 * ============================================================================
 */

/** @file csl_usb_poll_example.c
 *
 *  @brief USB functional layer polled mode example source file
 *
 * \page    page20  CSL USB EXAMPLE DOCUMENTATION
 *
 * \section USB1   USB EXAMPLE1 - POLL MODE TEST
 *
 * \subsection USB1x    TEST DESCRIPTION:
 * This test is to verify the operation of the CSL USB module. This test runs
 * in poll mode. In the poll mode interrupt registers are read continuously
 * in a while loop. If there is any interrupt observed it will be served and
 * test continues to poll for the next interrupts.
 *
 * This USB test can be verified by a host USB tool which can send/receive
 * data to/from the USB device. This tool should be installed on the host PC
 * for evaluating the CSL USB end point examples.
 * Please contact TI for more details on the USB host tool
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5535, C5515
 * AND C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 *
 * \subsection USB1y    TEST PROCEDURE:
 * @li Open the CCS and connect the target (C5535/C5515/C5517 EVM)
 * @li Open, build and load the USB program to the target
 * @li Set the PLL frequency to 100MHz
 * @li Run the program loaded on to the target
 * @li Run the USB host application. It displays following message
 * @verbatim

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
 *
 * @li Numbers from 1 to 99 should be entered to execute the operation given
 *     against each number
 *
 * @li Choose the option 6 to change the transfer mode to single blocking
 *
 * @li For read and write operations, choose option 4 which displays below
 *     messages
 * @verbatim
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
 * @endverbatim
 *
 * @li All the pipe numbers (end points) available for data transfers with
 *     supported data direction (IN/OUT) will be listed as shown above.
 *     In the current version of CSL USB end point example, four end points are
 *     supported with each end point capable of transferring and receiving
 *     data
 *
 * @li For writing data from host to device, choose an end point number with
 *     'OUT' direction. Enter the data size to be transferred and enter the
 *     data bytes to be transferred. Perform at least one write operation to
 *     an end point before reading data from it.
 *     Please check below log messages for write operation
 *
 * @verbatim
	 Enter option: 2

	 Please enter the pipe number (hex): 0x1
	 Please enter the size of the buffer (dec):  4
	 Please enter the input buffer (hex): 0xABCD1234
	 Transferred 4 bytes
 * @endverbatim
 *
 * @li For reading data from device to host, choose an end point number with
 *     'IN' direction. Enter the data size to be received. Data size entered
 *     for reading should be same as the size of data written previously.
 *     Please check below log messages for read operation
 *
 * @verbatim
	 Enter option: 1

	 Please enter the pipe number (hex): 0x81
	 Please enter the size of the buffer (dec):  4
	 Transferred 4 bytes
	 AB CD 12 34
 * @endverbatim
 *
 * @li For verifying the USB operations write few bytes of data to an end point,
 *     read same number of bytes from that end point and compare the data read
 *     and data written. Data read should be same as data written for a given
 *     end point
 *
 * @li Data read/write operations for a specific end point are independent of
 *     other end points. Data can be written to any end point any number of
 *     times with out effecting data buffers of other end points.
 *
 *  @li Repeat the test at the following PLL frequencies
 *      C5515: 60MHz and 100MHz
 *      C5517: 60MHz, 100MHz, 150MHz and 200MHz
 *      C5535 eZdsp: 60MHz and 100MHz
 *  @li Repeat the test in Release mode
 *
 *  @li Limitations of the test
 *      This example and the host test code are implemented to show case how to
 *      configure and use the CSL USB module at end point level, but they are
 *      not intended to demo full-fledged USB application.
 *
 *      For any endpoint, first operation should be write which can be followed
 *      by several read operations. Reading data from an end point without
 *      writing data will result in timeout error for USB host tool
 *
 *      Data size read from an end point should always be equal to the size
 *      of data written previously to that end point.
 *
 *      Read operation for an end point always returns data written previously
 *      to that end point
 *
 *
 * \subsection USB1z    TEST RESULT:
 * @li USB device should be detected by the host PC and should be accessible
 *     through the USB host application
 * @li Data written and data read should match in the host application
 *
 * ============================================================================
 */

/* ============================================================================
 * Revision History
 * ================
 * 24-Oct-2008 Created
 * 06-Aug-2010 Updated
 * 23-Jul-2012 Updated Documentation
 * 29-Mar-2013 Modified to support new USB host tool
 * 01-Apr-2013 Updated Documentation
 * ============================================================================
 */

#include <stdio.h>
#include "soc.h"
#include "csl_usb.h"
#include "csl_usbAux.h"
#include "csl_sar.h"
#include "csl_intc.h"
#include "csl_general.h"

#define CSL_USB_TEST_PASSED       (0)
#define CSL_USB_TEST_FAILED       (1)

#define CSL_USB_DATA_SIZE         (256)
#define CSL_USB_MAX_CURRENT       (50)
#define CSL_USB_WAKEUP_DELAY      (10)
//#define CSL_USB_SELFWAKEUP_TEST   (1)

/** Value of Keypad STOP button on C5515/C5517 EVM */
#define CSL_USB_SAR_STOP_KEY_VAL  (0x000)

extern CSL_UsbContext     gUsbContext;
extern CSL_UsbRegsOvly    usbRegisters;

/* SAR object structure */
CSL_SarHandleObj    SarObj;
/* SAR handle           */
CSL_SarHandleObj    *SarHandle;
/* SAR Read Buffer      */
Uint16              readBuffer;
Uint16              selfWakeupServiced = FALSE;

CSL_UsbDevHandle      hUsbDev;
CSL_UsbEpHandle       hEp[CSL_USB_ENDPOINT_COUNT+2];
CSL_UsbConfig         usbConfig;
CSL_UsbSetupStruct    usbSetup;

Uint16    *deviceDescPtr;
Uint16    *cfgDescPtr;
Uint16    *strDescPtr;
Uint16    *deviceQualDescPtr;
Uint16    *OtherSpeedcfgDescPtr;
Uint16    devAddr;
Uint32    selfwakeupDelay = 1000000;
volatile Bool      stopRunning = FALSE;
Bool	  usbDevDisconnect = TRUE;
Uint16    outEp1;
Uint16    inEp1;
Uint16    outEp2;
Uint16    inEp2;
Uint16    outEp3;
Uint16    inEp3;
Uint16    outEp4;
Uint16    inEp4;
Uint16    usbDataBuffer1[CSL_USB_DATA_SIZE];
Uint16    usbDataBuffer2[CSL_USB_DATA_SIZE];
Uint16    usbDataBuffer3[CSL_USB_DATA_SIZE];
Uint16    usbDataBuffer4[CSL_USB_DATA_SIZE];

Uint16    usb_income_num_bytes_ep1 = 0;
Uint16    usb_income_num_bytes_ep2 = 0;
Uint16    usb_income_num_bytes_ep3 = 0;
Uint16    usb_income_num_bytes_ep4 = 0;

Uint16  gConfiguration = 0;

Bool      sentLongEp0Pkt = 0;

Uint16    deviceDesc[9] = {0x0112, 0x0200, 0x0000, 0x4000, 0x0451,
                           0x9010, 0x0100, 0x0201, 0x0103};

Uint16    deviceQualDesc[] = {0x060A, 0x0200, 0x0000, 0x4000, 0x0002};

Uint16    cfgDesc[40] = {0x0209, 0x004A, 0x0101, 0xC001, 0x0928,	// configure descriptor
                         0x0004, 0x0800, 0x0000, 0x0000,		 	// interface descriptor
                         0x0507, 0x0281, 0x0200, 0x0700,			//endpoint 1 IN descriptor
                         0x0105, 0x0002, 0x0002, 					//endpoint 1 OUT descriptor
                         0x0507, 0x0282, 0x0200, 0x0700,			//endpoint 2 IN descriptor
                         0x0205, 0x0002, 0x0002, 					//endpoint 2 OUT descriptor
                         0x0507, 0x0283, 0x0200, 0x0700, 			//endpoint 3 IN descriptor
                         0x0305, 0x0002, 0x0002,					//endpoint 3 OUT descriptor
                         0x0507, 0x0284, 0x0200, 0x0700, 			//endpoint 4 IN descriptor
                         0x0405, 0x0002, 0x0002						//endpoint 4 OUT descriptor
                         };

Uint16    OtherSpeedcfgDesc[40] = {0x0709, 0x004A, 0x0201, 0xC001, 0x0928,    	 // Configure descriptor
                                 0x0004, 0x0800, 0x0000, 0x0000,          		 // Interface descriptor
		                         0x0507, 0x0281, 0x0040, 0x0700,			//endpoint 1 IN descriptor
		                         0x0105, 0x4002, 0x0000, 					//endpoint 1 OUT descriptor
		                         0x0507, 0x0282, 0x0040, 0x0700,			//endpoint 2 IN descriptor
		                         0x0205, 0x4002, 0x0000, 					//endpoint 2 OUT descriptor
		                         0x0507, 0x0283, 0x0040, 0x0700, 			//endpoint 3 IN descriptor
		                         0x0305, 0x4002, 0x0000,					//endpoint 3 OUT descriptor
		                         0x0507, 0x0284, 0x0040, 0x0700, 			//endpoint 4 IN descriptor
		                         0x0405, 0x4002, 0x0000						//endpoint 4 OUT descriptor
                                };

Uint16    strDesc[4][20] = {
							// string 0 English-USA
							{0x0304, 0x0409},
							// string 1 "Texas Instruments"
						    {0x0324, 0x0045, 0x0054, 0x0041, 0x0058, 0x0020, 0x0053, 0x004E, 0x0049,
						    0x0054, 0x0053, 0x0055, 0x0052, 0x0045, 0x004D, 0x0054, 0x004E, 0x0053},
#if (defined(CHIP_C5505_C5515) || defined(CHIP_C5504_C5514))
							// string 2 "C5515"
						    {0x030C, 0x0043, 0x0035, 0x0035, 0x0031, 0x0035},
#else
							// string 2 "C5505"
						    {0x030C, 0x0043, 0x0035, 0x0035, 0x0030, 0x0035},
#endif
							// string 3 "00001"
						    {0x030C, 0x0030, 0x0030, 0x0030, 0x0030, 0x0031}
						    };

extern void VECSTART(void);
CSL_Status CSL_configSarForSelfWakeup(void);
interrupt void sar_isr(void);
CSL_Status CSL_usbPollTest(void);
void CSL_suspendCallBack(CSL_Status    status);
void CSL_selfWakeupCallBack(CSL_Status    status);
CSL_Status CSL_startTransferCallback(void    *vpContext,
                                     void    *vpeps);
CSL_Status CSL_completeTransferCallback(void    *vpContext,
                                        void    *vpeps);
static void USB_delay(Uint32    mSecs);

Uint16 getPacketSize(CSL_UsbEpNum endPt)
{
	Uint16 packetSize;

	switch (endPt)
	{
		case CSL_USB_IN_EP1:
		case CSL_USB_OUT_EP1:
			packetSize = CSL_USB_EP1_PACKET_SIZE_FS;
			break;

		case CSL_USB_IN_EP2:
		case CSL_USB_OUT_EP2:
			packetSize = CSL_USB_EP2_PACKET_SIZE_FS;
			break;

		case CSL_USB_IN_EP3:
		case CSL_USB_OUT_EP3:
			packetSize = CSL_USB_EP3_PACKET_SIZE_FS;
			break;

		case CSL_USB_IN_EP4:
		case CSL_USB_OUT_EP4:
			packetSize = CSL_USB_EP4_PACKET_SIZE_FS;
			break;
	}

	return (packetSize);
}

int getEndPoint(int endPt)
{
	int retEndPt;

	switch (endPt)
	{
		case CSL_USB_IN_EP1:
		case CSL_USB_OUT_EP1:
			retEndPt = CSL_USB_EP1;
			break;

		case CSL_USB_IN_EP2:
		case CSL_USB_OUT_EP2:
			retEndPt = CSL_USB_EP2;
			break;

		case CSL_USB_IN_EP3:
		case CSL_USB_OUT_EP3:
			retEndPt = CSL_USB_EP3;
			break;

		case CSL_USB_IN_EP4:
		case CSL_USB_OUT_EP4:
			retEndPt = CSL_USB_EP4;
			break;
	}

	return (retEndPt);
}

/**
 *  \brief  main function
 *
 *  \param  None
 *
 *  \return None
 */
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 1 --
   /////  Define PaSs_StAtE variable for catching errors as program executes.
   /////  Define PaSs flag for holding final pass/fail result at program completion.
        volatile Int16 PaSs_StAtE = 0x0001; // Init to 1. Reset to 0 at any monitored execution error.
        volatile Int16 PaSs = 0x0000; // Init to 0.  Updated later with PaSs_StAtE when and if
   /////                                  program flow reaches expected exit point(s).
   /////
void main(void)
{
	CSL_Status    result;

	printf("\nUSB Polled Mode Test!!\n");

	result = CSL_usbPollTest();

	if(result == CSL_USB_TEST_PASSED)
	{
		printf("\nUSB Test Failed!!\n");
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 2 --
   /////  Reseting PaSs_StAtE to 0 if error detected here.
        PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
   /////
	}
	else
	{
		printf("\nUSB Test Passed!!\n");
	}
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 3 --
   /////  At program exit, copy "PaSs_StAtE" into "PaSs".
        PaSs = PaSs_StAtE; //If flow gets here, override PaSs' initial 0 with
   /////                   // pass/fail value determined during program execution.
   /////  Note:  Program should next exit to C$$EXIT and halt, where DSS, under
   /////   control of a host PC script, will read and record the PaSs' value.
   /////
}

/**
 *  \brief  Tests USB polled mode operation
 *
 *  \param  none
 *
 *  \return Test result
 */
CSL_Status CSL_usbPollTest(void)
{
	CSL_Status        status;
	CSL_Status        result;
	pUsbContext       pContext;
	CSL_UsbEpHandle   hEPx;
	CSL_UsbBoolean    txRxStatus;
	Uint16            bytesRem;
	Uint16            eventMask;
	Uint16            saveIndex;
	Uint16            endpt;
	CSL_UsbEpHandle   tempEpH;
	CSL_UsbEpConfig   epCfg;
	Uint16            tempLen;

	pContext = &gUsbContext;
	result   = CSL_USB_TEST_FAILED;
	outEp1   = CSL_USB_OUT_EP1;
	inEp1    = CSL_USB_IN_EP1;
	outEp2   = CSL_USB_OUT_EP2;
	inEp2    = CSL_USB_IN_EP2;
	outEp3   = CSL_USB_OUT_EP3;
	inEp3    = CSL_USB_IN_EP3;
	outEp4   = CSL_USB_OUT_EP4;
	inEp4    = CSL_USB_IN_EP4;

	*(ioport volatile int *)0x1c02 = 0x0;
	*(ioport volatile int *)0x1c03 = 0x0;

	usbConfig.opMode             = CSL_USB_OPMODE_POLLED;
	usbConfig.maxCurrent         = CSL_USB_MAX_CURRENT;
	usbConfig.appSuspendCallBack = (CSL_USB_APP_CALLBACK)CSL_suspendCallBack;
	usbConfig.appWakeupCallBack  = (CSL_USB_APP_CALLBACK)CSL_selfWakeupCallBack;
	usbConfig.startTransferCallback  = CSL_startTransferCallback;
	usbConfig.completeTransferCallback = CSL_completeTransferCallback;

#ifdef CSL_USB_SELFWAKEUP_TEST

	/* Configure SAR to generate event for self wakeup */
	/* NOTE: Self Wakeup is interfaced to the STOP key of the EVM keypad */
	status = CSL_configSarForSelfWakeup();
	if(status != CSL_SOK)
	{
		printf("\nSAR Configuration failed\n");
		return(result);
	}

#endif  // CSL_USB_SELFWAKEUP_TEST

	/* Initialize the USB module */
	USB_init();

	/* Initialize the USB module */
	hUsbDev = USB_open(CSL_USB0);
	if(hUsbDev == NULL)
	{
		printf("\nUSB open failed\n");
		return(result);
	}

	/* Initialize the USB module */
	status = USB_config(hUsbDev, &usbConfig);
	if(status != CSL_SOK)
	{
		printf("\nUSB config failed\n");
		return(result);
	}

	/* Reset the USB device */
	status = USB_resetDev(hUsbDev);
	if(status != CSL_SOK)
	{
		printf("\nUSB Reset failed\n");
		return(result);
	}

	hEp[0] = USB_requestEndpt(hUsbDev, CSL_USB_OUT_EP0, &status);
	if(status != CSL_SOK)
	{
		printf("\nUSB request ep failed\n");
		return(result);
	}

	eventMask = (CSL_USB_EVENT_RESET | CSL_USB_EVENT_SETUP |
				 CSL_USB_EVENT_SUSPEND | CSL_USB_EVENT_RESUME |
				 CSL_USB_EVENT_RESET | CSL_USB_EVENT_EOT);

	epCfg.xferType = CSL_USB_CTRL;
    epCfg.eventMask = eventMask;
    epCfg.maxPktSize = CSL_USB_EP0_PACKET_SIZE;
    epCfg.fxn = NULL;

	/* Initialize the Control Endpoint OUT 0 */
    status = USB_configEndpt(hEp[0], &epCfg);
	if(status != CSL_SOK)
	{
		printf("\nUSB_configEndpt failed\n");
		return(result);
	}

	hEp[1] = USB_requestEndpt(hUsbDev, CSL_USB_IN_EP0, &status);
	if(status != CSL_SOK)
	{
		printf("\nUSB request ep failed\n");
		return(result);
	}

	epCfg.xferType = CSL_USB_CTRL;
    epCfg.eventMask = CSL_USB_EVENT_EOT;
    epCfg.maxPktSize = CSL_USB_EP0_PACKET_SIZE;
    epCfg.fxn = NULL;

	/* Initialize the Control Endpoint IN 0 */
    status = USB_configEndpt(hEp[1], &epCfg);
	if(status != CSL_SOK)
	{
		printf("\nUSB_configEndpt failed\n");
		return(result);
	}

	hEp[3] = USB_requestEndpt(hUsbDev, (CSL_UsbEpNum)outEp1, &status);
	if(status != CSL_SOK)
	{
		printf("\nUSB request ep failed\n");
		return(result);
	}

	eventMask = (CSL_USB_EVENT_RESET | CSL_USB_EVENT_EOT);

	epCfg.xferType = CSL_USB_BULK;
    epCfg.eventMask = eventMask;
    epCfg.maxPktSize = getPacketSize((CSL_UsbEpNum)outEp1);
    epCfg.fxn = NULL;

    status = USB_configEndpt(hEp[3], &epCfg);
	if(status != CSL_SOK)
	{
		printf("\nUSB_configEndpt failed\n");
		return(result);
	}

	hEp[2] = USB_requestEndpt(hUsbDev, (CSL_UsbEpNum)inEp1, &status);
	if(status != CSL_SOK)
	{
		printf("\nUSB request ep failed\n");
		return(result);
	}

	eventMask = CSL_USB_EVENT_EOT;

	epCfg.xferType = CSL_USB_BULK;
    epCfg.eventMask = eventMask;
    epCfg.maxPktSize = getPacketSize((CSL_UsbEpNum)inEp1);
    epCfg.fxn = NULL;

    status = USB_configEndpt(hEp[2], &epCfg);
	if(status != CSL_SOK)
	{
		printf("\nUSB_configEndpt failed\n");
		return(result);
	}

	hEp[5] = USB_requestEndpt(hUsbDev, (CSL_UsbEpNum)outEp2, &status);
	if(status != CSL_SOK)
	{
		printf("\nUSB request ep failed\n");
		return(result);
	}

	eventMask = (CSL_USB_EVENT_RESET | CSL_USB_EVENT_EOT);

	epCfg.xferType = CSL_USB_BULK;
    epCfg.eventMask = eventMask;
    epCfg.maxPktSize = getPacketSize((CSL_UsbEpNum)outEp2);
    epCfg.fxn = NULL;

    status = USB_configEndpt(hEp[5], &epCfg);
	if(status != CSL_SOK)
	{
		printf("\nUSB_configEndpt failed\n");
		return(result);
	}

	hEp[4] = USB_requestEndpt(hUsbDev, (CSL_UsbEpNum)inEp2, &status);
	if(status != CSL_SOK)
	{
		printf("\nUSB request ep failed\n");
		return(result);
	}

	eventMask = CSL_USB_EVENT_EOT;

	epCfg.xferType = CSL_USB_BULK;
    epCfg.eventMask = eventMask;
    epCfg.maxPktSize = getPacketSize((CSL_UsbEpNum)inEp2);
    epCfg.fxn = NULL;

    status = USB_configEndpt(hEp[4], &epCfg);
	if(status != CSL_SOK)
	{
		printf("\nUSB_configEndpt failed\n");
		return(result);
	}

	hEp[7] = USB_requestEndpt(hUsbDev, (CSL_UsbEpNum)outEp3, &status);
	if(status != CSL_SOK)
	{
		printf("\nUSB request ep failed\n");
		return(result);
	}

	eventMask = (CSL_USB_EVENT_RESET | CSL_USB_EVENT_EOT);

	epCfg.xferType = CSL_USB_BULK;
    epCfg.eventMask = eventMask;
    epCfg.maxPktSize = getPacketSize((CSL_UsbEpNum)outEp3);
    epCfg.fxn = NULL;

    status = USB_configEndpt(hEp[7], &epCfg);
	if(status != CSL_SOK)
	{
		printf("\nUSB_configEndpt failed\n");
		return(result);
	}

	hEp[6] = USB_requestEndpt(hUsbDev, (CSL_UsbEpNum)inEp3, &status);
	if(status != CSL_SOK)
	{
		printf("\nUSB request ep failed\n");
		return(result);
	}

	eventMask = CSL_USB_EVENT_EOT;

	epCfg.xferType = CSL_USB_BULK;
    epCfg.eventMask = eventMask;
    epCfg.maxPktSize = getPacketSize((CSL_UsbEpNum)inEp3);
    epCfg.fxn = NULL;

    status = USB_configEndpt(hEp[6], &epCfg);
	if(status != CSL_SOK)
	{
		printf("\nUSB_configEndpt failed\n");
		return(result);
	}

	hEp[9] = USB_requestEndpt(hUsbDev, (CSL_UsbEpNum)outEp4, &status);
	if(status != CSL_SOK)
	{
		printf("\nUSB request ep failed\n");
		return(result);
	}

	eventMask = (CSL_USB_EVENT_RESET | CSL_USB_EVENT_EOT);

	epCfg.xferType = CSL_USB_BULK;
    epCfg.eventMask = eventMask;
    epCfg.maxPktSize = getPacketSize((CSL_UsbEpNum)outEp4);
    epCfg.fxn = NULL;

    status = USB_configEndpt(hEp[9], &epCfg);
	if(status != CSL_SOK)
	{
		printf("\nUSB_configEndpt failed\n");
		return(result);
	}

	hEp[8] = USB_requestEndpt(hUsbDev, (CSL_UsbEpNum)inEp4, &status);
	if(status != CSL_SOK)
	{
		printf("\nUSB request ep failed\n");
		return(result);
	}

	eventMask = CSL_USB_EVENT_EOT;

	epCfg.xferType = CSL_USB_BULK;
    epCfg.eventMask = eventMask;
    epCfg.maxPktSize = getPacketSize((CSL_UsbEpNum)inEp4);
    epCfg.fxn = NULL;

    status = USB_configEndpt(hEp[8], &epCfg);
	if(status != CSL_SOK)
	{
		printf("\nUSB_configEndpt failed\n");
		return(result);
	}

	/* Set the parameters */
	status = USB_setParams(hUsbDev, hEp, FALSE);
	if(status != CSL_SOK)
	{
		printf("\nUSB Set params failed\n");
		return(result);
	}

	/* Connect the USB device */
	status = USB_connectDev(hUsbDev);
	if(status != CSL_SOK)
	{
		printf("\nUSB Connect failed\n");
		return(result);
	}

	deviceDescPtr = (Uint16 *)deviceDesc;
	cfgDescPtr    = (Uint16 *)cfgDesc;
	strDescPtr    = (Uint16 *)strDesc;
    deviceQualDescPtr = (Uint16 *)deviceQualDesc;
    OtherSpeedcfgDescPtr = (Uint16 *)OtherSpeedcfgDesc;
	selfWakeupServiced = TRUE;

	while(stopRunning != TRUE)
	{
    	/* Read the masked interrupt status register */
		pContext->dwIntSourceL = usbRegisters->INTMASKEDR1;
		pContext->dwIntSourceH = usbRegisters->INTMASKEDR2;

		/* Clear the interrupts */
		if(pContext->dwIntSourceL != FALSE)
		{
			usbRegisters->INTCLRR1 = pContext->dwIntSourceL;
		}

		if(pContext->dwIntSourceH != FALSE)
		{
			usbRegisters->INTCLRR2 = pContext->dwIntSourceH;
		}

		/* Reset interrupt */
		if(pContext->dwIntSourceH & CSL_USB_GBL_INT_RESET)
		{
			saveIndex = usbRegisters->INDEX_TESTMODE;

			for(endpt = CSL_USB_EP1; endpt <= CSL_USB_EP4; endpt++)
			{
				CSL_FINS(usbRegisters->INDEX_TESTMODE,
						 USB_INDEX_TESTMODE_EPSEL, endpt);

				CSL_FINS(usbRegisters->PERI_RXCSR_INDX,
						 USB_PERI_RXCSR_INDX_DISNYET, TRUE);
			}

			usbRegisters->INDEX_TESTMODE = saveIndex;
		}

		/* Resume interrupt */
		if(pContext->dwIntSourceH & CSL_USB_GBL_INT_RESUME)
		{
			USB_setRemoteWakeup(hUsbDev, CSL_USB_TRUE);
			status = USB_issueRemoteWakeup(hUsbDev, TRUE);
			/* Give 10 msecs delay before resetting resume bit */
			USB_delay(CSL_USB_WAKEUP_DELAY);
			status = USB_issueRemoteWakeup(hUsbDev, FALSE);
			if(status != CSL_SOK)
			{
				printf("\nUSB Resume failed\n");
		   /////INSTRUMENTATION FOR BATCH TESTING -- Part 2 --
		   /////  Reseting PaSs_StAtE to 0 if error detected here.
		        PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
   		   /////
			}
		}

		/* Check End point0 interrupts */
		if(pContext->dwIntSourceL & CSL_USB_TX_RX_INT_EP0)
		{
			if(sentLongEp0Pkt == 1)
			{
				USB_processEP0In(pContext);
			}
			else
			{

				saveIndex = usbRegisters->INDEX_TESTMODE;
					CSL_FINS(usbRegisters->INDEX_TESTMODE,
							 USB_INDEX_TESTMODE_EPSEL, CSL_USB_EP0);

				USB_getSetupPacket(hUsbDev, &usbSetup, TRUE);

				if((usbRegisters->PERI_CSR0_INDX & CSL_USB_PERI_CSR0_INDX_RXPKTRDY_MASK)
						== CSL_USB_PERI_CSR0_INDX_RXPKTRDY_MASK)
				{
					/* Service the RXPKTRDY after reading the FIFO */
					CSL_FINS(usbRegisters->PERI_CSR0_INDX,
					         USB_PERI_CSR0_INDX_SERV_RXPKTRDY, TRUE);

					/* GET DESCRIPTOR Req */
					switch(usbSetup.bRequest)
					{
						/* zero data */
						case CSL_USB_SET_FEATURE:
							switch(usbSetup.wValue)
							{
								case CSL_USB_FEATURE_ENDPOINT_STALL:
									/* updated set and clear endpoint stall
									 * to work with logical endpoint num
									 */
									endpt = (usbSetup.wIndex) & 0xFF;
									hEPx = USB_epNumToHandle(hUsbDev, endpt);
									if(!(USB_getEndptStall(hEPx, &status)))
									{
										USB_stallEndpt(hEPx);
									}
									break;

								case CSL_USB_FEATURE_REMOTE_WAKEUP:
									if(!(USB_getRemoteWakeupStat(hUsbDev)))
									{
										USB_setRemoteWakeup(hUsbDev, CSL_USB_TRUE);
									}
									break;

								default:
									break;
							}

							usbRegisters->PERI_CSR0_INDX |= (CSL_USB_PERI_CSR0_INDX_SERV_RXPKTRDY_MASK |
															 CSL_USB_PERI_CSR0_INDX_DATAEND_MASK);
							break;

						case CSL_USB_CLEAR_FEATURE:
							switch(usbSetup.wValue)
							{
								case CSL_USB_FEATURE_ENDPOINT_STALL:
									endpt = (usbSetup.wIndex) & 0xFF;
									hEPx = USB_epNumToHandle(hUsbDev, endpt);
									if(USB_getEndptStall(hEPx, &status))
									{
										USB_clearEndptStall(hEPx);
									}
									break;

								case CSL_USB_FEATURE_REMOTE_WAKEUP:
									if(USB_getRemoteWakeupStat(hUsbDev))
									{
										USB_setRemoteWakeup(hUsbDev,
										                    CSL_USB_FALSE);
									}
									break;

								default:
									 break;
							}
							break;

						case CSL_USB_SET_CONFIGURATION :
						case CSL_USB_SET_INTERFACE:
							endpt = (usbSetup.wIndex) & 0xFF;
							hEPx = USB_epNumToHandle(hUsbDev, endpt);
							USB_postTransaction(hEPx, 0, NULL,
							                    CSL_USB_IOFLAG_NONE);

							/* DataEnd + ServicedRxPktRdy */
							usbRegisters->PERI_CSR0_INDX |= (CSL_USB_PERI_CSR0_INDX_SERV_RXPKTRDY_MASK |
															 CSL_USB_PERI_CSR0_INDX_DATAEND_MASK);

							gConfiguration = usbSetup.wValue;
							break;

						case CSL_USB_GET_CONFIGURATION :
						case CSL_USB_GET_INTERFACE:
							usbDataBuffer1[0] = gConfiguration;

							status = USB_postTransaction(hEp[1],
							                 1, usbDataBuffer1,
							                 CSL_USB_IN_TRANSFER);
							break;

						case CSL_USB_GET_DESCRIPTOR :

							switch(usbSetup.wValue >> 8)
							{
								case CSL_USB_DEVICE_DESCRIPTOR_TYPE:
									deviceDescPtr = (Uint16 *)deviceDesc;
									status = USB_postTransaction(hEp[1],
									                 deviceDesc[0]&0xFF, deviceDescPtr,
									                 CSL_USB_IN_TRANSFER);
									break;

						    	case CSL_USB_OTHERSPEED_CFG_DESCRIPTOR_TYPE:
									if(usbSetup.wLength == 0x0009)
									{
									    OtherSpeedcfgDescPtr = OtherSpeedcfgDesc;
										status = USB_postTransaction(hEp[1],
										                 9, OtherSpeedcfgDescPtr,
										                 CSL_USB_IN_TRANSFER);
									}
									else
									{
										if((OtherSpeedcfgDescPtr[1] & 0xFF) > 0x40)
										{
											sentLongEp0Pkt = 1;
										}

										OtherSpeedcfgDescPtr = OtherSpeedcfgDesc;
										status = USB_postTransaction(hEp[1],
										                 OtherSpeedcfgDesc[1], OtherSpeedcfgDescPtr,
										                 CSL_USB_IN_TRANSFER);
									}
						         	break;

								case CSL_USB_CONFIGURATION_DESCRIPTOR_TYPE:
									if(usbSetup.wLength == 0x0009)
									{
										cfgDescPtr = cfgDesc;
										status = USB_postTransaction(hEp[1],
										                 9, cfgDescPtr,
										                 CSL_USB_IN_TRANSFER);
									}
									else
									{
										if((cfgDesc[1] & 0xFF) > 0x40)
										{
											sentLongEp0Pkt = 1;
										}

										cfgDescPtr = cfgDesc;
										status = USB_postTransaction(hEp[1],
														 cfgDesc[1], cfgDescPtr,
														 CSL_USB_IN_TRANSFER);
									}

									break;

								case CSL_USB_STRING_DESCRIPTOR_TYPE:
									if((usbSetup.wValue & 0xFF) == 0x00)
									{
										strDescPtr = (Uint16 *)strDesc[0];
										status = USB_postTransaction(hEp[1],
										                 strDesc[0][0]&0xFF, strDescPtr,
										                 CSL_USB_IN_TRANSFER);
									}
									if((usbSetup.wValue & 0xFF) == 0x01)
									{
										strDescPtr = (Uint16 *)strDesc[1];
										status = USB_postTransaction(hEp[1],
										                 strDesc[1][0]&0xFF, strDescPtr,
										                 CSL_USB_IN_TRANSFER);
									}
									if((usbSetup.wValue & 0xFF) == 0x02)
									{
										strDescPtr = (Uint16 *)strDesc[2];
										status = USB_postTransaction(hEp[1],
										                 strDesc[2][0]&0xFF, strDescPtr,
										                 CSL_USB_IN_TRANSFER);
									}
									if((usbSetup.wValue & 0xFF) == 0x03)
									{
										strDescPtr = (Uint16 *)strDesc[3];
										status = USB_postTransaction(hEp[1],
										                 strDesc[3][0]&0xFF, strDescPtr,
										                 CSL_USB_IN_TRANSFER);
									}
									break;

								case CSL_USB_DEVICE_QUAL_DESCRIPTOR_TYPE:

								    deviceQualDescPtr = deviceQualDesc;

						    	    tempLen = deviceQualDesc[0]&0xFF;
						    	    /* select the smaller of two  */
						            tempLen = (tempLen < usbSetup.wLength) ? tempLen : usbSetup.wLength;

									status = USB_postTransaction(hEp[1],
									                 tempLen, deviceQualDescPtr,
									                 CSL_USB_IN_TRANSFER);
									break;

								default:
                                    status = USB_stallEndpt(hEp[0]);

									break;
							}

							deviceDescPtr        = (Uint16 *)deviceDesc;
							cfgDescPtr           = (Uint16 *)cfgDesc;
							strDescPtr           = (Uint16 *)strDesc[0];
                            deviceQualDescPtr    = (Uint16 *)deviceQualDesc;
                            OtherSpeedcfgDescPtr = (Uint16 *)OtherSpeedcfgDesc;

							if(sentLongEp0Pkt != 1)
							{
							    usbRegisters->PERI_CSR0_INDX |= (CSL_USB_PERI_CSR0_INDX_TXPKTRDY_MASK |
															     CSL_USB_PERI_CSR0_INDX_DATAEND_MASK);
							}

							break;

						case CSL_USB_GET_STATUS:

							switch(usbSetup.bmRequestType)
							{
								/* Device Status to be returned */
								case CSL_USB_REQUEST_TYPE_DEVICE_STATUS:
									// return the remote wake up status and the self power status
									usbDataBuffer1[0] =
									(((Uint16)USB_getRemoteWakeupStat(hUsbDev))<<1)|((cfgDescPtr[3]>>14)&0x0001);
									USB_postTransaction(hEp[1], 2, usbDataBuffer1,
									                    CSL_USB_IOFLAG_NONE);
									break;

								/* Interface status is to be returned */
								case CSL_USB_REQUEST_TYPE_INTERFACE_STATUS:
									// return 0x0000 (reserved for future use)
									usbDataBuffer1[0] = 0x0000;
									USB_postTransaction(hEp[1], 2, usbDataBuffer1,
									                    CSL_USB_IOFLAG_NONE);
									break;

								/* Endpoint status to be returned */
								case CSL_USB_REQUEST_TYPE_ENDPOINT_STATUS:
									// get endpoint from setup packet
									endpt = (usbSetup.wIndex) & 0xFF;
									// get the endpoint handle
									hEPx   =  USB_epNumToHandle(hUsbDev, endpt);
									// return the stall status
									usbDataBuffer1[0] = (Uint16)USB_getEndptStall(hEPx, &status);
									USB_postTransaction(hEp[1], 2, usbDataBuffer1, CSL_USB_IOFLAG_NONE);
									break;

								default:
									status = USB_stallEndpt(hEp[0]);

									break;
							}

							break;

						case CSL_USB_SET_ADDRESS :
							devAddr = usbSetup.wValue;

							usbRegisters->PERI_CSR0_INDX |= (CSL_USB_PERI_CSR0_INDX_SERV_RXPKTRDY_MASK |
															 CSL_USB_PERI_CSR0_INDX_DATAEND_MASK);
							break;

						default:
							status = USB_stallEndpt(hEp[0]);

							break;
					}
				}
				else
				{
					if(usbSetup.bRequest == 0x05)
					{
						USB_setDevAddr(hUsbDev, devAddr);
					}
				}

				usbRegisters->INDEX_TESTMODE  = saveIndex;
			}
		}

		/* Check Data Out Ready */
		if(pContext->dwIntSourceL & CSL_USB_RX_INT_EP1)
		{
			// read the RX packet size
			// validate the RX packet first
			if (USB_isValidDataInFifoOut(&pContext->pEpStatus[getEndPoint(outEp1)]))
			{
				// get the EP1 handle
				tempEpH = USB_epNumToHandle(hUsbDev, getEndPoint(outEp1));
				usb_income_num_bytes_ep1 = USB_getDataCountReadFromFifo(tempEpH);
			}

			status = USB_postTransaction(hEp[3], usb_income_num_bytes_ep1,
			                             usbDataBuffer1, CSL_USB_OUT_TRANSFER);
			if(status != CSL_SOK)
			{
				printf("\nUSB Transaction failed\n");
				return(result);
			}

			/* Check the transaction status */
			txRxStatus = USB_isTransactionDone(hEp[3], &status);
			if(txRxStatus != CSL_USB_TRUE)
			{
				bytesRem = USB_bytesRemaining(hEp[3], &status);
				if(bytesRem)
				{
					printf("\nBytes remaining: %d\n", bytesRem);
				}
			}

			/* This is to initiate filling the Tx FIFO with data for next read operation */
			pContext->dwIntSourceL |= CSL_USB_TX_INT_EP1;
		}


		if(pContext->dwIntSourceL & CSL_USB_RX_INT_EP2)
		{
			// read the RX packet size
			// validate the RX packet first
			if (USB_isValidDataInFifoOut(&pContext->pEpStatus[getEndPoint(outEp2)]))
			{
				// get the EP2 handle
				tempEpH = USB_epNumToHandle(hUsbDev, getEndPoint(outEp2));
				usb_income_num_bytes_ep2 = USB_getDataCountReadFromFifo(tempEpH);
			}

			status = USB_postTransaction(hEp[5], usb_income_num_bytes_ep2,
			                             usbDataBuffer2, CSL_USB_OUT_TRANSFER);
			if(status != CSL_SOK)
			{
				printf("\nUSB Transaction failed\n");
				return(result);
			}

			/* Check the transaction status */
			txRxStatus = USB_isTransactionDone(hEp[5], &status);
			if(txRxStatus != CSL_USB_TRUE)
			{
				bytesRem = USB_bytesRemaining(hEp[5], &status);
				if(bytesRem)
				{
					printf("\nBytes remaining: %d\n", bytesRem);
				}
			}

			/* This is to initiate filling the Tx FIFO with data for next read operation */
			pContext->dwIntSourceL |= CSL_USB_TX_INT_EP2;
		}


		if(pContext->dwIntSourceL & CSL_USB_RX_INT_EP3)
		{
			// read the RX packet size
			// validate the RX packet first
			if (USB_isValidDataInFifoOut(&pContext->pEpStatus[getEndPoint(outEp3)]))
			{
				// get the EP3 handle
				tempEpH = USB_epNumToHandle(hUsbDev, getEndPoint(outEp3));
				usb_income_num_bytes_ep3 = USB_getDataCountReadFromFifo(tempEpH);
			}

			status = USB_postTransaction(hEp[7], usb_income_num_bytes_ep3,
			                             usbDataBuffer3, CSL_USB_OUT_TRANSFER);
			if(status != CSL_SOK)
			{
				printf("\nUSB Transaction failed\n");
				return(result);
			}

			/* Check the transaction status */
			txRxStatus = USB_isTransactionDone(hEp[7], &status);
			if(txRxStatus != CSL_USB_TRUE)
			{
				bytesRem = USB_bytesRemaining(hEp[7], &status);
				if(bytesRem)
				{
					printf("\nBytes remaining: %d\n", bytesRem);
				}
			}

			/* This is to initiate filling the Tx FIFO with data for next read operation */
			pContext->dwIntSourceL |= CSL_USB_TX_INT_EP3;
		}

		if(pContext->dwIntSourceL & CSL_USB_RX_INT_EP4)
		{
			// read the RX packet size
			// validate the RX packet first
			if (USB_isValidDataInFifoOut(&pContext->pEpStatus[getEndPoint(outEp4)]))
			{
				// get the EP4 handle
				tempEpH = USB_epNumToHandle(hUsbDev, getEndPoint(outEp4));
				usb_income_num_bytes_ep4 = USB_getDataCountReadFromFifo(tempEpH);
			}

			status = USB_postTransaction(hEp[9], usb_income_num_bytes_ep4,
			                             usbDataBuffer4, CSL_USB_OUT_TRANSFER);
			if(status != CSL_SOK)
			{
				printf("\nUSB Transaction failed\n");
				return(result);
			}

			/* Check the transaction status */
			txRxStatus = USB_isTransactionDone(hEp[9], &status);
			if(txRxStatus != CSL_USB_TRUE)
			{
				bytesRem = USB_bytesRemaining(hEp[9], &status);
				if(bytesRem)
				{
					printf("\nBytes remaining: %d\n", bytesRem);
				}
			}

			/* This is to initiate filling the Tx FIFO with data for next read operation */
			pContext->dwIntSourceL |= CSL_USB_TX_INT_EP4;
		}

		/* Check Data In Ready */
		if(pContext->dwIntSourceL & CSL_USB_TX_INT_EP1)
		{
			/* Flush the FIFO to clean any data already present in the Tx FIFO */
			USB_flushFifo(CSL_USB_EP1, 1);

			status = USB_postTransaction(hEp[2], usb_income_num_bytes_ep1,
			                             usbDataBuffer1, CSL_USB_IN_TRANSFER);
			if(status != CSL_SOK)
			{
				printf("\nUSB Transaction failed\n");
				return(result);
			}

			/* Check the transaction status */
			txRxStatus = USB_isTransactionDone(hEp[2], &status);
			if(txRxStatus != CSL_USB_TRUE)
			{
				bytesRem = USB_bytesRemaining(hEp[2], &status);
				if(bytesRem)
				{
					printf("\nBytes remaining: %d\n", bytesRem);
				}
			}
		}

		if(pContext->dwIntSourceL & CSL_USB_TX_INT_EP2)
		{
			/* Flush the FIFO to clean any data already present in the Tx FIFO */
			USB_flushFifo(CSL_USB_EP2, 1);

			status = USB_postTransaction(hEp[4], usb_income_num_bytes_ep2,
			                             usbDataBuffer2, CSL_USB_IN_TRANSFER);
			if(status != CSL_SOK)
			{
				printf("\nUSB Transaction failed\n");
				return(result);
			}

			/* Check the transaction status */
			txRxStatus = USB_isTransactionDone(hEp[4], &status);
			if(txRxStatus != CSL_USB_TRUE)
			{
				bytesRem = USB_bytesRemaining(hEp[4], &status);
				if(bytesRem)
				{
					printf("\nBytes remaining: %d\n", bytesRem);
				}
			}
		}

		if(pContext->dwIntSourceL & CSL_USB_TX_INT_EP3)
		{
			/* Flush the FIFO to clean any data already present in the Tx FIFO */
			USB_flushFifo(CSL_USB_EP3, 1);

			status = USB_postTransaction(hEp[6], usb_income_num_bytes_ep3,
			                             usbDataBuffer3, CSL_USB_IN_TRANSFER);
			if(status != CSL_SOK)
			{
				printf("\nUSB Transaction failed\n");
				return(result);
			}

			/* Check the transaction status */
			txRxStatus = USB_isTransactionDone(hEp[6], &status);
			if(txRxStatus != CSL_USB_TRUE)
			{
				bytesRem = USB_bytesRemaining(hEp[6], &status);
				if(bytesRem)
				{
					printf("\nBytes remaining: %d\n", bytesRem);
				}
			}
		}

		if(pContext->dwIntSourceL & CSL_USB_TX_INT_EP4)
		{
			/* Flush the FIFO to clean any data already present in the Tx FIFO */
			USB_flushFifo(CSL_USB_EP4, 1);

			status = USB_postTransaction(hEp[8], usb_income_num_bytes_ep4,
			                             usbDataBuffer4, CSL_USB_IN_TRANSFER);
			if(status != CSL_SOK)
			{
				printf("\nUSB Transaction failed\n");
				return(result);
			}

			/* Check the transaction status */
			txRxStatus = USB_isTransactionDone(hEp[8], &status);
			if(txRxStatus != CSL_USB_TRUE)
			{
				bytesRem = USB_bytesRemaining(hEp[8], &status);
				if(bytesRem)
				{
					printf("\nBytes remaining: %d\n", bytesRem);
				}
			}
		}

		/* Connect interrupt */
		if(pContext->dwIntSourceH & CSL_USB_GBL_INT_DEVCONN)
		{
			status = USB_connectDev(hUsbDev);
			if(status != CSL_SOK)
			{
				printf("\nUSB Connect failed\n");
				return(result);
			}
		}

		/* Disconnect interrupt */
		if(pContext->dwIntSourceH & CSL_USB_GBL_INT_DEVDISCONN)
		{
			status = USB_disconnectDev(hUsbDev);
			usbDevDisconnect = TRUE;
			if(status != CSL_SOK)
			{
				printf("\nUSB Disconnect failed\n");
				return(result);
			}
		}

		/* Suspend interrupt */
		if(pContext->dwIntSourceH & CSL_USB_GBL_INT_SUSPEND)
		{
			status = USB_suspendDevice(hUsbDev);
			if(status != CSL_SOK)
			{
				printf("\nUSB Suspend failed\n");
				return(result);
			}
		}

		if ((usbDevDisconnect == TRUE) &&
			(CSL_FEXT(usbRegisters->DEVCTL, USB_DEVCTL_VBUS) !=
		  	CSL_USB_DEVCTL_VBUS_ABOVEVBUSVALID))
		{
			printf("\n\nUSB Cable Disconnected!!\n");

			while(CSL_FEXT(usbRegisters->DEVCTL, USB_DEVCTL_VBUS) !=
			         CSL_USB_DEVCTL_VBUS_ABOVEVBUSVALID);

			printf("\nUSB Cable Connection Detected!!\n");

			usbDevDisconnect = FALSE;
			USB_connectDev(hUsbDev);
		}
	}

	result = CSL_USB_TEST_PASSED;
	return(result);
}


/**
 *  \brief  USB suspend call back function
 *
 *  \param  none
 *
 *  \return Test result
 */
void CSL_suspendCallBack(CSL_Status    status)
{
	printf("\n\nUSB SUSPEND Callback\n");
}


/**
 *  \brief  USB self wakeup call back function
 *          NOTE: Self Wakeup is interfaced to the STOP key of the EVM keypad
 *          To test the Self wakeup send SUSPEND signal from the USB test application.
 *          Check whether the device is suspended or not(Any transaction fails in suspend mode)
 *          Now press the STOP key on the EVM, USB device send a wakeup signal to the host.
 *          All the transactions should happen properly after self wakeup.
 *
 *  \param  none
 *
 *  \return Test result
 */
void CSL_selfWakeupCallBack(CSL_Status    status)
{
	printf("\n\nUSB Self Wakeup CallBack\n");
	selfWakeupServiced = TRUE;
}

/**
 *  \brief  USB delay function
 *
 *  \param  mSecs - Delay in millisecs
 *
 *  \return None
 */
static void USB_delay(Uint32    mSecs)
{
	volatile Uint32    delay;
	volatile Uint32    msecCount;
	volatile Uint32    sysClk;  /* System clock value in KHz */

	sysClk = 100000;  /* It is assumed that system is running at 100MHz */

	for (msecCount = 0; msecCount < mSecs; msecCount++)
	{
		for (delay = 0; delay < sysClk; delay++)
		{
			asm ("\tNOP");
		}
	}
}


/**
 *  \brief  Configures SAR to trigger external event for self wakeup
 *
 *  \param  None
 *
 *  \return Test result
 */
CSL_Status CSL_configSarForSelfWakeup(void)
{
	CSL_SarChSetup    param;
    CSL_Status        result;
    CSL_Status        status;
    Bool              flag;
    int               chanNo;

    flag = 1;

    result = CSL_USB_TEST_FAILED;

    /* Disable interrupt */
    IRQ_globalDisable();
    /* Initialize Interrupt Vector table */
    IRQ_setVecs((Uint32)(&VECSTART));

    /* Initialize the SAR module */
    status = SAR_init();
    if(status != CSL_SOK)
    {
        printf("\nSAR Init Failed!!\n");
        return (result);
    }

    /* Open SAR channel */
    status = SAR_chanOpen(&SarObj,CSL_SAR_CHAN_3);
    if(status != CSL_SOK)
    {
        printf("\nSAR_chanOpen Failed!!\n");
        return result;
    }
    SarHandle = &SarObj;

    /* Initialize channel */
    status = SAR_chanInit(SarHandle);
    if(status != CSL_SOK)
    {
        printf("\nSAR_chanInit Failed!!\n");
        return(result);
    }

    /* Clear any pending Interrupt */
    IRQ_clear(SAR_EVENT);
    IRQ_test(SAR_EVENT,&flag);

    /* Register the ISR */
    IRQ_plug(SAR_EVENT,&sar_isr);

    param.OpMode     = CSL_SAR_INTERRUPT;
    param.MultiCh    = CSL_SAR_NO_DISCHARGE;
    param.SysClkDiv  = 0x11;
    param.RefVoltage = CSL_SAR_REF_VIN;

    /* Configuration for SAR module */
    status = SAR_chanSetup(SarHandle,&param);
    if(status != CSL_SOK)
    {
        printf("\nSAR_chanConfig Failed!!\n");
        return(result);
    }

    /* Set channel cycle set */
    status = SAR_chanCycSet(SarHandle,CSL_SAR_CONTINUOUS_CONVERSION);
    if(status != CSL_SOK)
    {
        printf("\nSAR_chanCycSet Failed!!\n");
        return(result);
    }

    /* set ADC Measurement parameters */
    status = SAR_A2DMeasParamSet(SarHandle,CSL_KEYPAD_MEAS,&chanNo);
    if(status != CSL_SOK)
    {
        printf("\nSAR_A2DMeasParamSet Failed!!\n");
        return(result);
    }

    /* Enabling Interrupt */
    IRQ_enable(SAR_EVENT);
    IRQ_globalEnable();

	/* start the conversion */
    status = SAR_startConversion(SarHandle);
    if(status != CSL_SOK)
	{
    	printf("\nSAR_startConversion Failed!!\n");
        return(result);
    }

    result = CSL_USB_TEST_PASSED;
    return(result);
}


/**
 *  \brief  SAR interrupt service routine
 *
 *  \param  None
 *
 *  \return None
 */
interrupt void sar_isr(void)
{
    SAR_readData(SarHandle, &readBuffer);

	if((readBuffer == CSL_USB_SAR_STOP_KEY_VAL) &&
	   (selfWakeupServiced == TRUE))
	{
		selfWakeupServiced = FALSE;
		USB_selfwakeup(hUsbDev, selfwakeupDelay);
	}
}

/**
 *  \brief  Start transfer call back function
 *
 *  \param  vpContext - USB context structure
 *  \param  vpeps     - End point status structure pointer
 *
 *  \return CSL_Status
 */
CSL_Status CSL_startTransferCallback(void    *vpContext,
                                     void    *vpeps)
{
	pUsbContext      pContext;
	pUsbEpStatus     peps;
	pUsbTransfer     pTransfer;
	CSL_Status       status;

	status = CSL_SOK;

	pContext  = (pUsbContext)vpContext;
	peps      = (pUsbEpStatus)vpeps;

    if((pContext == NULL) || (peps == NULL))
    {
        return(CSL_ESYS_INVPARAMS);
	}

	if(!pContext->fMUSBIsReady)
	{
		return(CSL_ESYS_INVPARAMS);
	}

	/* The endpoint should be initialized */
	if(!peps->fInitialized)
	{
		return(CSL_ESYS_INVPARAMS);
	}

    pTransfer = peps->pTransfer;
    pTransfer->fComplete=FALSE;

	if(pTransfer->dwFlags == CSL_USB_OUT_TRANSFER)
	{
		if(peps->dwEndpoint == CSL_USB_EP0)
		{
			status = USB_processEP0Out(pContext);
		}
		else if(peps->dwEndpoint == CSL_USB_EP1)
		{
			status = USB_handleRx(pContext, getEndPoint(outEp1));
		}
		else if(peps->dwEndpoint == CSL_USB_EP2)
		{
			status = USB_handleRx(pContext, getEndPoint(outEp2));
		}
		else if(peps->dwEndpoint == CSL_USB_EP3)
		{
			status = USB_handleRx(pContext, getEndPoint(outEp3));
		}
		else
		{
			status = USB_handleRx(pContext, getEndPoint(outEp4));
		}
	}
	else if(pTransfer->dwFlags == CSL_USB_IN_TRANSFER)
	{
		if(peps->dwEndpoint == CSL_USB_EP0 )
		{
			status = USB_processEP0In(pContext);
		}
		else if(peps->dwEndpoint == CSL_USB_EP1)
		{
			status = USB_handleTx(pContext, getEndPoint(inEp1));
		}
		else if(peps->dwEndpoint == CSL_USB_EP2)
		{
			status = USB_handleTx(pContext, getEndPoint(inEp2));
		}
		else if(peps->dwEndpoint == CSL_USB_EP3)
		{
			status = USB_handleTx(pContext, getEndPoint(inEp3));
		}
		else
		{
			status = USB_handleTx(pContext, getEndPoint(inEp4));
		}
	}
	else
	{
		status = CSL_ESYS_INVPARAMS;
	}

	return(status);
}


/**
 *  \brief  Complete transfer call back function
 *
 *  \param  vpContext - USB context structure
 *  \param  vpeps     - End point status structure pointer
 *
 *  \return CSL_Status
 */
CSL_Status CSL_completeTransferCallback(void    *vpContext,
                                        void    *vpeps)
{
	if(sentLongEp0Pkt == 1)
	{
		sentLongEp0Pkt = 0;
	}

	return(CSL_SOK);
}

