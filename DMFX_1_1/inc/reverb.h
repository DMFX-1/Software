/*****************************************************************************/
/*                                                                           */
/* FILENAME                                                                  */
/*  reverb.h                                                                 */
/*                                                                           */
/* DESCRIPTION                                                               */
/*   Header file to generate a reverb                                        */
/*   for the TMS320C5505 USB Stick.                                          */                                               
/*                                                                           */
/* REVISION                                                                  */
/*   Revision: 1.00                                                          */
/*   Author  : Diego de la Cruz                                              */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* HISTORY                                                                   */
/*   Revision 1.00                                                           */
/*   15/09/2013                                                              */
/*                                                                           */
/*****************************************************************************/
#include "csl_types.h"

#ifndef REVERB_H
#define REVERB_H
// 6-Low Pass Comb filters register sizes (maximum delay)
#define cD1max 2400 //50 ms
#define cD2max 2688 //56 ms
#define cD3max 2928 //61 ms
#define cD4max 3264 //68 ms
#define cD5max 3456 //72 ms
#define cD6max 3744 //78 ms

// All-Pass filter register size (maximum delay)
#define aDmax   288  //6 ms

//Early reflections FIR-18 register size (maximum delay)
#define erDmax 3840  //80 ms
 
signed short int *AllocReverbBuf(void);
signed short int *InitReverbBuf(void);
Bool DeInitReverbBuf(void);
signed short int fir18(signed short int x);
signed short int lpcomb(signed short int x, unsigned short int idx);
signed short int allpass(signed short int x);
signed short int reverb(signed short int *input, signed short int *output);

#endif

/*****************************************************************************/
/* End of reverb.c                                                           */
/*****************************************************************************/

  
