/*****************************************************************************/
/*                                                                           */
/* FILENAME                                                                  */
/*   tuner.c                                                            	 */
/*                                                                           */
/* DESCRIPTION                                                               */
/* Tuner:	    									 					     */
/* Implements autocorrelation over 1024 samples. Sampling frequency          */
/* Sampling frequency = 20kHz                                                */
/* Lowest  note (40) 2^(1/12*(40-69))*440 =    82.4Hz => 243 samples         */
/* Highest note (84) 2^(1/12*(84-69))*440 = 1046.5 Hz =>  19 samples         */
/* It is enough to implement an autocorrelation with more than 256 samples   */
/* for example 300.                                                          */
/* First 10 autocorrelation values can be ignored                            */
/*                                                                           */
/* REVISION                                                                  */
/*   Revision: 1.00                                                          */
/*   Author  : Diego de la Cruz                                              */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* HISTORY                                                                   */
/*   Revision 1.00                                                           */
/*   05/03/2016															     */
/*                                                                           */
/*****************************************************************************/
/* tuner()                                                                   */
/*****************************************************************************/
#include "gpio_control.h"
#include "csl_gpio.h"
#include "DMFX2.h"
#include <log.h>
#include "DMFX2_CSL_BIOS_cfg.h"
#include "DMFX_debug.h"
#include "tuner.h"

// Tones are calculated as
// round(20032.05/440*2^(-1/12*(tone#-69))) => (tone#69 = 440 Hz A)
// where 20032.05=1e8/156/32 the SAR sampling frequency
unsigned short int Tones[SCALE_SIZE] = {
	289, 273, 258,
	243, 229, 217, 204, 193, 182, 172, 162,
	153, 145, 136, 129, 122, 115, 108, 102,
	 96,  91,  86,  81,  77,  72,  68,  64,
	 61,  57,  54,  51,  48,  46,  43,  41,
	 38,  36,  34,  32,  30,  29,  27,  26,
	 24,  23,  21,  20,  19
};
signed short int TunerBuf[2][TUNER_SIZE];
signed short int AutoCorr[ACORR_MAX-ACORR_MIN];

unsigned short int GetPeakPeriod(void)
{
	signed long int temp = 0;
	unsigned short int i, j;
	signed short int Peak = 0;
	unsigned short int PeakPeriod = 0;
	static unsigned char pingpong = 0;

	for(i = ACORR_MIN; i < ACORR_MAX; i++)
	{
		temp = 0L;
		for(j = 0; j < TUNER_SIZE-i; j++)
		{
			temp += ((long)TunerBuf[pingpong][j]*TunerBuf[pingpong][j+i])>>15;
		}
		temp = ((AC_FILT*temp)>>15) + (((32767L-AC_FILT)*AutoCorr[i-ACORR_MIN])>>15);
		temp = (temp > 32767L) ? 32767 : (temp < -32767L) ? -32767 : (short)temp;
		AutoCorr[i-ACORR_MIN] = temp;
	}
	for(i = ACORR_MAX-ACORR_MIN-2; i > 0 ; i--) {
		if ((AutoCorr[i+1] < AutoCorr[i]) && (AutoCorr[i-1] < AutoCorr[i]))
			if (Peak < AutoCorr[i])
			{
				Peak = AutoCorr[i];
				PeakPeriod = i+ACORR_MIN;
			}
	}
	pingpong ^= 1;
	return(PeakPeriod);
}

unsigned short int Tuner (unsigned short int PeakPeriod)
{
	unsigned short int i;
	signed short int temp;

	LedTuner(PeakPeriod);
	for (i = SCALE_SIZE-1; i > 0; i--)
	{
		if(Tones[i] == PeakPeriod)
		{
			temp = 4*8*i;
			if (DEBUG>=9)
				LOG_printf(&trace, "noteidx = %d", temp);
			return(temp);  // 4 characters/note, 8 pixels/chracter
		}
		else if((Tones[i] < PeakPeriod)&&(Tones[i-1] > PeakPeriod))
		{
			temp = -(4*8*(PeakPeriod - Tones[i])/(Tones[i-1]-Tones[i]));
			temp += 4*8*i;

			if (DEBUG>=9)
				LOG_printf(&trace, "noteidx = %d", temp);
			return(temp);
		}
	}
	if(Tones[0] = PeakPeriod){
		if (DEBUG>=9)
			LOG_printf(&trace, "noteidx = 0");
		return(0);
	}
	else {
		if (DEBUG>=9)
			LOG_printf(&trace, "noteidx = %d", 4*8*SCALE_SIZE);
		return(4*8*SCALE_SIZE);
	}
}

void LedTuner(unsigned short int PeakPeriod)
{
	if((PeakPeriod <= 265)&&(PeakPeriod > 250))	// D#1
	{
		SET_LED3;
		CLR_LED4;
		CLR_LED5;
	}
	else if((PeakPeriod <= 250)&&(PeakPeriod > 236))	// E1 open note
	{
		CLR_LED3;
		SET_LED4;
		CLR_LED5;
	}
	else if((PeakPeriod <= 236)&&(PeakPeriod > 223))	// F1
	{
		CLR_LED3;
		CLR_LED4;
		SET_LED5;
	}
	else if((PeakPeriod <= 199)&&(PeakPeriod > 187))	// G#2
	{
		SET_LED3;
		CLR_LED4;
		CLR_LED5;
	}
	else if((PeakPeriod <= 187)&&(PeakPeriod > 177))	// A2 open note
	{
		CLR_LED3;
		SET_LED4;
		CLR_LED5;
	}
	else if((PeakPeriod <= 177)&&(PeakPeriod > 167))	// A#2
	{
		CLR_LED3;
		CLR_LED4;
		SET_LED5;
	}
	else if((PeakPeriod <= 149)&&(PeakPeriod > 140))	// C#3
	{
		SET_LED3;
		CLR_LED4;
		CLR_LED5;
	}
	else if((PeakPeriod <= 140)&&(PeakPeriod > 133))	// D3 open note
	{
		CLR_LED3;
		SET_LED4;
		CLR_LED5;
	}
	else if((PeakPeriod <= 133)&&(PeakPeriod > 125))	// D#3
	{
		CLR_LED3;
		CLR_LED4;
		SET_LED5;
	}
	else if((PeakPeriod <= 111)&&(PeakPeriod > 105))	// F#4
	{
		SET_LED3;
		CLR_LED4;
		CLR_LED5;
	}
	else if((PeakPeriod <= 105)&&(PeakPeriod > 99))	// G4 open note
	{
		CLR_LED3;
		SET_LED4;
		CLR_LED5;
	}
	else if((PeakPeriod <= 99)&&(PeakPeriod > 94))	// G#4
	{
		CLR_LED3;
		CLR_LED4;
		SET_LED5;
	}
	else if((PeakPeriod <= 88)&&(PeakPeriod > 83))	// A#5
	{
		SET_LED3;
		CLR_LED4;
		CLR_LED5;
	}
	else if((PeakPeriod <= 83)&&(PeakPeriod > 79))	// B5 open note
	{
		CLR_LED3;
		SET_LED4;
		CLR_LED5;
	}
	else if((PeakPeriod <= 79)&&(PeakPeriod > 74))	// C5
	{
		CLR_LED3;
		CLR_LED4;
		SET_LED5;
	}
	else if((PeakPeriod <= 66)&&(PeakPeriod > 63))	// d#6
	{
		SET_LED3;
		CLR_LED4;
		CLR_LED5;
	}
	else if((PeakPeriod <= 63)&&(PeakPeriod > 59))	// e6 open note
	{
		CLR_LED3;
		SET_LED4;
		CLR_LED5;
	}
	else if((PeakPeriod <= 59)&&(PeakPeriod > 56))	// f6
	{
		CLR_LED3;
		CLR_LED4;
		SET_LED5;
	}
	else
	{
		CLR_LED3;
		CLR_LED4;
		CLR_LED5;
	}
}
