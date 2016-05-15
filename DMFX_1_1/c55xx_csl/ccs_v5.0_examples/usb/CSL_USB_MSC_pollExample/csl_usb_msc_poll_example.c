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

/** @file csl_usb_msc_poll_example.c
 *
 *  @brief Example code to test USB MSC POLLED mode functionality
 *
 * \page    page20  CSL USB EXAMPLE DOCUMENTATION
 *
 *  NOTE: For Testing MSC module a macro CSL_MSC_TEST needs to be defined
 *  This includes some code in csl_usbAux.h file which is essential for MSC operation
 *  and not required for MUSB stand alone testing. define this macro in pre defined
 *  symbols in project build options (Defined in the current usb msc example pjt).
 *  Semaphores and mail boxes are used in the MSC example code as the USB operation
 *  is not possible with out OS calls. DSP BIOS is used for this purpose.
 *  Defining Start transfer and complete transfer call back functions is must
 *  and MSC module does not work if they are not implemented properly. A call back
 *  is sent to this functions from MUSB module.
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5505, C5515
 * AND C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 *
 * \subsection USB1y    TEST PROCEDURE:
 * @li Open the CCS and connect the target (C5535/C5515/C5517 EVM)
 * @li Open, build and load the USB program to the target
 * @li Set the PLL frequency to 100MHz
 * @li Run the program loaded on to the target
 * @li A new Storage device should appear in "My Computer". Open the drive and
 *     do some file transfers.
 *  @li Repeat the test at the following PLL frequencies
 *      C5515: 60MHz and 100MHz
 *      C5517: 60MHz, 100MHz, 150MHz and 200MHz
 *      C5535 eZdsp: 60MHz and 100MHz
 *  @li Repeat the test in Release mode
 * \subsection USB1z    TEST RESULT:
 * @li USB Storage device should be detected by the host PC and the file
 *     transfers should be successful
 * @li File transfers done using the example should be as desired, when the
 *     SD card contents are viewed when connected to the PC using a Card Reader
 *
 *  Path: /(CSLPATH)/example/usb/example4/csl_usb_msc_poll_example
 */

/* ============================================================================
 * Revision History
 * ================
 * 30-Oct-2008 Created
 * 10-Jun-2009 Added Support for CSL MMCSD
 * 21-Jun-2009 Added Support for MMCSD-DMA
 * 23-Jul-2012 Added Support for C5517
 * ============================================================================
 */

#include <log.h>
#include <std.h>
#include <mbx.h>
#include <sem.h>
#include <tsk.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "csl_usb.h"
#include "csl_msc.h"
#include "csl_usbAux.h"
#include "csl_mscAux.h"
#include "ata.h"
#include "csl_mmcsd_ataIf.h"
#include "ata_media.h"
#include "csl_mmcsd.h"
#include "csl_intc.h"
#include "chk_mmc.h"
#include "csl_pll.h"
#include "csl_general.h"
#include "csl_sysctrl.h"

#define ENABLE_DEBUG_PRINT

#ifdef ENABLE_DEBUG_PRINT

#define CSL_USB_DEBUG_PRINT printf

#else

#define CSL_USB_DEBUG_PRINT(string);

#endif


/* This macro is used to include or exclude the code to configure
 * the MMCSD interrupts during DMA mode operation. If this macro is not defined,
 * USB mass storage example works with out using MMCSD interrupts for checking
 * MMCSD Data transfer completion
 */
#define CSL_MMCSD_INTR_ENABLE         (1u)

/* This macro is used to include or exclude the code to read multiple sectors
 * at a time from the SD card. This code uses "MMC_readNSectors" and
 * "MMC_writeNSectors" APIs instead of "ATA_readSector" and "ATA_writeSector"
 * respectively . This code reads/writes multiple sectors only when there is
 * a request from the host to read/write CACHED_SECT_COUNT number of sectors.
 * This macro needs to be defined to include multiple read/write sector code
 */
#define CSL_MMCSD_MULTISECT_TXFER   (1u)

/* This macro is used to include or exclude the code to check
  the SD card partition type. This code is useful to determine
  the exact value for disk type parameter of ATA_systemInit() API */
#define CSL_CHECK_DISK_PARTITION    (1u)

/* This macro decides the number of multiple sectors read from or
 * written to the SD card at a time */
#define CACHED_SECT_COUNT           (16u)

/* SD clock divider value */
#define SD_CLOCK_DIV                (25000u)

/* USB Global Context structure */
extern CSL_UsbContext     gUsbContext;
/* USB Register Overlay structure */
extern CSL_UsbRegsOvly    usbRegisters;

/*----- Mailboxes -----*/
extern MBX_Obj MBX_musb;
extern MBX_Obj MBX_msc;

/*----- Semaphores -----*/
extern SEM_Obj SEM_MUSBMainTaskExited;
extern SEM_Obj SEM_MUSBMSCTaskExited;
extern SEM_Obj SEM_AbortTransferDone;
extern SEM_Obj SEM_DisconnectDeviceDone;
extern SEM_Obj SEM_ConnectDeviceDone;
extern SEM_Obj SEM_ResetDeviceDone;
extern SEM_Obj SEM_ClearEndpointStalltDone;

/*----- Tasks -----*/
extern TSK_Obj TSK_MUSBMSCTask;
extern TSK_Obj TSK_MUSBMainTask;

/* Semaphore to indicate DMA action complete */
extern SEM_Obj  SEM_mmcsd_dmaTransferDone;

volatile Bool mmcsdTxferComplete = 0;

/* CSL MMCSD Data structures */
CSL_MMCControllerObj 	pMmcsdContObj;
CSL_MmcsdHandle 		mmcsdHandle;
CSL_MMCCardObj			mmcCardObj;
CSL_MMCCardIdObj 		sdCardIdObj;
CSL_MMCCardCsdObj 		sdCardCsdObj;
CSL_MmcsdDmaConfig      mmcsdDmaConfig;

Bool     usbDevDisconnect = FALSE;

/* CSL DMA Data structures */
CSL_DMA_Handle        dmaWrHandle;
CSL_DMA_Handle        dmaRdHandle;
CSL_DMA_Config        dmaConfig;
CSL_DMA_ChannelObj    dmaWrChanObj;
CSL_DMA_ChannelObj    dmaRdChanObj;

/* ATA data structure definitions */
AtaMMCState    ataMMCState;
AtaState       ATALogicalUnit[2];
AtaUint16      _AtaBuffer[ATA_WORDS_PER_PHY_SECTOR];
AtaMMCState    *gpstrMMCState = &ataMMCState;

/* MSC Application handle */
CSL_MscInitStructApp  MSC_AppHandle;
pMscAppClassHandle    pAppClassHandle = &MSC_AppHandle;

/* MSC class data structure */
CSL_MscClassStruct  mscClassStruct;
pMscClassHandle     appMscClassHandle = &mscClassStruct;

CSL_UsbDevHandle gUsbDevHandle;

/* TI C5505 USB Product Id and Vendor Id */
//Uint16    pId = 0x5509;
Uint16    pId = 0x9010;
Uint16    vId = 0x0451;

/* MSC Logical unit data structure */
CSL_MscLunAttribApp    lunAttr;

#if (defined(CHIP_C5505_C5515) || defined(CHIP_C5504_C5514) || defined(CHIP_C5517))

/* USB string descriptor */
char *stringDescriptor[] =
{
  "  ",   /* This will eventually point to  the string descriptor */
  "Texas Instruments, Inc.",          /* iManufacturer */
  "TMS320VC5515 USB Device",          /* iProductName */
  "0000320C5515",                     /* iSerial No :RomId - F# */
  "Default",                          /* iConfiguration */
  "USB Mass Storage Class",           /* iInterface - Mass Storage */
  NULL
};

#else

/* USB string descriptor */
char *stringDescriptor[] =
{
  "  ",   /* This will eventually point to  the string descriptor */
  "Texas Instruments, Inc.",          /* iManufacturer */
  "TMS320VC5505 USB Device",          /* iProductName */
  "0000320C5505",                     /* iSerial No :RomId - F# */
  "Default",                          /* iConfiguration */
  "USB Mass Storage Class",           /* iInterface - Mass Storage */
  NULL
};

#endif

/* Structure holding the pointers to functions servicing USB requests */
CSL_MscRequestStruct USB_ReqTable[] =
{
  { CSL_MSC_REQUEST_GET_STATUS             , MSC_reqGetStatus },
  { CSL_MSC_REQUEST_CLEAR_FEATURE          , MSC_reqClearFeature },
  { CSL_MSC_REQUEST_SET_FEATURE            , MSC_reqSetFeature },
  { CSL_MSC_REQUEST_SET_ADDRESS            , MSC_reqSetAddress },
  { CSL_MSC_REQUEST_GET_DESCRIPTOR         , MSC_reqGetDescriptor },
  { CSL_MSC_REQUEST_SET_DESCRIPTOR         , MSC_reqUnknown },
  { CSL_MSC_REQUEST_GET_CONFIGURATION      , MSC_reqGetConfiguration },
  { CSL_MSC_REQUEST_SET_CONFIGURATION      , MSC_reqSetConfiguration },
  { CSL_MSC_REQUEST_GET_MAX_LUN            , MSC_reqGetMaxLUN },
  { CSL_MSC_REQUEST_BOT_MSC_RESET          , MSC_reqBotMscReset },
  { CSL_MSC_REQUEST_GET_INTERFACE          , MSC_reqGetInterface },
  { CSL_MSC_REQUEST_SET_INTERFACE          , MSC_reqSetInterface },
  { CSL_MSC_REQUEST_SYNC_FRAME             , MSC_reqUnknown },
  { 0, NULL }
};

/* Buffer used to store the data used to read/write to the Media */
#pragma DATA_ALIGN(usbDataBuffer, 32);
Uint16 usbDataBuffer[512];

Uint16 gStartCaching = 0;
Uint32 glbaStartNum = 0;
Uint16 glbaCaheCnt = 0;
Uint16 temp;
#pragma DATA_ALIGN(glbaCacheBuff, 32);
Uint16 glbaCacheBuff[256 * CACHED_SECT_COUNT];

/* USB Application function prototypes */

/**
 *  \brief  CSL MSC test function
 *
 *  NOTE: For Testing MSC module a macro CSL_MSC_TEST needs to be defined
 *  This includes some code in csl_usbAux.h file which is essential for MSC operation
 *  and not required for MUSB stand alone testing. define this macro in pre defined
 *  symbols in project build options (Defined in the current usb msc example pjt).
 *  Semaphores and mail boxes are used in the MSC example code as the USB operation
 *  is not possible with out OS calls. DSP BIOS version 5.32.03 is used for this purpose.
 *  Defining Start transfer and complete transfer call back functions is must
 *  and MSC module does not work if they are not implemented properly. A call back
 *  is sent to this functions from MUSB module.
 *
 *  \param  None
 *
 *  \return Test Status
 */
CSL_Status CSL_mscTest(void);

/**
 *  \brief  MMCSD Configuration function.
 *
 *   This function configures CSL MMCSD module and makes the
 *   MMCSD hardware ready for USB data transfers. This function
 *   only works with SD card.
 *
 *  \param  none
 *
 *  \return Test result
 */
CSL_Status configSdCard (CSL_MMCSDOpMode    opMode);

/**
 *  \brief  Configures DMA module for MMCSD data transfers
 *
 *   This function configures two DMA channels for MMCSD read
 *   and write operation.
 *
 *  \param  none
 *
 *  \return Test result
 */
CSL_Status configDmaforMmcSd(void);

/**
 *  \brief  USB Msc task
 *
 *  This task takes care of transferring the data between media
 *  and USB device
 *
 *  \param  None
 *
 *  \return None
 */
void MSCTask(void);

/**
 *  \brief  USB main task
 *
 *  This task takes care of the servicing the request coming from the
 *  USB host device
 *
 *  \param  None
 *
 *  \return None
 */
void MainTask(void);

/**
 *  \brief  Function to start USB data transfer
 *
 *  This functions starts the USB data transfer to read the data
 *  from an end point or to write data to an end point
 *  This is a very important function which is called from the MUSB
 *  layer using a call back pointer.
 *
 *  \param  pContext     - Pointer to the global MUSB controller context structure
 *  \param  peps         - Endpoint status structure pointer
 *
 *  \return TRUE  - Operation successful
 *          FALSE - Invalid input parameters
 */
CSL_Status StartTransfer(void    *vpContext,
                         void    *vpeps);

/**
 *  \brief  Function to complete the MSC data transfer
 *
 *  This function completes the data transfer between USB device
 *  and host. This is an important function which is called from
 *  MUSB layer using call back pointer
 *
 *  \param  pContext  - USB context structure pointer
 *  \param  peps      - End point status structure pointer
 *
 *  \return None
 */
CSL_Status CompleteTransfer(void    *vpContext,
                            void    *vpeps);

/**
 *  \brief  Application call back function for bulk transactions
 *
 *  \param  None
 *
 *  \return None
 */
void appBulkFxn(void);

/**
 *  \brief  Application call back function for control transactions
 *
 *  \param  None
 *
 *  \return None
 */
void appCtrlFxn(void);

/**
 *  \brief  Function to initialize the media
 *
 *  \param  lunNo    - Logical unit number
 *
 *  \return Media status
 */
CSL_MscMediaStatus AppMediaInit(Uint16 lunNo);

/**
 *  \brief  Function to get the media size
 *   Note: This function returns number of sectors available
 *   on the media.
 *
 *  \param  lunNo    - Logical unit number
 *
 *  \return Number of sectors on the media
 */
Uint32 AppGetMediaSize(Uint16 lunNo);

/**
 *  \brief  Function to know the media status
 *
 *  \param  lunNo    - Logical unit number
 *
 *  \return Media status
 */
CSL_MscMediaStatus AppGetMediaStatus (Uint16 lunNo);

/**
 *  \brief  Function to write data to storage media
 *
 *  \param  lunNo    - Logical unit number
 *  \param  srcptr   - Data buffer pointer
 *  \param  LBA      - LBA to read data from
 *  \param  lbaCnt   - Number of LBAs to read
 *
 *  \return Media status
 */
CSL_MscMediaStatus AppWriteMedia (Uint16    lunNo,
                                  Uint16    *srcptr,
                                  Uint32    LBA,
                                  Uint16    lbaCnt);

/**
 *  \brief  Function to read data from storage media
 *
 *  \param  lunNo    - Logical unit number
 *  \param  destptr  - Data buffer pointer
 *  \param  LBA      - LBA to read data from
 *  \param  lbaCnt   - Number of LBAs to read
 *
 *  \return Media status
 */
CSL_MscMediaStatus AppReadMedia (Uint16    lunNo,
                                 Uint16    *destptr,
                                 Uint32    LBA,
                                 Uint16    lbaCnt);

/**
 *  \brief  Function to Eject media
 *
 *  \param  lunNo  - Logical unit number
 *
 *  \return Media status
 */
CSL_MscMediaStatus AppMediaEject(Uint16 lunNo);

/**
 *  \brief  Function to lock media
 *
 *  \param  lunNo   - Logical unit number
 *  \param  status  - Media lock status
 *
 *  \return Media status
 */
CSL_MscMediaStatus AppLockMedia(Uint16                   lunNo,
                                CSL_MscMediaLockStatus   status);

/**
 *  \brief  Function to send device notification
 *
 *  \param  pContext    - USB context structure pointer
 *  \param  wUSBEvents  - USB events
 *
 *  \return None
 */
void DeviceNotification(pUsbContext    pContext,
				        WORD           wUSBEvents);

/**
 *  \brief  Function to start the USB device
 *
 *  \param  pContext - USB context structure pointer
 *
 *  \return TRUE  - Operation success
 *          FALSE - Invalid input parameter
 */
Bool StartDevice(pUsbContext pContext);

/**
 *  \brief  Function to stop the USB device
 *
 *  \param  pContext - USB context structure pointer
 *
 *  \return TRUE  - Operation success
 *          FALSE - Invalid input parameter
 */
Bool StopDevice(pUsbContext pContext);

/**
 *  \brief  Function to start USB data transfer
 *
 *  \param  pContext     - Pointer to the global MUSB controller context structure
 *  \param  dwEndpoint   - Endpoint Number
 *
 *  \return TRUE  - Operation successful
 *          FALSE - Invalid input parameters
 */
void USB_configEndpointDataSize(pUsbContext    pContext,
                                DWORD          dwEndpoint);

/**
 *  \brief  USB suspend call back function
 *
 *  \param  status
 *
 *  \return none
 */
void CSL_suspendCallBack(CSL_Status    status);

/**
 *  \brief  USB self wakeup call back function
 *
 *  \param  status
 *
 *  \return none
 */
void CSL_selfWakeupCallBack(CSL_Status    status);

/**
 *  \brief  USB Interrupt Service Routine
 *
 *  \param  none
 *
 *  \return none
 */
void USBisr();

/**
 *  \brief  USB Isr to post Message to main task
 *
 *  \param  None
 *
 *  \return None
 */
void USB_MUSB_Isr(void);

/**
 *  \brief  USB interrupt handler
 *
 *  This function identifies the interrupt generated by the
 *  USB device and calls corresponding function to service
 *  the interrupt
 *
 *  \param  None
 *
 *  \return None
 */
static Bool HandleUSBInterrupt(pUsbContext pContext);

/**
 *  \brief  Function to handle Ep0 interrupts
 *
 *  \param  pContext - USB context structure pointer
 *
 *  \return None
 */
void  MUSB_Handle_EP0_Intr(pUsbContext pContext);

/**
 *  \brief  function to handle resume interrupt
 *
 *  \param  None
 *
 *  \return None
 */
void MUSB_Handle_Resume_Intr(void);

/**
 *  \brief  USB delay function
 *
 *  \param  dwMicroSeconds delay in micro secs
 *
 *  \return None
 */
void genDelay(DWORD dwMicroSeconds);

/**
 *  \brief  MMCSD data call back function
 *
 *   This function is called from the MMCSD read and write APIs
 *   after starting the DMA data transfer
 *
 *  \param  none
 *
 *  \return none
 */
void mmcsdDataCallback(void *hMmcSd);

/**
 *  \brief  MMCSD ISR
 *
 *  \param  none
 *
 *  \return none
 */
void mmcsd0_isr(void);

/**
 *  \brief  DMA ISR function;Not in use
 *
 *  \param  none
 *
 *  \return none
 */
void dma_isr(void);

/**
 *  \brief  Function to check if the previous transfer is done.
 *
 *  \param  peps        - End point status structure pointer
 *  \param  dwEndpoint  - End point status structure pointer
 *
 *  \return Value of TxPktRdy bit
 */
Bool checkTxDone(pUsbEpStatus    peps,
                 DWORD           dwEndpoint);

/**
 *  \brief    Function to calculate the memory clock rate
 *
 * This function computes the memory clock rate value depending on the
 * CPU frequency. This value is used as clock divider value for
 * calling the API MMC_sendOpCond(). Value of the clock rate computed
 * by this function will change depending on the system clock value
 * and MMC maximum clock value passed as parameter to this function.
 * Minimum clock rate value returned by this function is 0 and
 * maximum clock rate value returned by this function is 255.
 * Clock derived using the clock rate returned by this API will be
 * the nearest value to 'memMaxClk'.
 *
 *  \param    memMaxClk  - Maximum memory clock rate
 *
 *  \return   MMC clock rate value
 */
Uint16 computeClkRate(Uint32    memMaxClk);

/**
 *  \brief  CSL MSC main function
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
	CSL_Status status;

	CSL_USB_DEBUG_PRINT("USB Mass Storage Class POLL Mode Test!\n\n");

	/* Initialize SD card */
	//status = configSdCard(CSL_MMCSD_OPMODE_DMA);
	status = configSdCard(CSL_MMCSD_OPMODE_POLLED);
	if(status != CSL_SOK)
	{
		printf("CSL MMCSD module Configuration Failed\n\n");
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 2 --
   /////  Reseting PaSs_StAtE to 0 if error detected here.
        PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
   /////
		if(status == CSL_ESYS_MEDIA_NOTPRESENT)
		{
			CSL_USB_DEBUG_PRINT("Insert SD Card!!\n");
		}
	}
	else
	{
		CSL_USB_DEBUG_PRINT("CSL MMCSD module Configuration Successful\n\n");
		/* Initialize Msc module */
		status = CSL_mscTest();
		if(status != CSL_SOK)
		{
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 2 --
   /////  Reseting PaSs_StAtE to 0 if error detected here.
        PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
   /////
			CSL_USB_DEBUG_PRINT("USB MSC Test Failed\n\n");
		}
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
 *  \brief  CSL MSC test function
 *
 *  NOTE: For Testing MSC module a macro CSL_MSC_TEST needs to be defined
 *  This includes some code in csl_usbAux.h file which is essential for MSC operation
 *  and not required for MUSB stand alone testing. define this macro in pre defined
 *  symbols in project build options (Defined in the current usb msc example pjt).
 *  Semaphores and mail boxes are used in the MSC example code as the USB operation
 *  is not possible with out OS calls. DSP BIOS version 5.32.03 is used for this purpose.
 *  Defining Start transfer and complete transfer call back functions is must
 *  and MSC module does not work if they are not implemented properly. A call back
 *  is sent to this functions from MUSB module.
 *
 *  \param  None
 *
 *  \return Test Status
 */
CSL_Status CSL_mscTest(void)
{
	CSL_Status      status;
	volatile Uint32 looper;
	pMscClassHandle pHandle;

	/* USB config structure */
	CSL_UsbConfig    usbConfig;

	*(ioport volatile int *)0x1c02 = 0x0;
	*(ioport volatile int *)0x1c03 = 0x0;

	/* USB interface for MMCSD */
	ATALogicalUnit[0].AtaInitAtaMediaState = (AtaError (*)(void *))MMC_initState;
	gpstrMMCState->hMmcSd = mmcsdHandle;
	ATALogicalUnit[0].pAtaMediaState = gpstrMMCState;
	ATALogicalUnit[0].AtaInitAtaMediaState(&ATALogicalUnit[0]);
	ATALogicalUnit[0]._AtaWriteBuffer = _AtaBuffer;

	memset((void*)&mscClassStruct,0x00,sizeof(mscClassStruct));

	/* Initializing the Pointer to the MSC Handle to the Buffer Allocated */
	MSC_AppHandle.pMscObj = (void*)&mscClassStruct;
	pHandle = (pMscClassHandle)(MSC_AppHandle.pMscObj);

	/* Initialize USB in POLLED mode. This mode corresponds to the way
	   USB FIFO data is accessed.
	 */
	usbConfig.opMode                   = CSL_USB_OPMODE_POLLED;
	usbConfig.maxCurrent               = 50;
	usbConfig.appSuspendCallBack       = (CSL_USB_APP_CALLBACK)CSL_suspendCallBack;
	usbConfig.appWakeupCallBack        = (CSL_USB_APP_CALLBACK)CSL_selfWakeupCallBack;
	usbConfig.startTransferCallback    = StartTransfer;
	usbConfig.completeTransferCallback = CompleteTransfer;

	lunAttr.removeableApp = 1;      /* Removable or Non removable Media */
	lunAttr.readwriteApp  = 0x0101;
	lunAttr.lbaSizeApp    = 512;    /* Logical Block Size (size of each LB) */
	lunAttr.mediaSizeApp  = 32760;  /* the total No. Of LB in the Media*/

	/* Initializing Configurable Items for SCSI Inquiry data for Lun */
	/* All the scsi Inquiry Data here can be modified to suit Customer need */
	lunAttr.scsiInquiryConfData[0]  =  'T' | ('I'<<8);
	lunAttr.scsiInquiryConfData[1]  = 0x2020;
	lunAttr.scsiInquiryConfData[2]  = 0x2020;
	lunAttr.scsiInquiryConfData[3]  = 0x2020;

	lunAttr.scsiInquiryConfData[4]  = 'T' | ('M'<<8); /* Product ID  8 words */
	lunAttr.scsiInquiryConfData[5]  = 'S' | ('3'<<8);
	lunAttr.scsiInquiryConfData[6]  = '2' | ('0'<<8);
	lunAttr.scsiInquiryConfData[7]  = 'C' | ('5'<<8);

#if (defined(CHIP_C5505_C5515) || defined(CHIP_C5504_C5514) || defined(CHIP_C5517))
	lunAttr.scsiInquiryConfData[8]  = '5' | ('1'<<8);
#else
	lunAttr.scsiInquiryConfData[8]  = '5' | ('0'<<8);
#endif

	lunAttr.scsiInquiryConfData[9]  = '5' | (' '<<8);
	lunAttr.scsiInquiryConfData[10] = ' ' | (' '<<8);
	lunAttr.scsiInquiryConfData[11] = ' ' | (' '<<8);

	lunAttr.scsiInquiryConfData[12] = 0x2E32;        /* Revision ID 2 words */
	lunAttr.scsiInquiryConfData[13] = 0x3030;
	lunAttr.scsiInquiryConfData[14] = '1' | ('2'<<8);/* Vendor-specific 10 words */
	lunAttr.scsiInquiryConfData[15] = '/' | ('1'<<8);
	lunAttr.scsiInquiryConfData[16] = '6' | ('/'<<8);
	lunAttr.scsiInquiryConfData[17] = '1' | ('9'<<8);
	lunAttr.scsiInquiryConfData[18] = '9' | ('9'<<8);
	lunAttr.scsiInquiryConfData[19] = 0x0000;
	lunAttr.scsiInquiryConfData[20] = 0x0000;
	lunAttr.scsiInquiryConfData[21] = 0x0000;
	lunAttr.scsiInquiryConfData[22] = 0x0000;
	lunAttr.scsiInquiryConfData[23] = 0x0000;

	/* Set the Logical Unit attributes */
	status = MSC_SetLunAttr(&MSC_AppHandle,&lunAttr,0);
	if(status != CSL_SOK)
	{
		CSL_USB_DEBUG_PRINT("MSC Set Lun Attributes Failed\n");
		return(status);
	}

	/* Calling init routine */
	/* Giving all the table handles and the buffers to the MSC module */
	MSC_AppHandle.strDescrApp    = &stringDescriptor[0];
	MSC_AppHandle.lbaBufferApp   = usbDataBuffer;
	MSC_AppHandle.mscReqTableApp = USB_ReqTable;
	MSC_AppHandle.pId    = pId;
	MSC_AppHandle.vId    = vId;
	MSC_AppHandle.numLun = 1;

	/* All Function Handlers need to be Initialized */
	MSC_AppHandle.readMediaApp            = AppReadMedia;
	MSC_AppHandle.writeMediaApp           = AppWriteMedia;
	MSC_AppHandle.mediaGetPresentStateApp = AppGetMediaStatus;
	MSC_AppHandle.mediaInitApp            = AppMediaInit;
	MSC_AppHandle.mediaEjectApp           = AppMediaEject;
	MSC_AppHandle.mediaLockUnitApp        = AppLockMedia;
	MSC_AppHandle.getMediaSizeApp         = AppGetMediaSize;
	MSC_AppHandle.bulkHandler             = appBulkFxn;
	MSC_AppHandle.ctrlHandler             = appCtrlFxn;

    pAppClassHandle->mscReqTableApp = USB_ReqTable;
	mscClassStruct.ctrlHandle.mscReqTable  = USB_ReqTable;

	/* Call Init API */
	status = MSC_Open(&MSC_AppHandle, &usbConfig);
	if(status != CSL_SOK)
	{
		if(status == CSL_ESYS_BADHANDLE)
		{
			CSL_USB_DEBUG_PRINT("Opening MSC Module Failed!\n");
			CSL_USB_DEBUG_PRINT("MSC_Open Returned - BAD HANDLE\n");
		}
		else if (status == CSL_ESYS_MEDIA_NOTPRESENT)
		{
			CSL_USB_DEBUG_PRINT("Opening MSC Module Failed!\n");
			CSL_USB_DEBUG_PRINT("MSC_Open Returned - MEDIA NOT PRESENT\n");
			CSL_USB_DEBUG_PRINT("Insert SD Card!!\n");
		}
		else if (status == CSL_ESYS_BADMEDIA)
		{
			CSL_USB_DEBUG_PRINT("Opening MSC Module Failed!\n");
			CSL_USB_DEBUG_PRINT("MSC_Open Returned - BAD MEDIA\n");
			CSL_USB_DEBUG_PRINT("Format SD Card!!\n");
		}
		else
		{
			CSL_USB_DEBUG_PRINT("MSC Open Failed\n");
		}

		return(status);
	}
	else
	{
		CSL_USB_DEBUG_PRINT("CSL MSC Module Open Successful\n");
	}

	gUsbDevHandle = mscClassStruct.usbDevHandle;

	/* USB API setParams called for setup */
	status = USB_setParams(gUsbDevHandle, pHandle->ctrlHandle.hEpHandleArray, 0);
	if(status != CSL_SOK)
	{
		return (status);
	}

	USB_connectDev(gUsbDevHandle);

	/* Enable CPU USB interrupts */
	CSL_FINST(CSL_CPU_REGS->IER1, CPU_IER1_USB, ENABLE);

	return(status);
}

/**
 *  \brief  Application call back function for bulk transactions
 *
 *  \param  None
 *
 *  \return None
 */
void appBulkFxn(void)
{
   MSC_Bulk(MSC_AppHandle.pMscObj);
}

/**
 *  \brief  Application call back function for control transactions
 *
 *  \param  None
 *
 *  \return None
 */
void appCtrlFxn(void)
{
  MSC_Ctrl(MSC_AppHandle.pMscObj);
}


/**
 *  \brief  Function to Eject media
 *
 *  \param  lunNo  - Logical unit number
 *
 *  \return Media status
 */
CSL_MscMediaStatus AppMediaEject(Uint16    lunNo)
{
	return(CSL_MSC_MEDIACCESS_SUCCESS);
}

/**
 *  \brief  Function to lock media
 *
 *  \param  lunNo   - Logical unit number
 *  \param  status  - Media lock status
 *
 *  \return Media status
 */
CSL_MscMediaStatus AppLockMedia(Uint16                   lunNo,
                                CSL_MscMediaLockStatus   status)
{
	return(CSL_MSC_MEDIACCESS_SUCCESS);
}

/**
 *  \brief  Function to read data from storage media
 *
 *  \param  lunNo    - Logical unit number
 *  \param  destptr  - Data buffer pointer
 *  \param  LBA      - LBA to read data from
 *  \param  lbaCnt   - Number of LBAs to read
 *
 *  \return Media status
 */
CSL_MscMediaStatus AppReadMedia (Uint16    lunNo,
                                 Uint16    *destptr,
                                 Uint32    LBA,
                                 Uint16    lbaCnt)
{
    CSL_MscMediaStatus    retMediaStat;
    AtaError              mediaError;

	mediaError = ATA_ERROR_NONE;

#ifdef  CSL_MMCSD_MULTISECT_TXFER

	ATALogicalUnit[lunNo].CurrentWord = 0;
	if((lbaCnt < CACHED_SECT_COUNT) && (gStartCaching == 0))
	{
		//mediaError = ATA_readSector(LBA, &ATALogicalUnit[lunNo], (AtaUint16*)destptr, 0);
		mediaError = MMC_readNSectors(ATALogicalUnit[lunNo].pAtaMediaState,
									  (AtaUint16*)destptr, LBA, 1);
	}
	else
	{
		if(gStartCaching == 0)
		{
			gStartCaching = 1;
			glbaCaheCnt   = 0;
		}
	}

	if(gStartCaching == 1)
	{
		if(glbaCaheCnt == 0)
		{
			mediaError = MMC_readNSectors(ATALogicalUnit[lunNo].pAtaMediaState,
		 						          (AtaUint16*)glbaCacheBuff,
										  LBA,
                                          CACHED_SECT_COUNT);
		}

		for(temp = 0; temp < 256; temp++)
		{
			destptr[temp] = glbaCacheBuff[temp + (glbaCaheCnt * 256)];
		}

		glbaCaheCnt++;

		if(glbaCaheCnt == CACHED_SECT_COUNT)
		{
			gStartCaching = 0;
			glbaCaheCnt = 0;
		}
	}

#else

    //mediaError = ATA_readSector(LBA, &ATALogicalUnit[lunNo], (AtaUint16*)destptr, 0);
	mediaError = MMC_readNSectors(ATALogicalUnit[lunNo].pAtaMediaState,
								  (AtaUint16*)destptr, LBA, 1);

#endif

    if(mediaError == ATA_ERROR_NONE)
    {
        retMediaStat = CSL_MSC_MEDIACCESS_SUCCESS;
	}
    else if (mediaError == ATA_ERROR_BAD_MEDIA)
    {
        retMediaStat = CSL_MSC_MEDIACCESS_BADMEDIA;
	}
    else if((mediaError == ATA_ERROR_MEDIA_NOT_FOUND) ||
            (mediaError == ATA_ERROR_MEDIA_REMOVED))
    {
        retMediaStat = CSL_MSC_MEDIACCESS_NOTPRESENT;
	}
    else if(mediaError == ATA_ERROR_DISK_FULL)
    {
       retMediaStat = CSL_MSC_MEDIACCESS_OVERFLOW;
    }

    return (retMediaStat);
}

/**
 *  \brief  Function to write data to storage media
 *
 *  \param  lunNo    - Logical unit number
 *  \param  srcptr   - Data buffer pointer
 *  \param  LBA      - LBA to read data from
 *  \param  lbaCnt   - Number of LBAs to read
 *
 *  \return Media status
 */
CSL_MscMediaStatus AppWriteMedia (Uint16    lunNo,
                                  Uint16    *srcptr,
                                  Uint32    LBA,
                                  Uint16    lbaCnt)
{
    CSL_MscMediaStatus    retMediaStat;
    AtaError              mediaError;

	mediaError = ATA_ERROR_NONE;

#ifdef  CSL_MMCSD_MULTISECT_TXFER

	if((lbaCnt < CACHED_SECT_COUNT) && (gStartCaching == 0))
	{
	    //mediaError = ATA_writeSector(LBA,&ATALogicalUnit[lunNo],(AtaUint16*)srcptr, 0);
		mediaError = MMC_writeNSectors(ATALogicalUnit[lunNo].pAtaMediaState,
									   (AtaUint16*)srcptr, LBA, 1);
	}
	else
	{
		if(gStartCaching == 0)
		{
			gStartCaching = 1;
			glbaStartNum  = LBA;
			glbaCaheCnt   = 0;
		}
	}

	if(gStartCaching == 1)
	{

		for(temp = 0; temp < 256; temp++)
		{
			glbaCacheBuff[temp + (glbaCaheCnt * 256)] = srcptr[temp];
		}

		mediaError = ATA_ERROR_NONE;
		glbaCaheCnt++;

		if(glbaCaheCnt == CACHED_SECT_COUNT)
		{
			mediaError = MMC_writeNSectors(ATALogicalUnit[lunNo].pAtaMediaState,
							               (AtaUint16*)glbaCacheBuff,
							               glbaStartNum,
        	                               CACHED_SECT_COUNT);
			glbaCaheCnt = 0;
			gStartCaching = 0;
		}
	}

#else

	//mediaError = ATA_writeSector(LBA,&ATALogicalUnit[lunNo],(AtaUint16*)srcptr, 0);
	mediaError = MMC_writeNSectors(ATALogicalUnit[lunNo].pAtaMediaState,
								   (AtaUint16*)srcptr, LBA, 1);

#endif

    if(mediaError == ATA_ERROR_NONE)
    {
        retMediaStat = CSL_MSC_MEDIACCESS_SUCCESS;
	}
    else if(mediaError == ATA_ERROR_BAD_MEDIA)
    {
        retMediaStat = CSL_MSC_MEDIACCESS_BADMEDIA;
	}
    else if((mediaError == ATA_ERROR_MEDIA_NOT_FOUND)||
            (mediaError == ATA_ERROR_MEDIA_REMOVED))
    {
        retMediaStat = CSL_MSC_MEDIACCESS_NOTPRESENT;
	}
    else if(mediaError == ATA_ERROR_DISK_FULL)
    {
        retMediaStat = CSL_MSC_MEDIACCESS_OVERFLOW;
    }

    return (retMediaStat);
}

/**
 *  \brief  Function to know the media status
 *
 *  \param  lunNo    - Logical unit number
 *
 *  \return Media status
 */
CSL_MscMediaStatus AppGetMediaStatus(Uint16    lunNo)
{
    return CSL_MSC_MEDIACCESS_SUCCESS;
}

/**
 *  \brief  Function to initialize the media
 *
 *  \param  lunNo    - Logical unit number
 *
 *  \return Media status
 */
CSL_MscMediaStatus AppMediaInit(Uint16 lunNo)
{
    CSL_MscMediaStatus    retMediaStat;
	AtaError              mediaError;
	unsigned int          diskType;

	mediaError = ATA_ERROR_NONE;

	/* For partitioned disk, 'diskType' should be 0
	   and for unpartiotioned disk, it should be 1
	 */
	 /* chk_mmc() function is used to check the partition type of
	    SD card.
	    ATA_systemInit() function needs to be called
	    with 'diskType' set to 0 before calling chk_mmc().
	    chk_mmc() function will check whether the disk is partitioned
	    or unpartitioned. If disk is not partitioned it will change the
	    'diskType' value to 1 otherwise it will not change the diskType value.
		After calling chk_mmc() if 'diskType' is not '0' , It means that
		the SD card is not partitioned and ATA_systemInit() needs to be
		called with 'diskType' value modified by chk_mmc() function */

#ifdef CSL_CHECK_DISK_PARTITION

	diskType = 0;
	/* Call ATA_systemInit() to initialize some values which are
	  used by chk_mmc() function */
	mediaError = ATA_systemInit(&ATALogicalUnit[lunNo], diskType);

	chk_mmc(&ATALogicalUnit[lunNo], &diskType);
	if(diskType != 0)
	{
		mediaError = ATA_systemInit(&ATALogicalUnit[lunNo], diskType);
	}

#else

	diskType = 1;
	mediaError = ATA_systemInit(&ATALogicalUnit[lunNo], diskType);

#endif

    if (mediaError == ATA_ERROR_NONE)
    {
        retMediaStat = CSL_MSC_MEDIACCESS_SUCCESS;
	}
    else if(mediaError == ATA_ERROR_BAD_MEDIA)
    {
        retMediaStat = CSL_MSC_MEDIACCESS_BADMEDIA;
	}
    else if((mediaError == ATA_ERROR_MEDIA_NOT_FOUND) ||
            (mediaError == ATA_ERROR_MEDIA_REMOVED))
    {
        retMediaStat = CSL_MSC_MEDIACCESS_NOTPRESENT;
	}

    return(retMediaStat);
}

/**
 *  \brief  Function to get the media size
 *   Note: This function returns number of sectors available
 *   on the media.
 *
 *  \param  lunNo    - Logical unit number
 *
 *  \return Number of sectors on the media
 */
Uint32 AppGetMediaSize(Uint16 lunNo)
{
    Uint32 size;

    //size = (Uint32)ATA_diskSize(&ATALogicalUnit[lunNo]);
	//size = mmcsdHandle->cardObj->cardCapacity;

	size = mmcsdHandle->cardObj->totalSectors;
// mwei avoid overflow
#if 0
	size = ((size)*(1024));
	size = ((size)/(512));
#else

	//size = (size)*2;
#endif

	return(size);
}

/**
 *  \brief  Wrapper for USB main task
 *
 *  \param  None
 *
 *  \return None
 */
void USBMainTask()
{
	MainTask();
}

/**
 *  \brief  Wrapper for USB Msc Task
 *
 *  \param  None
 *
 *  \return None
 */
void USBMSCTask()
{
	/* Call the USB Mass storage class task */
	MSCTask();
}

/**
 *  \brief  USB main task
 *
 *  This task takes care of the servicing the request coming from the
 *  USB host device
 *
 *  \param  None
 *
 *  \return None
 */
void MainTask(void)
{
    pUsbContext 	pContext;
	pUsbEpStatus 	peps;
	CSL_UsbMsgObj   USBMsg;
	volatile WORD 	temp;
	Bool			fExitMainTaskOnUSBError;

	fExitMainTaskOnUSBError = FALSE;
	pContext = &gUsbContext;

	while(1)
	{
        /* Wait for mailbox */
        if(MBX_pend(&MBX_musb, &USBMsg, SYS_FOREVER))
		{
			if(USBMsg.wMsg == CSL_USB_MSG_MAIN_TASK_EXIT)
			{
				break;
			}

			switch(USBMsg.wMsg)
			{
				case CSL_USB_MSG_USB_INT:
              		if(pContext->fMUSBIsReady)
					{
						/* Handle the interrupts */
				  		if(!HandleUSBInterrupt(pContext))
							fExitMainTaskOnUSBError = TRUE;
					}
					break;

				case CSL_USB_MSG_CONNECT_DEVICE:

					USB_connectDev(gUsbDevHandle);

					/* ack */
					SEM_post(&SEM_ConnectDeviceDone);
					break;

				case CSL_USB_MSG_DISCONNECT_DEVICE:
					USB_disconnectDev(gUsbDevHandle);
			        if (pContext->cableState != CSL_USB_DEVICE_DETACH)
					{
						if(!StopDevice(pContext))
							fExitMainTaskOnUSBError = TRUE;
					}
					/* ack */
					SEM_post(&SEM_DisconnectDeviceDone);
					break;

				case CSL_USB_MSG_RESET_DEVICE:
					if(!USB_resetDev(gUsbDevHandle))
						fExitMainTaskOnUSBError = TRUE;
					/* ack */
					SEM_post(&SEM_ResetDeviceDone);
					break;

				case CSL_USB_MSG_STALL_ENDPOINT:
					break;

				case CSL_USB_MSG_CLEAR_ENDPOINT_STALL:
					/* ack */
					SEM_post(&SEM_ClearEndpointStalltDone);
					break;

				case CSL_USB_MSG_ABORT_TRANSFER:

				 	SEM_post(&SEM_AbortTransferDone);
					break;

				case CSL_USB_MSG_SEND_HAND_SHAKE:
					break;

				case CSL_USB_MSG_DATA:
					fExitMainTaskOnUSBError = FALSE;
					/* Just trigger this task. */
					break;

				default:
					break;
			}
		}

		/* Non-Control endpoints */
		/* process the DATA IN */
		if(pContext->fWaitingOnFlagA)
		{
            /* Get the Endpoint currently assigned to Flag A */
			peps = &pContext->pEpStatus[CSL_USB_EP1];
            if(pContext->fInitialized)
            {
                if(peps->pTransfer)
				{
					if (checkTxDone(peps, CSL_USB_EP1))
					{
						if(!USB_handleTx(pContext, CSL_USB_EP1))
							fExitMainTaskOnUSBError = TRUE;
					}
				}
            }
		}

		/* process the DATA OUT */
		if(pContext->fWaitingOnFlagB)
		{
		    /* Get the Endpoint currently assigned to Flag B */
			peps = &pContext->pEpStatus[CSL_USB_EP2];

			if(USB_isValidDataInFifoOut(peps))
			{
	            if(pContext->fInitialized)
	            {
	                if(peps->pTransfer)
					{
						if(!USB_handleRx(pContext, CSL_USB_EP2))
						fExitMainTaskOnUSBError = TRUE;
					}
	            }
			}
		}/* fWaitingOnFlagB check */

    	/* process EP0 IN DATA */
		if(pContext->fWaitingOnEP0BUFAvail)
		{
			if(pContext->fEP0BUFAvailable)
			{
				if(!(USB_processEP0In(pContext)))
					fExitMainTaskOnUSBError = TRUE;
			}
		}

        /* fOutPhaseCmd is active when the MUSB receive EP0 data */
		if(pContext->fOutPhaseCmd)
		{
			if(!USB_processEP0Out(pContext))
				fExitMainTaskOnUSBError = TRUE;
		}

		if((usbDevDisconnect == TRUE) &&
		  (CSL_FEXT(usbRegisters->DEVCTL, USB_DEVCTL_VBUS) !=
		            CSL_USB_DEVCTL_VBUS_ABOVEVBUSVALID))
		{
			CSL_USB_DEBUG_PRINT("\nUSB Cable Disconnected!!\n");

			while(CSL_FEXT(usbRegisters->DEVCTL, USB_DEVCTL_VBUS) !=
		                   CSL_USB_DEVCTL_VBUS_ABOVEVBUSVALID);

			CSL_USB_DEBUG_PRINT("USB Cable Connection Detected!!\n");

			usbDevDisconnect = FALSE;
			USB_connectDev(gUsbDevHandle);
		}
	}

	if(!fExitMainTaskOnUSBError)
	{

		pContext->fMUSBIsReady = FALSE;

		/* must wait 500 ms for the VBus go down,
		then the MUSB can go to suspend */
		TSK_sleep(500*1);

		/* suspend the MUSB to save power */
		/* Ack for exit this task */
		SEM_post(&SEM_MUSBMainTaskExited);
	}
	else /* something wrong with USB*/
	{
		;
	}
}


/**
 *  \brief  USB Msc task
 *
 *  This task takes care of transferring the data between media
 *  and USB device
 *
 *  \param  None
 *
 *  \return None
 */
void MSCTask(void)
{
	CSL_UsbMscMsg    wMSCMsg;
    pUsbContext 	 pContext;
    volatile WORD 	 Msg;
	pUsbEpStatus 	 peps;

	pContext = &gUsbContext;

	while(TRUE)
	{
        /* wait for mailbox to be posted */
        MBX_pend(&MBX_msc, &wMSCMsg, SYS_FOREVER);
        Msg = wMSCMsg;

		if(Msg == CSL_USB_MSG_MSC_TASK_EXIT)
		{
			break;
		}

		switch(Msg)
		{
			case CSL_USB_MSG_MSC_CTL:
				peps = &pContext->pEpStatus[CSL_USB_EP0];
				if(peps->hEventHandler)
				{
					(*peps->hEventHandler)();
				}
				break;

			case CSL_USB_MSG_MSC_BULK_IN:
				peps = &pContext->pEpStatus[CSL_USB_EP1];
				if(peps->hEventHandler)
				{
					(*peps->hEventHandler)();
				}
				break;

			case CSL_USB_MSG_MSC_BULK_OUT:
				peps = &pContext->pEpStatus[CSL_USB_EP2];
				if(peps->hEventHandler)
				{
					(*peps->hEventHandler)();
				}
				break;

			default:
				break;
		}
	}

	/* Ack for exit this task */
	SEM_post(&SEM_MUSBMSCTaskExited);
}

/**
 *  \brief  USB Isr to post Message to main task
 *
 *  \param  None
 *
 *  \return None
 */
void USB_MUSB_Isr(void)
{
	CSL_UsbMsgObj    USBMsg;

	USBMsg.wMsg = CSL_USB_MSG_USB_INT;

    /* enqueue message */
	MBX_post(&MBX_musb, &USBMsg, 0);
}

/**
 *  \brief  USB interrupt handler
 *
 *  This function identifies the interrupt generated by the
 *  USB device and calls corresponding function to service
 *  the interrupt
 *
 *  \param  None
 *
 *  \return None
 */
static Bool HandleUSBInterrupt(pUsbContext pContext)
{
	/* If MUSB is not ready, do nothing */
	if(!pContext->fMUSBIsReady)
	{
		return FALSE;
	}

	if(pContext->dwIntSourceH & CSL_USB_GBL_INT_RESET)
	{
		usbRegisters->INDEX_TESTMODE = 0x0;
		USB_flushFifo(CSL_USB_EP0, TRUE);
	}

	if(pContext->dwIntSourceH & CSL_USB_GBL_INT_RESUME)
	{
		/* Resume interrupt */
		MUSB_Handle_Resume_Intr();
	}

	if(pContext->dwIntSourceL & CSL_USB_TX_RX_INT_EP0)
	{
		/* ep0 interrupt */
		MUSB_Handle_EP0_Intr(pContext);

	    /* post a message to the MSC task only when
		  there is a setup packet available */
		if(pContext->fSetupPktCmd == TRUE)
		{
		    pContext->fSetupPktCmd = FALSE;
            if (pContext->cableState == CSL_USB_DEVICE_DETACH)
			{
			    if(!StartDevice(pContext))
				{
				  /* fExitMainTaskOnUSBError = TRUE; */
				}
			}

			DeviceNotification(pContext, CSL_USB_EVENT_SETUP);
		}
    }

	/* BULK OUT, RX endpoint 2 */
	if(pContext->dwIntSourceL & CSL_USB_RX_INT_EP2)
	{
		/* rx interrupt (bulk out) */
		USB_handleRxIntr(pContext);

	}

	/* BULK IN, TX endpoint 1 */
	if(pContext->dwIntSourceL & CSL_USB_TX_INT_EP1)
	{
		/* tx interrupt (bulk in) */
		/* Tx interrupt is handled in different way: PR Mistral */
		//MUSB_Handle_TX_Intr();
	}

	if(pContext->dwIntSourceH & CSL_USB_GBL_INT_DEVDISCONN)
	{
		/* disconnect interrupt */
		USB_disconnectDev(gUsbDevHandle);
		usbDevDisconnect = TRUE;
	}

	if(pContext->dwIntSourceH & CSL_USB_GBL_INT_SUSPEND)
	{
		/* suspend interrupt */
		USB_suspendDevice(gUsbDevHandle);
	}

	return (TRUE);
}

/**
 *  \brief  function to handle resume interrupt
 *
 *  \param  None
 *
 *  \return None
 */
void MUSB_Handle_Resume_Intr()
{
	/* set the resume bit */
   	usbRegisters->FADDR_POWER |= CSL_USB_FADDR_POWER_RESUME_MASK;

	/* wait for 10ms for musb to generate resume signaling on the bus */
	genDelay(10);

	/* clear the resume bit */
   	usbRegisters->FADDR_POWER &= ~(CSL_USB_FADDR_POWER_RESUME_MASK);
}

/**
 *  \brief  USB delay function
 *
 *  \param  dwMicroSeconds delay in micro secs
 *
 *  \return None
 */
void genDelay(DWORD dwMicroSeconds)
{
	volatile DWORD dwmsecs;
	volatile DWORD delay;
	volatile DWORD looper;

	dwmsecs = dwMicroSeconds;
	delay = 0;

	while(dwmsecs--)
	{
		/* CLOCK = 100 MHz */
		for (looper = 0; looper < 5; looper++)
		{
			delay++;
		}
	}
}

/**
 *  \brief  Function to complete the MSC data transfer
 *
 *  This function completes the data transfer between USB device
 *  and host. This is an important function which is called from
 *  MUSB layer using call back pointer
 *
 *  \param  pContext  - USB context structure pointer
 *  \param  peps      - End point status structure pointer
 *
 *  \return None
 */
CSL_Status CompleteTransfer(void    *vpContext,
                            void    *vpeps)
{
	pUsbContext     pContext;
    pUsbEpStatus    peps;
    CSL_UsbMscMsg   wMSCMsg;
    pUsbTransfer    pTransfer;

    pContext  = (pUsbContext)vpContext;
    peps      = (pUsbEpStatus)vpeps;
	pTransfer = peps->pTransfer;

    if((pTransfer != NULL) && (pContext != NULL))
	{
         pTransfer->fComplete = TRUE;
         peps->pTransfer = NULL;
    }

    peps->wUSBEvents |= CSL_USB_EVENT_EOT;

    switch(peps->dwEndpoint)
    {
         case  CSL_USB_EP0:
             wMSCMsg = CSL_USB_MSG_MSC_CTL;
    	  	 /* enqueue message */
		  	 MBX_post(&MBX_msc, &wMSCMsg, SYS_FOREVER);
		  	 break;

         case  CSL_USB_EP1:
              wMSCMsg = CSL_USB_MSG_MSC_BULK_IN;
              /* enqueue message */
              MBX_post(&MBX_msc, &wMSCMsg, SYS_FOREVER);
              break;

         case  CSL_USB_EP2:
              wMSCMsg = CSL_USB_MSG_MSC_BULK_OUT;
              /* enqueue message */
              MBX_post(&MBX_msc, &wMSCMsg, SYS_FOREVER);
              break;

         default:
              break;
    }

    return(CSL_SOK);
}

/**
 *  \brief  Function to send device notification
 *
 *  \param  pContext    - USB context structure pointer
 *  \param  wUSBEvents  - USB events
 *
 *  \return None
 */
void DeviceNotification(pUsbContext    pContext,
				        WORD           wUSBEvents)
{
    CSL_UsbMscMsg    wMSCMsg;
    pUsbEpStatus     peps;

    peps = &pContext->pEpStatus[CSL_USB_EP0];
    peps->wUSBEvents |= wUSBEvents;

    wMSCMsg = CSL_USB_MSG_MSC_CTL;
    /* enqueue message */
    MBX_post(&MBX_msc, &wMSCMsg, SYS_FOREVER);

    if(wUSBEvents & CSL_USB_EVENT_RESET)
    {
		peps = &pContext->pEpStatus[CSL_USB_EP1];
		peps->wUSBEvents |= wUSBEvents;

		wMSCMsg = CSL_USB_MSG_MSC_BULK_IN;
		/* enqueue message */
		MBX_post(&MBX_msc, &wMSCMsg, SYS_FOREVER);

		peps = &pContext->pEpStatus[CSL_USB_EP2];
		peps->wUSBEvents |= wUSBEvents;

		wMSCMsg = CSL_USB_MSG_MSC_BULK_OUT;
		/* enqueue message */
		MBX_post(&MBX_msc, &wMSCMsg, SYS_FOREVER);
    }
}

/**
 *  \brief  Function to stop the USB device
 *
 *  \param  pContext - USB context structure pointer
 *
 *  \return TRUE  - Operation success
 *          FALSE - Invalid input parameter
 */
Bool StopDevice(pUsbContext pContext)
{
	pUsbEpStatus peps;
    DWORD dwEndpoint;

    if(pContext == NULL)
	{
        return (FALSE);
	}

    for (dwEndpoint = 0; dwEndpoint < CSL_USB_ENDPOINT_COUNT; ++dwEndpoint)
    {
	    peps = &pContext->pEpStatus[dwEndpoint];
		if(peps && peps->fInitialized)
		{
		    if(peps && peps->pTransfer)
			{
		    	peps->pTransfer->fComplete = TRUE;
				peps->pTransfer = NULL;
			}

			peps->fStalled = FALSE;
	    	peps->fInitialized = FALSE;
		}
    }

   	pContext->cableState = CSL_USB_DEVICE_DETACH;
	DeviceNotification(pContext, CSL_USB_EVENT_SUSPEND);

    return(TRUE);
}

/**
 *  \brief  Function to start the USB device
 *
 *  \param  pContext - USB context structure pointer
 *
 *  \return TRUE  - Operation success
 *          FALSE - Invalid input parameter
 */
Bool StartDevice(pUsbContext pContext)
{
	pUsbEpStatus    peps;
    DWORD           dwEndpoint;

    if(pContext == NULL)
	{
        return(FALSE);
	}

    if(!USB_checkSpeed(pContext, &pContext->busSpeed))
	{
    	return(FALSE);
	}

    for (dwEndpoint = 0; dwEndpoint < CSL_USB_ENDPOINT_COUNT; ++dwEndpoint)
    {
		if(peps && peps->fSelected)
		{
			USB_configEndpointDataSize(pContext, dwEndpoint);
		}
    }

    if(pContext->cableState != CSL_USB_DEVICE_ATTACH)
    {
        pContext->cableState = CSL_USB_DEVICE_ATTACH;
		DeviceNotification(pContext, CSL_USB_EVENT_RESUME);
    }

    /* Send Reset */
    DeviceNotification(pContext, CSL_USB_EVENT_RESET);

    return(TRUE);
}

/**
 *  \brief  Function to handle Ep0 interrupts
 *
 *  \param  pContext - USB context structure pointer
 *
 *  \return None
 */
void  MUSB_Handle_EP0_Intr(pUsbContext pContext)
{

    pUsbEpStatus 	  	  peps;
	CSL_UsbSetupStruct    *usbSetup;
	Uint16                flag;
	pMscClassHandle       pHandle;

	usbSetup = &pContext->usbSetup;
	peps = &pContext->pEpStatus[CSL_USB_EP0];

	pHandle = (pMscClassHandle)(MSC_AppHandle.pMscObj);

	/* set index register = 0 to select EP0 */
	usbRegisters->INDEX_TESTMODE &= ~CSL_USB_INDEX_TESTMODE_EPSEL_MASK;
   	usbRegisters->INDEX_TESTMODE = CSL_USB_EP0;

	/* check if STALLed or SetupEnd -- error condition */
    if(
       ((usbRegisters->PERI_CSR0_INDX & CSL_USB_PERI_CSR0_INDX_SENTSTALL_MASK) ==
         CSL_USB_PERI_CSR0_INDX_SENTSTALL_MASK) ||
        ((usbRegisters->PERI_CSR0_INDX & CSL_USB_PERI_CSR0_INDX_SETUPEND_MASK) ==
          CSL_USB_PERI_CSR0_INDX_SETUPEND_MASK)
      )
	{
		pContext->ep0State = CSL_USB_EP0_IDLE;
		//return;
	}

	if(pContext->ep0State == CSL_USB_EP0_STATUS_IN)
	{
	    if(pContext->usbSetup.bRequest == CSL_USB_SET_ADDRESS)
		{
			/* ask the main task to process it */
			pContext->fSetupPktCmd = TRUE;
			peps->wUSBEvents |= CSL_USB_EVENT_SETUP;
		}

	    pContext->ep0State = CSL_USB_EP0_IDLE;

        return;
	}

	if(CSL_USB_EP0_IDLE == pContext->ep0State)
	{
        if((usbRegisters->PERI_CSR0_INDX & CSL_USB_PERI_CSR0_INDX_RXPKTRDY_MASK)
            == CSL_USB_PERI_CSR0_INDX_RXPKTRDY_MASK)
		{
			/* setup packet received */
			USB_getSetupPacket(gUsbDevHandle, usbSetup, TRUE);

			/* check the request type */
			switch(pContext->usbSetup.bRequest)
			{
				/* zero data */
				case CSL_USB_SET_FEATURE:
				case CSL_USB_CLEAR_FEATURE:
				case CSL_USB_SET_CONFIGURATION:
				case CSL_USB_SET_INTERFACE:

					/* ask the main task to process it */
					pContext->fSetupPktCmd = TRUE;
					peps->wUSBEvents |= CSL_USB_EVENT_SETUP;

					/* DataEnd + ServicedRxPktRdy */
					flag = (CSL_USB_PERI_CSR0_INDX_SERV_RXPKTRDY_MASK |
					        CSL_USB_PERI_CSR0_INDX_DATAEND_MASK);
					//usbRegisters->PERI_CSR0_INDX |= CSL_USB_PERI_CSR0_INDX_SERV_RXPKTRDY_MASK;
					//usbRegisters->PERI_CSR0_INDX |= CSL_USB_PERI_CSR0_INDX_DATAEND_MASK;
					usbRegisters->PERI_CSR0_INDX |= flag;
          			pContext->ep0State = CSL_USB_EP0_STATUS_IN;
					/* no state change */
					break;

				case CSL_USB_SET_ADDRESS:

					flag = (CSL_USB_PERI_CSR0_INDX_SERV_RXPKTRDY_MASK |
					        CSL_USB_PERI_CSR0_INDX_DATAEND_MASK);
					//usbRegisters->PERI_CSR0_INDX |= CSL_USB_PERI_CSR0_INDX_SERV_RXPKTRDY_MASK;
					//usbRegisters->PERI_CSR0_INDX |= CSL_USB_PERI_CSR0_INDX_DATAEND_MASK;
					usbRegisters->PERI_CSR0_INDX |= flag;

           			pContext->ep0State = CSL_USB_EP0_STATUS_IN;
					/* no state change */
					break;

				case CSL_USB_SET_DESCRIPTOR:

					/* ask the main task to process it */
					pContext->fSetupPktCmd = TRUE;
					peps->wUSBEvents |= CSL_USB_EVENT_SETUP;

					/* ServicedRxPktRdy */
					usbRegisters->PERI_CSR0_INDX |= CSL_USB_PERI_CSR0_INDX_SERV_RXPKTRDY_MASK;

					/* write request; move to rx state */
					pContext->ep0State = CSL_USB_EP0_RX;
					break;

				case CSL_USB_GET_CONFIGURATION:
				case CSL_USB_GET_INTERFACE:
				case CSL_USB_GET_DESCRIPTOR:
				case CSL_USB_GET_STATUS:
				case CSL_USB_SYNCH_FRAME:
				case CSL_USB_GET_MAX_LUN:

					/* ask the main task to process it */
					pContext->fSetupPktCmd = TRUE;
					peps->wUSBEvents |= CSL_USB_EVENT_SETUP;

					/* ServicedRxPktRdy */

					/* read request; move to tx state */
					pContext->ep0State = CSL_USB_EP0_TX;
					usbRegisters->PERI_CSR0_INDX |= CSL_USB_PERI_CSR0_INDX_SERV_RXPKTRDY_MASK;
					break;

				case CSL_USB_BOT_MSC_RESET:

					if ((pContext->usbSetup.wIndex == 0) &&
					    (pContext->usbSetup.wLength == 0) &&
	    				(pContext->usbSetup.wValue == 0))
	    			{
						/* ask the main task to process it */
						pContext->fSetupPktCmd = TRUE;
						peps->wUSBEvents |= CSL_USB_EVENT_SETUP;
					}
					else
					{
						USB_stallEndpt(pHandle->ctrlHandle.ctrlOutEpHandle);
					}

					flag = (CSL_USB_PERI_CSR0_INDX_SERV_RXPKTRDY_MASK |
					        CSL_USB_PERI_CSR0_INDX_DATAEND_MASK);

					/* DataEnd + ServicedRxPktRdy */
					//usbRegisters->PERI_CSR0_INDX |= CSL_USB_PERI_CSR0_INDX_SERV_RXPKTRDY_MASK;
					//usbRegisters->PERI_CSR0_INDX |= CSL_USB_PERI_CSR0_INDX_DATAEND_MASK;
					usbRegisters->PERI_CSR0_INDX |= flag;

          			pContext->ep0State = CSL_USB_EP0_STATUS_IN;
					/* no state change */
					break;

				default:

					/* something's wrong */
					break;
			}
		}

		return;
	}

	if(CSL_USB_EP0_TX == pContext->ep0State)
	{
		pContext->fWaitingOnEP0BUFAvail = TRUE;
		return;
	}

	if(CSL_USB_EP0_RX == pContext->ep0State)
	{
        if((usbRegisters->PERI_CSR0_INDX & CSL_USB_PERI_CSR0_INDX_RXPKTRDY_MASK) ==
         CSL_USB_PERI_CSR0_INDX_RXPKTRDY_MASK)
		{
			USB_readEP0Buf(pContext, pContext->dwOutEP0Buf);

			//pContext->fOutPhaseCmd = TRUE;
			USB_processEP0Out(pContext);

			/* check if data end bit is set */
            if((usbRegisters->PERI_CSR0_INDX & CSL_USB_PERI_CSR0_INDX_DATAEND_MASK) ==
            CSL_USB_PERI_CSR0_INDX_DATAEND_MASK)
			{
				pContext->ep0State = CSL_USB_EP0_IDLE;
			}
			/* else remain in rx state */
		}

		return;
	}
}

/**
 *  \brief  Function to start USB data transfer
 *
 *  This functions starts the USB data transfer to read the data
 *  from an end point or to write data to an end point
 *  This is a very important function which is called from the MUSB
 *  layer using a call back pointer.
 *
 *  \param  pContext     - Pointer to the global MUSB controller context structure
 *  \param  peps         - Endpoint status structure pointer
 *
 *  \return TRUE  - Operation successful
 *          FALSE - Invalid input parameters
 */
CSL_Status StartTransfer(void    *vpContext,
                         void    *vpeps)
{
	pUsbContext      pContext;
	pUsbEpStatus     peps;
	pUsbTransfer     pTransfer;
	CSL_UsbMsgObj	 USBMsg;

	pContext  = (pUsbContext)vpContext;
	peps      = (pUsbEpStatus)vpeps;
	pTransfer = peps->pTransfer;

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

    peps->pTransfer = pTransfer;
    pTransfer->fComplete=FALSE;
    if (pTransfer->dwFlags == CSL_USB_OUT_TRANSFER)
    {
		if(peps->dwEndpoint == CSL_USB_EP2)
		{ /*EP0 will be handled in other way */
			pContext->fWaitingOnFlagB = TRUE;
		}
    }
    else if (pTransfer->dwFlags == CSL_USB_IN_TRANSFER)
    {
        if (peps->dwEndpoint == CSL_USB_EP0 )
		{
			pContext->fWaitingOnEP0BUFAvail = TRUE;
			if(peps->pTransfer->cbBuffer)
			{
				pContext->fEP0BUFAvailable = TRUE;
			}
		}
        else if (peps->dwEndpoint == CSL_USB_EP1)
		{
			pContext->fWaitingOnFlagA = TRUE;
		}
    }
	else
	{
		;
	}

	/* Just inform the main task */
  	USBMsg.wMsg = CSL_USB_MSG_DATA;
    /* enqueue message */
  	MBX_post(&MBX_musb, &USBMsg, SYS_FOREVER);

	return(CSL_SOK);
}

/**
 *  \brief  Function to start USB data transfer
 *
 *  \param  pContext     - Pointer to the global MUSB controller context structure
 *  \param  dwEndpoint   - Endpoint Number
 *
 *  \return TRUE  - Operation successful
 *          FALSE - Invalid input parameters
 */
void USB_configEndpointDataSize(pUsbContext    pContext,
                                DWORD         dwEndpoint)
{
	pUsbEpStatus    peps;

	peps = &pContext->pEpStatus[dwEndpoint];

	switch(dwEndpoint)
	{
		case CSL_USB_EP0:
			peps->dwPacketSizeAssigned = CSL_USB_EP0_PACKET_SIZE;
			break;

		case CSL_USB_EP1:
			if (pContext->busSpeed == CSL_USB_BS_HIGH_SPEED)
			{
				peps->dwPacketSizeAssigned = CSL_USB_EP1_PACKET_SIZE_HS;
			}
			else
			{
				peps->dwPacketSizeAssigned = CSL_USB_EP1_PACKET_SIZE_FS;
			}

			break;

		case CSL_USB_EP2:
			if (pContext->busSpeed == CSL_USB_BS_HIGH_SPEED)
			{
				peps->dwPacketSizeAssigned = CSL_USB_EP2_PACKET_SIZE_HS;
			}
			else
			{
				peps->dwPacketSizeAssigned = CSL_USB_EP2_PACKET_SIZE_FS;
			}

			break;

		case CSL_USB_EP3:
			if (pContext->busSpeed == CSL_USB_BS_HIGH_SPEED)
			{
				peps->dwPacketSizeAssigned = CSL_USB_EP3_PACKET_SIZE_HS;
			}
			else
			{
				peps->dwPacketSizeAssigned = CSL_USB_EP3_PACKET_SIZE_FS;
			}

			break;

		case CSL_USB_EP4:
			if (pContext->busSpeed == CSL_USB_BS_HIGH_SPEED)
			{
				peps->dwPacketSizeAssigned = CSL_USB_EP4_PACKET_SIZE_HS;
			}
			else
			{
				peps->dwPacketSizeAssigned = CSL_USB_EP4_PACKET_SIZE_FS;
			}

			break;

		default:
			break;
	}
}

/**
 *  \brief  USB Interrupt Service Routine
 *
 *  \param  none
 *
 *  \return none
 */
void USBisr()
{
	// clear the USB interrupts
	gUsbContext.dwIntSourceL = usbRegisters->INTMASKEDR1;
	usbRegisters->INTCLRR1 = gUsbContext.dwIntSourceL;
	gUsbContext.dwIntSourceH = usbRegisters->INTMASKEDR2;
	usbRegisters->INTCLRR2 = gUsbContext.dwIntSourceH;

	USB_MUSB_Isr();

	/* all interrupts are handled, signal 'End Of Interrupt' */
	usbRegisters->EOIR = CSL_USB_EOIR_RESETVAL;
}

/**
 *  \brief  USB suspend call back function
 *
 *  \param  status
 *
 *  \return none
 */
void CSL_suspendCallBack(CSL_Status    status)
{
	;
}

/**
 *  \brief  USB self wakeup call back function
 *
 *  \param  status
 *
 *  \return none
 */
void CSL_selfWakeupCallBack(CSL_Status    status)
{
	;
}

/**
 *  \brief  MMCSD Configuration function.
 *
 *   This function configures CSL MMCSD module and makes the
 *   MMCSD hardware ready for USB data transfers. This function
 *   only works with SD card.
 *
 *  \param  none
 *
 *  \return Test result
 */
CSL_Status configSdCard (CSL_MMCSDOpMode    opMode)
{
	CSL_Status      status;
	Uint16		    actCard;
	CSL_CardType    cardType;
	Uint16          cardAddr;
    Uint16          clockDiv;

	cardType = CSL_CARD_NONE;

	/* Initialize the CSL MMCSD module*/
	status = MMC_init();
	if(status != CSL_SOK)
	{
		CSL_USB_DEBUG_PRINT("MMC_init Failed\n");
		return (status);
	}

    status = SYS_setEBSR(CSL_EBSR_FIELD_SP0MODE,
                         CSL_EBSR_SP0MODE_0);
    status |= SYS_setEBSR(CSL_EBSR_FIELD_SP1MODE,
                          CSL_EBSR_SP1MODE_0);
    if(CSL_SOK != status)
    {
        printf("SYS_setEBSR failed\n");
        return (status);
    }

	status = CSL_ESYS_FAIL;

#ifdef C5515_EZDSP
	mmcsdHandle = MMC_open(&pMmcsdContObj, CSL_MMCSD1_INST, opMode, &status);
#else
	mmcsdHandle = MMC_open(&pMmcsdContObj, CSL_MMCSD0_INST, opMode, &status);
#endif
	if(mmcsdHandle == NULL)
	{
		CSL_USB_DEBUG_PRINT("MMC_open Failed\n");
		return (status);
	}

	if(opMode == CSL_MMCSD_OPMODE_DMA)
	{
		status = configDmaforMmcSd();
		if(status != CSL_SOK)
		{
			CSL_USB_DEBUG_PRINT("Configuring DMA for MMCSD Failed\n");
			return (status);
		}
		else
		{
			CSL_USB_DEBUG_PRINT("Configuring DMA for MMCSD Successful\n");
		}
	}

	status = MMC_sendGoIdle(mmcsdHandle);
	if(status != CSL_SOK)
	{
		CSL_USB_DEBUG_PRINT("MMC_sendGoIdle Failed\n");
		return (status);
	}

    status = MMC_selectCard(mmcsdHandle, &mmcCardObj);
	if((status == CSL_ESYS_BADHANDLE) ||
	   (status == CSL_ESYS_INVPARAMS))
	{
		CSL_USB_DEBUG_PRINT("MMC_selectCard Failed\n");
		return (status);
	}

	if(mmcCardObj.cardType == CSL_SD_CARD)
	{
		CSL_USB_DEBUG_PRINT("SD Card Detected!\n");
		cardType = CSL_SD_CARD;

		if(mmcCardObj.sdHcDetected == TRUE)
		{
			CSL_USB_DEBUG_PRINT("SD Card is High Capacity Card!\n");
		}
		else
		{
			CSL_USB_DEBUG_PRINT("SD Card is Standard Capacity Card!\n");
		}

        clockDiv = computeClkRate(SD_CLOCK_DIV);
	}
	else
	{
		CSL_USB_DEBUG_PRINT("NO SD Card Detected!\n");
		status = CSL_ESYS_MEDIA_NOTPRESENT;
		return(status);
	}

    status = MMC_sendOpCond(mmcsdHandle, 70);
	if(status != CSL_SOK)
	{
		CSL_USB_DEBUG_PRINT("MMC_sendOpCond Failed\n");
		return (status);
	}

	status = SD_sendAllCID(mmcsdHandle, &sdCardIdObj);
	if(status != CSL_SOK)
	{
		CSL_USB_DEBUG_PRINT("SD_sendAllCID Failed\n");
		return (status);
	}

	status = SD_sendRca(mmcsdHandle, &mmcCardObj, &cardAddr);
	if(status != CSL_SOK)
	{
		CSL_USB_DEBUG_PRINT("SD_sendRca Failed\n");
		return (status);
	}

	status = SD_getCardCsd(mmcsdHandle, &sdCardCsdObj);
	if(status != CSL_SOK)
	{
		CSL_USB_DEBUG_PRINT("SD_getCardCsd Failed\n");
		return (status);
	}

    status = MMC_sendOpCond(mmcsdHandle, clockDiv);
	if(status != CSL_SOK)
	{
		CSL_USB_DEBUG_PRINT("MMC_sendOpCond Failed\n");
		return (status);
	}

	status = MMC_setCardType(&mmcCardObj, cardType);
	if(status != CSL_SOK)
	{
		CSL_USB_DEBUG_PRINT("MMC_setCardType Failed\n");
		return (status);
	}

	status = MMC_setCardPtr(mmcsdHandle, &mmcCardObj);
	if(status != CSL_SOK)
	{
		CSL_USB_DEBUG_PRINT("MMC_setCardPtr Failed\n");
		return (status);
	}

	status = MMC_getNumberOfCards(mmcsdHandle, &actCard);
	if(status != CSL_SOK)
	{
		CSL_USB_DEBUG_PRINT("MMC_getNumberOfCards Failed\n");
		return (status);
	}

	status = MMC_setEndianMode(mmcsdHandle, CSL_MMCSD_ENDIAN_LITTLE,
	                           CSL_MMCSD_ENDIAN_LITTLE);
	if(status != CSL_SOK)
	{
		CSL_USB_DEBUG_PRINT("MMC_setEndian Failed\n");
		return (status);
	}

	/* Set block length for the memory card
	 * For high capacity cards setting the block length will have
	 * no effect
	 */
	status = MMC_setBlockLength(mmcsdHandle, CSL_MMCSD_BLOCK_LENGTH);
	if(status != CSL_SOK)
	{
		printf("API: MMC_setBlockLength Failed\n");
		return(status);
	}

	return (CSL_SOK);
}


/**
 *  \brief  Configures DMA module for MMCSD data transfers
 *
 *   This function configures two DMA channels for MMCSD read
 *   and write operation.
 *
 *  \param  none
 *
 *  \return Test result
 */
CSL_Status configDmaforMmcSd(void)
{
	CSL_Status status;

	status = CSL_SOK;

#ifdef CSL_MMCSD_INTR_ENABLE

	/* Disable CPU interrupts */
    IRQ_globalDisable();

#ifdef C5515_EZDSP
	IRQ_clear(PROG2_EVENT);

	/* Enable MMCSD interrupts */
	IRQ_enable(PROG2_EVENT);
#else
	IRQ_clear(PROG0_EVENT);

	/* Enable MMCSD interrupts */
	IRQ_enable(PROG0_EVENT);
#endif
	/* Enable CPU interrupts */
	IRQ_globalEnable();

#endif

	/* Initialize Dma */
    status = DMA_init();
    if (status != CSL_SOK)
    {
        CSL_USB_DEBUG_PRINT("CSL DMA module initialization Failed!\n");
    }

	/* Open Dma channel for MMCSD write */
	dmaWrHandle = DMA_open(CSL_DMA_CHAN0, &dmaWrChanObj, &status);
    if(dmaWrHandle == NULL)
    {
        CSL_USB_DEBUG_PRINT("DMA Channel Open for MMCSD Write Failed!\n");
		status = CSL_ESYS_FAIL;
		return(status);
    }

	/* Open Dma channel for MMCSD read */
	dmaRdHandle = DMA_open(CSL_DMA_CHAN1, &dmaRdChanObj, &status);
    if(dmaRdHandle == NULL)
    {
        CSL_USB_DEBUG_PRINT("DMA Channel Open for MMCSD Read Failed!\n");
		status = CSL_ESYS_FAIL;
		return(status);
    }

	/* Set the DMA handle for MMC read */
	status = MMC_setDmaHandle(mmcsdHandle, dmaWrHandle, dmaRdHandle);
	if(status != CSL_SOK)
	{
		CSL_USB_DEBUG_PRINT("Setting Dma Handle for MMCSD Failed\n");
		return(status);
	}

#ifdef CSL_MMCSD_INTR_ENABLE

	/* Set DMA Call back function */
	status = MMC_setDataTransferCallback(mmcsdHandle, mmcsdDataCallback);
	if(status != CSL_SOK)
	{
		CSL_USB_DEBUG_PRINT("Setting MMCSD Data Transfer Callback Failed\n");
		return(status);
	}

#endif

	return(status);
}

/**
 *  \brief  MMCSD data call back function
 *
 *   This function is called from the MMCSD read and write APIs
 *   after starting the DMA data transfer
 *
 *  \param  none
 *
 *  \return none
 */
void mmcsdDataCallback(void *hMmcSd)
{
	/* Wait for the MMCSD interrupt */
	//SEM_pend(&SEM_mmcsd_dmaTransferDone,SYS_FOREVER);
	while(mmcsdTxferComplete != 1);

	mmcsdTxferComplete = 0;
}

/**
 *  \brief  MMCSD ISR
 *
 *  \param  none
 *
 *  \return none
 */
void mmcsd0_isr(void)
{
	MMC_intEnable(mmcsdHandle, 0x0000);
	//SEM_post(&SEM_mmcsd_dmaTransferDone);
	mmcsdTxferComplete = 1;
}

/**
 *  \brief  DMA ISR function;Not in use
 *
 *  \param  none
 *
 *  \return none
 */
void dma_isr(void)
{

}

/**
 *  \brief  Function to check if the previous transfer is done.
 *
 *  \param  peps        - End point status structure pointer
 *  \param  dwEndpoint  - End point status structure pointer
 *
 *  \return Value of TxPktRdy bit
 */
Bool checkTxDone(pUsbEpStatus    peps,
                 DWORD           dwEndpoint)
{
	Uint16    saveIndex;
	Uint16    flag;

	saveIndex = 0;
	flag      = FALSE;

	/* Save the index register value */
	saveIndex = usbRegisters->INDEX_TESTMODE;

	usbRegisters->INDEX_TESTMODE &= ~CSL_USB_INDEX_TESTMODE_EPSEL_MASK;
    usbRegisters->INDEX_TESTMODE |= dwEndpoint;

	/* Check if TxPktRdy is set */
    if((usbRegisters->PERI_CSR0_INDX & CSL_USB_PERI_TXCSR_INDX_TXPKTRDY_MASK) ==
        CSL_USB_PERI_TXCSR_INDX_TXPKTRDY_MASK)
	{
		flag = FALSE;
	}
	else
	{
		flag = TRUE;
	}

	/* Restore the index register */
    usbRegisters->INDEX_TESTMODE = saveIndex;

	return(flag);
}

/**
 *  \brief    Function to calculate the memory clock rate
 *
 * This function computes the memory clock rate value depending on the
 * CPU frequency. This value is used as clock divider value for
 * calling the API MMC_sendOpCond(). Value of the clock rate computed
 * by this function will change depending on the system clock value
 * and MMC maximum clock value passed as parameter to this function.
 * Minimum clock rate value returned by this function is 0 and
 * maximum clock rate value returned by this function is 255.
 * Clock derived using the clock rate returned by this API will be
 * the nearest value to 'memMaxClk'.
 *
 *  \param    memMaxClk  - Maximum memory clock rate
 *
 *  \return   MMC clock rate value
 */
Uint16 computeClkRate(Uint32 memMaxClk)
{
	Uint32    sysClock;
	Uint32    remainder;
	Uint16    clkRate;

	sysClock  = 0;
	remainder = 0;
	clkRate   = 0;

	/* Get the clock value at which CPU is running */
	sysClock = getSysClk();

	if (sysClock > memMaxClk)
	{
		if (memMaxClk != 0)
		{
			clkRate   = sysClock / memMaxClk;
			remainder = sysClock % memMaxClk;

            /*
             * If the remainder is not equal to 0, increment clock rate to make
             * sure that memory clock value is less than the value of
             * 'memMaxClk'.
             */
			if (remainder != 0)
			{
				clkRate++;
			}

            /*
             * memory clock divider '(2 * (CLKRT + 1)' will always
             * be an even number. Increment the clock rate in case of
             * clock rate is not an even number.
             */
			if (clkRate%2 != 0)
			{
				clkRate++;
			}

			/*
			 * AT this point 'clkRate' holds the value of (2 * (CLKRT + 1).
			 * Get the value of CLKRT.
			 */
			clkRate = clkRate/2;
			clkRate = clkRate - 1;

			/*
			 * If the clock rate is more than the maximum allowed clock rate
			 * set the value of clock rate to maximum value.
			 * This case will become true only when the value of
			 * 'memMaxClk' is less than the minimum possible
			 * memory clock that can be generated at a particular CPU clock.
			 *
			 */
			if (clkRate > CSL_MMC_MAX_CLOCK_RATE)
			{
				clkRate = CSL_MMC_MAX_CLOCK_RATE;
			}
		}
		else
		{
			clkRate = CSL_MMC_MAX_CLOCK_RATE;
		}
	}

	return (clkRate);
}

