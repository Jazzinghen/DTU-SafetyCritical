#ifndef DECODER_H_INCLUDED
#define DECODER_H_INCLUDED

#include "utils.h"

uint8_t ErrorCheck (uint8_t parity_mode, GolayCW *codeWord);

uint8_t Correction (uint8_t parity_mode, GolayCW *codeWord);

#endif // DECODER_H_INCLUDED
