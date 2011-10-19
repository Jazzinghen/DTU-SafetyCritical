#include <stdio.h>
#include <stdlib.h>
                            //c75 ae3
#define GneratorPoly        0xae3

void PrintBinary(unsigned long CodeWord) {
     int i;
     for(i = 31; i >= 0; i--) {
           printf("%d", (CodeWord>>i)&0x01);
           if(!(i%8)) {
                   printf(" ");
           }
     }
     puts("");
}

unsigned long GetSyndom(unsigned long CodeWord) {
     int i;
     for (i = 1; i<=12; i++, CodeWord>>=1) {
         if(CodeWord & 1) {
             CodeWord ^= GneratorPoly;    
         }   
     }
     return(CodeWord);
}

int main(void) {
    PrintBinary(GneratorPoly);
    PrintBinary(0x555<<11);
    PrintBinary(GetSyndom(0x1));
    getchar();
}
