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

#include "nor.h"

/** ============================================================================
 *   @n@b NOR_unlockBlocks
 *
 *   @b Description
 *   @n This API unlocks the blocks specified.
 *
 *   @b Arguments
 *   @verbatim
            hEmif        Handle to the emif object
            startAddr    Starting address of the block to be unlocked
			blockLen     Length of each block
            blockCount   Number of blocks to be unlocked
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK            - NOR unlock is successful
 *   @li                    CSL_ESYS_BADHANDLE - Invalid handle
 *   @li                    CSL_ESYS_INVPARAMS - Invalid config structure
 *
 *   <b> Pre Condition </b>
 *   @n  Emif init and Asynchronous configurations should be called successfully
 *
 *   <b> Post Condition </b>
 *   @n  Unlocks specified blocks of NOR memory
 *
 *   @b Modifies
 *   @n Lock status of the memory blocks
 *
 *   @b Example
 *   @verbatim
            CSL_EmifObj            emifObj;
            CSL_Status             status;
            CSL_EmifConfig         emifCfg;
            CSL_EmifHandle         hEmif;

            status = EMIF_init(&emifObj);
            hEmif  = &emifObj;
            status = EMIF_asyncConfig(hEmif, &sdramConfig, &sdramTimr);
            ...
            ...
            ...
			status = NOR_unlockBlocks(hEmif, startAddr, blockLen, blockCount);
     @endverbatim
 *  ============================================================================
 */
CSL_Status NOR_unlockBlocks(CSL_EmifHandle  hEmif,
                            Uint32          startAddr,
                            Uint32          blockLen,
                            Uint32          blockCount)
{
    Uint16 *flashAddr;
	Uint32          count;
    CSL_Status      result;

	result = CSL_SOK;

    if(hEmif != NULL)
    {
		flashAddr = (Uint16*)startAddr;

	    for (count = 0; count < blockCount; count++)
	    {
            NOR_sendCommand(hEmif, (Uint32)flashAddr, CSL_NOR_CONFIG_BLOCK_LOCK_BITS_CMD);
            NOR_sendCommand(hEmif, (Uint32)flashAddr, CSL_NOR_CLEAR_LOCK_BITS_CMD);
            NOR_sendCommand(hEmif, (Uint32)flashAddr, CSL_NOR_READ_ARRAY_CMD);

			flashAddr += blockLen;
	    }
    }
    else
    {
		result = CSL_ESYS_BADHANDLE;
	}

    return (result);
}

/** ============================================================================
 *   @n@b NOR_eraseBlock
 *
 *   @b Description
 *   @n This API erases the contents of blocks specified.
 *
 *   @b Arguments
 *   @verbatim
            hEmif        Handle to the emif object
            startAddr    Starting address of the block to be erased
			blockLen     Length of each block
            blockCount   Number of blocks to be erased
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK            - NOR erase is successful
 *   @li                    CSL_ESYS_BADHANDLE - Invalid handle
 *   @li                    CSL_ESYS_FAIL      - In case of erase command failure
 *
 *   <b> Pre Condition </b>
 *   @n  Emif init and Asynchronous configurations should be called successfully
 *
 *   <b> Post Condition </b>
 *   @n  Erases data in the specified blocks of NOR memory
 *
 *   @b Modifies
 *   @n Data of the specified memory blocks
 *
 *   @b Example
 *   @verbatim
            CSL_EmifObj            emifObj;
            CSL_Status             status;
            CSL_EmifConfig         emifCfg;
            CSL_EmifHandle         hEmif;

            status = EMIF_init(&emifObj);
            hEmif  = &emifObj;
            status = EMIF_asyncConfig(hEmif, &sdramConfig, &sdramTimr);
            ...
            ...
            ...
			status = NOR_eraseBlock(hEmif, startAddr, blockLen, blockCount);
     @endverbatim
 *  ============================================================================
 */
CSL_Status NOR_eraseBlock(CSL_EmifHandle      hEmif,
                         Uint32              norAddr,
                         Uint32              blockLen,
                         Uint16              blockCount)
{
	Uint16          *flashAddr;
    CSL_Status      result;
	Uint16          index;

	result = CSL_SOK;

    if(hEmif != NULL)
    {
		/* Erase the flash blocks */
	    for (index = 0; index < blockCount; index++)
	    {
	        flashAddr  = (Uint16*)CSL_EMIF_NOR_BASE_ADDR + norAddr;
			NOR_unlockBlocks(hEmif, (Uint32)flashAddr, blockLen, 1);
			/* Send erase command to the block address */
            NOR_sendCommand(hEmif, (Uint32)flashAddr, CSL_NOR_READ_ARRAY_CMD);
            NOR_sendCommand(hEmif, (Uint32)flashAddr, CSL_NOR_BLOCK_ERASE_CMD);
            NOR_sendCommand(hEmif, (Uint32)flashAddr, CSL_NOR_ERASE_PROGRAM_RESUME_CMD);

	        /* Poll for program completion */
	        if (NOR_checkStatus(CSL_NOR_ERASE_TIMEOUT))
	        {
	            *flashAddr = CSL_NOR_READ_ARRAY_CMD;
	            result     = CSL_ESYS_FAIL;
				break;
	        }
	        /* Update NOR address to next block */
			flashAddr += blockLen;
	    }
        NOR_sendCommand(hEmif, (Uint32)flashAddr, CSL_NOR_READ_ARRAY_CMD);
    }
    else
    {
		result = CSL_ESYS_BADHANDLE;
	}
    return (result);
}


