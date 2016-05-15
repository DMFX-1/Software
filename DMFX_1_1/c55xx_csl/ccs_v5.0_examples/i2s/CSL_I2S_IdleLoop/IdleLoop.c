/* ============================================================================
 * Copyright (c) 2008-2012 Texas Instruments Incorporated.
 * Except for those rights granted to you in your license from TI, all rights
 * reserved.
 *
 * Software License Agreement
 * Texas Instruments (TI) is supplying this software for use solely and
 * exclusively on TI devices. The software is owned by TI and/or its suppliers,
 * and is protected under applicable patent and copyright laws.  You may not
 * combine this software with any open-source software if such combination would
 * cause this software to become subject to any of the license terms applicable
 * to such open source software.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
 * NO WARRANTIES APPLY TO THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY.
 * EXAMPLES OF EXCLUDED WARRANTIES ARE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE AND WARRANTIES OF NON-INFRINGEMENT,
 * BUT ALL OTHER WARRANTY EXCLUSIONS ALSO APPLY. FURTHERMORE, TI SHALL NOT,
 * UNDER ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, CONSEQUENTIAL
 * OR PUNITIVE DAMAGES, FOR ANY REASON WHATSOEVER.
 * ============================================================================
 */

/** @file IdleLoop.c
 *
 *  @brief Test code to verify the CSL I2S functionality with the Audio Code
 * in DMA Interrupt mode
 *
 *
 * \page    page6  I2S EXAMPLE DOCUMENTATION
 *
 * \section I2S4   I2S EXAMPLE4 - IDLE LOOP
 *
 * \subsection I2S4x    TEST DESCRIPTION:
 *		This test verifies the operation of CSL I2S module with the Hardware
 * Audio Codec in DMA Interrupt mode. I2S module on C5535/C5515/C5517 DSP is
 * used to exchange the audio data between the audio codec and DSP.
 *
 * 		Testing of the I2S with the audio codec is done by integrating the I2S,
 * I2C and DMA CSL Module, to send a sine wave to the Data input registers of
 * the Audio Codec, which inturn will pump this sine wave to the Data Output
 * register, and finally to the Stereo Output.
 *
 *		I2C CSL Module is used to configure the Audio Codec with proper
 * configurations. CSL DMA module should be configured for the I2S Tx and Rx
 * operations, I2S is configured to Slave mode and the Codec is configured in
 * Master Mode. DMA module is used to transfer the data between CPU memory and
 * I2S data registers, the I2S Module will inturn transfer the Data to the
 * Codec Data Input registers.
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5535, C5515
 * AND C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 *
 * \subsection I2S4y    TEST PROCEDURE:
 *  @li Open the CCS and connect the target (C5535/C5515 EVM or C5517 EVM)
 *  @li Open the project "CSL_I2S_IdleLoop_Example_Out.pjt" and build it
 *  @li Load the program on to the target
 *  @li Set the PLL frequency to 12.288MHz
 *  @li Run the program and observe the test result
 *  @li Repeat the test at the following PLL frequencies
 *      C5515: 60MHz and 100MHz
 *      C5517: 60MHz, 100MHz, 150MHz and 200MHz
 *      C5535 eZdsp: 60MHz and 100MHz
 *  @li Repeat the test in Release mode
 *
 * \subsection I2S4z    TEST RESULT:
 *  @li All the CSL APIs should return success
 *
 */

/* ============================================================================
 * Revision History
 * ================
 * 05-Sep-2008 Created
 * 10-Jul-2012 Added C5517 Compatibility
 * ============================================================================
 */

#include "stdio.h"
#include "stdlib.h"

/* CSLR include files */
#include "soc.h"
#include "cslr.h"
#include "cslr_sysctrl.h"
/* CSL include files */
#include "csl_gpio.h"
#include "csl_i2s.h"
#include "csl_dma.h"
#include "csl_intc.h"
#include "csl_sysctrl.h"

#include "pll_control.h"
#include "codec_aic3254.h"
#include "IdleLoop.h"

#if (defined(CHIP_C5517))
    #define INSTANCE0_I2S
#else
    #define INSTANCE2_I2S
#endif

#ifdef INSTANCE0_I2S
#define I2S_INSTANCE    ( I2S_INSTANCE0 )
#define DMA_CHAN_L      ( CSL_DMA_CHAN0 )
#define DMA_CHAN_R      ( CSL_DMA_CHAN1 )
#endif

#ifdef INSTANCE2_I2S
#define I2S_INSTANCE    ( I2S_INSTANCE2 )
#define DMA_CHAN_L      ( CSL_DMA_CHAN4 )
#define DMA_CHAN_R      ( CSL_DMA_CHAN5 )
#endif

extern void VECSTART(void); // defined in vector table
CSL_IRQ_Dispatch     dispatchTable;

CSL_GpioObj gGpioObj;
GPIO_Handle hGpio;

CSL_I2sHandle hI2s;

CSL_DMA_Handle dmaLeftRxHandle;
CSL_DMA_Handle dmaRightRxHandle;
CSL_DMA_ChannelObj dmaObj0, dmaObj1;
volatile Uint16 dmaFrameCount = 0;

/* Ping/pong buffers */
Uint16 i2sDmaReadBufLeft[I2S_DMA_BUF_LEN];  // left ping/pong buffer
Uint16 i2sDmaReadBufRight[I2S_DMA_BUF_LEN]; // right ping/pong buffer
Uint16 pingPongFlag = 0;

/* Input buffer left */
#pragma DATA_SECTION(inputDataBufLeft, ".inputDataBufLeft");
Int16 inputDataBufLeft[FRAME_LEN];
#pragma DATA_SECTION(inputDataBufRight, ".inputDataBufRight");
/* Input buffer right */
Int16 inputDataBufRight[FRAME_LEN];

/* Output circular buffer */
Int16 outCircBuf[OUT_CIRCBUF_LEN];

const unsigned short sine_750Hz_48kHzFs[64] =
{
0x0000,0x8c0c,0xf918,0x2825,0xfb30,0x563c,0x1c47,0x3351,
0x825a,0xf162,0x6d6a,0xe270,0x4176,0x7c7a,0x897d,0x617f,
0xff7f,0x617f,0x897d,0x7c7a,0x4176,0xe270,0x6d6a,0xf162,
0x825a,0x3351,0x1c47,0x563c,0xfb30,0x2825,0xf918,0x8c0c,
0x0000,0x74f3,0x07e7,0xd8da,0x05cf,0xaac3,0xe4b8,0xcdae,
0x7ea5,0x0f9d,0x9395,0x1e8f,0xbf89,0x8485,0x7782,0x9f80,
0x0180,0x9f80,0x7782,0x8485,0xbf89,0x1e8f,0x9395,0x0f9d,
0x7ea5,0xcdae,0xe4b8,0xaac3,0x05cf,0xd8da,0x07e7,0x74f3
};

// Initializes device
CSL_Status DeviceInit(void);

// Clock gates all peripherals
void ClockGatingAll(void);

// DSP LDO Switch
void DspLdoSwitch(Uint16 mode);

// USB LDO Switch
void UsbLdoSwitch(Uint16 mode);

// Initializes DMA
CSL_Status DmaInit(void);

// GPIO initialization
CSL_Status GpioInit(Uint32 ioDir, Uint32 intEn, Uint32 intEdg);

// I2S and DMA initialization
CSL_Status I2sDmaInit(void);

// User defined algorithm
void UserAlgorithm(void);

// Put CPU in idle
void UserIdle(void);

// DMA ISR
interrupt void DmaIsr(void);

void main(void)
{
    CSL_Status status;
    Uint32 gpioIoDir;

	CSL_SYSCTRL_REGS->PCGCR1 = 0x0000;
	CSL_SYSCTRL_REGS->PCGCR2 = 0x0000;

    *(volatile ioport Uint16*)(CSL_SYSCTRL_REGS) &= ~0x7000;   //
//    *(volatile ioport Uint16*)(CSL_SYSCTRL_REGS) |=  0x1000;   // Configure Parallel Port for I2S2
    /* Configure Serial Port */
    *(volatile ioport Uint16*)(CSL_SYSCTRL_REGS) &= ~0x0F00;   //
    *(volatile ioport Uint16*)(CSL_SYSCTRL_REGS) |=  0x0B00;   // Serial Port mode 1 (I2S1 and GP[11:10]).

    printf("I2S Idle Loop Example\n");
    printf("This Example will continuously Pump a Sine Wave Signal to the ");
    printf("Audio Output using the Codec\n\n");

    printf("Please connect the Headphone to the Audio Output (Headphone) ");
    printf("Jack\n\n");

    // Initialize DSP
    status = DeviceInit();
    if (status != CSL_SOK)
    {
        printf("ERROR: Unable to initialize DSP\n");
        exit(EXIT_FAILURE);
    }

#ifdef CHIP_C5517
    status = ioExpander_Setup();
	if (status != CSL_SOK)
    {
        printf("ERROR: ioExpander_Setup Failed\n");
        exit(EXIT_FAILURE);
    }

    status = ioExpander_Write(1, 0, 1);  /* Enable SEL_MMC0_I2S */
	if (status != CSL_SOK)
    {
        printf("ERROR: ioExpander_Write Failed\n");
        exit(EXIT_FAILURE);
    }

    status = ioExpander_Write(0, 5, 1);  /* Enable SPI_I2S2_S0 Mode */
	if (status != CSL_SOK)
    {
        printf("ERROR: ioExpander_Write Failed\n");
        exit(EXIT_FAILURE);
    }

    status = ioExpander_Write(0, 6, 0);  /* Disable SPI_I2S2_S1 Mode */
	if (status != CSL_SOK)
    {
        printf("ERROR: ioExpander_Write Failed\n");
        exit(EXIT_FAILURE);
    }
#endif

    // Initialize DMA
    status = DmaInit();
    if (status != CSL_SOK)
    {
        printf("ERROR: Unable to initialize DMA\n");
        exit(EXIT_FAILURE);
    }

    /* GPIO10 for AIC3204 reset */
    gpioIoDir = (((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN10);
    status = GpioInit(gpioIoDir, 0x00000000, 0x00000000);
    if (status != CSL_SOK)
    {
        printf("ERROR: Unable to initialize GPIO\n");
        exit(1);
    }

    // Initialize I2S with DMA
    status = I2sDmaInit();
    if (status != CSL_SOK)
    {
        printf("ERROR: Unable to initialize I2S and DMA\n");
        exit(1);
    }

    /* Intialize AIC3204 codec */
    status = AIC3254_init(48000, 48000, PLL_MHZ);
    if (status != CSL_SOK)
    {
        printf("ERROR: Unable to initialize AIC3204\n");
        exit(1);
    }


#ifdef INSTANCE0_I2S
	CSL_FINS(CSL_SYSCTRL_REGS->PCGCR2, SYS_PCGCR1_I2S0CG, CSL_SYS_PCGCR1_I2S0CG_DISABLED);
#endif

#ifdef INSTANCE2_I2S
	CSL_FINS(CSL_SYSCTRL_REGS->PCGCR2, SYS_PCGCR1_I2S2CG, CSL_SYS_PCGCR1_I2S2CG_DISABLED);
#endif

    /* Set CPUI bit in ICR */
    //*(volatile ioport Uint16 *)(0x0001) |= 0x000F;
    /* Set CPUI, DPORTI, XPORTI, and IPORTI in ICR */
    *(volatile ioport Uint16 *)(0x0001) |= 0x016F;

    /* Enable global interrupt */
    IRQ_globalEnable();

    printf("Pumping of a Sine Wave Signal to the Audio Output Started!!\n");

    while (1)
    {
        /* Perform your algorithm here */
        UserAlgorithm();

        /* Set CPU to Idle */
        UserIdle();
    }

    //printf("Exit the IdleLoop\n");
}

//*****************************************************************************
// DeviceInit
//*****************************************************************************
//  Description:
//      Sets up interrupt vector table,
//      Disables and clears interrupts,
//      Resets all peripherals,
//      Clock gates all peripherals,
//      Sets DSP LDO output to desired voltage,
//      Sets PLL output to desired MHz,
//      Disables USB LDO
//
//  Arguments:
//      none
//
//  Return:
//      CSL_Status: CSL_SOK - Device initialization successful
//                  other   - Device initialization unsuccessful
//
//*****************************************************************************
CSL_Status DeviceInit(void)
{
    Uint16 prcr;
    CSL_Status status;

    // Set up dispatch table
    status = IRQ_init(&dispatchTable, 0);
    if (status != CSL_SOK)
    {
        //printf("IRQ_init() fail, status=%d\n", status);
        return status;
    }

    // Disable global interrupts
    IRQ_globalDisable();

    // Disable all the interrupts
    IRQ_disableAll();

    // Clear any pending interrupts
    IRQ_clearAll();

    // Set the interrupt vector table address
    status = IRQ_setVecs((Uint32)&VECSTART);
    if (status != CSL_SOK)
    {
        //printf("IRQ_setVecs() fail, status=%d\n", status);
        return status;
    }

    // Reset all peripherals
    CSL_SYSCTRL_REGS->PSRCR = 0x0020;
    CSL_SYSCTRL_REGS->PRCR = 0x00BF;
    do
    {
        prcr = CSL_SYSCTRL_REGS->PRCR;
    } while (prcr != 0);

    // Clock gate all peripherals
//    ClockGatingAll();

    // Set DSP LDO to desired output voltage
//    DspLdoSwitch(DSP_LDO);

    // Set PLL output frequency
    // Note: desired frequency must be allowed for core voltage setting
    status = pll_sample(PLL_MHZ);
    if (status != CSL_SOK)
    {
        //printf("pll_sample() fail, status=%d\n", status);
        return status;
    }

    // Turn off USB LDO
    UsbLdoSwitch(0);

    return CSL_SOK;
}

//*****************************************************************************
// ClockGatingAll
//*****************************************************************************
//  Description:
//      Clock gates all peripherals,
//      Clock gates HWA FFT,
//      Disables CLKOUT (reduce I/O power),
//      Sets XF to 0 (reduce I/O power since LED not driven)
//
//  Arguments:
//      none
//
//  Return:
//      None
//
//*****************************************************************************
void ClockGatingAll(void)
{
    Uint16 pcgcr_value;
    //Uint16 clkstop_value;
    //Uint16 status;

    // enable the MPORT and disable HWA
    *(volatile ioport Uint16 *)0x0001 = 0x020E;
    asm("   idle");

    // set PCGCR1
    pcgcr_value = 0;
    // clock gating SPI
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_SPICG, DISABLED);
    // clock gating SD/MMC
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_MMCSD0CG, DISABLED);
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_MMCSD1CG, DISABLED);

    // clock stop request for UART
    //clkstop_value = CSL_FMKT(SYS_CLKSTOP_URTCLKSTPREQ, REQ);
    // write to CLKSTOP
    //CSL_FSET(CSL_SYSCTRL_REGS->CLKSTOP, 15, 0, clkstop_value);
    // wait for acknowledge
    //while (CSL_FEXT(CSL_SYSCTRL_REGS->CLKSTOP, SYS_CLKSTOP_URTCLKSTPACK)==0);
    // clock gating UART
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_UARTCG, DISABLED);

    // clock stop request for EMIF
    //clkstop_value = CSL_FMKT(SYS_CLKSTOP_EMFCLKSTPREQ, REQ);
    // write to CLKSTOP
    //CSL_FSET(CSL_SYSCTRL_REGS->CLKSTOP, 15, 0, clkstop_value);
    // wait for acknowledge
    //while (CSL_FEXT(CSL_SYSCTRL_REGS->CLKSTOP, SYS_CLKSTOP_EMFCLKSTPACK)==0);
    // clock gating EMIF
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_EMIFCG, DISABLED);

    // clock gating I2S I2S 0
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_I2S0CG, DISABLED);
#if (defined(CHIP_C5517))
    // clock gating McSPI
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_MCSPICG, DISABLED);
#else
    // clock gating I2S I2S 1
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_I2S1CG, DISABLED);
#endif
    // clock gating I2S I2S 2
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_I2S2CG, DISABLED);
    // clock gating I2S I2S 3
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_I2S3CG, DISABLED);

    // clock gating DMA0
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_DMA0CG, DISABLED);

    // clock gating Timer 0
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_TMR0CG, DISABLED);
    // clock gating Timer 1
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_TMR1CG, DISABLED);
    // clock gating Timer 2
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_TMR2CG, DISABLED);
#if (defined(CHIP_C5517))
    // clock gating McBSP
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_MCBSPCG, DISABLED);
 #endif
    // clock gating I2C
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_I2CCG, DISABLED);

    // write to PCGCR1
    CSL_FSET(CSL_SYSCTRL_REGS->PCGCR1, 15, 0, pcgcr_value);

    // set PCGCR2
    pcgcr_value = 0;
#if (defined(CHIP_C5517))
    // clock gating HPI
    pcgcr_value |= CSL_FMKT(SYS_PCGCR2_HPICG, DISABLED);
#else
    // clock gating LCD
    pcgcr_value |= CSL_FMKT(SYS_PCGCR2_LCDCG, DISABLED);
#endif
    // clock gating SAR
    pcgcr_value |= CSL_FMKT(SYS_PCGCR2_SARCG, DISABLED);

    // clock gating DMA1
    pcgcr_value |= CSL_FMKT(SYS_PCGCR2_DMA1CG, DISABLED);
    // clock gating DMA2
    pcgcr_value |= CSL_FMKT(SYS_PCGCR2_DMA2CG, DISABLED);
    // clock gating DMA3
    pcgcr_value |= CSL_FMKT(SYS_PCGCR2_DMA3CG, DISABLED);

    // clock gating analog registers
    pcgcr_value |= CSL_FMKT(SYS_PCGCR2_ANAREGCG, DISABLED);

    // clock gating USB
    pcgcr_value |= CSL_FMKT(SYS_PCGCR2_USBCG, DISABLED);

    // write to PCGCR2
    CSL_FSET(CSL_SYSCTRL_REGS->PCGCR2, 15, 0, pcgcr_value);

    // disable the CLKOUT. It is on reset
    // set bit 2 of ST3_55 to 1
    asm("    bit(ST3, #ST3_CLKOFF) = #1");

    // turn off the XF
    // set bit 13 of ST1_55 to 0
    asm("    bit(ST1, #ST1_XF) = #0");

#if 0
    // set all GPIO pins to be output and low to save power
    // set the GPIO pin 0 - 15 to output, set SYS_GPIO_DIR0 (0x1C06) bit 0 and 15 to 1
    *(volatile ioport Uint16 *)(0x1C06) = 0xFFFF;
    // set the GPIO pin 16 - 31 to output, set SYS_GPIO_DIR1 (0x1C07) bit 0 and 15 to 1
    *(volatile ioport Uint16 *)(0x1C07) = 0xFFFF;

    // set the GPIO 0 - 15 to 0, set SYS_GPIO_DATAOUT0 (0x1C0A) bit 0 and 15 to 0
    *(volatile ioport Uint16 *)(0x1C0A) = 0x0000;
    // set the GPIO 16 - 31 to 0, set SYS_GPIO_DATAOUT1 (0x1C0B) bit 0 and 15 to 0
    *(volatile ioport Uint16 *)(0x1C0B) = 0x0000;
#endif
}

//*****************************************************************************
// DspLdoSwitch
//*****************************************************************************
//  Description: Sets DSP LDO output to desired voltage
//
//  Arguments:
//      mode    105 - set DSP LDO to 1.05V
//              130 - set DSP LDO to 1.3V
//              other - no change
//
//  Return:
//      none
//
//*****************************************************************************
void DspLdoSwitch(Uint16 mode)
{
    Uint16 pcgcr2;

    pcgcr2 = CSL_SYSCTRL_REGS->PCGCR2; // save PCGCR2

    /* Enable the Analog Register only */
    CSL_SYSCTRL_REGS->PCGCR2 &= ~(0x0040);

#if (defined(CHIP_C5517))
    if (mode == 130)
    {
        /* Set DSP LDO to 1.3 V (clear bit 1) */
        CSL_LDO_REGS->LDOCTRL &= 0xFFFD;
    }

    if (mode == 105)
    {
        /* Set DSP LDO to 1.05 V (set bit 1) */
        CSL_LDO_REGS->LDOCTRL |= 0x0002;
    }
#else
    if (mode == 130)
    {
        /* Set DSP LDO to 1.3 V (clear bit 1) */
        CSL_SAR_REGS->USBLDOCNTL &= 0xFFFD;
    }

    if (mode == 105)
    {
        /* Set DSP LDO to 1.05 V (set bit 1) */
        CSL_SAR_REGS->USBLDOCNTL |= 0x0002;
    }
#endif

    CSL_SYSCTRL_REGS->PCGCR2 = pcgcr2; // restore PCGCR2
}

//*****************************************************************************
// UsbLdoSwitch
//*****************************************************************************
//  Description: Enables/disables USB LDO
//
//  Arguments:
//      mode:   0 - disable USB LDO
//              1 - enable USB LDO
//              other - no change
//
//  Return:
//      none
//
//*****************************************************************************
void UsbLdoSwitch(Uint16 mode)
{
    Uint16 pcgcr2;

    pcgcr2 = CSL_SYSCTRL_REGS->PCGCR2; // save PCGCR2

    /* Enable the Analog Register only */
    CSL_SYSCTRL_REGS->PCGCR2 &= ~(0x0040);

#if (defined(CHIP_C5517))
    if (mode == 0)
    {
        /* Disable USB LDO (clear bit 0) */
        CSL_LDO_REGS->LDOCTRL &= 0xFFFE;
    }

    if (mode == 1)
    {
        /* Enable USB LDO (set bit 0) */
        CSL_LDO_REGS->LDOCTRL |= 0x0001;
    }
#else
    if (mode == 0)
    {
        /* Disable USB LDO (clear bit 0) */
        CSL_SAR_REGS->USBLDOCNTL &= 0xFFFE;
    }

    if (mode == 1)
    {
        /* Enable USB LDO (set bit 0) */
        CSL_SAR_REGS->USBLDOCNTL |= 0x0001;
    }
#endif

    CSL_SYSCTRL_REGS->PCGCR2 = pcgcr2; // restore PCGCR2
}

//*****************************************************************************
// DmaInit
//*****************************************************************************
//  Description: Initializes DMA, enables DMA interrupts
//
//  Arguments:
//      none
//
//  Return:
//      CSL_Status: CSL_SOK - DMA initialization successful
//                  other   - DMA initialization unsuccessful
//
//*****************************************************************************
CSL_Status DmaInit(void)
{
    Uint16 ifrValue;
    CSL_Status status;

    // Disable DMA interrupt
    IRQ_disable(DMA_EVENT);

    // Clear DMA interrupt
    IRQ_clear(DMA_EVENT);

    // Disable DMA interrupts
    CSL_SYSCTRL_REGS->DMAIER = 0x0000;

    // Clear pending DMA interrupts
    ifrValue = CSL_SYSCTRL_REGS->DMAIFR;
    CSL_SYSCTRL_REGS->DMAIFR |= ifrValue;

    // Plug in DMA ISR
    IRQ_plug(DMA_EVENT, &DmaIsr);

    // DMA initialization
    status = DMA_init();
    if (status != CSL_SOK)
    {
        //printf("DMA_init() fail, status=%d\n", status);
        return status;
    }

    // Enable DMA interrupt
    IRQ_enable(DMA_EVENT);

    return CSL_SOK;
}

//*****************************************************************************
// GpioInit
//*****************************************************************************
//  Description: Initializes GPIO module
//
//  Arguments:
//      hGpio       - handle to GPIO (global)
//      gGpioObj    - GPIO parameter structure (global)
//      ioDir       - GPIO direction
//      intEn       - GPIO interrupt enables
//      intEdg      - GPIO edge
//
//  Return:
//      CSL_Status: CSL_SOK - GPIO module initialization successful
//                  other   - GPIO module initialization unsuccessful
//
//*****************************************************************************
CSL_Status GpioInit(
    Uint32 ioDir,
    Uint32 intEn,
    Uint32 intEdg
)
{
    CSL_Status status;
    CSL_GpioConfig config;

    /* Open GPIO module */
    hGpio = GPIO_open(&gGpioObj, &status);
    if ((hGpio == NULL) || (status != CSL_SOK))
    {
        return status;
    }

    /* Reset GPIO module */
    status = GPIO_reset(hGpio);
    if (status != CSL_SOK)
    {
        return status;
    }

    /* Configure GPIO module */
    config.GPIODIRL = ioDir & 0xFFFF;
    config.GPIODIRH = ioDir >> 16;
    config.GPIOINTENAL = intEn & 0xFFFF;
    config.GPIOINTENAH = intEn >> 16;
    config.GPIOINTTRIGL = intEdg & 0xFFFF;
    config.GPIOINTTRIGH = intEdg >> 16;
    status = GPIO_config(hGpio, &config);
    if (status != CSL_SOK)
    {
        return status;
    }

    return CSL_SOK;
}

//*****************************************************************************
// I2sDmaInit
//*****************************************************************************
//  Description:
//      Configures and starts I2S with DMA
//
//  Parameters:
//      hI2s                - I2S handle (global)
//      dmaLeftRxHandle     - handle for I2S left DMA (global)
//      dmaRightRxHandle    - handle for I2S right DMA (global)
//
//      CSL_Status: CSL_SOK - I2S & DMA initialization successful
//                  other   - I2S & DMA initialization unsuccessful
//
//*****************************************************************************
CSL_Status I2sDmaInit(void)
{
    I2S_Config hwConfig;
    CSL_DMA_Config dmaConfig;
    CSL_Status status;

    /* DMA engine initialization */
    /* Open the device with instance 2 (AIC3204 is connected to I2S2 on C5515 EVM) */
    hI2s = I2S_open(I2S_INSTANCE, DMA_POLLED, I2S_CHAN_STEREO);
    if(NULL == hI2s)
    {
        status = CSL_ESYS_FAIL;
        return (status);
    }
    else
    {
        printf ("I2S Module Instance opened successfully\n");
    }

    if (I2S_INSTANCE0 == I2S_INSTANCE)
    {
		status = SYS_setEBSR(CSL_EBSR_FIELD_SP0MODE,
							 CSL_EBSR_SP0MODE_1);
	}
	else
	{
		status = SYS_setEBSR(CSL_EBSR_FIELD_PPMODE,
							 CSL_EBSR_PPMODE_6);
	}
    if(CSL_SOK != status)
    {
        printf("SYS_setEBSR failed\n");
        return (status);
    }

    /* Set the value for the configure structure */
    hwConfig.dataFormat     = I2S_DATAFORMAT_LJUST;
    hwConfig.dataType       = I2S_STEREO_ENABLE;
    hwConfig.loopBackMode   = I2S_LOOPBACK_DISABLE;
    hwConfig.fsPol          = I2S_FSPOL_LOW;
    hwConfig.clkPol         = I2S_FALLING_EDGE;
    hwConfig.datadelay      = I2S_DATADELAY_ONEBIT;
    hwConfig.datapack       = I2S_DATAPACK_DISABLE;
    hwConfig.signext        = I2S_SIGNEXT_DISABLE;
    hwConfig.wordLen        = I2S_WORDLEN_16;
    hwConfig.i2sMode        = I2S_SLAVE;
    hwConfig.clkDiv         = I2S_CLKDIV2; // don't care for slave mode
    hwConfig.fsDiv          = I2S_FSDIV32; // don't care for slave mode
    hwConfig.FError         = I2S_FSERROR_DISABLE;
    hwConfig.OuError        = I2S_OUERROR_DISABLE;

    /* Configure hardware registers */
    status = I2S_setup(hI2s, &hwConfig);
    if(status != CSL_SOK)
    {
        return (status);
    }
    else
    {
        printf ("I2S Module Configured successfully\n");
    }

    /* Configure DMA channel 0 for I2S left channel read */
    dmaConfig.pingPongMode  = CSL_DMA_PING_PONG_DISABLE;
    dmaConfig.autoMode      = CSL_DMA_AUTORELOAD_ENABLE;
    dmaConfig.burstLen      = CSL_DMA_TXBURST_1WORD;
    dmaConfig.trigger       = CSL_DMA_EVENT_TRIGGER;

#ifdef INSTANCE0_I2S
    dmaConfig.dmaEvt        = CSL_DMA_EVT_I2S0_TX;
#endif

#ifdef INSTANCE2_I2S
    dmaConfig.dmaEvt        = CSL_DMA_EVT_I2S2_TX;
#endif

    dmaConfig.dmaInt        = CSL_DMA_INTERRUPT_ENABLE;
    dmaConfig.chanDir       = CSL_DMA_WRITE;
    dmaConfig.trfType       = CSL_DMA_TRANSFER_IO_MEMORY;
    dmaConfig.dataLen       = NUM_SAMP_PER_MS*NUM_MS_PER_FRAME*4*2; // two frames

#ifdef INSTANCE0_I2S
#if (defined(CHIP_C5517))
    dmaConfig.destAddr      = (Uint32)&CSL_I2S0_REGS->I2STXLTL;
#else
    dmaConfig.destAddr      = (Uint32)&CSL_I2S0_REGS->I2STXLT0;
#endif
#endif

#ifdef INSTANCE2_I2S
#if (defined(CHIP_C5517))
    dmaConfig.destAddr      = (Uint32)&CSL_I2S2_REGS->I2STXLTL;
#else
    dmaConfig.destAddr      = (Uint32)&CSL_I2S2_REGS->I2STXLT0;
#endif
#endif

    dmaConfig.srcAddr       = (Uint32)i2sDmaReadBufLeft;

    /* Open DMA ch0 for I2S left channel read */
    dmaLeftRxHandle = DMA_open(DMA_CHAN_L, &dmaObj0,&status);
    if (dmaLeftRxHandle == NULL)
    {
        printf("DMA_open CH4 Failed \n");
        dmaLeftRxHandle = NULL;
    }

    /* Configure DMA channel */
    status = DMA_config(dmaLeftRxHandle, &dmaConfig);
    if (status != CSL_SOK)
    {
        printf("DMA_config CH4 Failed \n");
        dmaLeftRxHandle = NULL;
    }

	I2S_transEnable(hI2s, TRUE);

    /* Configure DMA ch1 for I2S right channel read */
    dmaConfig.pingPongMode = CSL_DMA_PING_PONG_DISABLE;
    dmaConfig.autoMode     = CSL_DMA_AUTORELOAD_ENABLE;
    dmaConfig.burstLen     = CSL_DMA_TXBURST_1WORD;
    dmaConfig.trigger      = CSL_DMA_EVENT_TRIGGER;

#ifdef INSTANCE0_I2S
    dmaConfig.dmaEvt        = CSL_DMA_EVT_I2S0_TX;
#endif

#ifdef INSTANCE2_I2S
    dmaConfig.dmaEvt        = CSL_DMA_EVT_I2S2_TX;
#endif

    dmaConfig.dmaInt       = CSL_DMA_INTERRUPT_ENABLE;
    dmaConfig.chanDir      = CSL_DMA_WRITE;
    dmaConfig.trfType      = CSL_DMA_TRANSFER_IO_MEMORY;
    dmaConfig.dataLen      = NUM_SAMP_PER_MS*NUM_MS_PER_FRAME*4*2;  // two frames

#ifdef INSTANCE0_I2S
#if (defined(CHIP_C5517))
    dmaConfig.destAddr     = (Uint32)&CSL_I2S0_REGS->I2STXRTL;
#else
    dmaConfig.destAddr     = (Uint32)&CSL_I2S0_REGS->I2STXRT0;
#endif
#endif

#ifdef INSTANCE2_I2S
#if (defined(CHIP_C5517))
    dmaConfig.destAddr     = (Uint32)&CSL_I2S2_REGS->I2STXRTL;
#else
    dmaConfig.destAddr     = (Uint32)&CSL_I2S2_REGS->I2STXRT0;
#endif
#endif

    dmaConfig.srcAddr      = (Uint32)i2sDmaReadBufRight;

    /* Open DMA ch0 for I2S right channel read */
    dmaRightRxHandle = DMA_open(DMA_CHAN_R, &dmaObj1,&status);
    if (dmaRightRxHandle == NULL)
    {
        printf("DMA_open CH1 Failed \n");
        dmaRightRxHandle = NULL;
    }

    /* Configure DMA channel */
    status = DMA_config(dmaRightRxHandle, &dmaConfig);
    if (status != CSL_SOK)
    {
        printf("DMA_config CH1 Failed \n");
        dmaRightRxHandle = NULL;
    }

	I2S_transEnable(hI2s, TRUE);

    /* Start left Rx DMA */
    status = DMA_start(dmaLeftRxHandle);
    if (status != CSL_SOK)
    {
        printf("I2S Dma Left Failed!!\n");
        return (status);
    }

    /* Start right Rx DMA */
    status = DMA_start(dmaRightRxHandle);
    if (status != CSL_SOK)
    {
        printf("I2S Dma Right Failed!!\n");
        return (status);
    }

    /* Enable I2S */
    I2S_transEnable(hI2s, TRUE);

    return CSL_SOK;
}

volatile Uint32 LoopCount = 0;
volatile Uint16 numFrame = 0;
/* User defined algorithm */
void UserAlgorithm(void)
{
    Uint16 offset, i;

    if (dmaFrameCount >= 2)
    {
        /* Determine which frame to use ping or pong */
        offset = pingPongFlag * FRAME_LEN;

        /* Copy current frame from ping or pong buffer into input buffer */
        for (i=0; i<FRAME_LEN; i++)
        {
            /* Note: depending on algorithm requirements, can perform
            32 to 16 bit conversion as part of algorithm computation */

            /* Left channel 32 to 16 bit conversion */
//            inputDataBufLeft[i] = (Int16)(i2sDmaReadBufLeft[offset+i]>>16);
            /* Right channel 32 to 16 bit conversion */
            //inputDataBufRight[i] = (Int16)(i2sDmaReadBufRight[offset+i]>>16);

             i2sDmaReadBufLeft[i]  = (sine_750Hz_48kHzFs[i%64]);
             i2sDmaReadBufRight[i] = (sine_750Hz_48kHzFs[i%64]);
        }

        /* --- Insert algorithm here --- */

        /* Place output data in output circular buffer */
        for (i=0; i<FRAME_LEN; i++)
        {
            outCircBuf[numFrame*FRAME_LEN + i] = i2sDmaReadBufRight[i];
        }

        numFrame++;
        if (numFrame >= NUM_FRAMES_OUT_CIRCBUF)
        {
            numFrame = 0;
        }

#if 1 // debug -- stop DMAs on frame boundary
        /* Stop DMAs to get consistent DMA transfers */
        if ((pingPongFlag == 0) && (LoopCount > 500)) // 500*20e-3 = 10 sec.
        {
            *((ioport volatile unsigned int *)0x0D05) &= ~0x8000; // disable DMA
            *((ioport volatile unsigned int *)0x0D25) &= ~0x8000; // disable DMA
            while(1);
        }
#endif

        /* Toggle ping/pong flag */
        pingPongFlag ^= 1;

        /* Clear DMA frame count */
        dmaFrameCount = 0;

        LoopCount++;
    }
}

// Put CPU in idle
void UserIdle(void)
{
    /* Set CPUI bit in ICR */
    //*(volatile ioport Uint16 *)(0x0001) |= 0x000F;
    /* Set CPUI, DPORTI, XPORTI, and IPORTI in ICR */
    //*(volatile ioport Uint16 *)(0x0001) |= 0x016F;

    /* Execute idle instruction */
    asm("    idle");
}

Uint32 dmaIntCount = 0;
// DMA ISR
interrupt void DmaIsr(void)
{
    Uint16 ifrValue;

    /* Clear the DMA interrupt */
    ifrValue = CSL_SYSCTRL_REGS->DMAIFR;
    CSL_SYSCTRL_REGS->DMAIFR |= ifrValue;

    /* Check for DMA1 CH0 transfer completion */
#ifdef INSTANCE0_I2S
    if (ifrValue & 1)
    {
        dmaFrameCount++;
    }
#endif

#ifdef INSTANCE2_I2S
    if (ifrValue & (1<<4))
    {
        dmaFrameCount++;
    }
#endif

    /* Check for DMA1 CH1 transfer completion */
#ifdef INSTANCE0_I2S
    if (ifrValue & (1<<1))
    {
        dmaFrameCount++;
    }
#endif

#ifdef INSTANCE2_I2S
    if (ifrValue & (1<<5))
    {
        dmaFrameCount++;
    }
#endif

    dmaIntCount++;
}




