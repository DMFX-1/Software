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

#include "csl_error.h"
#include "csl_intc.h"
#include "csl_usb.h"
#include "csl_hid.h"
#include "csl_hidAux.h"
#include "soc.h"
#include "csl_usbAux.h"
#include "tistdtypes.h"


#define STRING_DESCR_SZ         (7)  /**< string descriptor size  */

#define HID_REPORT_SIZE_BYTES   (3)  /**< report size in bytes */

#define HID_REPORT_SIZE_WORDS   ((HID_REPORT_SIZE_BYTES + 1)/2 )


Uint16 wValue  = 0;
Uint16 wIndex  = 0;
Uint16 wLength = 0;

Uint8  mouseOn = 0;
Uint8  usbOn = 0;
Uint16 keyStore;

//USB Device Descriptor: 18-bytes
const unsigned char deviceDesc[] =
{
    0x12,       // bLength = 18d
    0x01,       // bDeviceDescriptorType=1(defined by USB spec)
    0x00, 0x02, // bcdUSB USB Version 2.0 (L-Byte/H-Byte)
    0x00,       // bDeviceClass
    0x00,       // bDeviceSubclass
    0x00,       // bDeviceProtocol
    0x40,       // bMaxPacketSize 64 Bytes
    0xB3, 0x04, // idVendor(L-Byte/H-Byte)
    0x0B, 0x31, // idProduct(L-Byte/H-Byte)
    0x10, 0x01, // bcdDevice(L-Byte/H-Byte): device's release number
    0,          // iManufacturer String Index
    0,          // iProduct String Index
    0,          // iSerialNumber String Index
    1
};         // bNumberConfigurations

//Configuration Descriptors
const unsigned char cfgDesc[] =
{
   // CONFIGURATION Descriptor
   0x09,      // bLength
   0x02,      // bDescriptorType = Config (constant)
   0x22,  0x00, // wTotalLength(L/H)
   0x01,      // bNumInterfaces
   0x01,      // bConfigValue
   0x00,      // iConfiguration
   0xC0,      // bmAttributes;
   0x28,      // MaxPower is 80mA
   // INTERFACE Descriptor
   0x09,      // length = 9
   0x04,      // type = IF; constant 4 for INTERFACE
   0x00,      // IF #0; Interface Identification #
   0x00,      // bAlternate Setting
   0x01,      // bNumber of Endpoints = 1 for 5515 mouse
   0x03,      // bInterfaceClass = HID
   0x01,      // bInterfaceSubClass = Boot Interface
   0x02,      // bInterfaceProtocol = Mouse
   0x00,      // iInterface
   // HID Descriptor
   0x09,      // length = 9
   0x21,      // bDescriptorType = HID
   0x00,      // bcdHID = HID Class Spec version
   0x01,      //
   0x00,      //
   0x01,      // bNumDescriptors = Number of Decriptors
   0x22,      // bDescriptorType = 34 (REPORT)
   0x34,      // wDescriptorLength = 52
   0x00,      //
   // EndPoint_1_IN Descriptor
   0x07,      // bLength
   0x05,      // bDescriptorType (Endpoint)
   0x81,      // bEndpointAddress and direction(IN)
   0x03,      // bmAttributes: B1B0->transfer-type: control=00; Iso=01; bulk=10; interrupt=11
   0x04,0x00, // wMaxPacketSize(L/H)=4
   0x0A,      // bInterval: Max latency
};

const unsigned char OtherSpeedcfgDesc[] = 
{
   // CONFIGURATION Descriptor
   0x09,      // bLength
   0x07,      // bDescriptorType = Config (constant)
   0x22,  0x00, // wTotalLength(L/H)
   0x01,      // bNumInterfaces
   0x01,      // bConfigValue
   0x00,      // iConfiguration
   0xC0,      // bmAttributes;
   0x28,      // MaxPower is 80mA
   // INTERFACE Descriptor
   0x09,      // length = 9
   0x04,      // type = IF; constant 4 for INTERFACE
   0x00,      // IF #0; Interface Identification #
   0x00,      // bAlternate Setting
   0x01,      // bNumber of Endpoints = 1 for 5515 mouse
   0x03,      // bInterfaceClass = HID
   0x01,      // bInterfaceSubClass = Boot Interface
   0x02,      // bInterfaceProtocol = Mouse
   0x00,      // iInterface
   // HID Descriptor
   0x09,      // length = 9
   0x21,      // bDescriptorType = HID
   0x00,      // bcdHID = HID Class Spec version
   0x01,      //
   0x00,      //
   0x01,      // bNumDescriptors = Number of Decriptors
   0x22,      // bDescriptorType = 34 (REPORT)
   0x34,      // wDescriptorLength = 52
   0x00,      //
   // EndPoint_1_IN Descriptor
   0x07,      // bLength
   0x05,      // bDescriptorType (Endpoint)
   0x81,      // bEndpointAddress and direction(IN)
   0x03,      // bmAttributes: B1B0->transfer-type: control=00; Iso=01; bulk=10; interrupt=11
   0x04,0x00, // wMaxPacketSize(L/H)=4
   0x0A,      // bInterval: Max latency
};
								
Uint8 reportDesc[] = {0x05,0x01,0x09,0x02,0xa1,0x01,0x09,0x01,
                      0xa1,0x00,0x05,0x09,0x19,0x01,0x29,0x03,
                      0x15,0x00,0x25,0x01,0x95,0x03,0x75,0x01,
                      0x81,0x02,0x95,0x01,0x75,0x05,0x81,0x01,
                      0x05,0x01,0x09,0x30,0x09,0x31,0x09,0x38,
                      0x15,0x81,0x25,0x7f,0x75,0x08,0x95,0x03,
                      0x81,0x06,0xc0,0xc0};

const unsigned char strDesc[4][64] = {
//string 0
{
           4,  //>> length-in-byte
        0x03,  //   bDescriptorType = string
  0x09, 0x04,  //   English-USA
},

//string 1
{
    36, 0x03, //string-length-in-byte, Manufacture ID
   'T', 0x00,
   'e', 0x00,
   'x', 0x00,
   'a', 0x00,
   's', 0x00,
   ' ', 0x00,
   'I', 0x00,
   'n', 0x00,
   's', 0x00,
   't', 0x00,
   'r', 0x00,
   'u', 0x00,
   'm', 0x00,
   'e', 0x00,
   'n', 0x00,
   't', 0x00,
   's', 0x00,
},

//string 2
{
    12, 0x03, //string-length-in-byte, Product ID
   'C', 0x00,
   '5', 0x00,
   '5', 0x00,
   '0', 0x00,
   '5', 0x00,
},

//string 3
{
    12, 0x03, //string-length-in-byte, Serial# ID
   '0', 0x00,
   '0', 0x00,
   '0', 0x00,
   '0', 0x00,
   '1', 0x00
}
};

const Uint16 deviceQualifierDescr[5] =
{
    0x060A,
    0x0200,
    0x0000,
    0x4000,
    0x0001
};

//USB2.0 test data package, constants from the spec
const unsigned char test_packet[53] = {
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,
  0xAA,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,
  0xEE,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0x7F,0xBF,0xDF,
  0xEF,0xF7,0xFB,0xFD,0xFC,0x7E,0xBF,0xDF,
  0xEF,0xF7,0xFB,0xFD,0x7E};

const unsigned char *string_descriptor[STRING_DESCR_SZ] =
{
    &strDesc[0][0],     /* Language String */
    &strDesc[1][0],     /* iManufacturer */
    &strDesc[2][0],     /* iProductName */
    &strDesc[3][0],     /* iSerial No :RomId - F# */
    NULL,               /*  */
    NULL,               /*  */
    NULL
};

/* Buffer used to store the HID Report data to send to USB */
#pragma DATA_ALIGN(lbaBufferHidReportApp, 4);
Uint16 lbaBufferHidReportApp[HID_REPORT_SIZE_WORDS+1];

CSL_HidRequestStruct USB_ReqTable[] =
{
  {CSL_HID_REQUEST_GET_STATUS       , HID_reqGetStatus},
  {CSL_HID_REQUEST_CLEAR_FEATURE    , HID_reqClearFeature},
  {CSL_HID_REQUEST_SET_FEATURE      , HID_reqSetFeature},
  {CSL_HID_REQUEST_SET_ADDRESS      , HID_reqSetAddress},
  {CSL_HID_REQUEST_GET_DESCRIPTOR   , HID_reqGetDescriptor},
  {CSL_HID_REQUEST_SET_DESCRIPTOR   , HID_reqUnknown},
  {CSL_HID_REQUEST_GET_CONFIGURATION, HID_reqGetConfiguration},
  {CSL_HID_REQUEST_SET_CONFIGURATION, HID_reqSetConfiguration},
  {CSL_HID_REQUEST_GET_MAX_LUN      , HID_reqGetMaxLUN},
  {CSL_HID_REQUEST_GET_INTERFACE    , HID_reqGetInterface},
  {CSL_HID_REQUEST_SET_INTERFACE    , HID_reqSetInterface},
  {CSL_HID_REQUEST_SYNC_FRAME       , HID_reqUnknown},
  {CSL_HID_REQUEST_SET_CUR          , HID_reqUnknown},
  {CSL_HID_REQUEST_GET_CUR          , HID_reqUnknown},
  {CSL_HID_REQUEST_SET_MIN          , HID_reqUnknown},
  {CSL_HID_REQUEST_GET_MIN          , HID_reqUnknown},
  {CSL_HID_REQUEST_GET_MAX          , HID_reqUnknown},
  {CSL_HID_REQUEST_SET_MAX          , HID_reqUnknown},
  {CSL_HID_REQUEST_GET_RES          , HID_reqUnknown},
  {CSL_HID_REQUEST_SET_RES          , HID_reqUnknown},
  {CSL_HID_REQUEST_HID_GET_REPORT   , HID_reqHidGetReport},
  {CSL_HID_REQUEST_HID_GET_IDLE     , HID_reqUnknown},
  {CSL_HID_REQUEST_HID_GET_PROTOCOL , HID_reqUnknown},
  {CSL_HID_REQUEST_HID_SET_REPORT   , HID_reqUnknown},
  {CSL_HID_REQUEST_HID_SET_IDLE     , HID_reqUnknown},
  {CSL_HID_REQUEST_HID_SET_PROTOCOL , HID_reqUnknown},
  {0, NULL }
};

CSL_Status initializeUsbInstance (void);

CSL_Status CSL_usbHidTest(void);
interrupt void usb_isr(void);

/*
void do_setup(void);
void std_request(void);
void set_address(void);
void class_request(void);
void vendor_request(void);
void stall_bus(void);
*/

CSL_Status completeTransfer(void  *vpContext,
                            void  *vpeps);
CSL_Status startTransfer(void  *vpContext,
                         void  *vpeps);
