/*
 * DMFX2.h
 *
 *  Created on: Dec 12, 2015
 *      Author: Diego de la Cruz
 */

#ifndef DMFX2_H_
#define DMFX2_H_

#define DMFX_PROC2
#undef 	DMFX_PROC1

#if defined(DMFX_PROC1)
	#define USE_SD0		// Serial Port 0 used for MMC/SD0 interface
	#define USE_SD1		// Serial Port 1 used for MMC/SD1 interface
#endif

#define SET_CLEAN 	  GPIO_write(hGpio, CSL_GPIO_PIN0, 1)
#define SET_OVD_DST   GPIO_write(hGpio, CSL_GPIO_PIN0, 0)
#define CLR_DRV0_16dB GPIO_write(hGpio, CSL_GPIO_PIN1, 0)
#define SET_DRV0_16dB GPIO_write(hGpio, CSL_GPIO_PIN1, 1)
#define CLR_DRV1_8dB  GPIO_write(hGpio, CSL_GPIO_PIN2, 0)
#define SET_DRV1_8dB  GPIO_write(hGpio, CSL_GPIO_PIN2, 1)
#define CLR_MD0_MIDS  GPIO_write(hGpio, CSL_GPIO_PIN3, 0)
#define SET_MD0_MIDS  GPIO_write(hGpio, CSL_GPIO_PIN3, 1)
#define CLR_MD1_CLIP  GPIO_write(hGpio, CSL_GPIO_PIN4, 0)
#define SET_MD1_CLIP  GPIO_write(hGpio, CSL_GPIO_PIN4, 1)
#define UNMUTE 		  GPIO_write(hGpio, CSL_GPIO_PIN5, 0)
#define MUTE   		  GPIO_write(hGpio, CSL_GPIO_PIN5, 1)
#define SET_LED1	  GPIO_write(hGpio, CSL_GPIO_PIN6, 0)
#define SET_LED2	  GPIO_write(hGpio, CSL_GPIO_PIN7, 0)
#define SET_LED3	  GPIO_write(hGpio, CSL_GPIO_PIN8, 0)
#define SET_LED4	  GPIO_write(hGpio, CSL_GPIO_PIN9, 0)
#define SET_LED5	  GPIO_write(hGpio, CSL_GPIO_PIN10, 0)
#define CLR_LED1	  GPIO_write(hGpio, CSL_GPIO_PIN6, 1)
#define CLR_LED2	  GPIO_write(hGpio, CSL_GPIO_PIN7, 1)
#define CLR_LED3	  GPIO_write(hGpio, CSL_GPIO_PIN8, 1)
#define CLR_LED4	  GPIO_write(hGpio, CSL_GPIO_PIN9, 1)
#define CLR_LED5	  GPIO_write(hGpio, CSL_GPIO_PIN10, 1)

#endif /* DMFX2_H_ */
