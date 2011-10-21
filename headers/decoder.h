#ifndef DECODER_H_INCLUDED
#define DECODER_H_INCLUDED

#include <stdint.h>
#include "utils.h"

uint8_t Decode (uint8_t correct_mode, uint8_t *errors, GolayCW *codeWord);
uint8_t DecodeFile (char *src, char *dst, uint8_t mode);

uint8_t Correction (uint8_t parity_mode, GolayCW *codeWord);

uint8_t ComputeDLT (uint8_t mode, uint32_t * LookupTable);

#endif // DECODER_H_INCLUDED
