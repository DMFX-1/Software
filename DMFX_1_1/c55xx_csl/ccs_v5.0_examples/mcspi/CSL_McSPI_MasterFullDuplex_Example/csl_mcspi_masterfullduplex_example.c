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

/** @file csl_mcspi_masterfullduplex_example.c
 *
 *  @brief McSPI functional layer sample source file
 *
 *
 * \page    page10   McSPI EXAMPLE DOCUMENTATION
 *
 * \section McSPI1   McSPI EXAMPLE1 - MASTER FULL DUPLEX TEST
 *
 * \subsection McSPI1x    TEST DESCRIPTION:
 *		This test code verifies the full duplex functionality of CSL McSPI module. The McSPI module
 * on the C5517 DSP is used to communicate with SPI MsP430.
 * This test code verifies the McSPI functionality with MsP430 SPI .
 *
 * CSL McSPI module will be configured using McSPI_config() API.Write,Read function used for data transfer operation.
 * After successful completion of the write operation read
 * sent to the SPI.Write and read buffers are compared for the data
 * verification. A small amount of delay is required after each data operation
 * to allow the device get ready for next data operation.
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5517.
 * MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 * # define CHIP_C5517
 *
 * \subsection McSPI1y    TEST PROCEDURE:
 *  @li Open the CCS and connect the target (C5517 EVM-Master board)
 *  @li Open the project "CSL_McSPI_MasterFullDulpex_Example.pjt" and build it
 *  @li Load the program on to the target
 *  @li Load the equivalent CSL_McSPIMaster_MSP430Slave_FullDuplex onto the MSP430
 *  @li Set the PLL frequency to 12.288MHz
 *  @li Run the program and observe the transmitting data
 *  @li Run the MSP430 program and observe the receiving data increase in value
 *  @li Repeat the test at the following PLL frequencies
 *      C5517: 60MHz, 100MHz, 150MHz and 200MHz
 *  @li Repeat the test in Release mode
 *
 * \subsection McSPI1z    TEST RESULT:
 *  @li All the CSL APIs should return success
 *  @li Run the master program first & then the slave program
 *  @li Data in the read and write buffers should match.(or)Data needs to be verified in Slave program buffer
 *
 *  If on board buffers latched reset the board & Run the program,Use the correct GEL file for CPU configuration
 */

/* ============================================================================
 * Revision History
 * ================
 * 21-Nov-2011 Created
 * 19-Jul-2012 Added C5517 Documentation
 * ============================================================================
 */

#include <csl_mcspi.h>
#include <stdio.h>
#include <csl_general.h>
#include <csl_intc.h>
#include <csl_i2c.h>
#include "csl_gpio.h"
#include "csl_mcspi.h"
#include "csl_sysctrl.h"

#define CSL_I2C_DATA_SIZE        (64)
#define CSL_EEPROM_ADDR_SIZE     (2)
#define CSL_I2C_OWN_ADDR         (0x2F)
//#define CSL_I2C_SYS_CLK          (12.228)
//#define CSL_I2C_SYS_CLK          (40)
//#define CSL_I2C_SYS_CLK          (60)
//#define CSL_I2C_SYS_CLK          (75)
#define CSL_I2C_SYS_CLK          (100)
#define CSL_I2C_BUS_FREQ         (10)
#define CSL_I2C_EEPROM_ADDR                            (0x50)
#define CSL_I2C_CODEC_ADDR                                (0x18)
#define CSL_I2C_TCA6416_ADDR             (0x21)


Uint16  gI2cWrBuf[CSL_I2C_DATA_SIZE + CSL_EEPROM_ADDR_SIZE];
Uint16  gI2cRdBuf[CSL_I2C_DATA_SIZE];



CSL_McSpiHandle  hMcspi;
CSL_McSpiObj 	 McSpiObj;



CSL_I2cSetup     i2cSetup;
CSL_I2cConfig    i2cConfig;


CSL_GpioObj    gpioObj;
CSL_GpioObj    *hGpio;


#define CSL_TEST_PASSED      (0)
#define CSL_TEST_FAILED      (1)
#define CSL_MIBSPI_BUF_LEN   (5)

Uint16 mibspiWriteBuff[CSL_MIBSPI_BUF_LEN];
Uint16 mcspiReadBuff[CSL_MIBSPI_BUF_LEN];
Uint32 tx_value,rd_value,rd_value1,rd_value2;

CSL_Status CSL_McspiTest(void);

volatile Int16 PaSs_StAtE = 0x0001;
volatile Int16 PaSs       = 0x0000;

McSPI_Config     McSPIHwConfig;


CSL_GpioObj    gpioObj;
CSL_GpioObj    *hGpio;



extern void VECSTART(void);
// Holds the IRQ event ids for Rx and Tx interrupts
Uint16 rxEventId = 0xFFFF, txEventId = 0xFFFF;


CSL_McSpiHandle  hMcspi;
CSL_McSpiObj 	 McSpiObj;


void main(void)
{

	CSL_Status status; //This is for the CSL status return


    printf("\r\nMcSPI Mode test case \r\n");

	status = CSL_McspiTest();

	if( CSL_TEST_PASSED == status )
	{
	 printf("\r\nMcSPI Mode test case passed\r\n");
	}
	else
	{
	 printf("\r\nMcSPI Mode test case failed\r\n");
	 PaSs_StAtE=0;

	}



    PaSs = PaSs_StAtE;


}



CSL_GpioConfig    config;

unsigned short 	  ioindata1_rdval=0;


CSL_Status CSL_McspiTest(void)
{

#if (defined(CHIP_C5517))
  //unsigned char error=0;
  volatile Uint16 mode = 0  ;
  CSL_Status Status;
  volatile Uint32 i=0;

/****    SEL_MMC1 High    ***/


ioExpander_Setup();

//Call this fn for enabling

// port,pin, value
 ioExpander_Write(1, 4, 0); // Select


  /* Open mcspi Port 0, this will return a mcspi handle that will */
  /* be used in calls to other CSl functions.                     */

#if (defined(CHIP_C5517))
  CSL_SYSCTRL_REGS->CLKSTOP2 = 0;
#else
  CSL_SYSCTRL_REGS->CLKSTOP1 = 0;
#endif

  hMcspi =  McSPI_open(McSPI_POLLING_MODE,&McSpiObj,&Status);
    Status = SYS_setEBSR(CSL_EBSR_FIELD_SP1MODE,
                         CSL_EBSR_SP1MODE_1);
    if(CSL_SOK != Status)
    {
        printf("SYS_setEBSR failed\n");
        return (Status);
    }

  McSPI_close(hMcspi);  // close if clock already started
  hMcspi =  McSPI_open(McSPI_POLLING_MODE,&McSpiObj,&Status);

  if ( (hMcspi == NULL) || (Status != CSL_SOK) )
  {
   printf("mcspi_open() Failed \n");
  }
  else
  {
	printf("mcspi_open() Success \n");
  }

    Status = SYS_setEBSR(CSL_EBSR_FIELD_SP1MODE,
                         CSL_EBSR_SP1MODE_1);
    if(CSL_SOK != Status)
    {
        printf("SYS_setEBSR failed\n");
        return (Status);
    }

	hMcspi->McSPIHwConfig.mode       = McSPI_MASTER;
//    hMcspi->McSPIHwConfig.duplex     = McSPI_HALF_DUPLEX_TX;
    hMcspi->McSPIHwConfig.duplex     = McSPI_FULL_DUPLEX;
//	hMcspi->McSPIHwConfig.duplex     = McSPI_HALF_DUPLEX_RX;
    hMcspi->McSPIHwConfig.wLen       = McSPI_WORD_LENGTH_8;
	hMcspi->McSPIHwConfig.dataDelay  = McSPI_DATA_DLY4;
//	hMcspi->McSPIHwConfig.dataDelay  = McSPI_DATA_NODLY;
    hMcspi->McSPIHwConfig.ClkDiv     = CSL_MCSPI_CH0CONFL_CLKD_DIV4;
    hMcspi->McSPIHwConfig.clkPh      = McSPI_CLK_PH_EVEN_EDGE;   //even edge ,polarity low important
//    hMcspi->McSPIHwConfig.clkPol     = McSPI_CLKP_HIGH;
    hMcspi->McSPIHwConfig.clkPol     = McSPI_CLKP_LOW;
    hMcspi->ch					     = McSPI_CS_NUM_0;
    hMcspi->McSPIHwConfig.csPol      = McSPI_CSP_ACTIVE_LOW;
    hMcspi->McSPIHwConfig.inSel      = McSPI_RX_SPIDAT0;
	hMcspi->McSPIHwConfig.enDat0_1   = (McSPI_TxEn_Dat0_1)McSPI_TX_ENABLE_SPIDAT0;


	Status = McSPI_config(hMcspi);
	if( CSL_SOK != Status )
	{
	 printf("\r\n McSPI_Config() is failed\r\n");
	}


//	error=0;

	printf("McSPI Mode %d Test start \r\n",mode);

	tx_value = 0x0; // u45 pin 9
	rd_value = CSL_MCSPI_REGS->CH0STATL;
    CSL_MCSPI_REGS->CH0STATL = rd_value & CSL_MCSPI_CH0STATL_RXS_MASK;

	while(1)
	{
		if(CSL_MCSPI_REGS->CH0STATL & CSL_MCSPI_CH0STATL_TXS_MASK) // Check if TXBUFF is EMPTY
		{

			 CSL_MCSPI_REGS->CH0TXL = (Uint16)tx_value;
			 CSL_MCSPI_REGS->CH0TXU = (Uint16)(tx_value>>16) ;
			 printf("\r\nMcSPI Tx data : %lx \r\n",tx_value);//need this line for delay
 			 while(!(CSL_MCSPI_REGS->CH0STATL & CSL_MCSPI_CH0STATL_EOT_MASK)); // WAIT till END of Txn
			 tx_value ++;

			//if(McSPI_RxFullInt_Chk(hMcspi))
			if(CSL_MCSPI_REGS->CH0STATL & CSL_MCSPI_CH0STATL_RXS_MASK)			// Check if Data is Received
			{
		      rd_value = hMcspi->Regs->CH0RXL;
 			  printf("McSPI Rx data : %lx \r\n",rd_value&0xFF);

			  rd_value =CSL_MCSPI_REGS->CH0STATL;
			  CSL_MCSPI_REGS->CH0STATL = rd_value & CSL_MCSPI_CH0STATL_RXS_MASK; // Clear RXFULL Mask

            }

			for(i = 0 ; i < 0X03FFFFF ; i++);  // Delay

        }


	}//while loop end
/*
    Status = McSPI_close(hMcspi);

 	if(error)
	{
		return(CSL_TEST_FAILED);
	}
	else
	{
		return(CSL_TEST_PASSED);
	}
*/
#endif
 }

