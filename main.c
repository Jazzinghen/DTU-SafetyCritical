#include <stdio.h>
#include <stdlib.h>

#include "headers/utils.h"
#include "headers/encoder.h"
#include "headers/decoder.h"

uint8_t Test(void) {
	uint8_t i;
	uint32_t error_mask_arr[] = {0x00, 0x01,0x03,0x07, 0x0f};
	uint32_t error_mask;

	GolayCW CodeWord1, CodeWord2;
	CodeWord1.CodeWord = 0;
	CodeWord2.CodeWord = 0;
	CodeWord1.cw.data = 0x555;
	CodeWord2.cw.data = 0x555;
	Encode(&CodeWord1);
	Encode(&CodeWord2);

	for(i = 1; i <= 3; i++) {
		for (error_mask = error_mask_arr[i]; error_mask<0x800000; error_mask=NextBitPermutation(error_mask)) {
			CodeWord1.CodeWord ^= error_mask;
			Correction(GOLAY_24, &CodeWord1);
			if(CodeWord2.CodeWord ^ CodeWord1.CodeWord) {
				return ALGORITHM_IS_NOT_CORRECT;
			}
		}
	}
	return ALGORITHM_IS_CORRECT;
}

int main(int argc, char** argv) {
	uint8_t i;
	GolayCW cw;

	printf("%d\n", Test());
getchar();

	cw.CodeWord = 0;
	cw.cw.data  = 0x555;
	PrintBinary(cw.CodeWord);
	Encode(&cw);
	PrintBinary(cw.CodeWord);
	PrintBinary(cw.cw.check);
	uint8_t err = 0;
	PrintBinary(cw.CodeWord);
	cw.cw.data = 0x555^0xf;
	PrintBinary(cw.CodeWord);
	printf ("%d\n After Correction:\n", Correction(GOLAY_24, &cw));
	PrintBinary(cw.CodeWord);

	getchar();
	Test();

	PrintBinary(GneratorPoly);
	PrintBinary(0x555<<11);
	PrintBinary(GetSyndrome(0x1));

	getchar();

	return EXIT_SUCCESS;
}
