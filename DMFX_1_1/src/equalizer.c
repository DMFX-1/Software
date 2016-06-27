/*****************************************************************************/
/*                                                                           */
/* FILENAME                                                                  */
/*   equalizer.c                                                             */
/*                                                                           */
/* DESCRIPTION                                                               */
/*   DMFX-1. Simulates a 3-band equalizer with Bass, Middle, treble levels   */
/*   between -12dB to 0dB													 */
/*   G=-12:0; V0=10.^(G/20); H0 = V0 - 1;           						 */
/*                                                                           */
/*   LPF first order low shelving filter for bass:							 */
/*     flc (cut freq) =  100 Hz, Kl = tan(pi*flc/fs);						 */
/* 	   if (G>=0) cl = (Kl - 1)/(Kl + 1)										 */
/* 	   if (G<0)  cl = (Kl - V0)/(Kl + V0)									 */
/* 	   coefficients: b0 = cl, b1 = 1, a0 = 1, a1 = cl  						 */
/*                                                                           */
/*   BPF second order peak filter for middle:								 */
/*     fmc (cut freq) =  440 Hz, fmb (BW) = 600, Km = tan(pi*fmb/fs)		 */
/* 	   if (G>=0) cm = (Kmb - 1)/(Kmb + 1)									 */
/* 	   if (G<0)  cm = (Kmb - V0)/(Kmb + V0)									 */
/*	   dm = -cos(2*pi*fmc/fs)												 */
/* 	   coefficients: b0 = cm, b1 = -dm*(1-cm), b2 = -1 						 */
/* 	   a0 = 1, a1 = dm*(1-cm), a2 = -cm	 									 */
/*                                                                           */
/*   HPF first order high shelving filter for treble:						 */
/*     fhc (cut freq) =  1936 Hz, Kh = tan(pi*fhc/fs);						 */
/* 	   if (G>=0) ch = (Kh - 1)/(Kh + 1)										 */
/* 	   if (G<0)  ch = (V0*Kh - 1)/(V0*Kh + 1)								 */
/* 	   coefficients: b0 = ch, b1 = 1, a0 = 1, a1 = ch  						 */
/*                                                                           */
/* REVISION                                                                  */
/*   Revision: 1.00                                                          */
/*   Author  : Diego de la Cruz                                              */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* HISTORY                                                                   */
/*   Revision 1.00                                                           */
/*   21/03/2016															     */
/*                                                                           */

/*****************************************************************************/
/* equalizer()                                                               */
/*****************************************************************************/
#include <std.h>
#include "csl_i2c.h"
#include "csl_intc.h"
#include "DMFX1.h"
#include "dsp_fx.h"
#include "DMFX_debug.h"
#include <log.h>
#include "DMFX1_CSL_BIOS_cfg.h"
#include "csl_i2c_intc.h"
#include "equalizer.h"

// Fx parameters
extern Uint16 FxCmd[FXCMD_SIZE];
extern Uint16 EqualizeOn;

typedef struct biquad_t
{
	char c24[3];
} biquad_t;


biquad_t bq[BANDS+1][5][EQ_STEPS] =
{
//Level
	//0 => -40				1 => -36				2 => -32				3 => -28				4 => -24				5 => -20				6 => -16				7 => -12				8 => -8					9 => -4					10 => 0
	{{{0x01, 0x47, 0xAE}, 	{0x02, 0x07, 0x56}, 	{0x03, 0x37, 0x18}, 	{0x05, 0x18, 0x84}, 	{0x08, 0x13, 0x85}, 	{0x0C, 0xCC, 0xCD}, 	{0x14, 0x49, 0x61}, 	{0x20, 0x26, 0xF3}, 	{0x32, 0xF5, 0x2D}, 	{0x50, 0xC3, 0x36}, 	{0x7F, 0xFF, 0xFF}}, //bq0
	{{0x00, 0x00, 0x00}, 	{0x00, 0x00, 0x00}, 	{0x00, 0x00, 0x00}, 	{0x00, 0x00, 0x00}, 	{0x00, 0x00, 0x00}, 	{0x00, 0x00, 0x00}, 	{0x00, 0x00, 0x00}, 	{0x00, 0x00, 0x00}, 	{0x00, 0x00, 0x00}, 	{0x00, 0x00, 0x00}, 	{0x00, 0x00, 0x00}}, //bq1
	{{0xFE, 0xB8, 0x52}, 	{0xFD, 0xF8, 0xAA}, 	{0xFC, 0xC8, 0xE8}, 	{0xFA, 0xE7, 0x7C}, 	{0xF7, 0xEC, 0x7B}, 	{0xF3, 0x33, 0x33}, 	{0xEB, 0xB6, 0x9F}, 	{0xDF, 0xD9, 0x0D}, 	{0xCD, 0x0A, 0xD3}, 	{0xAF, 0x3C, 0xCA}, 	{0x80, 0x00, 0x00}}, //bq2
	{{0x00, 0x00, 0x00}, 	{0x00, 0x00, 0x00}, 	{0x00, 0x00, 0x00}, 	{0x00, 0x00, 0x00}, 	{0x00, 0x00, 0x00}, 	{0x00, 0x00, 0x00}, 	{0x00, 0x00, 0x00}, 	{0x00, 0x00, 0x00}, 	{0x00, 0x00, 0x00}, 	{0x00, 0x00, 0x00}, 	{0x00, 0x00, 0x00}}, //aq1
	{{0x7F, 0xFF, 0xFF}, 	{0x7F, 0xFF, 0xFF}, 	{0x7F, 0xFF, 0xFF}, 	{0x7F, 0xFF, 0xFF}, 	{0x7F, 0xFF, 0xFF}, 	{0x7F, 0xFF, 0xFF}, 	{0x7F, 0xFF, 0xFF}, 	{0x7F, 0xFF, 0xFF}, 	{0x7F, 0xFF, 0xFF}, 	{0x7F, 0xFF, 0xFF}, 	{0x7F, 0xFF, 0xFF}}}, //aq2
//Bass	
	//-15					-12						-9						-6						-3						0						3						6						9						12						15
	{{{0x6A, 0xB9, 0x6E}, 	{0x6B, 0x2D, 0xB1}, 	{0x6B, 0x80, 0x98}, 	{0x6B, 0xBB, 0x97}, 	{0x6B, 0xE5, 0x82}, 	{0x6C, 0x03, 0x43}, 	{0x6C, 0x21, 0x0C}, 	{0x6C, 0x4B, 0x1F}, 	{0x6C, 0x86, 0x8D}, 	{0x6C, 0xDA, 0x7F}, 	{0x6D, 0x51, 0x13}}, //b0
	{{0x95, 0x8F, 0x66}, 	{0x95, 0x1B, 0x73}, 	{0x94, 0xC8, 0xC4}, 	{0x94, 0x8D, 0xED}, 	{0x94, 0x64, 0x1F}, 	{0x94, 0x46, 0x72}, 	{0x94, 0x46, 0x72}, 	{0x94, 0x46, 0x72}, 	{0x94, 0x46, 0x72}, 	{0x94, 0x46, 0x72}, 	{0x94, 0x46, 0x72}}, //b1
	{{0x6A, 0x2A, 0xC1}, 	{0x6A, 0x9E, 0x68}, 	{0x6A, 0xF0, 0xE0}, 	{0x6B, 0x2B, 0x90}, 	{0x6B, 0x55, 0x43}, 	{0x6B, 0x72, 0xDD}, 	{0x6B, 0x55, 0x13}, 	{0x6B, 0x2B, 0x01}, 	{0x6A, 0xEF, 0x93}, 	{0x6A, 0x9B, 0xA0}, 	{0x6A, 0x25, 0x0C}}, //b2
	{{0x7E, 0x22, 0xD3}, 	{0x7E, 0xAC, 0x3A}, 	{0x7F, 0x0E, 0x36}, 	{0x7F, 0x53, 0xF0}, 	{0x7F, 0x85, 0x7B}, 	{0x7F, 0xA8, 0xA5}, 	{0x7F, 0xA8, 0xA5}, 	{0x7F, 0xA8, 0xA5}, 	{0x7F, 0xA8, 0xA5}, 	{0x7F, 0xA8, 0xA5}, 	{0x7F, 0xA8, 0xA5}}, //a1
	{{0x83, 0xB6, 0xCF}, 	{0x82, 0xA3, 0xFC}, 	{0x81, 0xE0, 0x01}, 	{0x81, 0x54, 0x8C}, 	{0x80, 0xF1, 0x74}, 	{0x80, 0xAB, 0x1F}, 	{0x80, 0xAB, 0x1F}, 	{0x80, 0xAB, 0x1F}, 	{0x80, 0xAB, 0x1F}, 	{0x80, 0xAB, 0x1F}, 	{0x80, 0xAB, 0x1F}}}, //a2
//Mid		
	//-15					-12						-9						-6						-3						0						3						6						9						12						15              
	{{{0x67, 0x13, 0x4B}, 	{0x68, 0xC6, 0xD5}, 	{0x6A, 0x03, 0xF7}, 	{0x6A, 0xE9, 0x0D}, 	{0x6B, 0x8D, 0x95}, 	{0x6C, 0x03, 0x43}, 	{0x6C, 0x79, 0x72}, 	{0x6D, 0x20, 0x63}, 	{0x6E, 0x0C, 0x31}, 	{0x6F, 0x59, 0x47}, 	{0x71, 0x2F, 0xC7}}, //b0
	{{0x9A, 0x14, 0xFE}, 	{0x98, 0x66, 0x58}, 	{0x97, 0x2C, 0xC6}, 	{0x96, 0x4A, 0x43}, 	{0x95, 0xA7, 0x93}, 	{0x95, 0x33, 0x37}, 	{0x95, 0x33, 0x37}, 	{0x95, 0x33, 0x37}, 	{0x95, 0x33, 0x37}, 	{0x95, 0x33, 0x37}, 	{0x95, 0x33, 0x37}}, //b1
	{{0x64, 0xF0, 0x86}, 	{0x66, 0x9B, 0x0A}, 	{0x67, 0xD1, 0x9A}, 	{0x68, 0xB1, 0xF0}, 	{0x69, 0x53, 0x10}, 	{0x69, 0xC6, 0x4E}, 	{0x69, 0x50, 0x1F}, 	{0x68, 0xA9, 0x2E}, 	{0x67, 0xBD, 0x60}, 	{0x66, 0x70, 0x4A}, 	{0x64, 0x99, 0xCA}}, //b2
	{{0x78, 0xC7, 0x05}, 	{0x7A, 0xC5, 0x5C}, 	{0x7C, 0x38, 0xF4}, 	{0x7D, 0x45, 0x62}, 	{0x7E, 0x06, 0x2C}, 	{0x7E, 0x90, 0x10}, 	{0x7E, 0x90, 0x10}, 	{0x7E, 0x90, 0x10}, 	{0x7E, 0x90, 0x10}, 	{0x7E, 0x90, 0x10}, 	{0x7E, 0x90, 0x10}}, //a1
	{{0x8E, 0x3B, 0xAA}, 	{0x8A, 0x3E, 0x18}, 	{0x87, 0x56, 0x3F}, 	{0x85, 0x3C, 0xEB}, 	{0x83, 0xBB, 0x01}, 	{0x82, 0xA6, 0xFB}, 	{0x82, 0xA6, 0xFB}, 	{0x82, 0xA6, 0xFB}, 	{0x82, 0xA6, 0xFB}, 	{0x82, 0xA6, 0xFB}, 	{0x82, 0xA6, 0xFB}}}, //a2
//Treble	
	//-15					-12						-9						-6						-3						0						3						6						9						12						15              
	{{{0x5B, 0x25, 0x82}, 	{0x60, 0x7F, 0xC8}, 	{0x64, 0xAF, 0x61}, 	{0x67, 0xE0, 0x00}, 	{0x6A, 0x42, 0x1F}, 	{0x6C, 0x03, 0x43}, 	{0x6D, 0xCB, 0xD1}, 	{0x70, 0x50, 0xB8}, 	{0x73, 0xDF, 0xAB}, 	{0x78, 0xE6, 0x6B}, 	{0x7F, 0xFF, 0xFF}}, //b0
	{{0xA9, 0xBA, 0x10}, 	{0xA4, 0xA9, 0x0D}, 	{0xA0, 0xB2, 0xC0}, 	{0x9D, 0xAD, 0xCB}, 	{0x9B, 0x6C, 0x4A}, 	{0x99, 0xC3, 0x2A}, 	{0x99, 0xC3, 0x2A}, 	{0x99, 0xC3, 0x2A}, 	{0x99, 0xC3, 0x2A}, 	{0x99, 0xC3, 0x2A}, 	{0x99, 0xC3, 0x2A}}, //b1
	{{0x53, 0xD9, 0xBB}, 	{0x58, 0xC6, 0x51}, 	{0x5C, 0xA0, 0x23}, 	{0x5F, 0x8F, 0x63}, 	{0x61, 0xC0, 0xAC}, 	{0x63, 0x5D, 0xDC}, 	{0x61, 0x95, 0x4E}, 	{0x5F, 0x10, 0x67}, 	{0x5B, 0x81, 0x74}, 	{0x56, 0x7A, 0xB5}, 	{0x4F, 0x61, 0x20}}, //b2
	{{0x66, 0x3C, 0xD5}, 	{0x6C, 0x3D, 0xD9}, 	{0x70, 0xEF, 0xD8}, 	{0x74, 0x83, 0xD5}, 	{0x77, 0x30, 0x33}, 	{0x79, 0x27, 0xFE}, 	{0x79, 0x27, 0xFE}, 	{0x79, 0x27, 0xFE}, 	{0x79, 0x27, 0xFE}, 	{0x79, 0x27, 0xFE}, 	{0x79, 0x27, 0xFE}}, //a1
	{{0xB0, 0x9E, 0xDF}, 	{0xA4, 0x71, 0x2F}, 	{0x9A, 0xEB, 0x0C}, 	{0x93, 0xA9, 0x0C}, 	{0x8E, 0x3C, 0xE0}, 	{0x8A, 0x3E, 0xFB}, 	{0x8A, 0x3E, 0xFB}, 	{0x8A, 0x3E, 0xFB}, 	{0x8A, 0x3E, 0xFB}, 	{0x8A, 0x3E, 0xFB}, 	{0x8A, 0x3E, 0xFB}}},//a2
};


Uint16 WrBuffer[20*(BANDS+1)+1] =
	{
		0x24,		// Page 8.Reg 36(0x24), Page 9.Reg 44(0x2C)
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
	};

CSL_Status equalizer(void)
{
	static char level[BANDS+1] = { 0, 0, 0, 0};
	CSL_Status  status;
	Uint16 i, j, k;

	if (!EqualizeOn)
	{
		WrBuffer[0] = 0x24;  // Reg 36
		for(i = 0; i < BANDS+1; i++)
		{
			//5 coefficients b0, b1, b2, a1, a2
			for(j = 0; j < 5; j++)
				{
				//3x 8-bit coefficients
				for(k = 0; k < 3; k++)
				{
#if 0
					regaddr = 36+5*4*i+4*j+k;
					if(regaddr > 127)
					{
						Codec_Write( 0x00, 0x09);    // select page 9
						regaddr -= 120;
					}
#endif
					if(j == 0)
						if(k == 0)
							WrBuffer[5*4*i+4*j+k+1] = 0x7F;
						else
							WrBuffer[5*4*i+4*j+k+1] = 0xFF;
					else
						WrBuffer[5*4*i+4*j+k+1] = 0x00;
				}
			}
		}
		Codec_Write( 0x00, 0x08);    // select page 8
		status = I2C_write( &WrBuffer[0],
							20*(BANDS+1)+1,
							CSL_I2C_CODEC_ADDR,
							TRUE,
							((CSL_I2C_START) | (CSL_I2C_STOP)),
							CSL_I2C_MAX_TIMEOUT);
		if(status != CSL_SOK)
		{
			LOG_printf(&trace, "I2C Write Failed!!\n");
			return(status);
		}
		WrBuffer[0] = 0x2C;  // Reg 44
		Codec_Write( 0x00, 0x09);    // select page 9
		status = I2C_write( &WrBuffer[0],
							20*(BANDS+1)+1,
							CSL_I2C_CODEC_ADDR,
							TRUE,
							((CSL_I2C_START) | (CSL_I2C_STOP)),
							CSL_I2C_MAX_TIMEOUT);
		if(status != CSL_SOK)
		{
			LOG_printf(&trace, "I2C Write Failed!!\n");
			return(status);
		}

#if 0
		for(i = 0; i < BANDS+1; i++)
		{
			//5 coefficients b0, b1, b2, a1, a2
			for(j = 0; j < 5; j++)
				{
				//3x 8-bit coefficients
				for(k = 0; k < 3; k++)
				{
					regaddr = 36+5*4*i+4*j+k;
					if(regaddr > 127)
					{
						Codec_Write( 0x00, 0x0A);    // select page 9
						regaddr -= 120;
					}
					if(j == 0)
						if(k == 0)
							Codec_Write(regaddr, 0x7f);
						else
							Codec_Write(regaddr, 0xff);
					else
						Codec_Write(regaddr, 0x00);
				}
			}
		}

		Codec_Write( 0x00, 0x08);    // select page 8
		Codec_Write( 0x01, 0x01);	 // disable adaptive filtering
#endif
	}
	else
	{
		// Left channel
		// 4 Biquads Level, Bass, Middle, Treble => A, B, C, D
		WrBuffer[0] = 0x24;  // Reg 36
		for(i = 0; i < BANDS+1; i++)
		{
			if (level[i]!= FxCmd[EQ_LEVEL+i])
				level[i] = FxCmd[EQ_LEVEL+i];   // 0 to 10
				
			//5 coefficients b0, b1, b2, a1, a2
			for(j = 0; j < 5; j++)
				{
				//3x 8-bit coefficients
				for(k = 0; k < 3; k++)
				{
#if 0
					if(regaddr > 127)
					{
						Codec_Write( 0x00, 0x09);    // select page 9
						regaddr -= 120;
					}
#endif
					WrBuffer[5*4*i+4*j+k+1]  = bq[i][j][level[i]].c24[k];
				}
			}
		}
		Codec_Write( 0x00, 0x08);    // select page 8
		Codec_Write( 0x01, 0x5);	 // enable adaptive filtering, switch coeff buffers
		status = I2C_write( &WrBuffer[0],
							20*(BANDS+1)+1,
							CSL_I2C_CODEC_ADDR,
							TRUE,
							((CSL_I2C_START) | (CSL_I2C_STOP)),
							CSL_I2C_MAX_TIMEOUT);
		if(status != CSL_SOK)
		{
			LOG_printf(&trace, "I2C Write Failed!!\n");
			return(status);
		}
		WrBuffer[0] = 0x2C;  // Reg 44
		// Right channel
		Codec_Write( 0x00, 0x09);    // select page 9
		status = I2C_write( &WrBuffer[0],
							20*(BANDS+1)+1,
							CSL_I2C_CODEC_ADDR,
							TRUE,
							((CSL_I2C_START) | (CSL_I2C_STOP)),
							CSL_I2C_MAX_TIMEOUT);
		if(status != CSL_SOK)
		{
			LOG_printf(&trace, "I2C Write Failed!!\n");
			return(status);
		}
#if 0
		for(i = 0; i < BANDS; i++)
		{
			//5 coefficients b0, b1, b2, a1, a2
			for(j = 0; j < 5; j++)
				{
				//3x 8-bit coefficients
				for(k = 0; k < 3; k++)
				{
					regaddr = 44+5*4*i+4*j+k;
					if(regaddr > 127)
					{
						Codec_Write( 0x00, 0x0A);    // select page 10
						regaddr -= 120;
					}
					Codec_Write(regaddr, bq[i][j][level[i]].c24[k]);
				}
			}
		}
#endif
	}
	return(status);
};

#if 0
signed long int H0[13] =
	{-12269, -11766, -11203, -10571,  -9861,  -9066,
     -8173,  -7171,  -6046,  -4785,  -3370,  -1782, 0};
signed long int cl[13] =
	{-31388, -31535, -31667, -31785, -31890, -31984, -32068, -32144, -32211, -32271, -32325, -32372, -32415};
// if (G>=0) cm = (Kmb - 1)/(Kmb + 1), if (G<0)  cm = (Kmb - V0)/(Kmb + V0)
// fmc (cut freq) =  440 Hz, fmb (BW) = 600, Km = tan(pi*fmb/fs)
// G=-12:0; V0=10.^(G/20)
signed long int cm[13] =
    {-15694, -15768, -15833, -15892, -15945, -15992, -16034, -16072, -16105, -16135, -16162, -16186, 0};
// -dm*(1-cm), dm = -cos(2*pi*fmc/fs)
signed long int dm_1_cm[13] =
    {32070,  32144,  32209,  32268,  32321,  32368, 32410,  32448,  32481,  32511,  32538,  32562, 16380};
signed long int ch[13] =
    {-32413, -32370, -32322, -32269, -32208, -32141, -32065, -31980, -31886, -31780, -31661, -31529, -31381};
signed long int gain[13] =
    {24615, 25619, 26746, 28011, 29429, 31021, 32807, 34811, 37059, 39582, 42413, 45589, 49152};

signed short int equalizer(signed short int *input, signed short int *output)
{
  unsigned char Level_L, Level_M, Level_H, Level;
  static signed long int xl_1  = {0L};
  static signed long int yl1_1 = {0L};
  static signed long int xm_1  = {0L};
  static signed long int xm_2  = {0L};
  static signed long int ym1_1 = {0L};
  static signed long int ym1_2 = {0L};
  static signed long int xh_1  = {0L};
  static signed long int yh1_1 = {0L};
  signed long int yl1, ym1, yh1;
  signed long int yl, ym, yh;

  Level_L = FxCmd[EQ_BASS];		//Bass level is a value between 0 - 12
  Level_M = FxCmd[EQ_MIDDLE];	//Mid level is a value between 0 - 12
  Level_H = FxCmd[EQ_TREBLE];	//Treble level is a value between 0 - 12
  Level   = FxCmd[EQ_LEVEL];	//Equalizer level is a value between 0 - 12

  // Low shelving
  yl1 = xl_1 + (((long)cl[Level_L]*((long)*input - yl1_1))>>15);
  yl = (long)*input + (((long)H0[Level_L]*((long)*input + yl1))>>15);
#if DEBUG>=9
  if((yl > 32767L)||(yl < -32767L))
    LOG_printf(&trace, "Equalizer overflow: Low = %d", (short)(yl>>16));
#endif
  yl = (yl > 32767L) ? 32767L : (yl < -32767L) ? -32767L : yl;
  yl1_1 = yl1;
  xl_1 = (long)*input;

  // Peak filter
  ym1  = (xm_2>>1) + (((long)cm[Level_M]*(ym1_2 - yl))>>15) + (((long)dm_1_cm[Level_M]*(ym1_1 - xm_1))>>15);
  ym1 <<= 1;
#if DEBUG>=9
  if((ym1 > 32767L)||(ym1 < -32767L))
    LOG_printf(&trace, "Equalizer overflow: Mid ym1 = %d", (short)(ym1>>1));
#endif
  ym1 = (ym1 > 32767L) ? 32767L : (ym1 < -32767L) ? -32767L : ym1;
  //ym1 = iircas5(*input, h, r, dp, NBIQ, NX);
  xm_2 = xm_1;
  xm_1 = yl;
  ym =  yl + (((long)H0[Level_M]*(yl - ym1))>>15);
#if DEBUG>=9
  if((ym > 32767L)||(ym < -32767L))
    LOG_printf(&trace, "Equalizer overflow: Mid ym = %d", (short)(ym>>1));
#endif
  ym = (ym > 32767L) ? 32767L : (ym < -32767L) ? -32767L : ym;
  ym1_2 = ym1_1;
  ym1_1 = ym1;
  
  // High shelving
  yh1 =  xh_1 + (((long)ch[Level_H]*(ym - yh1_1))>>15);
  yh1 = (yh1 > 32767L) ? 32767L : (yh1 < -32767L) ? -32767L : yh1;
  yh =  ym + (((long)H0[Level_H]*(ym - yh1))>>15);
#if DEBUG>=9
  if((yh > 32767L)||(yh < -32767L))
    LOG_printf(&trace, "Equalizer overflow: Hi = %d", (short)(yh>>16));
#endif
  yh = (yh > 32767L) ? 32767L : (yh < -32767L) ? -32767L : yh;
  xh_1 = ym;
  yh1_1 = yh1;
  // Add gain to output
  yh = (yh*gain[Level])>>15;
  yh = (yh > 32767L) ? 32767L : (yh < -32767L) ? -32767L : yh;

  output[0] = (short)yh;
  output[1] = (short)yh;
  return((short)yh);
}
#endif
/*****************************************************************************/
/* End of equalizer.c                                                        */
/*****************************************************************************/
