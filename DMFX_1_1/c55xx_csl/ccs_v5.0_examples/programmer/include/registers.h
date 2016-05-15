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
*    registers.h
**********************************************************************/
#ifndef REGISTERS_H
#define REGISTERS_H

/* Definitions */
#define IOPORT_REGISTER(reg)     (*(ioport volatile unsigned short *)(reg))
#define IOPORT_REGISTER_PTR(reg)	((ioport volatile unsigned short *)(reg))

/* Register Definitions */

/*********************************************************************
* System Control Registers (0x1C00 - 0x1CFF)
**********************************************************************/
#define IOPORT_REG_PERIPHSEL0          (IOPORT_REGISTER(0x1C00))
#define IOPORT_REG_IDLE_PCGCR_LSW      (IOPORT_REGISTER(0x1C02))
#define IOPORT_REG_IDLE_PCGCR_MSW      (IOPORT_REGISTER(0x1C03))
#define IOPORT_REG_PER_RSTCOUNT        (IOPORT_REGISTER(0x1C04))
#define IOPORT_REG_PER_RESET           (IOPORT_REGISTER(0x1C05))
#define IOPORT_REG_GPIO_DIR0           (IOPORT_REGISTER(0x1C06))
#define IOPORT_REG_GPIO_DOUT0          (IOPORT_REGISTER(0x1C0A))

#define IOPORT_REG_CONFIG_MSW          (IOPORT_REGISTER(0x1C1F))

#define IOPORT_REG_PLL_CNTL1           (IOPORT_REGISTER(0x1C20))
#define IOPORT_REG_PLL_CNTL2           (IOPORT_REGISTER(0x1C21))
#define IOPORT_REG_PLL_CNTL3           (IOPORT_REGISTER(0x1C22))
#define IOPORT_REG_PLL_CNTL4           (IOPORT_REGISTER(0x1C23))

#define IOPORT_REG_SECUREROM_CNTL      (IOPORT_REGISTER(0x1C25))
#define IOPORT_REG_USB_CNTL            (IOPORT_REGISTER(0x1C32))
#define IOPORT_REG_EMIF_CNTL           (IOPORT_REGISTER(0x1C33))

#define IOPORT_REG_PTR_DIE_ID0_ADDR    (IOPORT_REGISTER_PTR(0x1C40))
#define IOPORT_REG_DIE_ID0             (IOPORT_REGISTER(0x1C40))
#define IOPORT_REG_DIE_ID1             (IOPORT_REGISTER(0x1C41))
#define IOPORT_REG_DIE_ID2             (IOPORT_REGISTER(0x1C42))
#define IOPORT_REG_DIE_ID3             (IOPORT_REGISTER(0x1C43))
#define IOPORT_REG_DIE_ID4             (IOPORT_REGISTER(0x1C44))
#define IOPORT_REG_DIE_ID5             (IOPORT_REGISTER(0x1C45))
#define IOPORT_REG_DIE_ID6             (IOPORT_REGISTER(0x1C46))
#define IOPORT_REG_DIE_ID7             (IOPORT_REGISTER(0x1C47))

#define IOPORT_REG_TEST_LOCK           (IOPORT_REGISTER(0x1C55))

/* Test-Lock register definitions */
#define TEST_LOCK_EFUSE_CTRL_CHNG_KEY   0x569A
#define TEST_LOCK_SET_REF_TRIM_CHNG_KEY 0xC001
#define TEST_LOCK_RELOCK                0x0000


/*********************************************************************
* Analog Registers (0x7000 - 0x70FF)
**********************************************************************/
#define IOPORT_REG_SET_REF_TRIM        (IOPORT_REGISTER(0x7003))


/*********************************************************************
* SPI Registers (0x3000 - 0x3009)
**********************************************************************/
#define IOPORT_REG_SPI_CLKDIV          (IOPORT_REGISTER(0x3000)) // SPI Clock Divider Register
#define IOPORT_REG_SPI_CLKCTR          (IOPORT_REGISTER(0x3001)) // SPI Clock Control Register
#define IOPORT_REG_SPI_DEVCS10         (IOPORT_REGISTER(0x3002)) // SPI External Device Setup Register for Chip Select 0 and 1
#define IOPORT_REG_SPI_DEVCS32         (IOPORT_REGISTER(0x3003)) // SPI External Device Setup Register for Chip Select 2 and 3
#define IOPORT_REG_SPI_CMD1            (IOPORT_REGISTER(0x3004)) // SPI Command 1 Register
#define IOPORT_REG_SPI_CMD2            (IOPORT_REGISTER(0x3005)) // SPI Command 2 Register
#define IOPORT_REG_SPI_STAT1           (IOPORT_REGISTER(0x3006)) // SPI Status 1 Register
#define IOPORT_REG_SPI_STAT2           (IOPORT_REGISTER(0x3007)) // SPI Status 2 Register
#define IOPORT_REG_SPI_DAT1            (IOPORT_REGISTER(0x3008)) // SPI Data 1 Register
#define IOPORT_REG_SPI_DAT2            (IOPORT_REGISTER(0x3009)) // SPI Data 2 Register


/*********************************************************************
* I2C Registers (0x1A00 - 0x1A6C)
**********************************************************************/
#define IOPORT_REG_I2C_OAR             (IOPORT_REGISTER(0x1A00)) // I2C Own Address Register [9:0]
#define IOPORT_REG_I2C_IMR             (IOPORT_REGISTER(0x1A04)) // I2C Interrupt Mask/Status Register [6:0]
#define IOPORT_REG_I2C_STR             (IOPORT_REGISTER(0x1A08)) // I2C Interrupt Status Register [14:0]
#define IOPORT_REG_I2C_CLKL            (IOPORT_REGISTER(0x1A0C)) // I2C Clock Divider Low Register [15:0]
#define IOPORT_REG_I2C_CLKH            (IOPORT_REGISTER(0x1A10)) // I2C Clock Divider High Register [15:0]
#define IOPORT_REG_I2C_CNT             (IOPORT_REGISTER(0x1A14)) // I2C Data Count Register [15:0]
#define IOPORT_REG_I2C_DRR             (IOPORT_REGISTER(0x1A18)) // READ-ONLY   I2C Data Recieve Register [7:0]
#define IOPORT_REG_I2C_SAR             (IOPORT_REGISTER(0x1A1C)) // I2C Slave Address Register [9:0]
#define IOPORT_REG_I2C_DXR             (IOPORT_REGISTER(0x1A20)) // I2C Data Transmit Register [7:0]
#define IOPORT_REG_I2C_MDR             (IOPORT_REGISTER(0x1A24)) // I2C Mode Register [15:0]
#define IOPORT_REG_I2C_IVR             (IOPORT_REGISTER(0x1A28)) // I2C Interrupt Vector Register [11:0]
#define IOPORT_REG_I2C_EMDR            (IOPORT_REGISTER(0x1A2C)) // I2C Extended Mode Register [1:0]
#define IOPORT_REG_I2C_PSC             (IOPORT_REGISTER(0x1A30)) // I2C Prescaler Register [7:0]
#define IOPORT_REG_I2C_PID1            (IOPORT_REGISTER(0x1A34)) // READ-ONLY   I2C Pheripheral ID 1 Register [15:0]
#define IOPORT_REG_I2C_PID2            (IOPORT_REGISTER(0x1A38)) // READ-ONLY   I2C Pheripheral ID 2 Register [7:0]
#define IOPORT_REG_I2C_DMAC            (IOPORT_REGISTER(0x1A3C)) // I2C DMA Control Register [1:0]
#define IOPORT_REG_I2C_PFUNC           (IOPORT_REGISTER(0x1A48)) // I2C Pin Function Register [0:0]
#define IOPORT_REG_I2C_PDIR            (IOPORT_REGISTER(0x1A4C)) // I2C Pin Direction Register [1:0]
#define IOPORT_REG_I2C_PDIN            (IOPORT_REGISTER(0x1A50)) // READ-ONLY   I2C Pin Data In Register [1:0]
#define IOPORT_REG_I2C_PDOUT           (IOPORT_REGISTER(0x1A54)) // I2C Pin Data Out Register [1:0]
#define IOPORT_REG_I2C_PDSET           (IOPORT_REGISTER(0x1A58)) // I2C Pin Data Set Register [1:0]
#define IOPORT_REG_I2C_PDCLR           (IOPORT_REGISTER(0x1A5C)) // I2C Pin Data Clear Register [1:0]
#define IOPORT_REG_I2C_PDRV            (IOPORT_REGISTER(0x1A60)) // I2C Pin Driver Mode Register [1:0]
#define IOPORT_REG_I2C_PPDIS           (IOPORT_REGISTER(0x1A64)) // I2C Pin Pull Disable Register [1:0]
#define IOPORT_REG_I2C_PPSEL           (IOPORT_REGISTER(0x1A68)) // I2C Pin Pull Select Register [1:0]
#define IOPORT_REG_I2C_PSRS            (IOPORT_REGISTER(0x1A6C)) // I2C Pin Slew Rate Register [1:0]


/*********************************************************************
* UART Registers (0x1B00 - 0x1B18)
**********************************************************************/
#define IOPORT_REG_UART_RBR            (IOPORT_REGISTER(0x1B00)) // Read - UART Receive Buffer Register
#define IOPORT_REG_UART_THR            (IOPORT_REGISTER(0x1B00)) // Write - UART Transmit Holding Register
#define IOPORT_REG_UART_IER            (IOPORT_REGISTER(0x1B02)) // UART Interrupt Enable Register
#define IOPORT_REG_UART_IIR            (IOPORT_REGISTER(0x1B04)) // UART Interrupt Identification Register (when reading)
#define IOPORT_REG_UART_FCR            (IOPORT_REGISTER(0x1B04)) // UART FIFO Control Register (when writing)
#define IOPORT_REG_UART_LCR            (IOPORT_REGISTER(0x1B06)) // UART Line Control Register
#define IOPORT_REG_UART_MCR            (IOPORT_REGISTER(0x1B08)) // UART Modem Control Register
#define IOPORT_REG_UART_LSR            (IOPORT_REGISTER(0x1B0A)) // UART Line Status Register
#define IOPORT_REG_UART_MSR            (IOPORT_REGISTER(0x1B0C)) // UART Mode Status Register
#define IOPORT_REG_UART_SCR            (IOPORT_REGISTER(0x1B0E)) // UART Scratch Register
#define IOPORT_REG_UART_DLL            (IOPORT_REGISTER(0x1B10)) // UART Divisor Latch LSB Register
#define IOPORT_REG_UART_DLH            (IOPORT_REGISTER(0x1B12)) // UART Divisor Latch MSB Register
#define IOPORT_REG_UART_PID1           (IOPORT_REGISTER(0x1B14)) // UART Peripheral ID 1 Register
#define IOPORT_REG_UART_PID2           (IOPORT_REGISTER(0x1B16)) // UART Peripheral ID 1 Register
#define IOPORT_REG_UART_PWREMU         (IOPORT_REGISTER(0x1B18)) // UART Power Mangement and Emulation Register


/*********************************************************************
* EMIF Registers (0x1000 - 0x10DD)
**********************************************************************/
#define IOPORT_REG_EMIF_ASYNC_WAIT_0         (IOPORT_REGISTER(0x1004))
#define IOPORT_REG_EMIF_ASYNC_WAIT_1         (IOPORT_REGISTER(0x1005))
#define IOPORT_REG_EMIF_ASYNC1_CONFIG_0      (IOPORT_REGISTER(0x1010))
#define IOPORT_REG_EMIF_ASYNC1_CONFIG_1      (IOPORT_REGISTER(0x1011))
#define IOPORT_REG_EMIF_ASYNC2_CONFIG_0      (IOPORT_REGISTER(0x1014))
#define IOPORT_REG_EMIF_ASYNC2_CONFIG_1      (IOPORT_REGISTER(0x1015))
#define IOPORT_REG_EMIF_ASYNC3_CONFIG_0      (IOPORT_REGISTER(0x1018))
#define IOPORT_REG_EMIF_ASYNC3_CONFIG_1      (IOPORT_REGISTER(0x1019))
#define IOPORT_REG_EMIF_ASYNC4_CONFIG_0      (IOPORT_REGISTER(0x101C))
#define IOPORT_REG_EMIF_ASYNC4_CONFIG_1      (IOPORT_REGISTER(0x101D))
#define IOPORT_REG_EMIF_NAND_CTRL            (IOPORT_REGISTER(0x1060))
#define IOPORT_REG_EMIF_NAND_STATUS          (IOPORT_REGISTER(0x1064))


/*********************************************************************
* Timer Registers (0x1800 - 0x1896)
**********************************************************************/
#define IOPORT_REG_TIMER0_CONTROL            (IOPORT_REGISTER(0x1810))
#define IOPORT_REG_TIMER0_PERIOD_0           (IOPORT_REGISTER(0x1812))
#define IOPORT_REG_TIMER0_PERIOD_1           (IOPORT_REGISTER(0x1813))
#define IOPORT_REG_TIMER0_COUNTER_0          (IOPORT_REGISTER(0x1814))
#define IOPORT_REG_TIMER0_COUNTER_1          (IOPORT_REGISTER(0x1815))

#define TIMER_ENABLE              0x8000 /* bit-15 = 1 */
#define TIMER_CLOCK_CPU           0x0000 /* bit-12 = 0 */
#define TIMER_DIVIDE_BY_2         0x0000 /* bits-[5..2] = 0000 */
#define TIMER_AUTO_RELOAD_DISABLE 0x0000 /* bit-1 = 0 */
#define TIMER_START               0x0001 /* bit-0 = 1 */

/*********************************************************************
* Register Pointers
**********************************************************************/
#define IOPORT_REG_PTR_SECUREROM_ADDR   (IOPORT_REGISTER_PTR(0x2400))


#endif // REGISTERS_H

