#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <stdint.h>

#define GneratorPoly        0xAE3 // Another alternative would be to use the codeword 0xC75

void PrintBinary(uint32_t CodeWord);

int32_t Weight (uint32_t CodeWord);
int32_t GetParity (uint32_t CodeWord);
uint32_t GetSyndrome (uint32_t CodeWord);

uint32_t RotL (uint32_t CodeWord, int32_t i);
uint32_t RotR (uint32_t CodeWord, int32_t i);

#endif // UTILS_H_INCLUDED