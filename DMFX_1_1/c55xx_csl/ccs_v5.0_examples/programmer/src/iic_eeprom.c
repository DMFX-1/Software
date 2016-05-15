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
*    iic_eeprom.c
**********************************************************************/
#include "sysctrl.h"
#include "iic.h"
#include "iic_eeprom.h"
#include "tipack.h"

/* Local Function Prototypes */
void iic_eeprom_waitloop(unsigned long count);


/*********************************************************************
* iic_eeprom_init
*********************************************************************/
int iic_eeprom_init()
{
   /* Reset I2C Module */
   PeripheralReset(PERIPH_RESET_I2C, 0x20, 100);
   /* Initialize I2C Module */
   //IIC_INIT(IIC_EEPROM_SLAVE_ADDR, 0, 9, 9);
   IIC_INIT(IIC_EEPROM_SLAVE_ADDR, 7, 0x26c, 0x26c);
   return(0);
}

/*********************************************************************
* iic_eeprom_read_n_words
*********************************************************************/
UInt16 iic_eeprom_read_n_words(UInt32 address, UInt16 *buffer, UInt16 count)
{
   UInt16 offset = (UInt16)address*2;
   UInt16 buffer2[24];                   /* Need a buffer twice as large to
                                             handle unpacked bytes */
   UInt16 count2;
   UInt16 ii;
#ifdef DISABLE_TI_PACK
   UInt16 jj;
#endif
   UInt16 ret_stat;

   for (ii=0; ii<count; ii+= count2)
   {
      count2 = (count-ii>12) ? 12 : count-ii;
      ret_stat = IIC_WRITE(0, offset+(ii*2), 0, NULL);
      if (ret_stat)
      {
         return(1);
      }
      ret_stat = IIC_READ(count2*2, (unsigned int *)&buffer2[0]);
      if (ret_stat > 0xFF)
      {
         return(1);
      }
#ifdef DISABLE_TI_PACK
      for (jj=0; jj<count2; jj++)
      {
         buffer[ii+jj] = buffer2[jj*2] << 8;
         buffer[ii+jj] |= buffer2[jj*2+1];
      }
#else
      TI_pack(buffer, (unsigned char *)buffer2, count2*2);
#endif
   }

   return(0);
}

/*********************************************************************
* iic_eeprom_write_n_words
*********************************************************************/
UInt16 iic_eeprom_write_n_words(UInt32 address, UInt16 *buffer, UInt16 count)
{
   UInt16 offset = (UInt16)(address<<1);
   UInt16 buffer2[2];
   UInt16 buffer3[2];
   UInt16 ii;
   UInt16 ret_stat;

   for (ii=0; ii<count; ii++, offset+=2)
   {
      buffer2[0] = buffer[ii] >> 8;
      buffer2[1] = buffer[ii] & 0xFF;
      ret_stat = IIC_WRITE(1, offset, 2, (unsigned int *)&buffer2[0]);
      if (ret_stat)
      {
         return(1);
      }
      iic_eeprom_waitloop(0x1FFF);
      ret_stat = IIC_WRITE(0, offset, 0, NULL);
      if (ret_stat)
      {
         return(1);
      }
      ret_stat = IIC_READ(2, (unsigned int *)&buffer3[0]);
      if ((ret_stat > 0xFF) || (buffer2[0] != buffer3[0]) ||
          (buffer2[1] != buffer3[1]))
      {
         return(1);
      }
   }

   return(0);
}

/*********************************************************************
* iic_eeprom_waitloop
*********************************************************************/
void iic_eeprom_waitloop(unsigned long count)
{
   volatile unsigned long counter;

   for (counter=0; counter<count; counter++)
   {
      asm("\tnop");
   }
}
