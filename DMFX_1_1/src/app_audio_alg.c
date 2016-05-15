/*
 * $$$MODULE_NAME app_audio_alg.c
 *
 * $$$MODULE_DESC app_audio_alg.c
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

#include <std.h>
#include <clk.h>
#include <string.h>
#include <dsplib.h>
#include <tms320.h>
#include "dsp_fx.h"
#include "i2s_sample.h"
#include "app_audio_alg.h"
#include "psp_i2s.h"
#include "csl_mmcsd.h"
#include <csl_intc.h>
#include <csl_general.h>
#if 0
#include "csl_mmcsd_dma.h"
#endif
#include "DMFX1.h"
#include "DMFX1_CSL_BIOS_cfg.h"
#include "DMFX_debug.h"
#include "clean.h"
#include "compressor.h"
#include "equalizer.h"
#include "phaser.h"
#include "variabledelay.h"

#undef TEST

// Fx parameters
extern Uint16 FxCmd[FXCMD_SIZE];

signed short int BL;
signed short int FF;
signed short int FB;
signed short int stereo;
signed short int type;
signed short int depth;
signed long int delay;
unsigned long int freq;

/* Perform Record (Rx) audio algorithm processing */
void Rx_Audio_Tsk(void)
{
    short *ptr_l_rx;
    short *ptr_r_rx;
    unsigned short i;
    signed short int fxin[2];
#ifdef TEST
    static signed short test = 0;
#endif

#if DEBUG>=10
    unsigned short semcnt = 0;
#endif

#ifdef THRLOAD_ENABLE
    TSK_Attrs attrs = TSK_ATTRS;
	Bool rc;
#endif

#ifdef STS_ENABLE
	STS_add(&STS_AudioRx_Tsk, 0); // debug
#endif
#if DEBUG>=10
    semcnt = SEM_count(&SEM_DmaRxLeftComplete);
    LOG_printf(&trace, "SEM_DmaRxLeftComplete init count = %d", semcnt);
#endif

    SEM_reset(&SEM_DmaRxLeftComplete, 0);
#ifdef STS_ENABLE
    STS_reset(&STS_AudioRx_Tsk); // debug
#endif
    while (1)
    {
#if DEBUG>=10
    	LOG_printf(&trace, "pend SEM_DmaRxLeftComplete. OutBufIdxW = %d, InBufIdx = %d", OutBufIdxW, InBufIdx);
#endif
        SEM_pend(&SEM_DmaRxLeftComplete, SYS_FOREVER);
        RxIntCounter--;
#ifdef THRLOAD_ENABLE
    	THRLOAD_update();
#endif
#ifdef STS_ENABLE
        STS_set(&STS_AudioRx_Tsk, CLK_gethtime()); // debug
#endif
        /* Get pointer to ping/pong buffer */
        ptr_l_rx = ping_pong_i2sRxLeftBuf + (left_rx_buf_sel)*I2S_RXBUFF_SZ;
        ptr_r_rx = ping_pong_i2sRxRightBuf + (left_rx_buf_sel)*I2S_RXBUFF_SZ;
        /* Get data from ping/pong buffer */
        for (i = 0; i < RXBUFF_SZ_ADCSAMPS; i++)
        {
        	recInBuf_l[InBufIdx + i] = *ptr_l_rx++;
        	recInBuf_r[InBufIdx + i] = *ptr_r_rx++;
        }
        codec_input_buffer_avail = TRUE; /* indicate buffer available to I2S Tx ISR */
        for (i = 0; i < RXBUFF_SZ_ADCSAMPS; i++)
        {
        	fxin[0] = recInBuf_l[InBufIdx + i];
        	fxin[1] = recInBuf_r[InBufIdx + i];
            // FX chain
#if 0
        	// Compressor implemented via DRC function in AIC3204 codec
            if(FxCmd[COMPRESSOR])
            {
            	compressor(&fxin[0], &fxin[0]);
            	fxin[1] = fxin[0];
            }
#endif
            if(FxCmd[DISTORTION])
            {
            	DistMix_l_r(&fxin[0], &fxin[1], &fxin[0]);
            	fxin[1] = fxin[0];
            }
#if 0
            if(FxCmd[EQUALIZER])
            {
            	equalizer(&fxin[0], &fxin[0]);
            	fxin[1] = fxin[0];
            }
#endif
            if(FxCmd[PHASER])
            {
            	phaser2(&fxin[0], fxin);
            }
            if(FxCmd[DELAY])
            {
            	// convert parameters from 0 to 255 (8-bits) to -32767 to 32767 (signed 15-bits)
            	BL  = (FxCmd[DEL_BL]-128)<<8;
            	FF  = (FxCmd[DEL_FF]-128)<<8;
            	FB  = (FxCmd[DEL_FB]-128)<<8;
            	type  = FxCmd[DEL_LFO_LIST];
            	depth = (FxCmd[DEL_DEPTH]-128)<<8;
            	delay = (long)FxCmd[DEL_DELAY]<<7;
            	// fmod is lfo modulation frequency in tenths of Hz
            	// sine function is called as sine(pi*phase(Q15)) where phase is a Q15 number between -1,1, -32768,32767
            	// lfo frequency is so low that the accumulated phase is represented in a Q31 number
            	// pi*freq is the digital frequency in radians in a Q31 number format
            	// sin(pi*2*fmod(Q31)/10/48000)=> 2*fmod*2^31/48000/10 => fmod*8948L
            	// FxCmd is a value between 0-255 to scale to 0-100 dHz => 8948*100/255=3509
            	// Fill-in buffer with input signal and increment present buffer index
            	freq = (long)FxCmd[DEL_RATE]*3510L;		// 255*3510 = 895050 => 895050/89480 = 10Hz
            	stereo = FxCmd[DEL_STEREO_LIST];		// PH_STEREO_LIST: 0=mono, 1=stereo, 2=leslie
            	variabledelay(&fxin[0], fxin);
            }
            /*-----------------------------------------------------------------------------*/
            /*             BL   FF    FB      DELAY       DEPTH        MOD                 */
            /*  ISChorus  1     0.7    0      1 30 ms     1 30 ms    0.15 Hz Lowpass Noise */
            /*  Chorus    0.7   1      0.7    1 30 ms     1 30 ms    0.15 Hz Lowpass Noise */
            /*-----------------------------------------------------------------------------*/
            else if(FxCmd[CHORUS])
            {
            	if(FxCmd[CH_TYPE_LIST] == ISCH)
            	{
                	BL = 32767;
                	FF = 0.7*32767;
                	FB = 0;
            	}
            	else
            	{
                	BL = 0.7*32768;
                	FF = 32767;
                	FB = 0.7*32768;
            	}
            	type  = NOISE;
            	depth = FxCmd[CH_DEPTH]<<7;
            	delay = (long)FxCmd[CH_DELAY]*6;	// 255*6 = 1530 samples => 1530/48 = 32ms
            	freq = (long)FxCmd[CH_RATE]*3510L;		// 255*3510 = 895050 => 895050/89480 = 10Hz
            	stereo = FxCmd[CH_STEREO_LIST];		// PH_STEREO_LIST: 0=mono, 1=stereo, 2=leslie
            	variabledelay(&fxin[0], fxin);
            }
            /*-----------------------------------------------------------------------------*/
            /*             BL   FF    FB      DELAY       DEPTH        MOD                 */
            /*  Flanger   0.7   0.7   -0.7    0 10 ms     0 10 ms    0.1 1 Hz sine         */
            /*-----------------------------------------------------------------------------*/
            else if(FxCmd[FLANGER])
            {
				BL = 0.7*32767;
				FF = 0.7*32767;
				FB = -0.7*32767;
            	type  = SINE;
            	depth = FxCmd[FL_DEPTH]<<7;
            	delay = (long)FxCmd[FL_DELAY]*15/8;	// 255*15/8 = 475 samples => 478/48 = 10ms
            	freq = (long)FxCmd[FL_RATE]*351L;		// 255*351 = 89505 => 89505/89480 = 1Hz
            	stereo = 0; //FxCmd[FL_STEREO_LIST];		// PH_STEREO_LIST: 0=mono, 1=stereo, 2=leslie
            	variabledelay(&fxin[0], fxin);
            }
            /*-----------------------------------------------------------------------------*/
            /*             BL   FF    FB      DELAY       DEPTH        MOD                 */
            /*  Tremolo   0     1      0      0 5 ms      0 5 ms     0.1 5 Hz sine         */
            /*-----------------------------------------------------------------------------*/
            else if(FxCmd[TREMOLO])
            {
				BL = 0;
				FF = 32767;
				FB = 0;
            	type  = SINE;
            	depth = FxCmd[TR_DEPTH]<<7;
            	delay = (long)FxCmd[TR_DELAY];			// 255 samples => 255/48 = 5ms
            	freq = (long)FxCmd[TR_RATE]*1754L;		// 255*351 = 447270 => 447270/89480 = 5Hz
            	stereo = 0; //FxCmd[TR_STEREO_LIST];	// PH_STEREO_LIST: 0=mono, 1=stereo, 2=leslie
            	variabledelay(&fxin[0], fxin);
            }
            /*-----------------------------------------------------------------------------*/
            /*             BL   FF    FB      DELAY       DEPTH        MOD                 */
            /*  Doubling  0.7   0.7    0      10 100 ms   10 100 ms  Lowpass noise         */
            /*  Echo      1     <1     <1     > 100 ms    0  ms      No                    */
            /*-----------------------------------------------------------------------------*/
            else if(FxCmd[ECHO])
            {
				BL = 32767;
            	FF  = (FxCmd[ECHO_FF]-128)<<8;
            	FB  = (FxCmd[ECHO_FB]-128)<<8;
            	type  = 0;
            	depth = FxCmd[ECHO_DEPTH]<<7;
            	delay = (long)FxCmd[ECHO_DELAY]<<7;		// 255*128 samples => 32640/48 = 680ms
            	freq = 0L;
            	stereo = FxCmd[ECHO_STEREO_LIST];		// PH_STEREO_LIST: 0=mono, 1=stereo, 2=leslie
            	variabledelay(&fxin[0], fxin);
            }
#ifdef TEST
            sine(&test, &recOutBufRight[OutBufIdxW + i], 1);
        	recOutBufLeft[OutBufIdxW + i] = recOutBufRight[OutBufIdxW + i];
        	test += 600;
#else
        	recOutBufRight[OutBufIdxW + i]= fxin[1];
        	recOutBufLeft[OutBufIdxW + i] = fxin[0];
#endif
        }
#if 0
     	variabledelay(&recInBuf_l[InBufIdx], &recOutBufLeft[OutBufIdxW], &recOutBufRight[OutBufIdxW]);
     	fixeddelay(32767, SamplesDel, &recInBuf_l[InBufIdx], &recOutBufLeft[OutBufIdxW], &recOutBufRight[OutBufIdxW]);
	    dist_blk(&recInBuf_l[InBufIdx][0], RXBUFF_SZ_ADCSAMPS, 7, &recOutBufLeft[OutBufIdxW][0], &recOutBufRight[OutBufIdxW][0]);
        compressor(&recInBuf_l[InBufIdx], RXBUFF_SZ_ADCSAMPS, 8, &recOutBufLeft[OutBufIdxW], &recOutBufRight[OutBufIdxW]);
        if(equalizer(&recInBuf_l[InBufIdx], RXBUFF_SZ_ADCSAMPS, &recOutBufLeft[OutBufIdxW], &recOutBufRight[OutBufIdxW]) != 0)
        	LOG_printf(&trace, "eq iircas saturation = %d", ++err);
        if(phaser(&recInBuf_l[InBufIdx], RXBUFF_SZ_ADCSAMPS, &recOutBufLeft[OutBufIdxW], &recOutBufRight[OutBufIdxW]) != 0)
        	LOG_printf(&trace, "phaser iircas saturation = %d", ++err);
        clean(&recInBuf_l[InBufIdx], RXBUFF_SZ_ADCSAMPS, &recOutBufLeft[OutBufIdxW], &recOutBufRight[OutBufIdxW]);
#endif
        InBufIdx += RXBUFF_SZ_ADCSAMPS;
        if(InBufIdx == (2*BUFFER_MAX_SIZE)) InBufIdx = 0;
        OutBufIdxW += RXBUFF_SZ_ADCSAMPS;
        if(OutBufIdxW == BUFFER_MAX_SIZE) OutBufIdxW = 0;
        left_rx_buf_sel ^= 0x1; /* update ping/pong */
#ifdef STS_ENABLE
        STS_delta(&STS_AudioRx_Tsk, CLK_gethtime()); // debug
#endif
#ifdef THRLOAD_ENABLE
        THRLOAD_update();
#endif
    }
}
void Tx_Audio_Tsk(void)
{
    short *pbOutBufLeft;
    short *pbOutBufRight;
    unsigned short i;

#if DEBUG>=10
    unsigned short semcnt = 0;
#endif

#ifdef THRLOAD_ENABLE
    TSK_Attrs attrs = TSK_ATTRS;
	Bool rc;
#endif

#ifdef STS_ENABLE
	STS_add(&STS_AudioTx_Tsk, 0); // debug
#endif
#if DEBUG>=10
	semcnt = SEM_count(&SEM_TxSampsSent);
	LOG_printf(&trace, "SEM_TxSampsSent init count = %d", semcnt);
#endif

    SEM_reset(&SEM_TxSampsSent, 0);
#ifdef STS_ENABLE
    STS_reset(&STS_AudioTx_Tsk); // debug
#endif
    while (1)
    {
#if DEBUG>=10
        LOG_printf(&trace, "Pend SEM_TxSampsSent");
#endif
        SEM_pend(&SEM_TxSampsSent, SYS_FOREVER);
#ifdef THRLOAD_ENABLE
        THRLOAD_update();
#endif
#ifdef STS_ENABLE
        STS_set(&STS_AudioTx_Tsk, CLK_gethtime()); // debug
#endif
        TxIntCounter--;
        pbOutBufLeft  = ping_pong_i2sTxLeftBuf  + (tx_buf_sel)*MAX_I2S_TXBUFF_SZ;
        pbOutBufRight = ping_pong_i2sTxRightBuf + (tx_buf_sel)*MAX_I2S_TXBUFF_SZ;
        // tx_buf_sel ^= 0x1;  moved down to be used by recOutBuf ping pong
        for (i = 0; i < MAX_I2S_TXBUFF_SZ; i++)
        {
            pbOutBufLeft[i]  = recOutBufLeft[OutBufIdxR];
            pbOutBufRight[i] = recOutBufRight[OutBufIdxR++];
        }
        codec_output_buffer_avail = TRUE; /* indicate buffer available to I2S Tx ISR */
#if 0
        if ((OutBufIdxR % (BUFFER_MAX_SIZE/2)) == 0)
        {
	#ifdef DEBUG
            LOG_printf(&trace, "post SEM_MMC_Read. OutBufIdxR = %d, InBufIdx = %d", OutBufIdxR, InBufIdx);
	#endif
            SEM_post(&SEM_MMC_Read);
        }
#endif
        if(OutBufIdxR == BUFFER_MAX_SIZE) OutBufIdxR = 0;
        tx_buf_sel ^= 0x1; /* update ping/pong */
#ifdef STS_ENABLE
        STS_delta(&STS_AudioTx_Tsk, CLK_gethtime()); // debug
#endif
#ifdef THRLOAD_ENABLE
        THRLOAD_update();
#endif
    }
}

/**
 *  \brief  Function to initialize the audio record
 *
 *  \param  status    - Status of audio record
 *  \param  ptr       - Pointer for future use
 *
 *  \return Media status
 */
CSL_Status appInitAudio(
    Uint16    status,
    void      *ptr
)
{
#if 0
	Uint16 i;
#endif

    // reset codec input circular buffer
    memset(codec_input_buffer, 0, CODEC_INPUT_BUFFER_SIZE);
    codec_input_buffer_input_index = CODEC_INPUT_BUFFER_SIZE/2;
    codec_input_buffer_output_index = 0;
    codec_input_buffer_underflow = 0;
    codec_input_buffer_overflow = 0;
	i2sTxBuf_idx = 0;
#if 0
    // Clear Delay Buffers
    for (i = 0; i < 2*BUFFER_MAX_SIZE; i++)
    {
   		DelayBuf[i] = 0;
    }
#endif
    memset(recInBuf_l, 0, 2*BUFFER_MAX_SIZE);
    memset(recInBuf_r, 0, 2*BUFFER_MAX_SIZE);
    memset(recOutBufLeft, 0, BUFFER_MAX_SIZE);
    memset(recOutBufRight, 0, BUFFER_MAX_SIZE);
    return CSL_SOK;
}

