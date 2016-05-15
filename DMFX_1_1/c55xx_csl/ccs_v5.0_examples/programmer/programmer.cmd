MEMORY
{
	/* SARAM[0..31] = Byte Addresses 0x10000 -> 0x4FFFF */
	SARAM_NAND_WRITE_BUFFER (RW) : origin = 0010000h length = 020000h
	SARAM_STACK  	(RW) : origin = 0030000h length = 002000h
	SARAM_SYSMEM  	(RW) : origin = 0032000h length = 002000h
	SARAM  			(RW) : origin = 0034000h length = 01C000h
}

SECTIONS
{
	.nandwrite :   > SARAM_NAND_WRITE_BUFFER  ALIGN = 4
	.stack :       > SARAM_STACK     ALIGN = 4
	.sysstack :    > SARAM_STACK     ALIGN = 4
	.sysmem :      > SARAM_SYSMEM    ALIGN = 0x1000
	.bss :         > SARAM     ALIGN = 4
	.text:         > SARAM     ALIGN = 4
	.switch:       > SARAM     ALIGN = 4
	.cinit:        > SARAM     ALIGN = 4
	.const:        > SARAM     ALIGN = 4
	.cio:          > SARAM     ALIGN = 4
	vectors:       > SARAM     ALIGN = 256
	
	.iic:          > SARAM     ALIGN = 4
	.spi:          > SARAM     ALIGN = 4
}

_IIC_WRITE               = _vIIC_WRITE;
_IIC_INIT                = _vIIC_INIT;
_IIC_READ                = _vIIC_READ;
_SPI_INIT                = _vSPI_INIT;
_SPI_RESET               = _vSPI_RESET;

