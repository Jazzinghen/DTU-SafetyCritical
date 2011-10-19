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
         int32_t l;
         while(i--) {

         }
         return CodeWord;
}

uint32_t RotR(uint32_t CodeWord, int32_t i) {

}
