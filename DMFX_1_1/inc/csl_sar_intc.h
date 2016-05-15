/*
 * csl_sar_intc.h
 *
 *  Created on: Nov 28, 2015
 *      Author: Diego de la Cruz
 */

#ifndef CSL_SAR_INTC_H_
#define CSL_SAR_INTC_H_
#include <csl_sar.h>

#define SAR_BUG 		1		// potentiometers wrongly connected between GND and VDD_ANA
#define POTENTIOMETER 	1		// Up to 4 potentiometers

//---------Function prototypes---------//
// Set SAR ADC parameters
CSL_Status SAR_A2DSetParam (SAR_Handle hSar);
// Set SAR ADC channel and Start single conversion
CSL_Status SAR_SetCh_StrtCnv (SAR_Handle hSar, Uint16 chanNo);
/** Interrupt Service Routine */
void sarISR(void);
/* Inits SAR in Interrupt mode */
int  sar_pot_init(void);
int  sar_pot_deinit(void);
int	 sar_tune_init(void);
int  sar_tune_deinit(void);

#endif /* CSL_SAR_INTC_H_ */
