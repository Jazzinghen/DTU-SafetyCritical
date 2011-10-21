#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "headers/utils.h"
#include "headers/encoder.h"

uint32_t Encode(uint8_t parity_mode, GolayCW *CodeWord) {
	CodeWord->cw.check = GetSyndrome(CodeWord->cw.data)&0x7ff;
	if (parity_mode == GOLAY_24){
		CodeWord->cw.parity = GetParity(CodeWord->CodeWord);
	}
}

uint32_t EncodeFile(char *src, char *dst, uint8_t mode) {
	FILE *fp_s = fopen(src, "r");
	FILE *fp_d = fopen(dst, "w");

	if(!fp_s || !fp_d) {
		return 1;	
	}
	
	GolayCW cw1,cw2;
	uint8_t src_data[3]={0};
	
	while(fread(src_data, 1, 3, fp_s)) {
		cw1.CodeWord  =  (src_data[0]<<4);
		cw1.CodeWord |=  (src_data[1]>>4)&0x00f;

		cw2.CodeWord  =  (src_data[1]<<8)&0xf00;
		cw2.CodeWord |=  (src_data[2]);

		Encode(mode, &cw1);
		Encode(mode, &cw2);

		PrintBinary(cw1.CodeWord);
		PrintBinary(cw2.CodeWord);

		fputc( cw1.CodeWord     &0xff,fp_d);
		fputc((cw1.CodeWord>>8) &0xff,fp_d);
		fputc((cw1.CodeWord>>16)&0xff,fp_d);
		
		fputc( cw2.CodeWord     &0xff,fp_d);
		fputc((cw2.CodeWord>>8) &0xff,fp_d);
		fputc((cw2.CodeWord>>16)&0xff,fp_d);
		
		
		
		memset(src_data, 0, sizeof(src_data));	
	}
	
	fclose(fp_s);
	fclose(fp_d);
	
	return 0;
}
