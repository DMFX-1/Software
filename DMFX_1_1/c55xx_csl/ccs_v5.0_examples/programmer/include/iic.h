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
*    iic.h
**********************************************************************/
#ifndef IIC_H
#define IIC_H

/**
 * @param address     - The address of the targetted slave device
 * @param iv          - The DSP clock divider
 * @param clk_low     - The clock divider for the clock low period
 * @param clk_high    - The clock divider for the clock high period
 *
 * @return void
 *
 * \par
 * This function initializes the IIC module to communicate to a particular
 * IIC slave device.  It also sets the module clock and clock high/low
 * periods if needed for a specific device.  The module clock should be
 * set to 12 Mhz.  Using the following formula
 * \f[
 *      12 Mhz = \frac{System Clock}{iv + 1}
 * \f]
 *
 * \par
 * The module clock is then divided by clk_low and clk_high to achieve the
 * desired frequency X
 * \f[
 *      X = \frac{12 Mhz}{clk low + clk high + 2}
 * \f]
 *
 * \par
 */
void IIC_INIT(unsigned int address, unsigned int iv, unsigned int clk_low,
                unsigned int clk_high);

/**
 * @param count       - The number of bytes to be read from device
 * @param buffer      - A pointer to an unpacked buffer where the data will read
 *
 * @return  The value of the first byte of data read in the lower byte of the
 *          return register.  In the upper byte, it returns the read status.
 *          For the upper byte, the status codes are as follows:
 *              <ul>
 *              <li>0 - Success
 *              <li>1 - Failure
 *              <li>2 - Zero read
 *              </ul>
 * \par
 * This function reads values from the IIC bus.  It assumes that the device
 * has already been issued a command to return a value.
 * \verbatim
       16          8 7          0
       +-------------------------+
       |   Status   | First Byte |
       +-------------------------+
   \endverbatim
 */
unsigned int IIC_READ(unsigned int count, unsigned int * pBuffer);

/**
 * @param stop        - Whether or not to issue the stop condition after write
 * @param address     - The address to write to the bus
 * @param count       - The number of bytes to write from the buffer
 * @param buffer      - Pointer to the unpacked buffer to write data from.
 *
 * @return Returns an error code:
 *          <ul>
 *          <li>0 - Success
 *          <li>1 - Failure
 *          <li>2 - Success with no stop
 *          </ul>
 * \par
 * This function writes data onto the IIC bus.
 */
unsigned int IIC_WRITE(unsigned int stop, unsigned int address,
                int count, unsigned int * buffer);

#endif /* IIC_H */
