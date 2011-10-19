#include <stdio.h>
#include <stdlib.h>

#include "headers/utils.h"
#include "headers/encoder.h"
#include "headers/decoder.h"

void Test(void) {

}

int main(int argc, char** argv) {
	uint8_t i;
	
	for (i = 0; i<32; i++) {
    	PrintBinary(RotR(0x55,i));
	}	
	for (i = 0; i<32; i++) {
    	PrintBinary(RotL(0x55,i));
	}
	
    printf("%d\n", GetParity(0));
    printf("%d\n", GetParity(0x1));
    printf("%d\n", GetParity(0x2));
    printf("%d\n", GetParity(0x3));
    printf("%d\n", GetParity(0xf4));
    printf("%d\n", GetParity(0xffff));

    PrintBinary(GneratorPoly);
    PrintBinary(0x555<<11);
    PrintBinary(GetSyndrome(0x1));

	getchar();

    return EXIT_SUCCESS;
}
