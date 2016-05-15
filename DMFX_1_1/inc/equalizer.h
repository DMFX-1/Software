/*
 * equalizer.h
 *
 *  Created on: 20/03/2016
 *  Author: Diego de la Cruz
 */

#ifndef EQUALIZER_H_
#define EQUALIZER_H_
#define BANDS 		3
#define EQ_STEPS	11

#if 0
signed short int equalizer(signed short int *input, signed short int *output);
#endif

CSL_Status equalizer(void);

#endif /* EQUALIZER_H_ */
