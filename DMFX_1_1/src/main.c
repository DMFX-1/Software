/*****************************************************************************/
/*                                                                           */
/* FILENAME                                                                  */
/*   main.c                                                                  */
/*                                                                           */
/* DESCRIPTION                                                               */
/*   DMFX-1.1 main file for DSP-1				                             */
/* REVISION                                                                  */
/*   Revision: 1.00                                                          */
/*   Author  : Diego de la Cruz                                              */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* HISTORY                                                                   */
/*   Revision 1.00                                                           */
/*   26/09/2015															     */
/*                                                                           */
/*****************************************************************************/
#include "DMFX1.h"
#include <std.h>
#include <string.h>
#include "csl_types.h"
#include "csl_error.h"
#include "csl_intc.h"
#include "csl_gpio.h"
#ifdef USB_APP
#include "csl_usb.h"
#include "csl_audioClass.h"
#include "app_usbac_descs.h"
#endif
#include "app_usb.h"
#include "app_usbac.h"
#include "soc.h"
#include "gpio_control.h"
#include "pll_control.h"
#include "app_globals.h"
#include "user_interface.h"
#include "i2s_sample.h"
#include "codec_aic3254.h"
#include "dda_dma.h"
#include "app_asrc.h"
#include "psp_i2s.h"
#include "app_audio_alg.h"
#include "dsplib.h"
#include "cslr.h"
#include "cslr_sysctrl.h"
#include "csl_dma.h"
#include <csl_general.h>
#include "lcdST7565.h"
#include "csl_i2c_intc.h"
#include "DMFX1_CSL_BIOS_cfg.h"
#include "DMFX_debug.h"
#include "fxcmd.h"

#ifdef EFFECTS_ON
#pragma DATA_SECTION(DelayBuf, "SARAM")
#pragma DATA_ALIGN(DelayBuf,4)
signed short int DelayBuf[2*BUFFER_MAX_SIZE];		/*[4][BUFFER_MAX_SIZE/2];	Four Delay Buffers*/
#endif

#pragma DATA_SECTION(recInBuf_l, "SARAM")
#pragma DATA_ALIGN(recInBuf_l,4)
signed short int  recInBuf_l[2*BUFFER_MAX_SIZE];
#pragma DATA_SECTION(recInBuf_r, "SARAM")
#pragma DATA_ALIGN(recInBuf_r,4)
signed short int  recInBuf_r[2*BUFFER_MAX_SIZE];

#pragma DATA_SECTION(recOutBufLeft, "SARAM")
#pragma DATA_ALIGN(recOutBufLeft,4)
Int16 recOutBufLeft[BUFFER_MAX_SIZE];
#pragma DATA_SECTION(recOutBufRight, "SARAM")
#pragma DATA_ALIGN(recOutBufRight,4)
Int16 recOutBufRight[BUFFER_MAX_SIZE];			/* BUFFER_MAX_SIZE/MAX_I2S_TXBUFF_SZ */

signed short int InBufIdx   = 0;
signed short int OutBufIdxW = 0;
signed short int OutBufIdxR = BUFFER_MAX_SIZE/2;
signed short int RdIdxDel = 0;

Uint16 active_sample_rate;

/* main task DMFX1 */
void DMFX1(void);
/* SPI Flash test */
void csl_spi_test(void);
/* Resets C5535 */
void C5535_reset(void);
/* Init DMFX Processor 1 or 2 */
void DMFX_init(void);
void CSL_acTest(void);
CSL_Status appInitAudio(Uint16 status, void *ptr);
/* Clock gating for unused peripherals */
void ClockGating(void);
/* Intializes active sample rate and I2S Tx buffer size */
Int16 initSampleRate(
    Uint16 usbSampleRate,
    Uint16 *pActiveSampleRate,
    Uint16 *pI2sTxBuffSz
);
/* SAR read voltage from potentiometers */
int	sar_test_pot_voltage(void);

#ifndef _FX_CMD_H_
Uint16 FxCmd[FXCMD_SIZE];
#endif
/*
 *  ======== main ========
 */
void main(void)
{

    LOG_printf(&trace, "main: DMFX_init DSP-1");
    DMFX_init();
    /* fall into DSP/BIOS idle loop */
    CSL_acTest();
    return;
}

/* Initializes application */
void DMFX1(void){

#if 0
    CSL_Status status;
#endif

    LOG_printf(&trace, "DMFX 1: Main task init audio codec");
#if 0
	LOG_printf(&trace, "CSL SAR Interrupt Mode Test\n\n");
    status = sar_test_pot_voltage();
    if(0 == status)
    {
    	LOG_printf(&trace, "\nCSL SAR Interrupt Mode Test Passed\n");
    }
    else
    {
    	LOG_printf(&trace, "\nCSL SAR Interrupt Mode Test Failed\n");
    }
#endif
	TSK_sleep(1000);
    Beep_Generator(2000, 2);
    while(1){
    	TSK_sleep(10000);
    }
}

/**
 *  \brief  Init DMFX processor
 *
 *  \param  processor#
 *
 *  \return None
 */
void DMFX_init(void){
    CSL_Status status;
    Uint32 gpioIoDir, gpioIoDat;
#if defined(DMFX_PROC2)
    Uint32 gpioIntEdg, gpioIntEn;
#endif

    /*
     * PCGCR1
     *    15       14     13     12   11    10      9      8       7       6   5      4       3      2     1     0
     * SYSCLKDIS I2S2CG TMR2CG TMR1CG RSV TMR0CG I2S1CG I2S0CG MMCSD1CG I2CCG RSV MMCSD0CG DMA0CG UARTCG SPICG I2S3CG
     * PCGCR2
     *   15...7      6       5      4      3     2      1     0
     * RSV15 RSV7 ANAREGCG DMA3CG DMA2CG DMA1CG USBCG SARCG LCDCG
     */
    /* Clock gate all peripherals */
    CSL_SYSCTRL_REGS->PCGCR1 = 0x7FFF;
    CSL_SYSCTRL_REGS->PCGCR2 = 0x007F;
    /* Configure EBSR */
#if defined(USE_SD0)
    /* SP0 Mode 0 (MMC/SD0) */
    CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_SP0MODE, MODE0);
#elif defined(USE_I2S0_PB) || defined(USE_I2S0_REC)
    /* SP0 Mode 1 (I2S0 and GP[5:4]) */
    CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_SP0MODE, MODE1);
#else
    /* SP0 Mode 2 (GP[5:0]) -- GPIO02/GPIO04 for debug  */
    CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_SP0MODE, MODE2);
#endif
#if defined(USE_SD1)
    /* SP1 Mode 0 (MMC/SD1) */
    CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_SP1MODE, MODE0);

#elif defined(USE_I2S1_PB) || defined(USE_I2S1_REC)
    /* SP1 Mode 1 (I2S1 and GP[11:10]) */
    CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_SP1MODE, MODE1);
#else
    /* SP1 Mode 2 (GP[11:6]) */
    CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_SP1MODE, MODE2); /* need GPIO10 for AIC3204 reset */
#endif
    /* PP Mode 1 (SPI, GPIO[17:12], UART, and I2S2) -- note this allows UART */
    CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_PPMODE, MODE1);
    /* Reset C5535 -- ungates all peripherals */
    C5535_reset();

#ifdef C5535_EZDSP_DEMO
        /* clock gating unused peripherals */
		ClockGating();
#endif
	DMA_init();
    /* Initialize DSP PLL */
    status = pll_sample();
    if (status != CSL_SOK)
    {
        LOG_printf(&trace, "ERROR: Unable to initialize PLL");
    }

    /* Clear pending timer interrupts */
    CSL_SYSCTRL_REGS->TIAFR = 0x7;

#if defined(DMFX_PROC1)
    LOG_printf(&trace, "DSP-1 Audio FX init");
    /* Initialize GPIO module set outputs to 1*/
    /*
     * CSL_GPIO_PIN12 = AIC3204I2C_ENN (out, active low)
     * CSL_GPIO_PIN13 = test point 14 (out)
     * CSL_GPIO_PIN14 = test point 15 (out)
     * CSL_GPIO_PIN15 = test point 16 (out)
     * CSL_GPIO_PIN16 = INT1_1N (out, active low)
     * CSL_GPIO_PIN17 = CPU1_LED (out, active high)
     */
    gpioIoDir = (((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN12) |
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN13) |
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN14) |
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN15) |
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN16) |
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN17);

    status = gpioInit(gpioIoDir, 0x00000000, 0x00000000);
    if (status != GPIOCTRL_SOK)
    {
        LOG_printf(&trace, "ERROR: Unable to initialize GPIO DIR on DSP-1");
    }
    gpioIoDat = (((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN13) |
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN14) |
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN15) |
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN16) |
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN17);
    CSL_FINS((*GPIO_DOUT0_ADDR), GPIO_DOUT0, gpioIoDat);
    CSL_FINS((*GPIO_DOUT1_ADDR), GPIO_DOUT1, gpioIoDat>>16);
    if(status != CSL_SOK)
    {
        LOG_printf(&trace, "ERROR: Unable to initialize GPIO DOUT on DSP-1");
    }
#endif
#if defined(DMFX_PROC2)
    LOG_printf(&trace, "DSP-2 Control init");
    /* Initialize GPIO module set outputs to 1*/
    /*
     * Overdrive Gain
     * OD(1:0) = 00 => 15 (24dB)
     * 		   = 10 => 40 (32dB)
     * 		   = 01 => 97 (40dB)
     * 		   = 11 => 122(42dB)
     */
    /*
     * Overdrive Mode
     * MODE(0) => mid enhance
     * MODE(1) => Fuzz
     */
    /*
     * CSL_GPIO_PIN0 = CLEAN (out, active high)
     * CSL_GPIO_PIN1 = OD_DRV0 (out, active high)
     * CSL_GPIO_PIN2 = OD_DRV1 (out, active high)
     * CSL_GPIO_PIN3 = OD_MODE0 (out, active high)
     *
     * CSL_GPIO_PIN4 = OD_MODE1 (out, active high)
     * CSL_GPIO_PIN5 = MUTE (out, active high)
     * CSL_GPIO_PIN6 = LED1 (out, active high)
     * CSL_GPIO_PIN7 = LED2 (out, active high)
     *
     * CSL_GPIO_PIN8 = LED3 (out, active high)
     * CSL_GPIO_PIN9 = LED4 (out, active high)
     * CSL_GPIO_PIN10 = LED5 (out, active high)
     * CSL_GPIO_PIN11 = CPU_LED (out, active high)
     *
     * CSL_GPIO_PIN12 = LCD_A0 (out, low command, high data)
     * CSL_GPIO_PIN13 = C2-LEFT (in, active low)
     * CSL_GPIO_PIN14 = C2-UP (in, active low)
     * CSL_GPIO_PIN15 = C2-CENTER, INT1 (in, active low)
     *
     * CSL_GPIO_PIN16 = C2-DOWN (in, active low)
     * CSL_GPIO_PIN17 = C2-RIGHT (in, active low)
     *
     * CSL_GPIO_PIN28 = FOOTSW (in, active low?)
     */
    gpioIoDir = (((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN0) |
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN1) |
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN2) |
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN3) |
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN4) |
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN5) |
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN6) |
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN7) |
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN8) |
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN9) |
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN10)|
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN11)|
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN12);
    gpioIntEn = (((Uint32)1)<<CSL_GPIO_PIN13) |
				(((Uint32)1)<<CSL_GPIO_PIN14) |
				(((Uint32)1)<<CSL_GPIO_PIN15) |
				(((Uint32)1)<<CSL_GPIO_PIN16) |
				(((Uint32)1)<<CSL_GPIO_PIN17);
    gpioIntEdg= (((Uint32)CSL_GPIO_TRIG_FALLING_EDGE)<< CSL_GPIO_PIN13) |
    			(((Uint32)CSL_GPIO_TRIG_FALLING_EDGE)<< CSL_GPIO_PIN14) |
    			(((Uint32)CSL_GPIO_TRIG_FALLING_EDGE)<< CSL_GPIO_PIN15) |
    			(((Uint32)CSL_GPIO_TRIG_FALLING_EDGE)<< CSL_GPIO_PIN16) |
    			(((Uint32)CSL_GPIO_TRIG_FALLING_EDGE)<< CSL_GPIO_PIN17);

    status = gpioInit(gpioIoDir, gpioIntEn, gpioIntEdg);
    if (status != GPIOCTRL_SOK)
    {
        LOG_printf(&trace, "ERROR: Unable to initialize GPIO DIR on DSP-2");
    }
    /* Clean, Mute, LED1 on, CPU_LED on */
    gpioIoDat = (((Uint32)CSL_GPIO_DIR_OUTPUT)<< CSL_GPIO_PIN0) |
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<< CSL_GPIO_PIN5) |
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<< CSL_GPIO_PIN7) |
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<< CSL_GPIO_PIN8) |
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<< CSL_GPIO_PIN9) |
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<< CSL_GPIO_PIN10) |
    			(((Uint32)CSL_GPIO_DIR_OUTPUT)<< CSL_GPIO_PIN11);
    CSL_FINS((*GPIO_DOUT0_ADDR), GPIO_DOUT0, gpioIoDat);
    CSL_FINS((*GPIO_DOUT1_ADDR), GPIO_DOUT1, gpioIoDat>>16);
    if(status != CSL_SOK)
    {
        LOG_printf(&trace, "ERROR: Unable to initialize GPIO DOUT on DSP-2");
    }
#endif

    /* Enable the USB LDO */
    *(volatile ioport unsigned int *)(0x7004) = 0x0001;
	/* Clear any pending Interrupt */
    IRQ_clear(INT0_EVENT);
	IRQ_enable(INT0_EVENT);
#ifndef _FX_CMD_H_
    memset(FxCmd, 0x0, sizeof(FxCmd));
#endif
}

/**
 *  \brief  Audio Class intialization function
 *
 *  \param  None
 *
 *  \return None
 */

void CSL_acTest(void)
{
    I2sInitPrms i2sInitPrms;
    Int16 status;
#ifdef USB_APP
    CSL_UsbConfig usbConfig;
#endif
    PSP_Result result;
#ifdef SAMPLE_BY_SAMPLE_PB
    HWI_Attrs attrs;
#endif

    LOG_printf(&trace, "Init DSP-1 FX (CSL_acTest)");

    /* Initialize audio codec */
    result = AIC3254_init();
    if(result != 0)
    {
        LOG_printf(&trace, "ERROR: Unable to configure audio codec");
    }
    else
    {
#if !defined(SAMPLE_BY_SAMPLE_PB) || !defined(SAMPLE_BY_SAMPLE_REC)
        DMA_HwInit();
        DMA_DrvInit();
#endif

        /* Initialize I2S and associated DMA channels for Playback and Record */
        i2sInitPrms.enablePlayback = TRUE;
        i2sInitPrms.enableStereoPb = TRUE;
#ifdef SAMPLE_BY_SAMPLE_PB
        i2sInitPrms.sampleBySamplePb = TRUE;
#else /* Configuration untested since ASRC only works with I2S in sample-by-sample mode */
        i2sInitPrms.sampleBySamplePb = FALSE;
        i2sInitPrms.enableDmaPingPongPb = TRUE;  //FALSE;
        i2sInitPrms.pingI2sTxLeftBuf  = (Int16 *)ping_pong_i2sTxLeftBuf;  //ping_i2sTxLeftBuf;
        i2sInitPrms.pongI2sTxLeftBuf  = NULL; 							  //pong_i2sTxLeftBuf;
        i2sInitPrms.pingI2sTxRightBuf = (Int16 *)ping_pong_i2sTxRightBuf; //ping_i2sTxRightBuf;
        i2sInitPrms.pongI2sTxRightBuf = NULL;							  //pong_i2sTxRightBuf;
        i2sInitPrms.zeroBuf = ZeroBuf;
#endif
        i2sInitPrms.i2sPb = PSP_I2S_TX_INST_ID;
        i2sInitPrms.enableRecord = TRUE;
        i2sInitPrms.enableStereoRec = FALSE;
#ifdef SAMPLE_BY_SAMPLE_REC
        i2sInitPrms.sampleBySampleRec = TRUE;
#else
        i2sInitPrms.sampleBySampleRec = FALSE;
        i2sInitPrms.enableDmaPingPongRec = TRUE;
        i2sInitPrms.pingI2sRxLeftBuf  = (Int16 *)ping_pong_i2sRxLeftBuf;
        i2sInitPrms.pongI2sRxLeftBuf  = NULL;
        i2sInitPrms.pingI2sRxRightBuf = (Int16 *)ping_pong_i2sRxRightBuf;
        i2sInitPrms.pongI2sRxRightBuf = NULL;
        i2sInitPrms.i2sRec = PSP_I2S_RX_INST_ID;
        status = i2sInit(&i2sInitPrms);
        if (status != I2SSAMPLE_SOK)
        {
            LOG_printf(&trace, "ERROR: Unable to initialize I2S");
        }
	#endif

#ifdef USB_APP  /* DCM: USB removed */
        /* Initialising the Pointer to the Audio Class Handle to the Buffer Allocated */
        AC_AppHandle.pAcObj = &ACAppBuffer[0];

        usbConfig.devNum                = CSL_USB0;
        usbConfig.opMode                = CSL_USB_OPMODE_POLLED;
#ifdef APP_USB_SELF_POWERED
        usbConfig.selfPowered           = TRUE;
#else
        usbConfig.selfPowered           = FALSE;
#endif
        usbConfig.maxCurrent            = APP_USB_MAX_CURRENT;
        usbConfig.appSuspendCallBack    = (CSL_USB_APP_CALLBACK)CSL_suspendCallBack;
        usbConfig.appWakeupCallBack     = (CSL_USB_APP_CALLBACK)CSL_selfWakeupCallBack;
        usbConfig.startTransferCallback  = StartTransfer;
        usbConfig.completeTransferCallback = CompleteTransfer;

        USB_init(&usbConfig);

        USB_setFullSpeedMode(0x40); /* parameter is EP0 data size in bytes */

        USB_resetDev(CSL_USB0);

        /* Calling init routine */
        /* Giving all the table hanldes and the buffers to the Audio Class module */
        AC_AppHandle.strDescrApp = (char **)&string_descriptor[0];
        AC_AppHandle.lbaBufferPbApp = &lbaBufferPbApp[0];
        AC_AppHandle.lbaBufferRecApp = &lbaBufferRecApp[0];
        AC_AppHandle.lbaBufferHidReportApp = &lbaBufferHidReportApp[0];
        AC_AppHandle.acReqTableApp = USB_ReqTable;
        AC_AppHandle.pId = pId;
        AC_AppHandle.vId = vId;

#ifndef ENABLE_PLAYBACK_TWO_SAMPLE_RATES
        #ifdef SAMPLE_RATE_TX_48kHz
        LOG_printf(&trace, "PLAYBACK: 48KHZ ");
        #ifdef ENABLE_STEREO_PLAYBACK
        LOG_printf(&trace, "STEREO");
        AC_AppHandle.rxPktSize = EP_PB_MAXP; /* max packet size for 48K stereo */
        #else /* ENABLE_STEREO_PLAYBACK */
        LOG_printf(&trace, "MONO");
        AC_AppHandle.rxPktSize = 0x60; /* max packet size for 48K mono */
        #endif /* ENABLE_STEREO_PLAYBACK */
        #endif /* SAMPLE_RATE_TX_48kHz */

        #ifdef SAMPLE_RATE_TX_44_1kHz
        LOG_printf(&trace, "PLAYBACK: 44.1KHZ ");
        #ifdef ENABLE_STEREO_PLAYBACK
        LOG_printf(&trace, "STEREO");
        AC_AppHandle.rxPktSize = 0xB0; /* max packet size for 44.1 stereo */
        #else /* ENABLE_STEREO_PLAYBACK */
        LOG_printf(&trace, "MONO");
        AC_AppHandle.rxPktSize = 0x58; /* max packet size for 44.1 mono */
        #endif /* ENABLE_STEREO_PLAYBACK */
        #endif /* SAMPLE_RATE_TX_44_1kHz */

        #ifdef SAMPLE_RATE_TX_32kHz
        LOG_printf(&trace, "PLAYBACK: 32KHZ ");
        #ifdef ENABLE_STEREO_PLAYBACK
        LOG_printf(&trace, "STEREO");
        AC_AppHandle.rxPktSize = 0x80; /* max packet size for 32K stereo */
        #else /* ENABLE_STEREO_PLAYBACK */
        LOG_printf(&trace, "MONO");
        AC_AppHandle.rxPktSize = 0x40; /* max packet size for 32K mono */
        #endif /* ENABLE_STEREO_PLAYBACK */
        #endif /* SAMPLE_RATE_TX_32kHz */

        #ifdef SAMPLE_RATE_TX_16kHz
        LOG_printf(&trace, "PLAYBACK: 16KHZ ");
        #ifdef ENABLE_STEREO_PLAYBACK
        LOG_printf(&trace, "STEREO");
        AC_AppHandle.rxPktSize = RX_PKT_SIZE_16K_PLAYBACK_STEREO; /* max packet size for 16K stereo */
        rx_pkt_size_16K_playback = RX_PKT_SIZE_16K_PLAYBACK_STEREO; /* max packet size for 16K stereo */
        #else // ENABLE_STEREO_PLAYBACK
        LOG_printf(&trace, "MONO");
        AC_AppHandle.rxPktSize = RX_PKT_SIZE_16K_PLAYBACK_MONO; /* max packet size for 16K mono */
        rx_pkt_size_16K_playback = RX_PKT_SIZE_16K_PLAYBACK_MONO;  /* max packet size for 16K mono */
        #endif /* ENABLE_STEREO_PLAYBACK */
        #endif /* SAMPLE_RATE_TX_16kHz */

#else /* ENABLE_PLAYBACK_TWO_SAMPLE_RATES */
        LOG_printf(&trace, "PLAYBACK: 48KHZ ");
        #ifdef ENABLE_STEREO_PLAYBACK
        LOG_printf(&trace, "STEREO");
        AC_AppHandle.rxPktSize = EP_PB_MAXP; /* max packet size for 48K stereo */
        #else /* ENABLE_STEREO_PLAYBACK */
        LOG_printf(&trace, "MONO");
        AC_AppHandle.rxPktSize = 0x60; /* max packet size for 48K mono */
        #endif /* ENABLE_STEREO_PLAYBACK */

        LOG_printf(&trace, "PLAYBACK: 16KHZ ");
        #ifdef ENABLE_STEREO_PLAYBACK
        rx_pkt_size_16K_playback = RX_PKT_SIZE_16K_PLAYBACK_STEREO; /* max packet size for 16K stereo */
        LOG_printf(&trace, "STEREO");
        #else // ENABLE_STEREO_PLAYBACK
        rx_pkt_size_16K_playback = RX_PKT_SIZE_16K_PLAYBACK_MONO;  /* max packet size for 16K mono */
        LOG_printf(&trace, "MONO");
        #endif /* ENABLE_STEREO_PLAYBACK */
#endif /* ENABLE_PLAYBACK_TWO_SAMPLE_RATES */

        AC_AppHandle.txPktSize = EP_REC_MAXP; /* max packet size for 16K mono */
        AC_AppHandle.hidTxPktSize = EP_HID_MAXP; /* max packet size for HID output report */

        /* All Function Handlers need to be Initialised */
        AC_AppHandle.playAudioApp = appPlayAudio;
        AC_AppHandle.recordAudioApp = appRecordAudio;
        AC_AppHandle.initPlayAudioApp = appInitPlayAudio;
        AC_AppHandle.initRecordAudioApp = appInitRecordAudio;
        AC_AppHandle.stopPlayAudioApp = appStopPlayAudio;
        AC_AppHandle.stopRecordAudioApp = appStopRecordAudio;
        AC_AppHandle.mediaGetPresentStateApp = AppGetMediaStatus;
        AC_AppHandle.mediaInitApp = AppMediaInit;
        AC_AppHandle.mediaEjectApp = AppMediaEject;
        AC_AppHandle.mediaLockUnitApp = AppLockMedia;
        AC_AppHandle.getMediaSizeApp = AppGetMediaSize;
        AC_AppHandle.getHidReportApp = appGetHidReport;
        AC_AppHandle.ctrlHandler  = appCtrlFxn;
        AC_AppHandle.isoHandler   = appIsoFxn;
        AC_AppHandle.hidHandler = appHidFxn;

        AC_AppHandle.numLun = 2;

        /* Initialize End point descriptors */
        AC_initDescriptors(AC_AppHandle.pAcObj, (Uint16 *)deviceDescriptorB,
                            CSL_AC_DEVICE_DESCR, sizeof(deviceDescriptorB));

        AC_initDescriptors(AC_AppHandle.pAcObj, (Uint16 *)deviceQualifierDescr,
                            CSL_AC_DEVICE_QUAL_DESCR, 10);

        AC_initDescriptors(AC_AppHandle.pAcObj, (Uint16 *)configDescriptor,
                            CSL_AC_CONFIG_DESCR, sizeof(configDescriptor));

        AC_initDescriptors(AC_AppHandle.pAcObj, (Uint16 *)stringLanId,
                            CSL_AC_STRING_LANGID_DESC, 6);

        AC_initDescriptors(AC_AppHandle.pAcObj, (Uint16 *)acHidReportDescriptor,
                            CSL_AC_HID_REPORT_DESC, sizeof(acHidReportDescriptor));

        /* Initialize HID */
        AC_AppHandle.acHidIfNum = IF_NUM_HID; /* HID interface number */
        AC_AppHandle.acHidReportId = HID_REPORT_ID; /* HID report ID */
        AC_AppHandle.acHidReportLen = HID_REPORT_SIZE_BYTES; /* HID report length (bytes) */
        genHidReport(UI_PUSH_BUTTON_NONE, gHidReport); /* init. HID report for Get Report */

        /* Call Init API */
        AC_Open(&AC_AppHandle);

        /* Enable CPU USB interrupts */
        CSL_FINST(CSL_CPU_REGS->IER1, CPU_IER1_USB, ENABLE);
#endif  // DCM: end remove USB

        /* Initialize active sample rate */
#ifdef  SAMPLE_RATE_48kHz
        initSampleRate(RATE_48_KHZ, &active_sample_rate,
            &i2sTxBuffSz);
#else
        initSampleRate(RATE_16_KHZ, &active_sample_rate,
            &i2sTxBuffSz);
#endif
        /* Initialize ASRC */
        //Init_Sample_Rate_Converter(active_sample_rate);

        /* Reset codec output buffer */
        reset_codec_output_buffer();

        //DCM: Init Record Audio independently of USB application
        appInitAudio(0, NULL);
#if 0
        appRecordAudio(0, NULL, NULL, NULL);
		MenuDisplayTask();
		// Init SD card DMA
		status = mmcsdDmaInit(&cardAddr);
		//status = mmcsdDmaCallbackInit(&cardAddr);
#endif

	    if (status != CSL_SOK)
	    {
	        LOG_printf(&trace, "ERROR: Unable to initialize MMC/SD");
	    }

        //#ifdef ENABLE_RECORD
        #ifdef SAMPLE_RATE_RX_48kHz
        LOG_printf(&trace, "RECORD: 48KHZ ");
        #else
        LOG_printf(&trace, "RECORD: 16KHZ ");
        #endif // SAMPLE_RATE_RX_48kHz

        /* start the Rx DMAs */
        DMA_StartTransfer(hDmaRxLeft);
        #ifdef ENABLE_STEREO_RECORD
        LOG_printf(&trace, "STEREO  NOT SUPPORTED - RECORD WILL BE MONO");
        DMA_StartTransfer(hDmaRxRight);
        #else
        LOG_printf(&trace, "RECORD: MONO");
        #endif

        /* start the Tx DMAs */
        DMA_StartTransfer(hDmaTxLeft);
		#ifdef ENABLE_STEREO_PLAYBACK
        DMA_StartTransfer(hDmaTxRight);
        LOG_printf(&trace, "PLAYBACK: STEREO");
		#else
        LOG_printf(&trace, "PLAYBACK: MONO");
		#endif
        /* #endif  ENABLE_RECORD */

#ifdef STORE_PARAMETERS_TO_SDRAM
        initSdram(FALSE, 0x0000);
#endif /* STORE_PARAMETERS_TO_SDRAM */
#ifdef SAMPLE_BY_SAMPLE_PB
        /* SampleBySample, init interrupt */
        /* Use with compiler "interrupt" keyword */
        /* IRQ_plug(I2S_TX_EVENT, i2s_txIsr); */

        /* Use with dispatcher, no "interrupt" keyword */
        attrs.ier0mask = 0xFFFF;
        attrs.ier1mask = 0xFFFF;
        HWI_dispatchPlug(I2S_TX_EVENT, (Fxn)i2s_txIsr, &attrs);

        IRQ_enable(I2S_TX_EVENT);   /* SampleBySample, enable IRQ for I2S Tx */
#endif
#if defined(SAMPLE_BY_SAMPLE_REC) && !defined(COMBINE_I2S_TX_RX_ISR)
        /* SampleBySample, init interrupt */
        /* Use with compiler "interrupt" keyword */
        IRQ_plug(I2S_RX_EVENT, i2s_rxIsr);

        /* Use with dispatcher, no "interrupt" keyword */
        /* attrs.ier0mask = 0xFFFF; */
        /* attrs.ier1mask = 0xFFFF; */
        /* HWI_dispatchPlug(I2S_RX_EVENT, (Fxn)i2s_rxIsr, &attrs); */

        IRQ_enable(I2S_RX_EVENT);    /* SampleBySample, enable IRQ for I2S Rx */
#endif
#if defined(SAMPLE_BY_SAMPLE_PB) || defined(SAMPLE_BY_SAMLE_REC)
        DDC_I2S_transEnable((DDC_I2SHandle)i2sHandleTx, TRUE);    /* SampleBySample, enable I2S transmit and receive */
#endif
#ifndef SAMPLE_BY_SAMPLE_PB
        i2sTxStart(); /* - moved from appPlayAudio() */
#endif
    }
    LOG_printf(&trace, "End of Main task: CSL_acTsk");
}

/* Resets C5535 */
void C5535_reset(void)
{
    volatile int i;

    /* disable all interrupts (IER0 and IER1) */
    *(volatile ioport unsigned int *)(0x0000) = 0x0000;
    *(volatile ioport unsigned int *)(0x0045) = 0x0000;

    /* clear all interrupts (IFR0 and IFR1) */
    *(volatile ioport unsigned int *)(0x0001) = 0xFFFF;
    *(volatile ioport unsigned int *)(0x0046) = 0xFFFF;

    /* enable all peripherials */
    *(volatile ioport unsigned int *)(0x1c02) = 0;
    *(volatile ioport unsigned int *)(0x1c03) = 0;

    /* peripheral reset */
    *(volatile ioport unsigned int *)(0x1c04) = 0x0020;
    *(volatile ioport unsigned int *)(0x1c05) = 0x00BF;
    // some delay
    for (i=0; i<0xFFF; i++);

    /* clear all interrupts (IFR0 and IFR1) */
    *(volatile ioport unsigned int *)(0x0001) = 0xFFFF;
    *(volatile ioport unsigned int *)(0x0046) = 0xFFFF;
}
#ifdef C5535_EZDSP_DEMO
#if 0
// Power Display task code
void PowerDisplayTask(void)
{
	while (1)
	{
		// sleep for one second
		TSK_sleep(1000);
		// read and display the power usage
		if (DemoSwitchFlag==0)
		{
			CSL_i2cPowerTest();
		}
	}
}
#endif

#if 0
// Spectrum Display Task code
void SpectrumDisplayTask(void)
{
	// display the play audio message
	//print_playaudio();

	//TSK_sleep(1);
	while (1)
	{
		menu();
		TSK_sleep(100);
		/*// wait on bufferIn ready semaphore
		SEM_pend(&SEM_BufferInReady, SYS_FOREVER);
		// compute and display the bargraph
		if (DemoSwitchFlag)
		{
			calculate_FFT(bufferIn, 256);
			// clear the bufferInIdx to 0
			bufferInIdx = 0;
		}*/
		SetDistParams();
	}
}
#endif
#if 0
// PRD function. Runs every 10 second to switch the demo mode between
// power display mode and spectrum analyzer mode
void DemoSwitch(void)
{
	DemoSwitchFlag++;
	if (DemoSwitchFlag==1)
	{
		// if we were in power display mode, swtch to spectrum analyzer mode
		// clear the bufferInIdx
		bufferInIdx = 0;
	}
	else if (DemoSwitchFlag==3)
	{
		// if we were in spectrum analyzer mode, switch to power display mode
		DemoSwitchFlag = 1;
		// stop data collection for spectrum analyzer
		bufferInIdx = 0;
	}
}
#endif
// Clock gating for unused peripherals
//DCM: InitMMC/SD DMA after clockgating!!
void ClockGating(void)
{
	Uint16 pcgcr_value, clkstop_value;

	// set PCGCR1
	pcgcr_value = 0;
	// clock gating SPI
	//pcgcr_value |= CSL_FMKT(SYS_PCGCR1_SPICG, DISABLED);
	// clock gating SD/MMC
	//pcgcr_value |= CSL_FMKT(SYS_PCGCR1_MMCSD0CG, DISABLED);
	//pcgcr_value |= CSL_FMKT(SYS_PCGCR1_MMCSD0CG, DISABLED);
	// clock stop request for UART
	clkstop_value = CSL_FMKT(SYS_CLKSTOP_URTCLKSTPREQ, REQ);
	// write to CLKSTOP
	CSL_FSET(CSL_SYSCTRL_REGS->CLKSTOP, 15, 0, clkstop_value);
	// wait for acknowledge
	while (CSL_FEXT(CSL_SYSCTRL_REGS->CLKSTOP, SYS_CLKSTOP_URTCLKSTPACK)==0);
	// clock gating UART
	pcgcr_value |= CSL_FMKT(SYS_PCGCR1_UARTCG, DISABLED);
	// clock stop request for EMIF
	//clkstop_value = CSL_FMKT(SYS_CLKSTOP_EMFCLKSTPREQ, REQ);
	// write to CLKSTOP
	//CSL_FSET(CSL_SYSCTRL_REGS->CLKSTOP, 15, 0, clkstop_value);
	// wait for acknowledge
	//while (CSL_FEXT(CSL_SYSCTRL_REGS->CLKSTOP, SYS_CLKSTOP_EMFCLKSTPACK)==0);
	// clock gating EMIF
	//pcgcr_value |= CSL_FMKT(SYS_PCGCR1_EMIFCG, DISABLED);
	// clock gating unused I2S (I2S 0, 1, 3)
	pcgcr_value |= CSL_FMKT(SYS_PCGCR1_I2S0CG, DISABLED);
	pcgcr_value |= CSL_FMKT(SYS_PCGCR1_I2S1CG, DISABLED);
	pcgcr_value |= CSL_FMKT(SYS_PCGCR1_I2S3CG, DISABLED);
	// clock gating DMA0
	//pcgcr_value |= CSL_FMKT(SYS_PCGCR1_DMA0CG, DISABLED);
	// clock gating Timer 1
	//pcgcr_value |= CSL_FMKT(SYS_PCGCR1_TMR1CG, DISABLED);
	// clock gating Timer 2
	pcgcr_value |= CSL_FMKT(SYS_PCGCR1_TMR2CG, DISABLED);
	// write to PCGCR1
	CSL_FSET(CSL_SYSCTRL_REGS->PCGCR1, 15, 0, pcgcr_value);

	// set PCGCR2
	pcgcr_value = 0;
	// clock gating LCD
	pcgcr_value |= CSL_FMKT(SYS_PCGCR2_LCDCG, DISABLED);
	// clock gating DMA2
	//pcgcr_value |= CSL_FMKT(SYS_PCGCR2_DMA2CG, DISABLED);
	// clock gating DMA3
	//pcgcr_value |= CSL_FMKT(SYS_PCGCR2_DMA3CG, DISABLED);
	// write to PCGCR2
	CSL_FSET(CSL_SYSCTRL_REGS->PCGCR2, 15, 0, pcgcr_value);
	return;
}
#endif

/* Intializes active sample rate and I2S Tx buffer size */
Int16 initSampleRate(
    Uint16 usbSampleRate,
    Uint16 *pActiveSampleRate,
    Uint16 *pI2sTxBuffSz
)
{
    Int16 status = APP_USBAC_SOK;

    switch (usbSampleRate)
    {
    case RATE_16_KHZ:
        *pActiveSampleRate = ACTIVE_SAMPLE_RATE_16KHZ;
        *pI2sTxBuffSz = 2*TXBUFF_SZ_I2SSAMPS_16KHZ; /* 2x for stereo */
        break;
    case RATE_48_KHZ:
        *pActiveSampleRate = ACTIVE_SAMPLE_RATE_48KHZ;
        *pI2sTxBuffSz = 2*TXBUFF_SZ_I2SSAMPS_48KHZ; /* 2x for stereo */
        break;
    default:
        status = APP_USBAC_INV_USBSR;
        break;
    }

    return status;
}
