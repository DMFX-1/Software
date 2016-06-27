/*****************************************************************************/
/*                                                                           */
/* FILENAME                                                                  */
/*   reverb.c                                                                */
/*                                                                           */
/* DESCRIPTION                                                               */
/*   TMS320C5505 USB Stick. Reverberation effect                             */
/* REVISION                                                                  */
/*   Revision: 1.00                                                          */
/*   Author  : Diego de la Cruz                                              */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* HISTORY                                                                   */
/*   Revision 1.00                                                           */
/*   14/09/2013															     */
/*                                                                           */

/*****************************************************************************/
/* reverberation()                                                           */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Based on Moorer reverberation algorithm      							 */
/*                                                                           */
/* Pre-Delay early FIR filter with 18 coefficients up to 80ms:    			 */
/* 																			 */
/* Followed by 6 low pass comb filters with the following parameters:   	 */
/* lpcomb delay (D): 50, 56, 61, 68, 72, 78 ms         						 */
/* lpcomb gain (g): 0.83                    								 */
/* lpcomb lp filter gain (lpg1): 0.46 0.48 0.5 0.52 0.53 0.55  			     */
/* b=[zeros(1,D) 1 -lpg1];  a=[1 -lpg1 zeros(1,D-2) -g*(1-lpg1)];			 */
/*                                                                           */
/* Followed by an all-pass filter with coefficients            				 */
/* allpass filter delay (D): 6 ms											 */
/* allpass filter gain (g): 0.7												 */
/* b=[g zeros(1,d-1) 1];  a=[1 zeros(1,d-1) g];								 */
/*****************************************************************************/
#include "reverb.h"
#include "std.h"
#include "mem.h"

extern Int dlybuf;

unsigned short int fird[18] = {
	 206,    // 4.30
	1032,    // 21.5
	1080,    // 22.5
	1286,    // 26.8
	1296,    // 27.0
	1430,    // 29.8
	2198,    // 45.8
	2328,    // 48.5
	2746,    // 57.2
	2818,    // 58.7
	2856,    // 59.5
	2938,    // 61.2
	3394,    // 70.7
	3398,    // 70.8
	3485,    // 72.6
	3557,    // 74.1
	3614,    // 75.3
	3826     // 79.7
};              

// Early reflections FIR-18 tap normalized gain
signed short int firg[18] = {
	27557,    // 0.841
	16515,    // 0.504
	16056,    // 0.490
	12419,    // 0.379
	12451,    // 0.380
	11337,    // 0.346
	9470,     // 0.289
	8913,     // 0.272
	6291,     // 0.192
	6324,     // 0.193
	7110,     // 0.217
	5931,     // 0.181
	5898,     // 0.180
	5931,     // 0.181
	5767,     // 0.176
	4653,     // 0.142
	5472,     // 0.167
	4391      // 0.134
};        

// 6-Low Pass Comb filters registers
#if 0
signed short int lpr1[cD1max];
signed short int lpr2[cD2max];
signed short int lpr3[cD3max];
signed short int lpr4[cD4max];
signed short int lpr5[cD5max];
signed short int lpr6[cD6max];
signed short int apr[aDmax];
signed short int err[erDmax];
#endif

signed short int *lpr[6]; //= { lpr1, lpr2, lpr3, lpr4, lpr5, lpr6};
signed short int *apr;
signed short int *err;
signed short int lprD[6] = {cD1max, cD2max, cD3max, cD4max, cD5max, cD6max};

// Low pass Comb filters gains
//round(32767*[0.46 0.48 0.5 0.52 0.53 0.55])
signed short int lpg1[6] = {15073,  15729,  16384,  17039,  17367,  18022};
//round(32767*[0.418 0.436 0.455 0.473 0.482 0.5])  
//signed short int lpg1[6] = {13703, 14299, 14895, 15490, 15788, 16384};
signed short int lpg = 27197;	// 0.83 => 2s reverb
signed short int lpg2[6] = {14686, 14142, 13598, 13054, 12782, 12238};
signed short int ag = 27853;		// All-Pass Filter Gain 0.7 recommended value

// Reverb parameters
// Loop time = 10 - 100ms
// Reverb Time 0.1 - 5.0s => lpg 0.7 0.83 0.99
// wet/dry mix = 0 - 100%
// Early reflections FIR-18 tap delay
signed short int erg = 27853;  // Early Reflections Gain
signed short int lrg = 27853;  // Late Reflections Gain
signed short int dry = 27853;  // Dry (direct) Gain
#if 0
extern Ptr baseaddress;
extern MEM_Stat statbuf[1];
extern Bool status;
#endif

signed short int *AllocReverbBuf(void)
{
	unsigned short int i;

	for(i = 0; i < 6; i++)
	{
		lpr[i]= (signed short int *)MEM_alloc(dlybuf, lprD[i], 0);
#if 0
		baseaddress= MEM_getBaseAddress(dlybuf);
		status = MEM_stat(dlybuf, statbuf);
#endif
		if(lpr[i] == NULL) return(NULL);
	}
	apr = (signed short int *)MEM_alloc(dlybuf, aDmax, 0);
	if(apr == NULL) return(NULL);
	err = (signed short int *)MEM_alloc(dlybuf, erDmax, 0);
	if(err == NULL) return(NULL);
	return(lpr[0]);
}

signed short int *InitReverbBuf(void)
{
	unsigned short int i;
	unsigned short int j;
	signed short int *ptr;

	for(i = 0; i < 6; i++)
	{
		ptr = lpr[i];
		for(j = 0; j < lprD[i]; j++)
			*ptr++ = 0;
	}
	ptr = apr;
	for(j = 0; j < aDmax; j++)
		*ptr++ = 0;
	ptr = err;
	for(j = 0; j < erDmax; j++)
		*ptr++ = 0;
	return(ptr);
}

Bool DeInitReverbBuf(void)
{
	Bool temp;
	unsigned short int i;

	for(i = 0; i < 6; i++)
	{
		temp = MEM_free(dlybuf, (void *)lpr[i], lprD[i]);
		if(temp == FALSE) return FALSE;
	}
	temp = MEM_free(dlybuf, (void *)apr, aDmax);
	if(temp == FALSE) return FALSE;
	temp = MEM_free(dlybuf, (void *)err, erDmax);
	if(temp == FALSE) return FALSE;
	return(temp);
}

signed short int fir18(signed short int x)
{
	signed long int y;
	static signed short int i = 0;
	signed short int i_D, idx;
	signed short int out;

	y = err[i] = x;
	for(idx = 0; idx < 18; idx++)
	{
		i_D  = (i >= fird[idx]) ? i-fird[idx]-1	: i+erDmax-fird[idx]-1;
		y += ((long)firg[idx]*err[i_D])>>15;
	}
	out = (y > 32767L)? 32767 : (y < -32768L)? -32768 : (short)y;
	i = (i == erDmax-1) ? 0 : i+1;
	return(out);
}

signed short int lpcomb(signed short int x, unsigned short int idx)
{
	signed long int y;
	static signed short int i[6] = { 0, 0, 0, 0, 0, 0};
	signed short int i_1, i_D, i_D1;
	signed short int out;
	
	i_1  = (i[idx] == 0) ? lprD[idx] : i[idx] - 1;
	i_D1 = (i[idx] == lprD[idx]-2) ? 0 : (i[idx] == lprD[idx]-1) ? 1 : i[idx] + 2;
	i_D  = (i[idx] == lprD[idx]-1) ? 0 : i[idx] + 1;

	y  = x/2;
	y += ((long)lpg1[idx]*lpr[idx][i_1])>>15;
	y += ((long)lpg2[idx]*lpr[idx][i_D])>>15;
	lpr[idx][i[idx]] = (y > 32767L)? 32767 : (y < -32768L)? -32768 : (short)y;

	y  = (long)lpr[idx][i_D];
	y -= ((long)lpg1[idx]*lpr[idx][i_D1])>>15;
	out = (y > 32767L)? 32767 : (y < -32768L)? -32768 : (short)y;
	i[idx] = (i[idx] == lprD[idx]-1) ? 0 : i[idx]+1;
	return(out);
}

signed short int allpass(signed short int x)
{
	signed long int y;
	static signed short int i = 0;
	signed short int i_D;
	signed short int out;

	i_D  = (i == aDmax-1) ? 0 : i+1;

	y  = x;
	y -= ((long)ag*apr[i_D])>>15;
	apr[i] = (y > 32767L)? 32767 : (y < -32768L)? -32768 : (short)y;
	
	y = (long)apr[i_D];
	y+= ((long)ag*apr[i])>>15;
	out = (y > 32767L)? 32767 : (y < -32768L)? -32768 : (short)y;
	i = (i == aDmax-1) ? 0 : i+1;
	return(out);
}

signed short int reverb(signed short int *input, signed short int *output)
{
	signed short int i;
	signed short int sr;
	signed short int erx;
	signed long int lr, y;
	
	for(i = 0; i < 6; i++) lpg2[i] = ((long)lpg*(32767-lpg1[i]))>>15;
	erx = fir18(*input);
	lr = 0L;
	for(i = 0; i < 6; i++) lr += lpcomb(erx, i);
	lr >>= 2;
	sr = (lr > 32767L)? 32767 : (lr < -32768L)? -32768 : (short)lr;
	y = ((long)lrg*allpass(sr))>>15;
	y += ((long)erg*erx)>>15;
	y += ((long)dry*(*input))>>15;
	y>>=1;
	output[0] = (y > 32767L)? 32767 : (y < -32768L)? -32768 : (short)y;
	output[1] = output[0];
	return(output[0]);
}


/*****************************************************************************/
/* End of reverb.c                                                           */
/*****************************************************************************/
