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

/*
 *  ======== tistdtypes.h ========
 */

/*
 * These types are also defined by other TI components.  They are bracketed
 * with _TI_STD_TYPES to avoid warnings for duplicate definition.
 *
 * You may get warnings about duplicate type definitions when using this
 * header file with earlier versions of DSP/BIOS and CSL.
 *
 * You can use the '-pds303' compiler option to suppress these warnings.
 */
#ifndef _TI_STD_TYPES
#define _TI_STD_TYPES

/*
 * This '#ifndef STD_' is needed to protect from duplicate definitions
 * of Int, Uns, etc. in DSP/BIOS v4.x (e.g. 4.90, 4.80) since these versions
 * of DSP/BIOS did not contain the '#ifndef_TI_STD_TYPES' logic.
 */
#ifndef STD_

/*
 * Aliases for standard C types
 */
typedef int                 Int;
typedef unsigned            Uns;
typedef char                Char;

/* pointer to null-terminated character sequence */
typedef char                *String;

typedef void                *Ptr;       /* pointer to arbitrary type */

typedef unsigned short      Bool;       /* boolean */

#endif /* STD_ */

/*
 * Uint8, Uint16, Uint32, etc are defined to be "smallest unit of
 * available storage that is large enough to hold unsigned or integer
 * of specified size".
 */
 #ifndef TRUE
  #define TRUE  1
  #define FALSE 0
#endif


/* Handle the 6x ISA */
#if defined(_TMS320C6X)
    /* Unsigned integer definitions (32bit, 16bit, 8bit) follow... */
    typedef unsigned int    Uint32;
    typedef unsigned short  Uint16;
    typedef unsigned char   Uint8;

    /* Signed integer definitions (32bit, 16bit, 8bit) follow... */
    typedef int             Int32;
    typedef short           Int16;
    typedef char            Int8;

/* Handle the 54x, 55x and 28x ISAs */
#elif defined(_TMS320C5XX) || defined(__TMS320C55X__) || defined(_TMS320C28X)
    /* Unsigned integer definitions (32bit, 16bit, 8bit) follow... */
    typedef unsigned long   Uint32;
    typedef unsigned short  Uint16;
    typedef unsigned char   Uint8;

    /* Signed integer definitions (32bit, 16bit, 8bit) follow... */
    typedef long            Int32;
    typedef short           Int16;
    typedef char            Int8;

#else
    /* Other ISAs not supported */
    #error <tistdtypes.h> is not supported for this target
#endif  /* defined(_6x_) */

#endif  /* _TI_STD_TYPES */

