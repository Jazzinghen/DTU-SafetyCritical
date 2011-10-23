#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <stdint.h>
#include <stdio.h>

#define GneratorPoly                0xAE3 // Another alternative would be to use the codeword 0xC75

//modes
#define GOLAY_23                    0x00
#define GOLAY_24                    0x01
#define LT_ENABLED					        0x02

#define ELT_FILE_NAME				        ".ELT.lt"
#define DLT_FILE_NAME				        ".DLT.lt"

#define DECODE_NO_ERRORS            0
#define DECODE_PARITY_ERRORS        1
#define DECODE_FIXED                2
#define DECODE_SYNDROME_ERRORS      3

#define ALGORITHM_IS_CORRECT		0
#define ALGORITHM_IS_NOT_CORRECT	1

typedef union {
  uint32_t CodeWord;
  struct {
    uint32_t data   : 12;
    uint32_t check  : 11;
    uint32_t parity  : 1;
  } cw;
} GolayCW;

void PrintBinary (uint32_t CodeWord);

int32_t Weight (uint32_t CodeWord);
int32_t GetParity (uint32_t CodeWord);
uint32_t GetSyndrome (uint32_t CodeWord);

uint32_t RotL (uint32_t CodeWord, int32_t i);
uint32_t RotR (uint32_t CodeWord, int32_t i);

uint32_t NextBitPermutation(uint32_t word);

#endif // UTILS_H_INCLUDED
