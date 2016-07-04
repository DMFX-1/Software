/*****************************************************************************/
/*                                                                           */
/* FILENAME                                                                  */
/*   pitch_shift.c                                                           */
/*                                                                           */
/* DESCRIPTION                                                               */
/*   Implements a pitch shift algorithm in DMFX-1                            */
/* REVISION                                                                  */
/*   Revision: 1.00                                                          */
/*   Author  : Diego de la Cruz                                              */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* HISTORY                                                                   */
/*   Revision 1.00                                                           */
/*   01/07/2016															     */
/*                                                                           */
/*****************************************************************************/
/* pitch_shift()                                                             */
/*---------------------------------------------------------------------------*/
#include "pitch_shift.h"
#include "DMFX1.h"
#include "dsplib.h"

unsigned long int F[7] = {0x80000, 0xAAAAA, 0xC0000, 0x100000, 0x155555, 0x180000, 0x200000}; // 1/2, 2/3, 3/4, 1, 4/3, 3/2, 2 <<20
signed short int psbuf[PSBUF_SIZE];
signed short int PSPitch;
signed short int PSMix;
ushort_ulong_t phasel[3] =  { {0, 0}, {0x5555, 0x5555}, {0xAAAA, 0xAAAA} };

signed short int pitch_shift(signed short int *input, signed short int *output)
{
	static unsigned short int del[3]   = {0, 0, 0}; //{PSBUF_SIZE/2, PSBUF_SIZE/6, 5*(PSBUF_SIZE/6)};
	unsigned short int pstap_h[3]  = {0, 0, 0};
#if 0
	unsigned long int pstap_l[3] = {0, 0, 0};
#endif
	unsigned short int psdel[3]  = {0, 0, 0};
	signed short int psout[3] = {0, 0, 0};
	static unsigned long int psidx = 0;
	unsigned short int phase[3] = {0, 0, 0};
	signed long int outl;
	signed short int psgain[3];
	unsigned short int i;

	psbuf[psidx++] = *input;
	if(psidx == PSBUF_SIZE) psidx = 0;
	for(i = 0; i < 3; i++)
	{
		phasel[i].l += F[PSPitch];
		phase[i] = phasel[i].s[0] - 16384;
		// Calculate hi/integer tap value and low/fractional tap value
		// F is shifted 20 bits left
		pstap_h[i] = phasel[i].s[0] >> 4;
#if 0
		pstap_l[i] = (unsigned short)((phasel[i].l>>4) & 0xFFFF);
#endif
	}
	sine((DATA *)phase, (DATA *)psgain, 3);
	
	for(i = 0; i < 3; i++)
	{
		// Calculate real delayed tap value from present buffer index
		psdel[i] = (del[i]+pstap_h[i] < PSBUF_SIZE) ? del[i]+pstap_h[i] : (del[i]+pstap_h[i])-PSBUF_SIZE;
		// Calculate and return interpolated fractional real modulated delayed sample
#if 0
		psout[i] = ((long)psbuf[psdel[i]]*(65536UL-pstap_l[i]))>>17;
		psdel[i] = (psdel[i] == 0) ? PSBUF_SIZE-1 : psdel[i]-1;
		psout[i] += ((long)psbuf[psdel[i]]*pstap_l[i])>>17;
#endif
		psout[i] = psbuf[psdel[i]]>>1;
		psout[i] += ((long)psbuf[psdel[i]]*psgain[i])>>16;
	}
	outl = (psout[0] + psout[1] + psout[2])>>1;
	outl = (outl*PSMix)>>15;
	outl += ((32767L - PSMix)*(*input))>>15;
	output[0] = (outl > 32767L) ?  32767 : (outl < -32767L) ? -32767 : (signed short int)outl;
	output[1] = output[0];
	return(output[0]);
}
