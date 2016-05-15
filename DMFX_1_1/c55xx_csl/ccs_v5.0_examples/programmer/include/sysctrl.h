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

/*********************************************************************
*    sysctrl.h
**********************************************************************/
#ifndef SYSCTRL_H
#define SYSCTRL_H

/*********************************************************************
* Bit-field definitions for System Control registers...
**********************************************************************/

/* Peripheral (External) Bus Selection Bit Field Definitions */
#define PERIPH_BUS_S0_MMCSD0             (0<<8)   // MMC/SD0 module routed to Serial Port 0 pins
#define PERIPH_BUS_S0_I2S0               (1<<8)   // I2S0 routed to Serial Port 0 pins
#define PERIPH_BUS_S0_GPIO               (2<<8)   // Serial Port 0 pins configured as GPIO
#define PERIPH_BUS_S1_MMCSD1             (0<<10)  // MMC/SD1 module routed to Serial Port 1 pins
#define PERIPH_BUS_S1_I2S1               (1<<10)  // I2S1 routed to Serial Port 1 pins
#define PERIPH_BUS_S1_GPIO               (2<<10)  // Serial Port 1 pins configured as GPIO
#define PERIPH_BUS_PP_LCD                (0<<12)  // 21 Parallel port pins configured as 16-bit LCD
#define PERIPH_BUS_PP_GPIO_SPI_UART_I2S2 (1<<12)  // 6:GPIO + 7:SPI(CS-0,1,2,3) + 4:UART + 4:I2S2
#define PERIPH_BUS_PP_LCD_GPI0           (2<<12)  // 10:8-bit LCD + 8:GPIO
#define PERIPH_BUS_PP_LCD_SPI_I2S3       (3<<12)  // 10:8-bit LCD + 4:SPI(CS-0) + 4:I2S2
#define PERIPH_BUS_PP_LCD_UART_I2S2      (4<<12)  // 10:8-bit LCD + 4:UART + 4:I2S2
#define PERIPH_BUS_PP_LCD_UART_SPI       (5<<12)  // 10:8-bit LCD + 4:UART + 4:SPI(CS-0)
#define PERIPH_BUS_PP_GPIO_SPI_I2S2_3    (6<<12)  // 6:GPIO + 7:SPI(CS-0,1,2,3) + 4:I2S2 + 4:I2S3

/* Peripheral Clock Gating Bit Field Definitions */
#define PERIPH_CLK_IIS3              (1UL<<0)   // IIS3 Idle Control Bit
#define PERIPH_CLK_SPI               (1UL<<1)   // SPI Idle Control Bit
#define PERIPH_CLK_UART              (1UL<<2)   // UART Control Bit
#define PERIPH_CLK_DMA0              (1UL<<3)   // DMA0 Idle Control Bit
#define PERIPH_CLK_MMCSD0            (1UL<<4)   // MMCSD0 Idle Control Bit
#define PERIPH_CLK_COPROC            (1UL<<5)   // CoProcessor Idle Control Bit
#define PERIPH_CLK_I2C               (1UL<<6)   // I2C Idle Control Bit
#define PERIPH_CLK_MMCSD1            (1UL<<7)   // MMCSD1 Idle Control Bit
#define PERIPH_CLK_IIS0              (1UL<<8)   // I2S0 Idle Control Bit
#define PERIPH_CLK_IIS1              (1UL<<9)   // I2S1 Idle Control Bit
#define PERIPH_CLK_TIMER0            (1UL<<10)  // Timer 0 Idle Control Bit
#define PERIPH_CLK_EMIF              (1UL<<11)  // EMIF Idle Control Bit
#define PERIPH_CLK_TIMER1            (1UL<<12)  // Timer 1 Module Clock off
#define PERIPH_CLK_TIMER2            (1UL<<13)  // Timer 2 Module Clock off
#define PERIPH_CLK_IIS2              (1UL<<14)  // IIS2 Idle Control Bit
#define PERIPH_CLK_MASTER_CLK        (1UL<<15)  // Disables clock to ALL domains (can only wake via: RTC alarms, WAKEUP, INT0, INT1, & RESET)
#define PERIPH_CLK_LCD               (1UL<<16)   // LCD Idle Control Bit
#define PERIPH_CLK_SARADC            (1UL<<17)   // SAR A/D Idle Control Bit
#define PERIPH_CLK_USB               (1UL<<18)   // USB Idle Control Bit
#define PERIPH_CLK_DMA1              (1UL<<19)   // DMA1 Idle Control Bit
#define PERIPH_CLK_DMA2              (1UL<<20)   // DMA2 Idle Control Bit
#define PERIPH_CLK_DMA3              (1UL<<21)   // DMA3 Idle Control Bit
#define PERIPH_CLK_ANAREG            (1UL<<22)   // ANAREG Idle Control Bit

/* Peripheral Reset Bit Field Definitions */
#define PERIPH_RESET_I2C                      (1<<0)   // I2C Reset Bit
#define PERIPH_RESET_EMIF_TIMERS_RTC          (1<<1)   // EMIF_TIMERS_RTC Reset Bit
#define PERIPH_RESET_SAR                      (1<<2)   // SAR Reset Bit
#define PERIPH_RESET_USB                      (1<<3)   // USB Reset Bit
#define PERIPH_RESET_DMA                      (1<<4)   // DMA Reset Bit
#define PERIPH_RESET_MMCSD0_MMCSD1_IIS0_IIS1  (1<<5)   // MMCSD0_MMCSD1_IIS0_IIS1 Reset Bit
#define PERIPH_RESET_FFTCOP                   (1<<6)   // FFT CoProcessor Reset Bit
#define PERIPH_RESET_LCD_IIS2_IIS3_UART_SPI   (1<<7)   // LCD_IIS2_IIS3_UART_SPI Reset Bit


/*********************************************************************
* Function prototypes...
**********************************************************************/

/*******************************************************************
* PeripheralPinMapping
* - Serial0 = Serial-Port-0 pin mapping (PERIPH_BUS_S0_*)
* - Serial1 = Serial-Port-1 pin mapping (PERIPH_BUS_S1_*)
* - Parallel = Parallel-Port pin mapping (PERIPH_BUS_PP_*)
* This function sets the pin-mapping for each peripheral bus.
********************************************************************/
void PeripheralPinMapping(unsigned short Serial0, unsigned short Serial1, unsigned short Parallel);

/*******************************************************************
* PeripheralReset
* - PeripheralResets = Peripherals to reset (PERIPH_RESET_*)
* - ResetHoldCycles = # sys-clk cycles to hold reset signal low
* - DelayCycles = # of delay cycles to wait for peripheral-ready
* This function resets the specified peripheral(s).
********************************************************************/
void PeripheralReset(unsigned short PeripheralResets, unsigned short ResetHoldCycles, unsigned long DelayCycles);

/*******************************************************************
* PeripheralClkEnableOnly
* - PeripheralClks = PeripheralClks to enable (PERIPH_CLK_*)
* This function enables the specified peripheral-clks ONLY.
* (all other peripheral-clks are disabled).
********************************************************************/
void PeripheralClkEnableOnly(unsigned long PeripheralClks);

/*******************************************************************
* PeripheralClkEnable
* - PeripheralClks = PeripheralClks to enable (PERIPH_CLK_*)
* This function enables the specified peripheral-clks.
* (all other peripheral-clks are unchanged).
********************************************************************/
void PeripheralClkEnable(unsigned long PeripheralClks);

#endif // SYSCTRL_H

