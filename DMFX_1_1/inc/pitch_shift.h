#ifndef PITCH_SHIFT_H_
#define PITCH_SHIFT_H_

#define PSBUF_SIZE  4096  // 85ms = 12Hz

signed short int pitch_shift(signed short int *input, signed short int *output);

#endif /* PITCH_SHIFT_H_*/
