#ifndef DECODER_H_INCLUDED
#define DECODER_H_INCLUDED

#include "utils.h"

uint8_t Decode (uint8_t correct_mode, uint8_t *errors, GolayCW *codeWord);

uint8_t Correction  (GolayCW codeWord);

#endif // DECODER_H_INCLUDED
