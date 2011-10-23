#ifndef DECODER_H_INCLUDED
#define DECODER_H_INCLUDED

#include <stdint.h>
#include "utils.h"

uint8_t DecodeFile (char *src, char *dst, uint8_t mode);

void DecodeLT (uint8_t mode, GolayCW *CodeWord, uint32_t *LookupTable);

uint8_t Correction (uint8_t parity_mode, GolayCW *codeWord);

size_t ComputeDLT (uint8_t message, uint32_t * LookupTable);

#endif // DECODER_H_INCLUDED
