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

//Compute the lexicographically next bit permutation
//from: Bit Twiddling Hacks
//http://www-graphics.stanford.edu/~seander/bithacks.html#NextBitPermutation
//ret: next permutation of bits
uint32_t NextBitPermutation(uint32_t word) {
	uint32_t t = (word | (word - 1)) + 1;
	return t | ((((t & -t) / (word & -word)) >> 1) - 1);
}

//Counting 1-bits, "divide and conquer" strategy.
//from boook: Hackers delight p. 95;
int32_t Weight(uint32_t CodeWord) {
	CodeWord = CodeWord - ((CodeWord >> 1) & 0x55555555);
	CodeWord = (CodeWord & 0x33333333) + ((CodeWord >> 2) & 0x33333333);
	CodeWord = (CodeWord + (CodeWord >> 4)) & 0x0F0F0F0F;
	CodeWord = CodeWord + (CodeWord >> 8);
	CodeWord = CodeWord + (CodeWord >> 16);

	return CodeWord & 0x0000003F;
}

//Computing the Parity of a Word
//from boook: Hackers delight p. 105;
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

#ifdef __unix__
struct timespec ClockDifference (struct timespec begin, struct timespec end) {
  struct timespec difference;

	if ((end.tv_nsec - begin.tv_nsec) < 0) {
		difference.tv_sec = end.tv_sec - begin.tv_sec - 1;
		difference.tv_nsec = 1000000000 + end.tv_nsec - begin.tv_nsec;
	} else {
		difference.tv_sec = end.tv_sec - begin.tv_sec;
		difference.tv_nsec = end.tv_nsec - begin.tv_nsec;
	}

	return difference;
}
#endif
