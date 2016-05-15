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

/** @file programmer.c
 *
 *  @brief Programmer for C5505/15/17 EVMs
 *
 * \page    page22  CSL PROGRAMMER DOCUMENTATION
 *
 * * This utility allows a user-specified binary file to be programmed into
* one of the following supported devices...
*  @li NAND FLASH
*  @li NOR FLASH
*  @li SPI EEPROM
*  @li IIC EEPROM
*  @li MMC
*  @li SD
*  @li SPI Flash
*  @li MCSPI Flash
*
* This utility also allows data from these devices to be read and transferred
* to a file.
 * ============================================================================
 */

/* ============================================================================
 * Revision History
 * ================
 * 23-Jul-2012 Added to CSL 3.0
 * ============================================================================
 */


// use this defination for C5517 EVM with INTEL P30 Family FLASH
//#define C5517_EVM
// use this defination for C5515 EVM
//#define C5515_EVM
// use this defination for VC5505 EVM
//#define VC5505_EVM
// use this defination for VC5517 EVM with SPANSION S29GLxxxS FLASH
#define C5517_EVM_SPANSION_NORFLASH


#include <stdio.h>
#include <stdlib.h>
#include "registers.h"
#include "sysctrl.h"

#ifdef C5517_EVM_SPANSION_NORFLASH
#include "nor_flash.h"
FlashDeviceData_t NorBootFlashDeviceData;
#endif

#ifdef VC5505
#include "nor_flash.h"
#endif

#ifdef C5515_EVM
#include "machine.h"
#include "norflash_c5515evm.h"
#endif

#ifdef C5517_EVM
#include "machine.h"
#include "norflash_c5515evm.h"
#endif

#include "spi_eeprom.h"
#include "mcspi_eeprom.h"
#include "iic_eeprom.h"
#include "nand_flash.h"
#include "mmc_sd.h"

// use programmer as a comparison tool
//#define COMPAREONLY

typedef UInt16 (*fpREAD_N_WORDS)(UInt32 address, UInt16 *buffer, UInt16 count);
typedef UInt16 (*fpWRITE_N_WORDS)(UInt32 address, UInt16 *buffer, UInt16 count);

void waitloop(unsigned long loopval);     /* Waits an amount of time specified by loopval */
void nand_readBack(char *fileName);

/* Functions to program and read a device */
void program_device(fpREAD_N_WORDS READ_N_WORDS, fpWRITE_N_WORDS WRITE_N_WORDS, char *fileName, int IIC, int SPIFLASH);
void read_device(fpREAD_N_WORDS READ_N_WORDS, char *fileName, int numBits);

Uint16 flash_id[4] = {0, 0, 0, 0}; 
Uint16 i;
Uint32 addr = 0x00000000;

/******************************************************************************
* main
******************************************************************************/
void main(void)
{
	/* Function pointers to the init and read/write n words functions */
	UInt16 (*READ_N_WORDS)(UInt32 address, UInt16 *buffer, UInt16 count);
	UInt16 (*WRITE_N_WORDS)(UInt32 address, UInt16 *buffer, UInt16 count);
	char buf[120];
	int InputIndex=0;
	int DeviceTypeIndex;
	int ChipSelectIndex;
	int OperationIndex;
	char *filename;
	Uint16 ChipSelect;
	Uint16 id[3];

	/* Get the user's requested device to program */
	printf("Choose the device...\n");
	printf("1x - NAND Flash [CSx: 2,3,4,5]\n");
	printf("2x - NOR Flash [CSx: 2,3,4,5]\n");
#ifdef C5515_EVM
	printf("3 - SPI EEPROM (16 bit address)\n");
#endif
	printf("4 - IIC EEPROM\n");
	printf("5 - MMC\n");
	printf("6 - SD\n");
#ifndef C5515_EVM
	printf("7x - SPI Serial Flash (24 bit address) [PinMap x: 1=MODE5,2=MODE6]\n");
	printf("8 - MCSPI Serial Flash\n");
#endif
	scanf("%s", buf);
	InputIndex = 0;
	DeviceTypeIndex = InputIndex;
	InputIndex++;
	if ((buf[DeviceTypeIndex] < '0') || (buf[DeviceTypeIndex] > '8'))
	{
	  printf("ERROR: Invalid device\n");
	  exit(1);
	}

	/* Get the user's requested chip-select (only if applicable) */
	switch (buf[DeviceTypeIndex])
	{
		case '1':
		case '2':
			ChipSelectIndex = InputIndex;
			InputIndex++;
			if ((buf[ChipSelectIndex] < '2') || (buf[ChipSelectIndex] > '5'))
			{
				printf("ChipSelect [2,3,4,5]?\n");
				scanf("%s", &buf[ChipSelectIndex]);
			}
			if ((buf[ChipSelectIndex] < '2') || (buf[ChipSelectIndex] > '5'))
			{
				printf("ERROR: Invalid ChipSelect\n");
				exit(1);
			}
		break;
	}
	/* Get the user's requested SPI Pin-Mapping (only if applicable) for SPI flash */
	//if (buf[DeviceTypeIndex] == '3' | buf[DeviceTypeIndex] == '7')
	if (buf[DeviceTypeIndex] == '3' || buf[DeviceTypeIndex] == '7')
	//if (buf[DeviceTypeIndex] == '7')
	{
		ChipSelectIndex = InputIndex;
		InputIndex++;
		if ((buf[ChipSelectIndex] < '1') || (buf[ChipSelectIndex] > '2'))
		{
			printf("ChipSelect [1=EVM,2=USB Stick]?\n");
			scanf("%s", &buf[ChipSelectIndex]);
		}
		if ((buf[ChipSelectIndex] < '1') || (buf[ChipSelectIndex] > '2'))
		{
			printf("ERROR: Invalid ChipSelect Selected\n");
			exit(1);
		}
	}

	/* Get the user's requested operation */
	OperationIndex = InputIndex;
	InputIndex++;
	filename = &buf[InputIndex];
	if ((((buf[DeviceTypeIndex] >= '0') && (buf[DeviceTypeIndex] <= '4')) &&
	((buf[OperationIndex] < '0') || (buf[OperationIndex] > '2'))) ||
	(((buf[DeviceTypeIndex] == '5') || (buf[DeviceTypeIndex] == '6')) &&
	((buf[OperationIndex] < '0') || (buf[OperationIndex] > '3'))))
	{
		printf("Operation to perform...\n");
		printf("1<file> - Program device with <file>\n");
		printf("2<file> - Read device data to output <file>\n");
		if (buf[DeviceTypeIndex] == '5')
		{
			printf("3 - List files\n");
		}
		scanf("%s", &buf[OperationIndex]);
	}

	if ((((buf[DeviceTypeIndex] >= '0') && (buf[DeviceTypeIndex] <= '4')) &&
	((buf[OperationIndex] < '0') || (buf[OperationIndex] > '2'))) ||
	(((buf[DeviceTypeIndex] == '5') || (buf[DeviceTypeIndex] == '6')) &&
	((buf[OperationIndex] < '0') || (buf[OperationIndex] > '3'))))
	{
		printf("ERROR: Invalid operation\n");
		exit(1);
	}

	/* Initialize and setup read/write function pointers for the selected device */
	switch (buf[DeviceTypeIndex])
	{
	case '1':
		switch (buf[ChipSelectIndex])
		{
		case '2': ChipSelect = NAND_FLASH_CS2; break;
		case '3': ChipSelect = NAND_FLASH_CS3; break;
		case '4': ChipSelect = NAND_FLASH_CS4; break;
		case '5': ChipSelect = NAND_FLASH_CS5; break;
		default:
		 printf("ERROR: Invalid ChipSelect\n");
		 exit(1);
		}
		printf("NAND Flash...\n");
		/* The following setup is required for QT when accessing EMIF */
		/* Setup pin-mapping (Serial Port 0 pins configured as GPIO) */
		PeripheralPinMapping(PERIPH_BUS_S0_GPIO, PERIPH_BUS_S1_MMCSD1, PERIPH_BUS_PP_LCD_UART_SPI);
		IOPORT_REG_GPIO_DIR0 = 1;  /* GPIO-0 == output */
		IOPORT_REG_GPIO_DOUT0 = 1; /* GPIO-0 == high (disables SDRAM) */
		/* Enable EMIF Clock */
		PeripheralClkEnableOnly(PERIPH_CLK_MASTER_CLK | PERIPH_CLK_EMIF);
		/* Reset the EMIF */
		PeripheralReset(PERIPH_RESET_EMIF_TIMERS_RTC, 0x20, 500);
		/* Initialize the device */
		nand_init(ChipSelect);
		break;

	case '2':
		switch (buf[ChipSelectIndex])
		{
			case '2': ChipSelect = NOR_FLASH_CS2; break;
			case '3': ChipSelect = NOR_FLASH_CS3; break;
			case '4': ChipSelect = NOR_FLASH_CS4; break;
			case '5': ChipSelect = NOR_FLASH_CS5; break;
			default:
				printf("ERROR: Invalid ChipSelect\n");
				exit(1);
		}

		printf("NOR Flash...\n");
		/* The following setup is required for QT when accessing EMIF */
		/* Setup pin-mapping (Serial Port 0 pins configured as GPIO) */
		PeripheralPinMapping(PERIPH_BUS_S0_GPIO, PERIPH_BUS_S1_MMCSD1, PERIPH_BUS_PP_LCD_UART_SPI);
		IOPORT_REG_GPIO_DIR0 = 1;  /* GPIO-0 == output */
		IOPORT_REG_GPIO_DOUT0 = 1; /* GPIO-0 == high (disables SDRAM) */
		/* Enable EMIF Clock */
		PeripheralClkEnableOnly(PERIPH_CLK_MASTER_CLK | PERIPH_CLK_EMIF);
		/* Reset the EMIF */
		PeripheralReset(PERIPH_RESET_EMIF_TIMERS_RTC, 0x20, 500);
		/* Initialize the device */
#ifdef C5517_EVM_SPANSION_NORFLASH
		nor_flash_init(ChipSelect);
		flashGetDeviceData(&NorBootFlashDeviceData);
		printf("The Flash ID is Manufacturer Id: 0x%x,\n"
				" Spansion S29GLxxxS specific Device Id word: 0x%x, \n"
				" Size Indicator: 0x%x, \n"
				" Spansion S29GLxxxS specific Device Id word: 0x%x\n",
				NorBootFlashDeviceData.MfrId,
				NorBootFlashDeviceData.DevId[0],
				NorBootFlashDeviceData.DevId[1],
				NorBootFlashDeviceData.DevId[2]);
		READ_N_WORDS = nor_flash_read_n_words;
		WRITE_N_WORDS = nor_flash_write_n_words;
#endif

#ifdef VC5505_EVM
		nor_flash_init(ChipSelect);
		flashGetDeviceData(&NorBootFlashDeviceData);
		READ_N_WORDS = nor_flash_read_n_words;
		WRITE_N_WORDS = nor_flash_write_n_words;
#endif

#ifdef C5515_EVM
		norflash_init();
		//_FLASH_getId( &id[0] );
		READ_N_WORDS = norflash_read;
		WRITE_N_WORDS = norflash_write;
#endif

#ifdef C5517_EVM
		norflash_init();
		_FLASH_getId( &id[0] );
		printf("The Flash ID is %x, %x, %x\n", id[0], id[1], id[2]);
		READ_N_WORDS = norflash_read;
		WRITE_N_WORDS = norflash_write;
#endif
		break;


	case '3':
		printf("SPI EEPROM...\n");
		/* Setup pin-mapping (SPI on parallel bus) */
		if(buf[ChipSelectIndex] == '1')
			// EVM
			PeripheralPinMapping(PERIPH_BUS_S0_MMCSD0, PERIPH_BUS_S1_MMCSD1, PERIPH_BUS_PP_LCD_SPI_I2S3);
		else
			// USB Stick Board
			PeripheralPinMapping(PERIPH_BUS_S0_MMCSD0, PERIPH_BUS_S1_MMCSD1, PERIPH_BUS_PP_GPIO_SPI_UART_I2S2);

		/* Enable SPI Clock */
		PeripheralClkEnableOnly(PERIPH_CLK_MASTER_CLK | PERIPH_CLK_SPI);
		/* Initialize the device */
		spi_eeprom_init();
		READ_N_WORDS = spi_eeprom_read_n_words;
		WRITE_N_WORDS = spi_eeprom_write_n_words;
		break;

	case '4':
		printf("IIC EEPROM...\n");
		/* Enable I2C Clock */
		PeripheralClkEnableOnly(PERIPH_CLK_MASTER_CLK | PERIPH_CLK_I2C);
		/* Initialize the device */
		iic_eeprom_init();
		READ_N_WORDS = iic_eeprom_read_n_words;
		WRITE_N_WORDS = iic_eeprom_write_n_words;
		break;

	case '5':
		printf("MMC...\n");
		/* Enable MMC/SD-0 Clock */
		PeripheralClkEnableOnly(PERIPH_CLK_MASTER_CLK | PERIPH_CLK_MMCSD0);
		/* Reset MMC/SD-0 */
		PeripheralReset(PERIPH_RESET_MMCSD0_MMCSD1_IIS0_IIS1, 0x20, 500);
		/* Setup pin-mapping (SPI on parallel bus) */
		PeripheralPinMapping(PERIPH_BUS_S0_MMCSD0, PERIPH_BUS_S1_MMCSD1, PERIPH_BUS_PP_GPIO_SPI_I2S2_3);
		/* Initialize the device */
		mmc_sd_init(MMC_SD_MMC0);
		break;

	case '6':
		printf("SD...\n");
		/* Enable MMC/SD-0 Clock */
		PeripheralClkEnableOnly(PERIPH_CLK_MASTER_CLK | PERIPH_CLK_MMCSD0);
		/* Reset MMC/SD-0 */
		PeripheralReset(PERIPH_RESET_MMCSD0_MMCSD1_IIS0_IIS1, 0x20, 500);
		/* Setup pin-mapping (SPI on parallel bus) */
		PeripheralPinMapping(PERIPH_BUS_S0_MMCSD0, PERIPH_BUS_S1_MMCSD1, PERIPH_BUS_PP_GPIO_SPI_I2S2_3);
		/* Initialize the device */
		mmc_sd_init(MMC_SD_SD0);
		break;


	case '7':
		printf("SPI Serial Flash...\n");
		/* Setup pin-mapping (SPI on parallel bus) */
		if (buf[ChipSelectIndex] == '1') // EVM
			PeripheralPinMapping(PERIPH_BUS_S0_MMCSD0, PERIPH_BUS_S1_MMCSD1, PERIPH_BUS_PP_LCD_UART_SPI);
		else // (buf[PinMappingIndex] == '2') // VDB
			PeripheralPinMapping(PERIPH_BUS_S0_MMCSD0, PERIPH_BUS_S1_MMCSD1, PERIPH_BUS_PP_GPIO_SPI_I2S2_3);
			//PeripheralPinMapping(PERIPH_BUS_S0_MMCSD0, PERIPH_BUS_S1_MMCSD1, PERIPH_BUS_PP_GPIO_SPI_UART_I2S2);
		/* Enable SPI Clock */
		PeripheralClkEnableOnly(PERIPH_CLK_MASTER_CLK | PERIPH_CLK_SPI);
		/* Initialize the device */
		spi_eeprom_init();
		READ_N_WORDS = spi_eeprom_read_n_words;
		WRITE_N_WORDS = spi_eeprom_write_n_words;

		// get SPI flash ID
		spi_read_id(flash_id);
		printf("SPI Flash ID is: 0x%x, 0x%x, 0x%x, 0x%x\n", flash_id[0], flash_id[1], flash_id[2], flash_id[3]);

		if (buf[OperationIndex]=='1')
		{
			// erase the whole chip
			printf("Erasing chip, this may take a while...\n");
			spiFlashChipErase();
			printf("Chip erase done.\n");
		}

		break;

	case '8':
		printf("MCSPI Serial Flash...\n");
		ioExpander_Setup();
		/* Test Requires SW4 HPI_ON to be OFF */
		/* Set SEL1_MMC1_MCSPI and SEL0_MMC1_MCSPI high for routing GPIO[6:11] to JP46*/
		ioExpander_Write(1, 4 , 0);// SEL0_MMC1_MCSPI = 0
		ioExpander_Write(1, 5 , 1);// SEL1_MMC1_MCSPI = 1

		/* Setup pin-mapping (MCSPI on serial bus 1) */
		PeripheralPinMapping(PERIPH_BUS_S0_MMCSD0, PERIPH_BUS_S1_I2S1, PERIPH_BUS_PP_GPIO_SPI_I2S2_3);
		/* Enable MCSPI Clock */
		PeripheralClkEnableOnly(PERIPH_CLK_MASTER_CLK | PERIPH_CLK_IIS1);
		/* Initialize the device */
		mcspi_eeprom_init(5);
		READ_N_WORDS = mcspi_eeprom_read_n_words;
		WRITE_N_WORDS = mcspi_eeprom_write_n_words;

		// erase the chip
		printf("Erasing chip, this may take a while...\n");
		mcspiFlashChipErase();
		printf("Chip erase done.\n");
#if 0
		while (1)
		{
			// read four word start from address 0
			for (i=0; i<4; i++)
			flash_id[i] = 0;
			mcspi_eeprom_read_n_words(addr, flash_id, 4);
			addr +=4;
		}
#endif

		break;

	}

	/* Perform the requested operation */
	switch (buf[OperationIndex])
	{
	case '1':
		if (buf[DeviceTypeIndex] == '1')
		{
			printf("Writing data to NAND...\n");
			nand_main(filename);
		}
		else if (buf[DeviceTypeIndex] == '2')
		{
			printf("Erasing chip (NOR)...\n");
#ifdef C5517_EVM_SPANSION_NORFLASH
    	    if (nor_flash_chip_erase() != 0)
#endif

#ifdef VC5505_EVM
    	    if (nor_flash_chip_erase() != 0)
#endif
#ifdef C5515_EVM
			if ( norflash_erase( FLASH_BASE, 8 * FLASH_PAGESIZE )!= 0)
#endif
#ifdef C5517_EVM
			if ( norflash_erase( FLASH_BASE, 8 * FLASH_PAGESIZE )!= 0)
#endif
			{
				printf("ERROR: Chip-Erase failed!!\n");
				exit(1);
			}
		}
		else if ((buf[DeviceTypeIndex] == '5') || (buf[DeviceTypeIndex] == '6'))
		{
			if (mmc_sd_file_delete() != 0)
		{
			printf("ERROR: Delete-file failed!!\n");
			exit(1);
		}
			mmc_sd_file_write(filename);
			exit(0);
		}
		printf("Writing data to device...\n");
		program_device(READ_N_WORDS, WRITE_N_WORDS, filename, (buf[DeviceTypeIndex] == '4'), (buf[DeviceTypeIndex] == '7')||(buf[DeviceTypeIndex] == '8'));
		printf("Programming Complete\n");
		break;

	case '2':
		if (buf[DeviceTypeIndex] == '1')
		{
			printf("Reading data from NAND...\n");
			nand_readBack(filename);
		}
		else if ((buf[DeviceTypeIndex] == '5') || (buf[DeviceTypeIndex] == '6'))
		{
			if (buf[DeviceTypeIndex] == '5')
				printf("Reading data from MMC...\n");
			else
				printf("Reading data from SD...\n");
			mmc_sd_file_read(filename);
		} else if ((buf[DeviceTypeIndex] == '7') || (buf[DeviceTypeIndex] == '8'))
		{
			printf("Reading data from serial flash...\n");
			read_device(READ_N_WORDS, filename, 24);
		} else
		{
			printf("Reading data from EEPROM...\n");
			read_device(READ_N_WORDS, filename, 16);
		}
		printf("Reading Complete (no status)\n");
		break;

	case '3':
		if ((buf[DeviceTypeIndex] == '5') || (buf[DeviceTypeIndex] == '6'))
		{
			mmc_sd_file_list();
		}
		printf("File-List Complete\n");
		break;

	default:
		printf("ERROR: Invalid programming choice\n");
		exit(1);
	}
	exit(0);
}

/******************************************************************************
* program_device
******************************************************************************/
void program_device(fpREAD_N_WORDS READ_N_WORDS, fpWRITE_N_WORDS WRITE_N_WORDS, char *fileName, int IIC, int SPIFLASH)
{
	FILE *pInputFile = NULL;
	int retry;
	unsigned short FileData[2];
	unsigned short OutData;
	unsigned short VerifyData;
//	unsigned int addr = 0x0000;   /* start address is 0 */
	unsigned long addr = 0x0000;   /* start address is 0 */
	size_t rcv_count;

	if(SPIFLASH)
		addr = 0x80000000;



	if (fileName[0] == '\0')
	{
		printf("Opening flashimg.bin...\n");
		pInputFile = fopen("flashimg.bin","rb");
	}
	else
	{
		printf("Opening %s...\n",fileName);
		pInputFile = fopen(fileName, "rb");
	}
	if (pInputFile == NULL)
	{
		printf("Unable to find file!\n");
		exit(2);
	}
	else
	{
		printf("Input file opened\n");
	}

	/* Read all data from file and write it to the device */
	while (!feof(pInputFile))
	{
		rcv_count = fread(FileData, 1, 2, pInputFile);
		if (rcv_count == 0)
		{
			break; /* no more data to read */
		}
		if ((rcv_count & 1) != 0)
		{
			printf("Input File has an odd number of bytes (invalid)\n");
			exit(1);
		}
		OutData = (FileData[0] << 8) | FileData[1];
#ifndef COMPAREONLY
		retry = 5;
		do
		{
			//if (addr==0x80000000)
			//{
			//	printf("Erasing chip, this may take a while...\n");
			//}
			//if (IIC == 1) /* need delay for I2C */
			{
				//asm("\tNOP");
				//asm("\tNOP");
				//asm("\tNOP");
				//asm("\tNOP");
				waitloop(0xFFF); /* wasn't there, newly added */
			}
			WRITE_N_WORDS(addr, &OutData, 1);
			//if (addr==0x80000000)
			//{
			//	printf("Chip erase done.\n");
			//}
			//if (IIC == 1) /* need delay for I2C */
			{
				//asm("\tNOP");
				//asm("\tNOP");
				//asm("\tNOP");
				//asm("\tNOP");
				waitloop(0xFFF); /* was 0xFFF */
			}
#else
			//addr &= ~(0x80000000); // clear 24-bit SPI device indicator bit
#endif
			/* Read back the word */
			READ_N_WORDS(addr, &VerifyData, 1);
#ifndef COMPAREONLY
			retry--;
		} while ((retry > 0) && (OutData != VerifyData));
#endif
		if (OutData != VerifyData)
		{
#ifndef COMPAREONLY
			printf("WRITE ERROR! at 0x%lx\n", addr);
			printf("Wrote 0x%04x  Read 0x%04x\n", OutData, VerifyData);

			fclose(pInputFile);
			exit(2);
#else
			printf("COMPARE ERROR! at 0x%04x\n", addr);
			printf("File_Read 0x%04x  Device_Read 0x%04x\n", OutData, VerifyData);
#endif
		}
		addr++;
	}

	fclose(pInputFile);
}

/******************************************************************************
* read_device
******************************************************************************/
void read_device(fpREAD_N_WORDS READ_N_WORDS, char *fileName, int numBits)
{
	FILE *pOutputFile = NULL;
	unsigned long addr = 0;
	unsigned long max_words = 0;
	unsigned short read_word;
	unsigned short FileData[2];
	size_t write_count;

	max_words = 300;
	printf("Reading up to %ld words...\n", max_words);    

	if ((pOutputFile = fopen(fileName, "wb")) == NULL)
	{
		printf("Error in opening file\n");
		exit(1);
	}
	
	if (numBits==16)
		addr = 0;
	else
		addr = 0x80000000;

	while ((addr&0xFFFFFF) < max_words)
	{
		if ((addr % 0x100) == 0)
		{
			printf("Reading...0x%lx\n", (addr&0xFFFFFF));
		}
		READ_N_WORDS(addr, &read_word, 1);
		addr++;
		FileData[0] = read_word >> 8;
		FileData[1] = read_word & 0xFF;
		write_count = fwrite(&FileData, 1, 2, pOutputFile);
		if (write_count != 2)
		{
			printf("Error writing to the file\n");
			fclose(pOutputFile);
			exit(1);
		}
	}
	fclose(pOutputFile);
	printf("Done\n");
}

/******************************************************************************
* waitloop
******************************************************************************/
void waitloop(unsigned long loopval)
{
	//volatile unsigned short counter = loopval;
	volatile unsigned long counter = loopval;
	while (counter)
	{
		counter--;
	}
}
