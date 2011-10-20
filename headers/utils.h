#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <stdint.h>

#define GneratorPoly                0xAE3 // Another alternative would be to use the codeword 0xC75
#define DETECT_ERRORS               0
#define CORRECT_ERRORS              1

#define DECODE_NO_ERRORS            0
#define DECODE_PARITY_ERRORS        1
#define DECODE_SYNDROME_ERRORS      2

typedef union {
  uint32_t CodeWord;
  struct {
    uint8_t parity  : 1;
    uint16_t data   : 12;
    uint16_t check  : 12;
  } cw;
} GolayCW;

void PrintBinary(uint32_t CodeWord);

int32_t Weight (uint32_t CodeWord);
int32_t GetParity (uint32_t CodeWord);
uint32_t GetSyndrome (uint32_t CodeWord);

uint32_t RotL (uint32_t CodeWord, int32_t i);
uint32_t RotR (uint32_t CodeWord, int32_t i);

uint32_t NextBitPermutation(uint32_t word);

#endif // UTILS_H_INCLUDED
