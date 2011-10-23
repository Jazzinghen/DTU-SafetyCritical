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

int32_t InjectErrorsFile (char *src, uint8_t mode) {
	FILE *fp_s = fopen(src, "rb+");
	int32_t ret = 0;

	if(!fp_s || mode > 3) {
		return -1;
	}
	
	uint8_t err_mask[4] = {0x10, 0x01, 0x88, 0x11};
	uint8_t error_mask; 
	uint8_t src_data;
	while(fread(&src_data, 1, 1, fp_s)) {
		fseek ( fp_s , -1 , SEEK_CUR );
		error_mask = err_mask[mode]<<(rand()%8);
		fputc(src_data ^ error_mask, fp_s);
		ret+=Weight(error_mask);
		fseek ( fp_s ,  0 , SEEK_CUR );
		memset(&src_data, 0, sizeof(src_data));
	}
	
	fclose(fp_s);
	return ret;
}
