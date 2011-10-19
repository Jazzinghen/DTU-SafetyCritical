#include <stdio.h>
#include <stdlib.h>

#include "headers/utils.h"
#include "headers/encoder.h"
#include "headers/decoder.h"


void Test(void) {

}

int main(int argc, char** argv) {
    printf("%d\n", GetParity(0));
    printf("%d\n", GetParity(0x1));
    printf("%d\n", GetParity(0x2));
    printf("%d\n", GetParity(0x3));
    printf("%d\n", GetParity(0xf4));
    printf("%d\n", GetParity(0xffff));

    PrintBinary(GneratorPoly);
    PrintBinary(0x555<<11);
    PrintBinary(GetSyndrome(0x1));

    return EXIT_SUCCESS;
}
