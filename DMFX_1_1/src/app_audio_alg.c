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
#include "flanger.h"
#include "variabledelay.h"
#include "reverb.h"

#undef TEST

// Fx parameters
extern Uint16 FxCmd[FXCMD_SIZE];
#if 0
extern Uint16 CompressOn;
extern Uint16 EqualizeOn;
#endif
extern Uint16 DistOn;
extern Uint16 PhaserOn;
extern Uint16 FlangerOn;
extern Uint16 DelayOn;
extern Uint16 ChorusOn;
extern Uint16 TremoloOn;
extern Uint16 EchoOn;
extern Uint16 ReverbOn;

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
        	// Compressor implemented by DRC function in AIC3204 codec
            if(CompressOn)
            {
            	compressor(&fxin[0], fxin);
            }
#endif
            if(DistOn)
            {
            	DistMix_l_r(&fxin[0], &fxin[1], fxin);
            }
#if 0
            // Equalizer implemented by Equalize function in AIC3204 codec
            if(EqualizeOn)
            {
            	equalizer(&fxin[0], fxin);
            }
#endif
            if(PhaserOn)
            {
            	phaser(&fxin[0], fxin);
            }
            if(FlangerOn)
            {
            	flanger(&fxin[0], fxin);
            }
            if(DelayOn || ChorusOn || TremoloOn || EchoOn)
            {
            	variabledelay(&fxin[0], fxin);
            }
            if(ReverbOn)
            {
            	reverb(&fxin[0], fxin);
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

