/*****************************************************************************/
/*                                                                           */
/* FILENAME                                                                  */
/*   main.c                                                                  */
/*                                                                           */
/* DESCRIPTION                                                               */
/*   DMFX-1.@ main file for DSP-2				                             */
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
#include "gpio_control.h"
#include "csl_gpio.h"
#include "DMFX2.h"
#include <std.h>
#include <log.h>
#include <string.h>
#include "csl_types.h"
#include "csl_error.h"
#include "csl_intc.h"
#include "csl_audioClass.h"
#include "csl_dma.h"
#include "soc.h"
#include "pll_control.h"
#include "app_globals.h"
#include "user_interface.h"
#include "csl_ioport.h"
#include "dsplib.h"
#include "soc.h"
#include "cslr.h"
#include "cslr_sysctrl.h"
#include "DMFX2_CSL_BIOS_cfg.h"
#include "lcdST7565.h"
#include "csl_sar_intc.h"
#include "csl_gpio_intc.h"
#include "csl_i2c_intc.h"
#include "DMFX_debug.h"

// main task DMFX2
void DMFX2(void);
// SPI Flash test
//void csl_spi_test(void);
// I2C SEEPROM test
void csl_i2c_test(void);
/* Resets C5535 */
void C5535_reset(void);
/* Init DMFX Processor 1 or 2 */
void DMFX_init(void);
// LCD test
//CSL_Status lcd_test(int argv);
// I2C Master Tx test: write to slave DSP
void I2c_Intc_MasterTx(void);
// Navigator Button GPIO interrupt test
//int gpio_int_test(void);
// SAR read voltage from potentiometers
//int  sar_pot_init(void);
extern Uint16 FxCmd[FXCMD_SIZE];
/*
 *  ======== main ========
 */
void main(void)
{
    //TRC_enable(TRC_GBLTARG);
    //LOG_enable(&trace);
	LOG_printf(&trace, "main: DMFX_init DSP-2");
    DMFX_init();
	//I2c_Intc_Init();
#ifdef BIST
    /* Test Hardware */
    csl_spi_test();
    csl_i2c_test();
    result = ;
    if(gpio_int_test())
		LOG_printf(&trace, "GPIO Int Test Failed!!");
	else
        LOG_printf(&trace, "GPIO Int Test Passed!!");
#if 0
    lcd_test(9);
#endif
#endif
	CLR_DRV1_8dB;
	CLR_DRV0_16dB;
	CLR_MD0_MIDS;
	CLR_MD1_CLIP;
	SET_CLEAN;
    UNMUTE;
    /* fall into DSP/BIOS idle loop */
    return;
}
// Main task DMFX2
void DMFX2(void)
{
    LOG_printf(&trace, "DMFX2: Main task");
    while(1)
    {
    	TSK_sleep(1000);
    	if (FxCmd[DISTORTION])
    	{
    		SET_OVD_DST;
    		if (FxCmd[DIST_TYPE])
    		{
    			switch(FxCmd[DIST_TYPE_LIST])
    			{
    			case VINTAGE: //Vintage 0dB
#if DEBUG>=10
    			    LOG_printf(&trace, "Overdrive: Vintage");
#endif
    				CLR_DRV1_8dB;
    				CLR_DRV0_16dB;
    				CLR_MD0_MIDS;
    				SET_MD1_CLIP;
    				break;
    			case WARM: //Warm 0dB with mids
#if DEBUG>=10
    			    LOG_printf(&trace, "Overdrive: Warm");
#endif
    				CLR_DRV1_8dB;
    				CLR_DRV0_16dB;
    				SET_MD0_MIDS;
    				SET_MD1_CLIP;
    				break;
    			case CRUNCH: //Crunch 8dB
#if DEBUG>=10
    			    LOG_printf(&trace, "Distortion: Crunch");
#endif
    				SET_DRV1_8dB;
    				CLR_DRV0_16dB;
    				CLR_MD0_MIDS;
    				SET_MD1_CLIP;
    				break;
    			case BLUES: //Blues 8dB with mids
#if DEBUG>=10
    			    LOG_printf(&trace, "Distortion: Blues");
#endif
    				SET_DRV1_8dB;
    				CLR_DRV0_16dB;
    				SET_MD0_MIDS;
    				SET_MD1_CLIP;
    				break;
    			case HARD: //Hard 16dB with Mids
#if DEBUG>=10
    			    LOG_printf(&trace, "Distortion: Hard");
#endif
    				CLR_DRV1_8dB;
    				SET_DRV0_16dB;
    				SET_MD0_MIDS;
    				CLR_MD1_CLIP;
    				break;
    			case METAL: //Metal 16dB
#if DEBUG>=10
    			    LOG_printf(&trace, "Distortion: Metal");
#endif
    				SET_DRV1_8dB;  //Try CLR_DRV1_8dB
    				SET_DRV0_16dB;
    				CLR_MD0_MIDS;
    				CLR_MD1_CLIP;
    				break;
    			case FUZZ:  //Fuzz
#if DEBUG>=10
    			    LOG_printf(&trace, "Distortion: Fuzz");
#endif
    				SET_DRV1_8dB;
    				SET_DRV0_16dB;
    				CLR_MD0_MIDS;
    				SET_MD1_CLIP;
    				break;
    			default:  // Default is Fuzz
#if DEBUG>=10
    			    LOG_printf(&trace, "Distortion: General");
#endif
    			    if (FxCmd[DRV1_8DB])
    			    	SET_DRV1_8dB;
    			    else
    			    	CLR_DRV1_8dB;
    			    if (FxCmd[DRV0_16DB])
    			    	SET_DRV0_16dB;
    			    else
    			    	CLR_DRV0_16dB;
    			    if (FxCmd[MD0_MIDS])
    			    	SET_MD0_MIDS;
    			    else
    			    	CLR_MD0_MIDS;
    			    if (FxCmd[MD1_CLIP])
    			    	SET_MD1_CLIP;
    			    else
    			    	CLR_MD1_CLIP;
    				break;
    			}
    		}
    		else
    		{
			    LOG_printf(&trace, "Distortion: No Type selected");
				CLR_DRV1_8dB;
				CLR_DRV0_16dB;
				CLR_MD0_MIDS;
				CLR_MD1_CLIP;
    		}
		}
    	else
    	{
		    LOG_printf(&trace, "Distortion: Clean");
        	SET_CLEAN;
			CLR_DRV1_8dB;
			CLR_DRV0_16dB;
			CLR_MD0_MIDS;
			CLR_MD1_CLIP;
    	}
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
    Uint32 gpioIoDir, gpioIoDat, gpioIntEn, gpioIntEdg;

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
     * CSL_GPIO_PIN6 = LED1 (out, active low)
     * CSL_GPIO_PIN7 = LED2 (out, active low)
     *
     * CSL_GPIO_PIN8 = LED3 (out, active low)
     * CSL_GPIO_PIN9 = LED4 (out, active low)
     * CSL_GPIO_PIN10 = LED5 (out, active low)
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
    // GPIO Interrupt Enable C2_LEFT, UP, CENTER, DOWN, RIGHT
    gpioIntEn = (((Uint32)1)<<CSL_GPIO_PIN13) |
				(((Uint32)1)<<CSL_GPIO_PIN14) |
				(((Uint32)1)<<CSL_GPIO_PIN15) |
				(((Uint32)1)<<CSL_GPIO_PIN16) |
				(((Uint32)1)<<CSL_GPIO_PIN17);
    // GPIO interrupt trigger falling edge
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
    // Clean, Mute, LED1 on, CPU_LED on
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
	/* Disable CPU Interrupts */
	IRQ_globalDisable();
	/* Clear any pending interrupts */
	IRQ_clearAll();
	/* Disable all the interrupts */
	IRQ_disableAll();
	IRQ_globalEnable();
	/* Clear and enable HW INT_0 from DSP-1 */
    IRQ_clear(INT0_EVENT);
	IRQ_enable(INT0_EVENT);
	// Initialize Navigator buttons gpio_int_init() not used
	/* Clear and enable GPIO Interrupt */
    IRQ_clear(GPIO_EVENT);
    IRQ_enable(GPIO_EVENT);
    // Initialize SAR channel and interrupt
    sar_pot_init();
#ifndef _FX_CMD_H_
    memset(FxCmd, 0x0, sizeof(FxCmd));
#endif
}

/* Resets C5535 */
void C5535_reset(void)
{
    volatile int i;

    // disable all interrupts (IER0 and IER1)
    *(volatile ioport unsigned int *)(0x0000) = 0x0000;
    *(volatile ioport unsigned int *)(0x0045) = 0x0000;

    // clear all interrupts (IFR0 and IFR1)
    *(volatile ioport unsigned int *)(0x0001) = 0xFFFF;
    *(volatile ioport unsigned int *)(0x0046) = 0xFFFF;

    // enable all peripherials PCGCR1 PCGCR2
    *(volatile ioport unsigned int *)(0x1c02) = 0;
    *(volatile ioport unsigned int *)(0x1c03) = 0;		//0x0004

    // peripheral reset PSRCR PRCR
    *(volatile ioport unsigned int *)(0x1c04) = 0x0004;	//0x0020
    *(volatile ioport unsigned int *)(0x1c05) = 0x00BF;
    // some delay
    for (i=0; i<0xFFF; i++);

    // clear all interrupts (IFR0 and IFR1)
    *(volatile ioport unsigned int *)(0x0001) = 0xFFFF;
    *(volatile ioport unsigned int *)(0x0046) = 0xFFFF;
}
// Clock gating for unused peripherals
void ClockGating(void)
{
	Uint16 pcgcr_value, clkstop_value;

	// set PCGCR1
	pcgcr_value = 0;
	// clock gating SPI
	pcgcr_value |= CSL_FMKT(SYS_PCGCR1_SPICG, DISABLED);
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
	//DMA_init();
	return;
}
