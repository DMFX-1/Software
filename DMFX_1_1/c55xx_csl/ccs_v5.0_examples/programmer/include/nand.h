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
*    nand.h
**********************************************************************/
#ifndef NAND_H
#define NAND_H

#include "ata_nand.h"

/* Definitions for the ChipSelect used by the NAND device */
#define NAND_FLASH_CS2   0x0001
#define NAND_FLASH_CS3   0x0002
#define NAND_FLASH_CS4   0x0004
#define NAND_FLASH_CS5   0x0008

/* NAND-Flash base addresses for each chip-select (16-bit word address) */
#define NAND_FLASH_BASE_ADDRESS_CS2   0x400000
#define NAND_FLASH_BASE_ADDRESS_CS3   0x600000
#define NAND_FLASH_BASE_ADDRESS_CS4   0x700000
#define NAND_FLASH_BASE_ADDRESS_CS5   0x780000

typedef struct NandStruct_definition
{
   volatile unsigned short data;       /* word offset 0x0000 */
   volatile unsigned short gap1[0xFFF];
   volatile unsigned short ale;        /* word offset 0x1000 */
   volatile unsigned short gap2[0xFFF];
   volatile unsigned short cmd;        /* word offset 0x2000 */
} NandStruct;

unsigned int NandReadID();  /* returns Maker Code in upper byte, Device Code in lowever byte */
unsigned int xNandReadID();  /* returns Maker Code in upper byte, Device Code in lowever byte */
int NandConfig(Uint16 ChipSelect);
void NandSelect();
void NandDeselect();
void NandIssueReadCommand(unsigned long page_address, AtaNANDState *pNAND);
unsigned short NandWriteData(unsigned long page_address, unsigned short *data, unsigned short size);
void NandSetAddress(unsigned long start_address, int page_only, AtaNANDState *pNAND);
int NandTimeoutCheck(int (fpStatusCheck)(void *ptr), void *ptr);
int xNandTimeoutCheck(int (fpStatusCheck)(void *ptr), void *ptr);
int Nand_widthDetect(void *pNAND);

/*
Parameter                        Symbol      Min      Max      Unit  Note
Program Time                     tPROG    -     500      ms
Number of Partial Program Cycles
in the Same Page
         Main Array              Nop         -     2     cycles
         Spare Array                      -     3     cycles
Block Erase Time                 tBERS       -     3     ms
CLE setup Time                   tCLS     0     -     ns
CLE Hold Time                    tCLH     10       -     ns
CE setup Time                    tCS         0     -     ns
CE Hold Time                     tCH      10    -     ns
WE Pulse Width                   tWP      25 NOTE  -     ns    NOTE: 35 ns min. if tCS < 10 ns
ALE setup Time                   tALS     0     -     ns
ALE Hold Time                    tALH     10    -     ns
Data setup Time                     tDS         20    -     ns
Data Hold Time                   tDH         15    -     ns
Write Cycle Time                 tWC         60    -     ns
WE High Hold Time                tWH         25    -     ns
Data Transfer from Cell to Register    tR       -     10    ms
ALE to RE Delay( ID read )             tAR1     100      -     ns
ALE to RE Delay(Read cycle)         tAR2     50       -     ns
CE to RE Delay( ID read)            tCR         100      -     ns
Ready to RE Low                     tRR         20       -     ns
RE Pulse Width                   tRP         30       -     ns
WE High to Busy                     tWB         -     100      ns
Read Cycle Time                     tRC         60       -     ns
RE Access Time tREA - 35 ns
RE High to Output Hi-Z              tRHZ     15    30    ns
CE High to Output Hi-Z              tCHZ     -     20    ns
RE High Hold Time                tREH     25       -     ns
Output Hi-Z to RE Low               tIR         0     -     ns
Last RE High to Busy(at sequential read)tRB        -     100      ns
CE High to Ready(in case of interception by CE at read)
                              tCRY     -     50 +tr(R/B) (1) ns
CE High Hold Time(at the last serial read)(2)
                              tCEH     100   -     ns
CE Low to RE if CE don't care used     tCEA     45    -     ns
RE Low to Status Output             tRSTO    -     35    ns
CE Low to Status Output             tCSTO    -     45    ns
WE High to RE Low                tWHR     60    -     ns
RE access time(Read ID)             tREADID     -     35    ns
Device Resetting Time(Read/Program/Erase)
                              tRST     -     5/10/500ms
*/

#define NAND_tRSU   1  /* read address set up          */
#define NAND_tREA   7  /* 7 cycles = >49nsec @ 12 MHz. */
#define NAND_tREH   3  /* 3 cycles = 25nsec @ 12 MHz. */
#define NAND_tRHZ   1  /* tREH + tRHZ = >35nsec @ 12 MHz. */

#define NAND_tWSU   1  /* write address setup          */
#define NAND_tWP    5  /* 5 cycles = >35nsec @ 12 MHz. */
#define NAND_tWH    3  /* 3 cycles = 25nsec @ 12 MHz.  */


/* NAND FLASH COMMANDS */
#define NAND_CMD_READ                  0x00
#define NAND_CMD_READ_CONFIRM          0x30
#define NAND_CMD_READ_LOW              0x00
#define NAND_CMD_READ_HIGH             0x01
#define NAND_CMD_READ_ID               0x90
#define NAND_READ_ID_ADDR              0x00
#define NAND_CMD_PAGE_PROGRAM          0x80
#define NAND_CMD_PAGE_PROGRAM_CONFIRM  0x10
#define NAND_CMD_BLOCK_ERASE           0x60
#define NAND_CMD_BLOCK_ERASE_CONFIRM   0xD0
#define NAND_CMD_STATUS                0x70
#define NAND_CMD_RESET                 0xFF

/*STATUS OUTPUT*/
#define NAND_STATUS_FAIL           0x01  /* used for program/erase */
#define NAND_STATUS_READY          0x40
#define NAND_STATUS_NOT_PROTECTED  0x80

/*Bad Block Marking */
#define FREE_BLOCK 0xFFEE
#define BAD_BLOCK 0xBADB
#define MAX_LOG_BLOCKS_PER_ZONE 1000
#define MAX_PHY_BLOCKS_PER_ZONE 1024

#endif // NAND_H
