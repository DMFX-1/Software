/*
 * tuner.h
 *
 *  Created on: Mar 5, 2016
 *      Author: Diego de la Cruz
 */

#ifndef TUNER_H_
#define TUNER_H_

#define TUNER_SIZE	1024
#define ACORR_MIN 	10
#define ACORR_MAX 	300
#define SCALE_SIZE	48
#define AC_FILT 	8000L

unsigned short int GetPeakPeriod(void);
unsigned short int Tuner (unsigned short int PeakPeriod);
void LedTuner(unsigned short int PeakPeriod);

#endif /* TUNER_H_ */
