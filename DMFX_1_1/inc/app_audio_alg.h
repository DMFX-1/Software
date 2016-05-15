/*
 * $$$MODULE_NAME app_audio_alg.h
 *
 * $$$MODULE_DESC app_audio_alg.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

#ifndef _APP_AUDIO_ALG_H_
#define _APP_AUDIO_ALG_H_

CSL_Status appInitAudio(
    Uint16    status,
    void      *ptr
);

#ifndef SAMPLE_BY_SAMPLE_PB
extern Int16 ZeroBuf[MAX_I2S_TXBUFF_SZ];
extern DMA_ChanHandle   hDmaTxLeft;
extern DMA_ChanHandle   hDmaTxRight;
extern Int16 ping_pong_i2sTxLeftBuf[];
extern Int16 ping_pong_i2sTxRightBuf[];
#endif


#ifdef SAMPLE_RATE_48kHz
	#define SAMPLES_PER_MS 48
#else
	#define SAMPLES_PER_MS 16
#endif

// It used to be defined on csl_mmcsd.h according to MMC/SD block length
#ifdef SAMPLE_RATE_48kHz
	#define BUFFER_MAX_SIZE  (512u)	//(16*CSL_MMCSD_BLOCK_LENGTH)
#else
	#define BUFFER_MAX_SIZE  (512u) // (CSL_MMCSD_BLOCK_LENGTH)
#endif

extern Uint16 i2sTxBuf_idx;

extern signed short int recInBuf_l[2*BUFFER_MAX_SIZE];
extern signed short int recInBuf_r[2*BUFFER_MAX_SIZE];
extern signed short int recOutBufLeft[BUFFER_MAX_SIZE];
extern signed short int recOutBufRight[BUFFER_MAX_SIZE];
#if 0
extern signed short int DelayBuf[2*BUFFER_MAX_SIZE];
#endif
extern signed short int InBufIdx;
extern signed short int OutBufIdxW;
extern signed short int OutBufIdxR;
extern unsigned long int SamplesDel;

/* Perform Record (Rx) audio algorithm processing */
void Rx_Audio_Tsk(void);
/* Perform Play (Tx) audio algorithm processing */
void Tx_Audio_Tsk(void);

//extern TskRec tskRecs[NUMTSKS];
extern Int16 ping_pong_i2sTxLeftBuf[]; /* 2x for ping/pong */
extern Int16 ping_pong_i2sTxRightBuf[]; /* 2x for ping/pong */

/* Indicates whether codec output buffer available */
extern Bool codec_output_buffer_avail;
/* Indicates whether codec input buffer available */
extern Bool codec_input_buffer_avail;
extern Int16 TxIntCounter;
extern Int16 RxIntCounter;

#endif /* _APP_AUDIO_ALG_H_ */
