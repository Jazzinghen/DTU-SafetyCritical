#include <stdio.h>
#include <stdlib.h>

#include "headers/utils.h"
#include "headers/encoder.h"
#include "headers/decoder.h"

void Test(void) {

}

int main(int argc, char** argv) {
	uint8_t i;
	
	uint32_t v = 3; // current permutation of bits 
	for ( i = 0; i < 100; i++) {
		PrintBinary(v);
		v = NextBitPermutation(v);
	}
	
/*	for (i = 0; i<32; i++) {
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
