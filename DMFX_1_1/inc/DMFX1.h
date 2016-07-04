/*
 * DMFX1.h
 *
 *  Created on: Dec 12, 2015
 *      Author: Diego de la Cruz
 */

#ifndef DMFX1_H_
#define DMFX1_H_

#define DMFX_PROC1
#undef 	DMFX_PROC2
#undef 		USB_APP

#if defined(DMFX_PROC1)
	#define USE_SD0		// Serial Port 0 used for MMC/SD0 interface
	#define USE_SD1		// Serial Port 1 used for MMC/SD1 interface
#endif

typedef union short_long_u{
    signed short int s[2];
    signed long int l;
} short_long_t;

typedef union ushort_ulong_u{
    unsigned short int s[2];
    unsigned long int l;
} ushort_ulong_t;

#endif /* DMFX1_H_ */
