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

unsigned long GetSyndrome(unsigned long CodeWord) {
     int i;
     for (i = 1; i<=12; i++, CodeWord>>=1) {
         if(CodeWord & 1) {
             CodeWord ^= GneratorPoly;    
         }   
     }
     return(CodeWord);
}

//Counting 1-bits, "divide and conquer" strategy.
//from boook: Hackers delight p. 95;
int Weight(unsigned long CodeWord) {
    CodeWord = CodeWord - ((CodeWord >> 1) & 0x55555555);
    CodeWord = (CodeWord & 0x33333333) + ((CodeWord >> 2) & 0x33333333);
    CodeWord = (CodeWord + (CodeWord >> 4)) & 0x0F0F0F0F;
    CodeWord = CodeWord + (CodeWord >> 8);
    CodeWord = CodeWord + (CodeWord >> 16);
    
    return CodeWord & 0x0000003F;
}

//Computing the Parity of a Word
//from boook: Hackers delight p. 105;
int GetParity(unsigned long CodeWord) {
    CodeWord = CodeWord ^ (CodeWord >> 1);
    CodeWord = (CodeWord ^ (CodeWord >> 2)) & 0x11111111;
    CodeWord = CodeWord*0x11111111;
    return((CodeWord >> 28) & 1); 
}

unsigned long RotL(unsigned long CodeWord, int i) {
         while(i--) {
                    CodeWord = (CodeWord<<1) | !(!(CodeWord & 0x400000));         
         }
         return CodeWord & 0x7fffff;             
}

unsigned long RotR(unsigned long CodeWord, int i) {
         
}

void Correction(void) {
     
}

void Decoder(void) {
     
}

void Test(void) {
     
}

int main(void) {
    
     PrintBinary(RotL(0xf00,1)); 
     PrintBinary(RotL(0xf00,2));
     PrintBinary(RotL(0xf00,0)); 
     PrintBinary(RotL(0xf00,10));
     PrintBinary(RotL(0xf00,11));
     PrintBinary(RotL(0xf00,12));
     PrintBinary(RotL(0xf00,13));
     PrintBinary(RotL(0xf00,14));     
                  
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
}
