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
#include "flanger.h"
#include "variabledelay.h"
#include "dsp_fx.h"

#define	ORDER  6
#define	R      30000    			// R = exp(-pi*B/48000) B = 48 Hz
#define	R2     ((long)R*R)>>16    	// R^2/2, coefficients scaled to 0.5 for iircas61 filtering
// Fx parameters
extern Uint16 FxCmd[FXCMD_SIZE];
//unsigned short int notches;

// Flanger parameters
signed short int FlGain;
signed short int FlDepth;
unsigned long int FlFreq;
signed short int FlStereo;
#if 0
signed short int FlType;
#endif

signed short int flanger(signed short int *input, signed short int *output)
{

	static signed short int lfo_out[2] = {0, -32768};
    //      						 0     pi/7   2pi/7  3pi/7  4pi/7  5pi/7
    // 							--> pi/7  2pi/7  3pi/7  4pi/7  5pi/7  6pi/7
    //      					   2pi/7  3pi/7  4pi/7  5pi/7  6pi/7   pi
    // cos(th0) = sin(th0+pi/2) --> 9/14   11/14  13/14 -13/14 -11/14  -9/14
	static signed short int th0[ORDER] = {21064, 25745, 30426, -30427, -25746, -21065};
	//static signed short int th0[ORDER] = {18904, 21424, 23945, 26465, 28986, 31506, -31507, -28987, -26466, -23946, -21425, -18905};
	static signed long int x_1[ORDER] = {0L, 0L, 0L, 0L, 0L, 0L};
	static signed long int x_2[ORDER] = {0L, 0L, 0L, 0L, 0L, 0L};
	static signed long int y_1[ORDER] = {0L, 0L, 0L, 0L, 0L, 0L};
	static signed long int y_2[ORDER] = {0L, 0L, 0L, 0L, 0L, 0L};
	signed short int thl[ORDER] = {0, 0, 0, 0, 0, 0};
	signed short int costhl[ORDER] = {0, 0, 0, 0, 0, 0};
#if 0
	signed short int thr[ORDER] = {0, 0, 0, 0, 0, 0};
	signed short int costhr[ORDER] = {0, 0, 0, 0, 0, 0};
#endif
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
	signed short int x;
	signed long int y;

	//notches = ORDER; //FxCmd[FL_NOTCHES]+1;
	lfo(FlFreq, SINE, lfo_out);	// FlType = SINE
	for (i = 0; i < ORDER; i++) {
		thl[i] = th0[i];
		thl[i] += (FlDepth*(long)lfo_out[0]/(ORDER+1))>>15;
#ifdef _STEREO
		if(FlStereo == STEREO)
		{
			thr[i] = th0[i];
			thr[i] += (FlDepth*(long)lfo_out[1]/(ORDER+1))>>15;
		}
#endif
	}
	sine(thl, costhl, ORDER);		// cos(theta(i)) i = 0..ORDER-1
#ifdef _STEREO
	if(FlStereo == STEREO)
		sine(thr, costhr, ORDER);		// cos(theta(i)) i = 0..ORDER-1
#endif
	x = (*input)>>1;	// max(abs(1+H(z))= 2 (6dB)> when all-pass-filter is constructive output is doubled
	lout[0] = 0L;
	for (i = 0; i < ORDER; i++)
	{
		apl[i][1] = (-(long)costhl[i]*R)>>15;	// a1 = b1 = beta = -R*cos(th) coeffs scaled to 0.5 for iircas61
		y = (x_2[i]>>1) - (((long)apl[i][0]*(y_2[i] - (long)x))>>15) - (((long)apl[i][1]*(y_1[i] - x_1[i]))>>15);
		//y >>= 1;
		x_2[i] = x_1[i];
		x_1[i] = x;
		y_2[i] = y_1[i];
		y_1[i] = y;
		lout[0] += y;
#ifdef _STEREO
		if(FlStereo == STEREO)
		{
			apr[i][1] = (-(long)costhr[i]*R)>>15;	// a1 = b1 = beta = -R*cos(th) coeffs scaled to 0.5 for iircas61
			peak_filter(x, &out[1], &apr[i][0], i);
		}
#endif
	}
	lout[0] += ((long)FlGain*x)>>15;		// add to input with gain (notch depth and direction)
#ifdef _STEREO
	if(FlStereo == STEREO)
	{
		lout[1] = (long)out[1];
		lout[1] += ((long)FlGain*x)>>15;		// add to input with gain (notch depth and direction)
	}
	else
#endif
	lout[1] = lout[0];

	if(FlStereo == LESLIE) {
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
/* End of flanger.c                                                          */
/*****************************************************************************/
