#include <stdio.h>
#include <stdlib.h>

#include "headers/utils.h"
#include "headers/encoder.h"
#include "headers/decoder.h"

uint8_t Test(uint32_t x) {
	uint8_t i;
	uint32_t error_mask_arr[] = {0x00, 0x01,0x03,0x07, 0x0f};
	uint32_t error_mask;
	uint8_t error_status;

	GolayCW CodeWord1, CodeWord2;
	CodeWord1.CodeWord = x;
	CodeWord2.CodeWord = x;
	Encode(GOLAY_23, &CodeWord1);
	Encode(GOLAY_23, &CodeWord2);

	for(i = 1; i <= 3; i++) {
		for (error_mask = error_mask_arr[i]; error_mask<0x800000; error_mask=NextBitPermutation(error_mask)) {
			CodeWord1.CodeWord = CodeWord2.CodeWord ^ error_mask;
			error_status = Correction(GOLAY_23, &CodeWord1);
			if(CodeWord2.CodeWord ^ CodeWord1.CodeWord) {
        		printf ("Something went wrong. Status: %d\n", error_status);
        		PrintBinary(CodeWord2.CodeWord ^ CodeWord1.CodeWord);
				return ALGORITHM_IS_NOT_CORRECT;
			}
		}
	}
	return ALGORITHM_IS_CORRECT;
}

int main(int argc, char** argv) {
	uint16_t i;
	GolayCW cw;
	cw.CodeWord = 0;

	GolayCW encLookUp[4096];

  ComputeELT(GOLAY_24, encLookUp);
	/*for(i = 0x000; i<=0x0ff; i++) {
		Test(i);
	}
	printf("ok\n");*/

	cw.cw.data = 10;
	PrintBinary(cw.CodeWord);
	Encode(GOLAY_24, &cw);
	printf("The On-The-Fly Enconding:\n");
	PrintBinary(cw.CodeWord);
	printf("The LUT Enconding:\n");
	PrintBinary(encLookUp[cw.cw.data].CodeWord);
	uint8_t err = 0;
	PrintBinary(cw.CodeWord);
	cw.cw.data = 0x555^0xf;
	PrintBinary(cw.CodeWord);
	printf ("%d\n After Correction:\n", Correction(GOLAY_24, &cw));
	PrintBinary(cw.CodeWord);

	//getchar();

	PrintBinary(GneratorPoly);
	PrintBinary(0x555<<11);
	PrintBinary(GetSyndrome(0x1));

	//getchar();

	return (EXIT_SUCCESS);
}
