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

#ifndef _CSL_USB_HID_SAR_H_
#define _CSL_USB_HID_SAR_H_

CSL_Status SAR_initInstance(void);
CSL_Status SAR_closeInstance(void);

Uint16 SAR_GetKey(void);

#define SW6_MIN_VALUE  0x367
#define SW6_MAX_VALUE  0x374

#define SW7_MIN_VALUE  0x24F
#define SW7_MAX_VALUE  0x258

#define SW8_MIN_VALUE  0x30B
#define SW8_MAX_VALUE  0x316

#define SW9_MIN_VALUE  0x230
#define SW9_MAX_VALUE  0x238

#define SW10_MIN_VALUE 0x20F
#define SW10_MAX_VALUE 0x216

#define SW11_MIN_VALUE 0x1D3
#define SW11_MAX_VALUE 0x1D8

#define SW12_MIN_VALUE 0x163
#define SW12_MAX_VALUE 0x167

#define SW13_VALUE     0x000

#define SW14_MIN_VALUE 0x2BC
#define SW14_MAX_VALUE 0x2C6

#define SW15_MIN_VALUE 0xD5
#define SW15_MAX_VALUE 0xD8

#define NoKey_MIN_VALUE 0x3FB
#define NoKey_MAX_VALUE 0x3FF


#define UP_KEY          (1)
#define DOWN_KEY        (2)
#define RIGHT_KEY       (3)
#define LEFT_KEY        (4)
#define RIGHT_CLICK_KEY (5)
#define LEFT_CLICK_KEY  (6)
#define STOP_KEY        (7)
#define NO_KEY          (8)

#define ERROR_STATUS    (0x10)

#define NO_OF_ATTEMPTS  (10)
/**< This macro indicates the no of times the key values must be read for
     sending a single mouse command */

#endif /* _CSL_USB_HID_SAR_H_ */
