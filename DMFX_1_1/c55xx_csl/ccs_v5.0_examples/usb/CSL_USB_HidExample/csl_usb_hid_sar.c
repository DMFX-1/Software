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

#include "csl_sar.h"
#include "csl_general.h"
#include "stdio.h"
#include "csl_usb_hid_sar.h"

Uint16 preKey = NoKey_MIN_VALUE;
Uint16 keyCnt =0;

/* SAR object Structure    */
CSL_SarHandleObj SarObj;
CSL_SarHandleObj *SarHandle;

CSL_Status SAR_initInstance(void)
{
    CSL_Status       status;
    CSL_SarChSetup   param;
    int              result;
    int              chanNo;

    result = 0;

    /* Initialize the SAR module */
    status = SAR_init();
    if(status != CSL_SOK)
    {
        printf("SAR Init Failed!!\n");
        return (result);
    }

    /* Open SAR channel */
    status = SAR_chanOpen(&SarObj,CSL_SAR_CHAN_3);
    SarHandle = &SarObj;
    if(status != CSL_SOK)
    {
        printf("SAR_chanOpen Failed!!\n");
        return (result);
    }

    /* Initialize channel */
    status = SAR_chanInit(SarHandle);
    if(status != CSL_SOK)
    {
        printf("SAR_chanInit Failed!!\n");
        return (result);
    }

    param.OpMode = CSL_SAR_POLLING;
    param.MultiCh = CSL_SAR_NO_DISCHARGE;
    param.RefVoltage = CSL_SAR_REF_VIN;
    param.SysClkDiv = 0x0b ;
    /* Configuration for SAR module */
    status = SAR_chanSetup(SarHandle,&param);
    if(status != CSL_SOK)
    {
        printf("SAR_chanConfig Failed!!\n");
        return (result);
    }

    /* Set channel cycle set */
    status = SAR_chanCycSet(SarHandle,CSL_SAR_CONTINUOUS_CONVERSION);
    if(status != CSL_SOK)
    {
        printf("SAR_chanCycSet Failed!!\n");
        return (result);
    }

    /* set ADC Measurement parameters */
    status = SAR_A2DMeasParamSet(SarHandle,CSL_KEYPAD_MEAS,&chanNo);
    if(status != CSL_SOK)
    {
        printf("SAR_A2DMeasParamSet Failed!!\n");
    }

    return (result);
}

CSL_Status SAR_startReadingKeys()
{
    CSL_Status status;

    /* start the conversion */
    status = SAR_startConversion(SarHandle);
    if(status != CSL_SOK)
    {
        printf("SAR_startConversion Failed!!\n");
    }

    return (status);
}

CSL_Status SAR_stopReadingKeys()
{
    CSL_Status status;

    /* stop the conversion */
    status = SAR_stopConversion(SarHandle);
    if(status != CSL_SOK)
    {
        printf("SAR_stopConversion Failed!!\n");
    }

    return (status);
}

CSL_Status SAR_closeInstance(void)
{
    CSL_Status status;

    /* Stop the conversion */
    status = SAR_stopConversion(SarHandle);
    if(status != CSL_SOK)
    {
        printf("SAR_stopConversion Failed!!\n");
        return (status);
    }

    /* Close the channel */
    status = SAR_chanClose(SarHandle);
    if(status != CSL_SOK)
    {
        printf("SAR_chanClose Failed!!\n");
        return (status);
    }
    /* Deinit */
    status = SAR_deInit();
    if(status != CSL_SOK)
    {
        printf("SAR_deInit Failed!!\n");
    }

    return (status);
}

static Uint16 getKeyName(Uint16 key)
{
	Uint16 keyName;

    if ((SW7_MIN_VALUE <= key) && (key <= SW7_MAX_VALUE)) // UP
    {
		keyName = (Uint16)UP_KEY;
	}
    else if ((SW14_MIN_VALUE <= key) && (key <= SW14_MAX_VALUE))  // Down
	{
		keyName = (Uint16)DOWN_KEY;
	}
    else if ((SW10_MIN_VALUE <= key) && (key <= SW10_MAX_VALUE))  // Left
	{
		keyName = (Uint16)LEFT_KEY;
	}
    else if ((SW12_MIN_VALUE <= key) && (key <= SW12_MAX_VALUE))  // Right
	{
		keyName = (Uint16)RIGHT_KEY;
	}
    else if ((SW6_MIN_VALUE <= key) && (key <= SW6_MAX_VALUE))    // Mouse 1
	{
		keyName = (Uint16)LEFT_CLICK_KEY;
	}
    else if ((SW8_MIN_VALUE <= key) && (key <= SW8_MAX_VALUE))    // Mouse 2
	{
		keyName = (Uint16)RIGHT_CLICK_KEY;
	}
	else if(SW13_VALUE == key)  // End mouse session
	{
		keyName = (Uint16)STOP_KEY;
	}
	else
	{
		keyName = (Uint16)NO_KEY;
	}

    return (keyName);
}


Uint16 SAR_GetKey(void)
{
    Uint16      i;
    Uint16      looper;
    CSL_Status  status;
    Uint16      readBuffer[NO_OF_ATTEMPTS];
	Uint16      keyPressed[NO_OF_ATTEMPTS];

    keyCnt++;
    if(keyCnt > 90)
    {
        keyCnt = 0;
        preKey = NO_KEY;
    }

    SAR_startReadingKeys();

    while(1)
    {
		for(looper = 0; looper < NO_OF_ATTEMPTS; looper++)
		{
			for(i=0;i<500; i++)
				asm("	nop");

			/* Check whether the ADC data is available or not */
			while(CSL_SAR_DATA_AVAILABLE !=
				SAR_getStatus(SarHandle,&status));

			status = SAR_readData(SarHandle, &readBuffer[looper]);
			if(status != CSL_SOK)
			{
				printf("SAR_readData Failed!!\n");
				return (ERROR_STATUS);
			}
		}

		for (looper = 0; looper < NO_OF_ATTEMPTS; looper++)
		{
			if((readBuffer[looper] & 0x8000) == 0)
			{
				keyPressed[looper] = getKeyName(readBuffer[looper]);
			}
			else
			{
				break; /* Start reading the key values again */
			}
		}

		if (looper == NO_OF_ATTEMPTS)
		{
			for (looper = 1; looper < NO_OF_ATTEMPTS; looper++)
			{
				if (keyPressed[0] != keyPressed[looper])
				{
					break;
				}
			}

			if (looper == NO_OF_ATTEMPTS)
			{
				break; /* Stop Reading the keys and send the respective cmd */
			}
		}
    }

    SAR_stopReadingKeys();

	if (keyPressed[0] == RIGHT_CLICK_KEY)
	{
		preKey = keyPressed[0];
	}

    if((keyPressed[0] == UP_KEY)          ||
       (keyPressed[0] == DOWN_KEY)        ||
       (keyPressed[0] == RIGHT_KEY)       ||
       (keyPressed[0] == LEFT_KEY)        ||
       (keyPressed[0] == RIGHT_CLICK_KEY) ||
       (keyPressed[0] == LEFT_CLICK_KEY)  ||
       (keyPressed[0] == STOP_KEY))
    {
        if(keyPressed[0] != preKey)
        {
            preKey = keyPressed[0];
        }
    }

	return (keyPressed[0]);

}

