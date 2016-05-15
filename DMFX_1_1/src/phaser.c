/*****************************************************************************/
/*                                                                           */
/* FILENAME                                                                  */
/*   phaser.c                                                                */
/*                                                                           */
/* DESCRIPTION                                                               */
/*   TMS320C5505 USB Stick. Simulates a phaser                               */
/* REVISION                                                                  */
/*   Revision: 1.00                                                          */
/*   Author  : Diego de la Cruz                                              */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* HISTORY                                                                   */
/*   Revision 1.00                                                           */
/*   18/02/2012															     */
/*                                                                           */

/*****************************************************************************/
/* phaser()                                                                 */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Based on All-pass approach to Phasing and Flanging by Julius O'Smith      */
/*                                                                           */
/* A sequence of 6 All-pass filters added to the original signal generates   */
/* 6 notches that can be moved up and down 								     */
/* Radius of the pole must be R<1 for stability, the closer to 1 the         */
/* narrower the notch. We will use R=30000/32768 = 0.9155,                   */
/* Pole angle in Z plane controls the tuning, theta = pi*flfo/fs (flfo=1Hz)  */
/* theta = [3*pi/4, 3*pi/8, 3*pi/16, 3*pi/32, 3*pi/64, 3*pi/128];            */
/* All-pass filter coefficients:										     */
/* b0 = 1,   b1 = -2*R*cos(theta), b2 = R^2		                             */
/* a0 = R^2, a1 = -2*R*cos(theta), a2 = 1                                    */
/*																			 */
/*****************************************************************************/
#include <std.h>
#include <dsplib.h>
#include <tms320.h>
#include "phaser.h"
#include "variabledelay.h"
#include "dsp_fx.h"

#define	ORDER  6
#define	R      30000    			// R = exp(-pi*B/48000) B = 48 Hz
#define	R2     ((long)R*R)>>16    	// R^2/2, coefficients scaled to 0.5 for iircas61 filtering
// Fx parameters
extern Uint16 FxCmd[FXCMD_SIZE];

#if 0
signed short int phaser(signed short int *input, signed short int *output)
{

	static signed short int lfo_out[2] = {0, -32768};
	static unsigned short int odd = 0;
    //      						 pi     pi/2   pi/4   pi/8   pi/16  pi/32
    // 							--> 3pi/4  3pi/8  3pi/16 3pi/32 3pi/64 3pi/128
    //      						 pi/2   pi/4   pi/8   pi/16  pi/32  pi/64
    // cos(th0) = sin(th0+pi/2) --> -3/4    7/8   11/16   19/32  35/64  67/128
	static signed short int th0[ORDER] = {-24577, 28671, 22527, 19455, 17919, 17151};
	signed short int thl[ORDER] = {0, 0, 0, 0, 0, 0};
	signed short int thr[ORDER] = {0, 0, 0, 0, 0, 0};
	signed short int costhl[ORDER] = {0, 0, 0, 0, 0, 0};
	signed short int costhr[ORDER] = {0, 0, 0, 0, 0, 0};
  	// b0, b1, b2, a1, a2  -> all pass  filter
  	DATA apl[ORDER*5] = {R2, 0, 16384, 0, R2,	// b0 = alpha = R^2, b1 = beta = -2R*cos(th), b2 = 1
  					  	 R2, 0, 16384, 0, R2,   // a1 = beta = -2R*cos(th), a2 = alpha = R^2
  					  	 R2, 0, 16384, 0, R2,
  					  	 R2, 0, 16384, 0, R2,
  					  	 R2, 0, 16384, 0, R2,
  					  	 R2, 0, 16384, 0, R2};
  	DATA apr[ORDER*5] = {R2, 0, 16384, 0, R2,	// b0 = alpha = R^2, b1 = beta = -2R*cos(th), b2 = 1
  					  	 R2, 0, 16384, 0, R2,   // a1 = beta = -2R*cos(th), a2 = alpha = R^2
  					  	 R2, 0, 16384, 0, R2,
  					  	 R2, 0, 16384, 0, R2,
  					  	 R2, 0, 16384, 0, R2,
  					  	 R2, 0, 16384, 0, R2};
    static DATA dpl[ORDER*4+1]= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    static DATA dpr[ORDER*4+1]= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    DATA out[2];
    signed long int lout[2];
	unsigned short int i = 0;
	signed short int gain;
	signed short int depth;
	unsigned long int freq;
#if 0
	signed short int type;
#endif
	signed short int stereo;
	signed short int x;
	
	gain  = FxCmd[PH_RESONANCE]<<7; // convert parameter from 0-255 (8-bits) to 0-32767 (signed 15-bits)
	depth = FxCmd[PH_DEPTH]<<7; 	// convert parameter from 0-255 (8-bits) to 0-32767 (signed 15-bits)
	// fmod is lfo modulation frequency in tenths of Hz
	// sine function is called as sine(pi*phase(Q15)) where phase is a Q15 number between -1,1, -32768,32767
	// lfo frequency is so low that the accumulated phase is represented in a Q31 number
	// pi*freq is the digital frequency in radians in a Q31 number format
	// sin(pi*2*fmod(Q31)/10/48000)=> 2*fmod*2^31/48000/10 => fmod*8948L
	// FxCmd is a value between 0-255 to scale to 0-100 dHz => 8948*100/255=3509
	freq = (long)FxCmd[PH_RATE]*3510L;
#if 0
	type = FxCmd[PH_TYPE];			// type = sine, noise, triangle, sawtooth
#endif
	stereo = FxCmd[PH_STEREO_LIST];		// PH_STEREO_LIST: 0=mono, 1=stereo, 2=leslie
	lfo(freq, SINE, lfo_out);	// type = SINE
	for (i = 0; i < ORDER; i++) {
		thl[i] = th0[i];
		thl[i] += (depth*((long)lfo_out[0]>>(i+2)))>>15;
		if(stereo)
		{
			thr[i] = th0[i];
			thr[i] += (depth*((long)lfo_out[1]>>(i+2)))>>15;
		}
	}
	sine(thl, costhl, ORDER);		// cos(theta(i)) i = 0..ORDER-1
	if(stereo == STEREO) sine(thr, costhr, ORDER);		// cos(theta(i)) i = 0..ORDER-1
	for (i = 0; i < ORDER; i++) {
		apl[5*i+3] = apl[5*i+1] = (-(long)costhl[i]*R)>>15;	// a1 = b1 = beta = -R*cos(th) coeffs scaled to 0.5 for iircas61
		if(stereo == STEREO) apr[5*i+3] = apr[5*i+1] = (-(long)costhr[i]*R)>>15;	// a1 = b1 = beta = -R*cos(th) coeffs scaled to 0.5 for iircas61
	}
	x = (*input)>>1;	// max(abs(1+H(z))= 2 (6dB)> when all-pass-filter is constructive output is doubled
	iircas61(&x, apl, &out[0], dpl, ORDER, 1, odd);  // apply ORDER all pass 2nd order IIR sections
	if(stereo == STEREO) iircas61(&x, apr, &out[1], dpr, ORDER, 1, odd);  // apply ORDER all pass 2nd order IIR sections
	lout[0] = (long)out[0];
	lout[0] += ((long)gain*x)>>15;		// add to input with gain (notch depth and direction)
	if(stereo == STEREO)
	{
		lout[1] = (long)out[1];
		lout[1] += ((long)gain*x)>>15;		// add to input with gain (notch depth and direction)
	}
	else lout[1] = lout[0];
	odd ^= 0x1;

	if(stereo == LESLIE) {
		lout[0] += ((long)lfo_out[0]*lout[0])>>16;
		lout[1] += ((long)lfo_out[1]*lout[1])>>16;
		lout[0] += (23170L*lout[1])>>16;
		lout[1] += (23170L*lout[0])>>16;
	}
	output[0] = (lout[0] > 32767)? 32767 : (lout[0] < -32768)? -32768 : (short)lout[0];
	output[1] = (lout[1] > 32767)? 32767 : (lout[1] < -32768)? -32768 : (short)lout[1];
	return (output[0]);
}
#endif

signed short int phaser2(signed short int *input, signed short int *output)
{

	static signed short int lfo_out[2] = {0, -32768};
    //      						 pi     pi/2   pi/4   pi/8   pi/16  pi/32
    // 							--> 3pi/4  3pi/8  3pi/16 3pi/32 3pi/64 3pi/128
    //      						 pi/2   pi/4   pi/8   pi/16  pi/32  pi/64
    // cos(th0) = sin(th0+pi/2) --> -3/4    7/8   11/16   19/32  35/64  67/128
	static signed short int th0[ORDER] = {-24577, 28671, 22527, 19455, 17919, 17151};
	static signed long int x_1[ORDER] = {0L, 0L, 0L, 0L, 0L, 0L};
	static signed long int x_2[ORDER] = {0L, 0L, 0L, 0L, 0L, 0L};
	static signed long int y_1[ORDER] = {0L, 0L, 0L, 0L, 0L, 0L};
	static signed long int y_2[ORDER] = {0L, 0L, 0L, 0L, 0L, 0L};
	signed short int thl[ORDER] = {0, 0, 0, 0, 0, 0};
#if 0
	signed short int thr[ORDER] = {0, 0, 0, 0, 0, 0};
	signed short int costhr[ORDER] = {0, 0, 0, 0, 0, 0};
#endif
	signed short int costhl[ORDER] = {0, 0, 0, 0, 0, 0};
  	// b0, b1, b2, a1, a2  -> all pass  filter
	signed short int apl[ORDER][2] = {
  						 R2, 0,	// b0 = alpha = R^2, b1 = beta = -2R*cos(th), b2 = 1
  					  	 R2, 0,   // a1 = beta = -2R*cos(th), a2 = alpha = R^2
  					  	 R2, 0,
  					  	 R2, 0,
  					  	 R2, 0,
  					  	 R2, 0};
#if 0
	signed short int apr[ORDER][2] = {
  						 R2, 0,	// b0 = alpha = R^2, b1 = beta = -2R*cos(th), b2 = 1
  					  	 R2, 0,   // a1 = beta = -2R*cos(th), a2 = alpha = R^2
  					  	 R2, 0,
  					  	 R2, 0,
  					  	 R2, 0,
  					  	 R2, 0};
#endif
    signed long int lout[2];
	unsigned short int i = 0;
	signed short int gain;
	signed short int depth;
	unsigned long int freq;
#if 0
	signed short int type;
#endif
	signed short int stereo;
	signed short int x;
	signed long int y;

	gain  = FxCmd[PH_RESONANCE]<<7; // convert parameter from 0-255 (8-bits) to 0-32767 (signed 15-bits)
	depth = FxCmd[PH_DEPTH]<<7; 	// convert parameter from 0-255 (8-bits) to 0-32767 (signed 15-bits)
	// fmod is lfo modulation frequency in tenths of Hz
	// sine function is called as sine(pi*phase(Q15)) where phase is a Q15 number between -1,1, -32768,32767
	// lfo frequency is so low that the accumulated phase is represented in a Q31 number
	// pi*freq is the digital frequency in radians in a Q31 number format
	// sin(pi*2*fmod(Q31)/10/48000)=> 2*fmod*2^31/48000/10 => fmod*8948L
	// FxCmd is a value between 0-255 to scale to 0-100 dHz => 8948*100/255=3509
	freq = (long)FxCmd[PH_RATE]*3510L;
#if 0
	type = FxCmd[PH_TYPE];			// type = sine, noise, triangle, sawtooth
#endif
	stereo = FxCmd[PH_STEREO_LIST];		// PH_STEREO_LIST: 0=mono, 1=stereo, 2=leslie

	lfo(freq, SINE, lfo_out);	// type = SINE
	for (i = 0; i < ORDER; i++) {
		thl[i] = th0[i];
		thl[i] += (depth*((long)lfo_out[0]>>(i+2)))>>15;
#ifdef _STEREO
		if(stereo == STEREO)
		{
			thr[i] = th0[i];
			thr[i] += (depth*((long)lfo_out[1]>>(i+2)))>>15;
		}
#endif
	}
	sine(thl, costhl, ORDER);		// cos(theta(i)) i = 0..ORDER-1
#ifdef _STEREO
	if(stereo == STEREO)
		sine(thr, costhr, ORDER);		// cos(theta(i)) i = 0..ORDER-1
#endif
	x = (*input)>>1;	// max(abs(1+H(z))= 2 (6dB)> when all-pass-filter is constructive output is doubled
	lout[0] = 0L;
	for (i = 0; i < ORDER; i++)
	{
		apl[i][1] = (-(long)costhl[i]*R)>>15;	// a1 = b1 = beta = -R*cos(th) coeffs scaled to 0.5 for iircas61
		y = (x_2[i]>>1) - (((long)apl[i][0]*(y_2[i] - (long)x))>>15) - (((long)apl[i][1]*(y_1[i] - x_1[i]))>>15);
		y >>= 1;
		x_2[i] = x_1[i];
		x_1[i] = x;
		y_2[i] = y_1[i];
		y_1[i] = y;
		lout[0] += y;
#ifdef _STEREO
		if(stereo == STEREO)
		{
			apr[i][1] = (-(long)costhr[i]*R)>>15;	// a1 = b1 = beta = -R*cos(th) coeffs scaled to 0.5 for iircas61
			peak_filter(x, &out[1], &apr[i][0], i);
		}
#endif
	}
	lout[0] += ((long)gain*x)>>15;		// add to input with gain (notch depth and direction)
#ifdef _STEREO
	if(stereo == STEREO)
	{
		lout[1] = (long)out[1];
		lout[1] += ((long)gain*x)>>15;		// add to input with gain (notch depth and direction)
	}
	else
#endif
	lout[1] = lout[0];

	if(stereo == LESLIE) {
		lout[0] += ((long)lfo_out[0]*lout[0])>>16;
		lout[1] += ((long)lfo_out[1]*lout[1])>>16;
		lout[0] += (23170L*lout[1])>>16;
		lout[1] += (23170L*lout[0])>>16;
	}
	output[0] = (lout[0] > 32767)? 32767 : (lout[0] < -32768)? -32768 : (short)lout[0];
	output[1] = (lout[1] > 32767)? 32767 : (lout[1] < -32768)? -32768 : (short)lout[1];
	return (output[0]);
}

/*****************************************************************************/
/* End of phaser.c                                                           */
/*****************************************************************************/
