#include <stdio.h>

#include "headers/utils.h"

void PrintBinary(uint32_t CodeWord) {
	int32_t i;
	for(i = 31; i >= 0; i--) {
		printf("%d", (CodeWord>>i)&0x01);
		if(!(i%8)) {
			printf(" ");
		}
	}
	puts("");
}

uint32_t NextBitPermutation(uint32_t word) {
	uint32_t t = (word | (word - 1)) + 1;
	return t | ((((t & -t) / (word & -word)) >> 1) - 1);
}

int32_t Weight(uint32_t CodeWord) {
	CodeWord = CodeWord - ((CodeWord >> 1) & 0x55555555);
	CodeWord = (CodeWord & 0x33333333) + ((CodeWord >> 2) & 0x33333333);
	CodeWord = (CodeWord + (CodeWord >> 4)) & 0x0F0F0F0F;
	CodeWord = CodeWord + (CodeWord >> 8);
	CodeWord = CodeWord + (CodeWord >> 16);

	return CodeWord & 0x0000003F;
}

int32_t GetParity(uint32_t CodeWord) {
	CodeWord = CodeWord ^ (CodeWord >> 1);
	CodeWord = (CodeWord ^ (CodeWord >> 2)) & 0x11111111;
	CodeWord = CodeWord*0x11111111;
	return((CodeWord >> 28) & 1);
}

uint32_t GetSyndrome(uint32_t CodeWord) {
	int32_t i;
	for (i = 1; i<=12; i++, CodeWord>>=1) {
		if(CodeWord & 1) {
			CodeWord ^= GneratorPoly;
		}
	}
	return(CodeWord);
}

uint32_t RotL(uint32_t CodeWord, int32_t i) {
	i%=23;
	CodeWord = (CodeWord << i) | (CodeWord >> 23-i);
	return (CodeWord & 0x7fffff);
}

uint32_t RotR(uint32_t CodeWord, int32_t i) {
	i%=23;
	CodeWord = (CodeWord >> i) | (CodeWord << (23-i));
	return (CodeWord & 0x7fffff);
}
