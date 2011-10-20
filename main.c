#include <stdio.h>
#include <stdlib.h>

#include "headers/utils.h"
#include "headers/encoder.h"
#include "headers/decoder.h"

void Test(void) {
	uint8_t i;
	uint32_t error_mask_arr[] = {0x00, 0x01,0x03,0x07};
	
	uint32_t error_mask; 
	
	uint32_t CodeWord = 0x555;
	for(i = 1; i <= 2; i++) {
		for (error_mask = error_mask_arr[i]; error_mask<0x800000; error_mask=NextBitPermutation(error_mask)) {
			PrintBinary(error_mask);	
			//Decode(CodeWord ^ error_mask);
		}
	}
}

int main(int argc, char** argv) {
	uint8_t i;
	
	GolayCW cw;
	
	cw.CodeWord = 0x00;
	cw.cw.data = 0xfff;
	PrintBinary(cw.CodeWord);
	PrintBinary(cw.CodeWord);
	
	getchar();
	
	Test();
	
	/*for (i = 0; i<32; i++) {
    	PrintBinary(RotR(0x55,i));
	}
	for (i = 0; i<32; i++) {
    	PrintBinary(RotL(0x55,i));
	}*/

	PrintBinary(GneratorPoly);
	PrintBinary(0x555<<11);
	PrintBinary(GetSyndrome(0x1));

	getchar();

	return EXIT_SUCCESS;
}
