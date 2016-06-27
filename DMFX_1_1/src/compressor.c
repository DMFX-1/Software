/*****************************************************************************/
/*                                                                           */
/* FILENAME                                                                  */
/*   compressor.c                                                            */
/*                                                                           */
/* DESCRIPTION                                                               */
/* Compressor:    									 					     */
/*                                                                           */
/* Threshold (CT = log threshold, ct = linear threshold)					 */
/*   CTmin = -10dB, CTmax = -2dB, ct = ceil(32767*10^(CT/20))			     */
/*                 															 */
/* Compression ratio and compression slope (CS logarithmic)					 */
/*   ratio max = log10(sqrt(2)), ratio min = log10(8)					 	 */
/*   ratio = 10^((ratio) => slope = ceil(32767*(1-1/ratio));				 */
/*   CS = 1 - 1/ratio 														 */
/* Envelope filter (equivalent to RMS)							             */
/*   envelope filter delay = 500 samples								     */
/*   a = exp(-1/(2*delay^2/(2*pi)^4));										 */
/* Attack / Release:														 */
/*   attack/relase = exp(-1/delay);											 */
/*   attack delay (samples) = 50,100,200,300,400,500,1000,2000,3000,4000     */
/*   release delay (4*att) = 200,300,400,800,1200,1600,2000,4000,8000,16000  */
/* Total delay = envelope delay + attack delay (limited to 2500)			 */
/*																			 */
/*   y(n) = G(n)*x(n)										                 */
/*                                                                           */
/* REVISION                                                                  */
/*   Revision: 1.00                                                          */
/*   Author  : Diego de la Cruz                                              */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* HISTORY                                                                   */
/*   Revision 1.00                                                           */
/*   21/02/2016															     */
/*                                                                           */
/*****************************************************************************/
/* compressor()                                                              */
/*****************************************************************************/
#include <std.h>
#include "DMFX_debug.h"
#include "compressor.h"
#include <dsplib.h>
#include "DMFX1_CSL_BIOS_cfg.h"
#include "DMFX1.h"
#include "dsp_fx.h"

// Compressor parameters
unsigned char CmpLevel;
unsigned char CmpAtt;
unsigned char CmpRel;

// Ratio Max = log10(sqrt(2));
// Ratio Min = log10(8);
// Ratio = 10^(ratio);
// Slope = ceil(32767*(1-1/ratio));
signed short int slope[10] = {9598, 13656, 17003, 19764, 22041, 23920, 25469, 26748, 27802, 28672};

// Threshold
// CT min = -10 dB;
// CT max = -2dB;
// ct = ceil(32767*10^(CT/20));
signed short int ct[10] = {26028/2, 23496/2, 21211/2, 19148/2, 17285/2, 15604/2, 14086/2, 12716/2, 11479/2, 10362/2};

// Envelope filter a coefficient
// envelope filter delay = 500 samples
//a = exp(-1/(2*delay^2/(2*pi)^4));
//const signed short int a = 32666;

// Attack
// attack delay (samples) = {50, 100, 200, 300, 400, 500, 1000, 2000, 3000, 4000;
// att = exp(-1./delay);      % attack time constant
signed short int att[10] = {648, 326, 163, 109, 81, 65, 32, 16, 10, 8};

// Release
// Release delay (4*attack) = {200, 300, 400, 800, 1200, 1600, 2000, 4000, 8000, 16000};
// rel = exp(-1./delay);      % attack time constant
signed short int rel[10] = {163, 109, 81, 40, 27, 20, 16, 8, 4, 2};

// Delay = envelope + attack delay
signed short int del[10] = {550, 600, 700, 800, 900, 1000, 1500, COMP_BUF_SIZE, COMP_BUF_SIZE, COMP_BUF_SIZE};
signed short int WrBufIdx = 0;
signed short int RdBufIdx = 0;

#if 0
void init_compressor(unsigned char CmpLevel)
{
	WrBufIdx = 0;
	RdBufIdx = WrBufIdx + del[CmpLevel] - 1;
}
#endif

signed short int compressor(signed short int *input, signed short int *output)
{
	static signed short int comp_buf[COMP_BUF_SIZE] ={0};
	static ushort odd = 0;
	static unsigned char old_level = 0;
	static DATA dp1[4]= {0, 0, 0, 0};
	//envelope filter coefficients and parameters
	// Note that calculated iircas61 coefficients must be divided by 2
	// b0 = 1-a , b1=0, b2=0, a1=-a, a2=0
	DATA lp1[5] = {51, 0, 0, -16334, 0};
	DATA absx = 0;
	short_long_t env = {0L};
	short_long_t gain = {0L};
	short_long_t out = {0L};
	signed long int theta = 0;

#if DEBUG>=10
	ushort oflag;
#endif

	if(CmpLevel != old_level){
		RdBufIdx = WrBufIdx + del[CmpLevel] - 1;
		if(RdBufIdx > COMP_BUF_SIZE-1)
			RdBufIdx -= COMP_BUF_SIZE;
		old_level = CmpLevel;
	}
	absx = (*input < 0)? -*input : *input;
#if DEBUG>=10
	oflag =
#endif
	iircas61(&absx, lp1, &absx, dp1, 1, 1, odd);
	odd ^= 1;
#if DEBUG>=10
	if (oflag) LOG_printf(&trace, "Compressor: envelope filter Overflow Error!!!");
#endif
	absx = (absx > 16383/2) ? 32767 : (absx < -16383/2) ? -32767 : (absx<<2);
	if(absx > env.s[1])
		theta = att[CmpAtt];
	else
		theta = rel[CmpRel];
	env.l = (((long)(32767L - theta)*absx)>>15) + (((long)env.l*theta)>>15);
#if DEBUG
	if ((env.l > 32767L) || (env.l < -32767))
		LOG_printf(&trace, "Compressor: envelope OVERFLOW!!");
#else
	env.s[1]  = (env.l > 32767L) ? 32767 : (env.l < -32767L) ? -32767 : env.s[1];
#endif
	if(env.s[1] > ct[CmpLevel])
	{
		gain.l = 32767L - (((long)(env.l - ct[CmpLevel])*slope[CmpLevel])>>15);
#if DEBUG
		if ((gain.l > 32767L) || (gain.l < -32767))
			LOG_printf(&trace, "Compressor: gain OVERFLOW!!");
#else
		gain.s[1]  = (gain.l > 32767L) ? 32767 : (gain.l < -32767L) ? -32767 : gain.s[1];
#endif
	}
	else
		gain.s[1] = 32767;
	comp_buf[WrBufIdx] = *input;
	if(++WrBufIdx == COMP_BUF_SIZE) WrBufIdx = 0;
	out.l = (gain.l*comp_buf[RdBufIdx]) >> 15;	// Removed: Multiply the final gain by 2. Add 6dB
	out.s[1]  = (out.l > 32767L) ? 32767 : (out.l < -32767L) ? -32767 : out.s[1];
	if(++RdBufIdx == COMP_BUF_SIZE) RdBufIdx = 0;
#if DEBUG>=10
	LOG_printf(&trace, "Compressor gain = %d", gain.s[1]);
#endif
	output[0] = out.s[1];
	output[1] = out.s[1];
 	return(out.s[1]);
}

/*****************************************************************************/
/* End of compressor.c                                                       */
/*****************************************************************************/
