#include <stdio.h>

#include "headers/utils.h"

/*!\brief	This is the function used to print 
 * 			CodeWord in binary representation
 *
 * \param	CodeWord:	 value to print
 * 
 */
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

/*!\brief	This is the function used to compute 
 *			the lexicographically next bit permutation
 *
 * \param	word:	previous permutation of bits
 * 
 * \retval			next permutation of bits
 * \note	FROM:	Bit Twiddling Hacks
 * 			URL:	http://www-graphics.stanford.edu/~seander/bithacks.html
 */
uint32_t NextBitPermutation(uint32_t word) {
	uint32_t t = (word | (word - 1)) + 1;
	return t | ((((t & -t) / (word & -word)) >> 1) - 1);
}

/*!\brief	This is the function used to count 1-bits in a word
 *			by "devide and conquer" strategy.
 *
 * \param	CodeWord: a word
 * 
 * \retval			number of 1-bits
 * \note	FROM:	Hackers delight p. 95;
 */
int32_t Weight(uint32_t CodeWord) {
	CodeWord = CodeWord - ((CodeWord >> 1) & 0x55555555);
	CodeWord = (CodeWord & 0x33333333) + ((CodeWord >> 2) & 0x33333333);
	CodeWord = (CodeWord + (CodeWord >> 4)) & 0x0F0F0F0F;
	CodeWord = CodeWord + (CodeWord >> 8);
	CodeWord = CodeWord + (CodeWord >> 16);

	return CodeWord & 0x0000003F;
}

/*!\brief	This is the function used to compute the parity of a word
 *			by "devide and conquer" strategy.
 *
 * \param	CodeWord: a word
 * 
 * \retval			parity bit
 * \note	FROM:	Hackers delight p. 105;
 */
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

/*!\brief	This is the function used to rotate with carry 
 *          23 bit long word by n bits to the left
 *			
 * \param	CodeWord: a word to rotate
 * \param	i:		  number of rotation
 * \retval			  result of rotation
 */
uint32_t RotL(uint32_t CodeWord, int32_t i) {
	i%=23;
	CodeWord = (CodeWord << i) | (CodeWord >> 23-i);
	return (CodeWord & 0x7fffff);
}

/*!\brief	This is the function used to rotate with carry 
 *          23 bit long word by n bits to the right
 *			
 * \param	CodeWord: a word to rotate
 * \param	i:		  number of rotation
 * \retval			  result of rotation
 */
uint32_t RotR(uint32_t CodeWord, int32_t i) {
	i%=23;
	CodeWord = (CodeWord >> i) | (CodeWord << (23-i));
	return (CodeWord & 0x7fffff);
}

/*!\brief	This is the function used to rotate with carry 
 *          23 bit long word by n bits to the right
 *			
 * \param	src:		path to the file.
 * \param	mode:		'power' of added noise to the file 
 * 						0: about 0.5  bit error per 1 byte
 *						1: about 1    bit error per 1 byte
 *						2: about 1.4 bit error per 1 byte
 *						3: about 1.5  bit error per 1 byte 
 * \retval				number of injected errors
 */
int32_t InjectErrorsFile (char *src, uint8_t mode) {
	uint8_t err_mask[4] = {0x10, 0x01, 0x18, 0x11};
	uint8_t error_mask; 
	uint8_t src_data;

	/* open file to read */	
	FILE *fp_s = fopen(src, "rb+");
	int32_t ret = 0;

	/* Upon not successful open return -1 */
	if(!fp_s || mode > 3) {
		return -1;
	}
	
	/* Read file byte by byte */
	while(fread(&src_data, 1, 1, fp_s)) {
		fseek ( fp_s , -1 , SEEK_CUR );
		error_mask = err_mask[mode]<<(rand()%8);
		/* add to the readed value some random noise */
		fputc(src_data ^ error_mask, fp_s);
		ret+=Weight(error_mask);
		fseek ( fp_s ,  0 , SEEK_CUR );
		memset(&src_data, 0, sizeof(src_data));
	}
	/* Close the file */
	fclose(fp_s);
	return ret;
}
