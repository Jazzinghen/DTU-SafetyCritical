#ifndef ENCODER_H_INCLUDED
#define ENCODER_H_INCLUDED

#include <stdint.h>
#include "utils.h"

uint32_t Encode (uint8_t parity_mode, GolayCW *CodeWord);
uint32_t EncodeLT (uint8_t parity_mode, GolayCW *CodeWord);

uint32_t EncodeFile(char *src, char *dst, uint8_t mode);

uint8_t  ComputeELT(uint8_t mode);

#endif // ENCODER_H_INCLUDED
