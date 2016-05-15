/*****************************************************************************/
/*                                                                           */
/* FILENAME                                                                  */
/*   variabledelay.c                                                         */
/*                                                                           */
/* DESCRIPTION                                                               */
/*   TMS320C5505 USB Stick. Simulates vibrato effect                         */
/* REVISION                                                                  */
/*   Revision: 1.00                                                          */
/*   Author  : Diego de la Cruz                                              */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* HISTORY                                                                   */
/*   Revision 1.00                                                           */
/*   18/02/2012															     */
/*                                                                           */

/*******************************************************************************/
/* variabledelay()                                                             */
/*-----------------------------------------------------------------------------*/
/*             BL   FF    FB      DELAY       DEPTH        MOD                 */
/*  Vibrato   0     1      0      0 5 ms      0 5 ms     0.1 5 Hz sine         */
/*  Flanger   0.7   0.7   -0.7    0 10 ms     0 10 ms    0.1 1 Hz sine         */
/*  ISChorus  1     0.7    0      1 30 ms     1 30 ms    0.15 Hz Lowpass Noise */
/*  Chorus    0.7   1      0.7    1 30 ms     1 30 ms    0.15 Hz Lowpass Noise */
/*  Doubling  0.7   0.7    0      10 100 ms   10 100 ms  Lowpass noise         */
/*  Echo      1     <1     <1     > 100 ms    0  ms      No                    */
/*                                                                             */
/*******************************************************************************/
#include <std.h>
#include <dsplib.h>
#include <tms320.h>
#include "variabledelay.h"
#include "dsp_fx.h"

#define DEBUG  10
#define MAX_SIZE			32768L	// max delay = 32768/48000 = 680ms

#if 1
#pragma DATA_SECTION(delaybuf, ".dlybuf")
signed short delaybuf[MAX_SIZE];
#endif
// Fx parameters
extern Uint16 FxCmd[FXCMD_SIZE];

extern signed short int BL;
extern signed short int FF;
extern signed short int FB;
extern signed short int stereo;
extern signed short int type;
extern signed short int depth;
extern signed long int delay;
extern unsigned long int freq;

// x(input), output(pointer to output stereo samples) BL(blend), FF(Feed Forward), FB(Feed Back)
//delay(average delay value in samples), depth(0..1  in Q0.15), modulation freq
signed short int variabledelay(signed short int *input, signed short int *output)
{
	signed long int x;
	signed long int y[2];
	signed short int xh;
	signed short int *v;
	static signed short int xh_k = 0;

	x = (long)(*input>>1);	// max(abs(1+H(z))= 2 (6dB)> when all-pass-filter is constructive output is doubled
	xh = x + (((long)xh_k*FB)>>16);
	//xh = (x) + ((FB * (long)xh_k)>>15);
	//xh  = (xh > 32767L) ? 32767 : (xh < -32767L) ? -32767 : xh;
	v  = delayline(xh, &xh_k);
	y[0] = ((long)v[0]*FF)>>15;
	y[0] += ((long)xh*BL)>>15;
	//y[0] <<= 1;
	y[1] = ((long)v[1]*FF)>>15;
	y[1] += ((long)xh*BL)>>15;
	//y[1] <<= 1;
	output[0] = (y[0] > 32767L) ?  32767 : (y[0] < -32767L) ? -32767 : (signed short int)y[0];
	output[1] = (y[1] > 32767L) ?  32767 : (y[1] < -32767L) ? -32767 : (signed short int)y[1];
	return(output[0]);	// return mono value
}

// Low Pass Noise generation: random samples each T are interpolated between 0 - T 
void lp_noise(signed short int *lpn, unsigned long int freq)
{
	static unsigned long int i = 0;
	static signed short int u[4] = {0, 0, 0, 0};
	static union {
		signed short int s[2][2];
		signed long int l[2];
	} llpn = {0, 0, 0, 0};
	unsigned long int T;
	
	T = (1UL<<31)/(freq>>1);  // 2T represented in Q31
	if(i == 0) {		// init random function and noise vector u
		rand16init();
		rand16(u, 4);
	}
	llpn.l[0] = ((long)u[1] - u[0])*((freq*i)>>15);
	llpn.l[0] += ((long)u[0]<<16);
	llpn.l[1] = ((long)u[2] - u[3])*((freq*i)>>15); 
	llpn.l[1] += ((long)u[3]<<16);
	lpn[0] = llpn.s[0][0];
	lpn[1] = llpn.s[1][0];
	if(i++ == T) {
		i = 1;
		u[0] = u[1];
		u[3] = u[2];	// order inverted to make single call to rand16
		rand16(&u[1], 2);
	}
}

signed short int* delayline(signed short int x, signed short int *xh_k)
{
	unsigned long int ltap[2] = {0, 0};
	unsigned long int tap_h[2] = {0, 0};
	unsigned short int tap_l[2] = {0, 0};
	unsigned long int y_k = 0;
	unsigned long int y_del[2] = {0, 0};
	//signed short int *lfo_out;
	static signed short int lfo_out[2] = {0, -32768};
	//static signed short int leslie lfo[2] = {0, -32768};
	static signed short int output[2] = {0, 0};
	static unsigned long int y_idx = 0;
	
	delaybuf[y_idx++] = x;
	if(y_idx == MAX_SIZE) y_idx = 0;

	// Calculate real unmodulated tap delayed value from present buffer index
	y_k   = (y_idx >= delay) ? y_idx-delay : MAX_SIZE + (y_idx-delay);
	// Return unmodulated delayed sample for feedback (echo Comb IIR filter) 
	*xh_k = delaybuf[y_k];

	// Calculate long tap delay value from LFO
	if (type)
	{
		lfo(freq, type, lfo_out);
		ltap[0] = ((long)lfo_out[0]*depth)>>15;
		ltap[0] = (ltap[0] + 32767L)*delay + 32767L;
		ltap[1] = ((long)lfo_out[1]*depth)>>15;
		ltap[1] = (ltap[1] + 32767L)*delay + 32767L;
	}
	else
	{
		ltap[0] = 32767L*delay + 32767L;
		ltap[1] = 32767L*delay + 32767L;
	}
	// Calculate hi/integer tap value and low/fractional tap value
	//fmod(Hz)*2*32768(2*pi)/48000Hz = fmod*22369/32768
	tap_h[0] = ltap[0] >> 15;
	tap_h[0] = (tap_h[0] > MAX_SIZE-1) ? MAX_SIZE-1 : tap_h[0];
	tap_l[0] = ltap[0] & 0x7FFF;
	// Calculate second modulated tap delay for stereo/leslie effects
	tap_h[1] = ltap[1] >> 15;
	tap_h[1] = (tap_h[1] > MAX_SIZE-1) ? MAX_SIZE-1 : tap_h[1];
	tap_l[1] = ltap[1] & 0x7FFF;
	
	// Calculate real delayed tap value from present buffer index
	y_del[0] = (y_idx >= tap_h[0]) ? y_idx-tap_h[0] : MAX_SIZE + (y_idx-tap_h[0]);
	y_del[1] = (y_idx >= tap_h[1]) ? y_idx-tap_h[1] : MAX_SIZE + (y_idx-tap_h[1]);
	// Calculate and return interpolated fractional real modulated delayed sample 
	output[0] = ((long)delaybuf[y_del[0]]*(32768-tap_l[0]))>>15;
	y_del[0] = (y_del[0] == 0) ? MAX_SIZE-1 : y_del[0]-1;
	output[0] += ((long)delaybuf[y_del[0]]*tap_l[0])>>15;
	output[1] = ((long)delaybuf[y_del[1]]*(32768-tap_l[1]))>>15;
	y_del[1] = (y_del[1] == 0) ? MAX_SIZE-1 : y_del[1]-1;
	output[1] += ((long)delaybuf[y_del[1]]*tap_l[1])>>15;
	//lfo(4*89480L, 48000L/4, 0, leslie lfo);
	if(stereo == LESLIE) {
		output[0] += ((long)lfo_out[0]*output[0])>>15;
		output[1] += ((long)lfo_out[1]*output[1])>>15;
		output[0] += (23170L*output[1])>>15;
		output[1] += (23170L*output[0])>>15;
	}
	return(output); 
}

signed short int* lfo(unsigned long int freq, signed short int type, signed short int *lfo_out)
{
	static signed short int phase[2] = {0, -32768};
	static union {
		signed short int s[2][2];
		signed long int l[2];
	} lphase= {0, 0, 16384, 0};
	static signed short int slope = 2;
	//static signed short int lfo_out[2] = {0, 32768};

	switch(type)
	{
	case 1:  // LFO = sinewave
		lphase.l[0] += freq;
		lphase.l[1] += freq;
		phase[0] = lphase.s[0][0]; 
		phase[1] = lphase.s[1][0];
		// sine pi*phase(Q15)
		sine(phase, lfo_out, 2);
		break;
	case 2:		// LFO Low pass noise
		lp_noise(lfo_out, freq);
		break;
	case 3:		// LFO triangle
		if(!(slope & 1)) 
		{
			lphase.l[0] += freq;
			lfo_out[0] = lphase.s[0][0]; 
			if(lphase.l[0] > (0x40000000-(signed)freq)) slope |= 1;
		}
		else
		{
			lphase.l[0] -= freq;
			lfo_out[0] = lphase.s[0][0]; 
			if(lphase.l[0] < (-0x40000000+(signed)freq)) slope &= 2;
		}
		if(!(slope & 2)) 
		{
			lphase.l[1] += freq;
			lfo_out[1] = lphase.s[1][0]; 
			if(lphase.l[1] > (0x40000000-(signed)freq)) slope |= 2;
		}
		else
		{
			lphase.l[1] -= freq;
			lfo_out[1] = lphase.s[1][0]; 
			if(lphase.l[1] > (-0x40000000+(signed)freq)) slope &= 1;
		}
		break;
	case 4: 	// LFO sawtooth
		lphase.l[0] += freq;
		lphase.l[1] += freq;
		lfo_out[0] = lphase.s[0][0];
		lfo_out[1] = lphase.s[1][0];
		break;
	default:	// default is sinewave
		lphase.l[0] += freq;
		lphase.l[1] += freq;
		phase[0] = lphase.s[0][0]; 
		phase[1] = lphase.s[1][0]; 
		sine(phase, lfo_out, 2);
		break;
	}		
	return(lfo_out);
}

/*****************************************************************************/
/* End of variabledelay.c                                                    */
/*****************************************************************************/
