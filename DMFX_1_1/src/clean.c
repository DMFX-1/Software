/*****************************************************************************/
/*                                                                           */
/* FILENAME                                                                  */
/*   clean.c                                                                 */
/*                                                                           */
/* DESCRIPTION                                                               */
/*   DistMix_l_r(): Mixes left and right channels, mix = 1 => left channel   */
/*		mix = 0 => right channel, mix = 0.5 => left = right channel			 */
/* REVISION                                                                  */
/*   Revision: 1.00                                                          */
/*   Author  : Diego de la Cruz                                              */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* HISTORY                                                                   */
/*   Revision 1.00                                                           */
/*   01/11/2016															     */
/*                                                                           */
/*****************************************************************************/
/* clean()                                                                   */
/*****************************************************************************/
#include <std.h>
#include "DMFX1.h"
#include "clean.h"
#include "dsp_fx.h"


// Fx parameters
extern Uint16 FxCmd[FXCMD_SIZE];

#if 0
// clean copies an input buffer of size samples on left and right output buffers
unsigned short int clean(signed short int *input, unsigned short int size, signed short int *OutLeft, signed short int *OutRight)
{
	unsigned short int j = 0;

	for (j = 0; j < size; j++)
	{
		OutLeft[j] = OutRight[j] = input[j];
	}
	return (0);
}
#endif

// DistMix_l_r mixes left and right input buffers of size samples according to mix parameter where
// mix = 1 => left channel only, mix = 0 => right channel only and mix = 0.5 left = right channel
// Distortion uses left channel, and clean signal uses right channel, in that case mix parameter can
// be used as a driving level: mix = 1 => full drive or distortion, mix = 0 => clean signal
signed short int *DistMix_l_r(signed short int *input_l, signed short int *input_r, signed short int *output)
{
	short_long_t out;
	signed short int mix;

	mix = (FxCmd[DIST_DRIVE]<<5);	// converts distortion drive from 0-1023 to 0 - 32767
	out.l = (((long)mix*input_l[0])>>16);
	out.l += (((long)(32767L-mix)*input_r[0])>>16);
	out.l = (out.l > 32767L) ? 32767L : (out.l < -32767L) ? -32767L : out.l;
	*output =  out.s[1];
	return (&out.s[1]);
}
/*****************************************************************************/
/* End of clean.c                                                           */
/*****************************************************************************/
