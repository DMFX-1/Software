#ifndef VIBRATO_H_
#define VIBRATO_H_

#define SINE		1
#define NOISE		2
#define TRIANGLE	3
#define SAWTOOTH	4

#define	MONO 	1
#define	STEREO 	2
#define	LESLIE 	3

#define ISCH	1	//Industry Standard Chorus
#define WCH		2	//White Chorus

signed short int variabledelay(signed short int *input, signed short int *output);

signed short int *delayline(signed short int x, signed short int *xh_k);

signed short int *lfo(unsigned long int freq, signed short int type, signed short int *lfo_out);

void lp_noise(signed short int *lpn, unsigned long int freq);

#endif /*VIBRATO_H_*/
