#ifndef ENCODER_H_INCLUDED
#define ENCODER_H_INCLUDED

#include <stdint.h>
#include "utils.h"

void Encode (uint8_t parity_mode, GolayCW *CodeWord);
void EncodeLT (uint8_t parity_mode, GolayCW *CodeWord, GolayCW * LookupTable);

uint32_t EncodeFile(char *src, char *dst, uint8_t mode);

size_t  ComputeELT(uint8_t mode, GolayCW * LookupTable);

#endif // ENCODER_H_INCLUDED
