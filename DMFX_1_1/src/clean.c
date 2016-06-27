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

// Clean/Distortion parameters
signed short int DistMix;

// DistMix_l_r mixes left and right input buffers of size samples according to mix parameter where
// mix = 1 => left channel only, mix = 0 => right channel only and mix = 0.5 left = right channel
// Distortion uses left channel, and clean signal uses right channel, in that case mix parameter can
// be used as a driving level: mix = 1 => full drive or distortion, mix = 0 => clean signal
signed short int DistMix_l_r(signed short int *input_l, signed short int *input_r, signed short int *output)
{
	short_long_t out;

	out.l = (((long)DistMix*input_l[0])>>16);
	out.l += (((long)(32767L-DistMix)*input_r[0])>>16);
	out.l = (out.l > 32767L) ? 32767L : (out.l < -32767L) ? -32767L : out.l;
	output[0] =  out.s[1];
	output[1] =  out.s[1];
	return (output[0]);
}
/*****************************************************************************/
/* End of clean.c                                                           */
/*****************************************************************************/
