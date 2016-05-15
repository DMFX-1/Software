
(On C5517 EVM)
NOTE: HPI_ON SW4 switch 1  must be towards ON away from DOT

Choose the device...
1x - NAND Flash [CSx: 2,3,4,5]
2x - NOR Flash [CSx: 2,3,4,5]
4 - IIC EEPROM
5 - MMC
6 - SD
7x - SPI Serial Flash (24 bit address) [PinMap x: 1=MODE5,2=MODE6]
8 - MCSPI Serial Flash

>>> Here please enter:

141<file_path> - For NAND Flash write
221<file_path> - For NOR Flash write
41<file_path> - For IIC EEPROM write
51<file_path> - For MMC write
53 - For MMC File Listing
61<file_path> - For SD write
63 - For SD File Listing
711<file_path> - For SPI Flash write
81<file_path> - For McSPI Flash write

!!!CAUTION!!!
SPI Flash and McSPI Flash related options are not available for C5515 EVM.
The C5515 EVM does not have these interfaces.
SPI EEPROM related options are not available for C5517 EVM.
The C5517 EVM does not have this interface.


Expected Results:

141F:\sample.bin
NAND Flash...
Writing data to NAND...
Writing Sector 0...
Writing Sector 1 (MBR)...
Flushing Data...
Writing Boot Record...
Flushing Data...
Opening F:\sample.bin...
Input file opened
Writing Boot-image to NAND...
Programming... [TotalSize=256 (0x100)]
Programming... [TotalSize=512 (0x200)]
Programming... [TotalSize=768 (0x300)]
Programming... [TotalSize=1024 (0x400)]
...
...
...
...
Programming... [TotalSize=12800 (0x3200)]
Programming... [TotalSize=12936 (0x3288)]
Flushing Data...
done

221F:\sample.bin
NOR Flash...
The Flash ID is Manufacturer Id: 0x1,
 Spansion S29GLxxxS specific Device Id word: 0x227e, 
 Size Indicator: 0x2221, 
 Spansion S29GLxxxS specific Device Id word: 0x2201
Erasing chip (NOR)...
Writing data to device...
Opening F:\sample.bin...
Input file opened
Programming Complete

41F:\sample.bin
IIC EEPROM...
Writing data to device...
Opening F:\sample.bin...
Input file opened
Programming Complete

51F:\sample.bin
MMC...
Delete complete
Opening F:\sample.bin...
Input file opened
Writing Boot-image to device...
Programming... [TotalSize=256 (0x100)]
Programming... [TotalSize=512 (0x200)]
Programming... [TotalSize=768 (0x300)]
Programming... [TotalSize=1024 (0x400)]
...
...
...
..
.
BOOTIMG.BIN complete

61F:\sample.bin
SD...
Delete complete
Opening F:\sample.bin...
Input file opened
Writing Boot-image to device...
Programming... [TotalSize=256 (0x100)]
Programming... [TotalSize=512 (0x200)]
Programming... [TotalSize=768 (0x300)]
Programming... [TotalSize=1024 (0x400)]
...
...
...
..
.
BOOTIMG.BIN complete

711F:\sample.bin
SPI Serial Flash...
SPI Flash ID is: 0x0, 0x0, 0x0, 0x0
Erasing chip, this may take a while...
Chip erase done.
Writing data to device...
Opening F:\sample.bin...
Input file opened
Programming Complete

81F:\sample.bin
MCSPI Serial Flash...
Erasing chip, this may take a while...
Chip erase done.
Writing data to device...
Opening F:\sample.bin...
Input file opened
Programming Complete


(On C5515 EVM)

Choose the device...
1x - NAND Flash [CSx: 2,3,4,5]
2x - NOR Flash [CSx: 2,3,4,5]
3 - SPI EEPROM (16 bit address)
4 - IIC EEPROM
5 - MMC
6 - SD

>>> Here please enter:

141<file_path> - For NAND Flash write
221<file_path> - For NOR Flash write
311<file_path> - For SPI EEPROM write
41<file_path> - For IIC EEPROM write
51<file_path> - For MMC write
53 - For MMC File Listing
61<file_path> - For SD write
63 - For SD File Listing

!!!CAUTION!!!
SPI Flash and McSPI Flash related options are not available for C5515 EVM.
The C5515 EVM does not have these interfaces.
SPI EEPROM related options are not available for C5517 EVM.
The C5517 EVM does not have this interface.


Expected Results:

141F:\sample.bin
NAND Flash...
Writing data to NAND...
Writing Sector 0...
Writing Sector 1 (MBR)...
Flushing Data...
Writing Boot Record...
Flushing Data...
Opening F:\sample.bin...
Input file opened
Writing Boot-image to NAND...
Programming... [TotalSize=256 (0x100)]
Programming... [TotalSize=512 (0x200)]
Programming... [TotalSize=768 (0x300)]
Programming... [TotalSize=1024 (0x400)]
...
...
...
...
Programming... [TotalSize=12800 (0x3200)]
Programming... [TotalSize=12936 (0x3288)]
Flushing Data...
done

221F:\sample.bin
NOR Flash...
Erasing chip (NOR)...
Writing data to device...
Opening F:\sample.bin...
Input file opened
Programming Complete

311D:\sample.bin
SPI EEPROM...
Writing data to device...
Opening D:\sample.bin...
Input file opened
Programming Complete

41F:\sample.bin
IIC EEPROM...
Writing data to device...
Opening F:\sample.bin...
Input file opened
Programming Complete

51F:\sample.bin
MMC...
Delete complete
Opening F:\sample.bin...
Input file opened
Writing Boot-image to device...
Programming... [TotalSize=256 (0x100)]
Programming... [TotalSize=512 (0x200)]
Programming... [TotalSize=768 (0x300)]
Programming... [TotalSize=1024 (0x400)]
...
...
...
..
.
BOOTIMG.BIN complete

61F:\sample.bin
SD...
Delete complete
Opening F:\sample.bin...
Input file opened
Writing Boot-image to device...
Programming... [TotalSize=256 (0x100)]
Programming... [TotalSize=512 (0x200)]
Programming... [TotalSize=768 (0x300)]
Programming... [TotalSize=1024 (0x400)]
...
...
...
..
.
BOOTIMG.BIN complete

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

NOTE: The input file must be a C55x boot image file!!!
You can generate the C55x boot image files from the CCS *.out files using the hex55 or C55BootImage.exe

NOTE: You will need to set the system PLL to 12.288Mhz.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


 
The attached utility is intended to run on C5517 EVM, C5515 EVM or VC5505EVM using an JTAG emulator with CCS.
Change the compiler option in src\programmer.c "#define C5517_EVM", "#define C5515_EVM" or "#define VC5505_EVM" for 
desired platform. The default setting is for C5517 EVM.

To make sure the utility works properly for all peripherals, you will need to set the system PLL to 12.288Mhz.

It uses stdout and stdin to allow the user to place the contents of a PC file (generally a boot-image) 
onto a peripheral device on the C5517 EVM, VC5505 EVM or C5515 EVM.  It supports NOR, NAND, SPI EEPROM, SPI Flash McSPI Flash and I2C EEPROM,
as well as MMC and SD if the card is properly formatted (recommend to use the HPUSBFW.EXE from HP).  

We used this utility extensively for setting up bootloader tests for QuickTurn and real silicon, with EVMs.
 
The utility guides the user on the stdout display in CCS, and requests user input with a small popup window in CCS.  
Just load the program using CCS and run it.  You can then either follow these directions from the utility, or you 
can type in all of the answers to all questions at the first prompt as a shortcut.

Following are some examples, assuming that you want to load a file named "C:\c5505\bootimage.bin"...
 
For writing to SPI EEPROM on an EVM, at the first prompt enter...
    311c:\c5505\bootimage.bin                        [notice there are no spaces]
 
For writing to I2C EEPROM, at the first prompt enter...
    41c:\c5505\bootimage.bin
 
 
Some devices take a while to complete writing all data. Always wait for an error message or "Programming Complete" 
message. To run the utility again, you will need to use "Restart" (or reload).
 
