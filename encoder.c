#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "headers/utils.h"
#include "headers/encoder.h"
#include "headers/test.h"


void Encode(uint8_t parity_mode, GolayCW *CodeWord) {
	CodeWord->cw.check = GetSyndrome(CodeWord->cw.data)&0x7ff;
	/*
	 * Add parity bit if needed
	 */
	if (parity_mode == GOLAY_24){
		CodeWord->cw.parity = GetParity(CodeWord->CodeWord);
    }
}

void EncodeLT (uint8_t parity_mode, GolayCW *CodeWord, GolayCW *LookupTable) {
	if(parity_mode == GOLAY_24) {
		CodeWord->CodeWord = LookupTable[CodeWord->cw.data].CodeWord;
	} else {
		/*
	 	 * Remove parity bit from lookup table if needed
	 	 */
		CodeWord->CodeWord = LookupTable[CodeWord->cw.data].CodeWord & 0x7fffff;
	}
}

uint32_t EncodeFile(char *src, char *dst, uint8_t mode) {
	/* open source and destination files */
	FILE *fp_s = fopen(src, "rb");
	FILE *fp_d = fopen(dst, "wb");

	/* Upon not successful open return 1 */
	if(!fp_s || !fp_d) {
		return 1;
	}

	GolayCW cw1,cw2;
	uint8_t src_data[3]={0};

	/* Everytime read from file 3 bytes of data to encode */
	while(fread(src_data, 1, 3, fp_s)) {
		/* 3 bytes of data = 8*3 bits = 24 bits = 12*2 bits = 2*CodeWord.data */
		cw1.CodeWord  =  (src_data[0]<<4);
		cw1.CodeWord |=  (src_data[1]>>4)&0x00f;

		cw2.CodeWord  =  (src_data[1]<<8)&0xf00;
		cw2.CodeWord |=  (src_data[2]);

		/* Encode our two codewords */
		Encode(mode, &cw1);
		Encode(mode, &cw2);

		/* Save encoded codewords in a file */
		fputc( cw1.CodeWord     &0xff,fp_d);
		fputc((cw1.CodeWord>>8) &0xff,fp_d);
		fputc((cw1.CodeWord>>16)&0xff,fp_d);

		fputc( cw2.CodeWord     &0xff,fp_d);
		fputc((cw2.CodeWord>>8) &0xff,fp_d);
		fputc((cw2.CodeWord>>16)&0xff,fp_d);

		memset(src_data, 0, sizeof(src_data));
	}

	/* Close file descrpytors */
	fclose(fp_s);
	fclose(fp_d);

	return 0;
}

size_t ComputeELT(uint8_t messages, uint8_t mode, GolayCW * LookupTable) {
  uint16_t fakeData = 0;    //  Fake data used to generate the entire codeword
  uint16_t i = 0;
  uint16_t j = 0;
  FILE *LTFile;             //  File descriptor for the Lookup Table File
  size_t res;               //  Result of fwrite/fread

  union {
    uint8_t bytes[4];
    uint32_t cw;
  } tempData;               //  Union used to store an entire error mask and then split it into bytes.

  GolayCW tempCW;           //  Temporary CodeWord used for the generation

  uint8_t data [4096 * 3];  //  Temporary array used for file operations

  //  We first run a memset over the Lookup Table, we don't want any garbage data.
  memset(LookupTable, 0, sizeof(GolayCW) * 4096);

  //  Try to open the file ("rb" is used for Windows compatibility)
  LTFile = fopen(ELT_FILE_NAME, "rb");

  //  If the file exists then we read the Table from it.
  if (LTFile != NULL) {
    if (messages == MESSAGES_ON) {
      ("Now reading the Encoding Lookup Table: ");
    };
    res = fread(data, sizeof(uint8_t) * 3, 4096, LTFile);
    for (i = 0; i < 4096; i++){
      j = i*3;
      tempData.cw = 0;
      tempData.bytes[0] = data[j];
      tempData.bytes[1] = data[j+1];
      tempData.bytes[2] = data[j+2];
      LookupTable[i].CodeWord = tempData.cw;
      //  If we are working with 23-bit Golay CodeWords, then we don't need the Parity bit
      if (mode == GOLAY_23) {
        LookupTable[i].cw.parity = 0;
      }
      if ((i%256) == 0 && messages == MESSAGES_ON) {
        printf (".");
      }
    }
  } else {
    if (messages == MESSAGES_ON) {
      printf("Now generating the Encoding Lookup Table: ");
    }
    //  If the file does not exist then we have to create it.
    LTFile = fopen(ELT_FILE_NAME, "wb");
    //  For every single possibile chunk of 12-bits data, from 0x0 to 0xFFF
    for (fakeData = 0; fakeData < 4096; fakeData++) {
      tempData.cw = 0;
      tempCW.CodeWord = 0;
      j = fakeData * 3;
      tempCW.cw.data = fakeData;
      //  We compute the Codeword for that data. We use GOLAY_24 and not “method” since
      //  we can always ignore the Parity bit when reading the Table or when doing any
      //  Operation and because packing 24 bits codewords (3 bytes) instead of 23 is easier.
      Encode(GOLAY_24, &tempCW);
      //  And then we put it in the Lookup Table using the data itself as index.
      LookupTable[fakeData].CodeWord = tempCW.CodeWord;
      tempData.cw = tempCW.CodeWord;

      data[j] = tempData.bytes[0];
      data[j+1] = tempData.bytes[1];
      data[j+2] = tempData.bytes[2];

      if ((fakeData%256) == 0 && messages == MESSAGES_ON) {
        printf (".");
      }
    }
    //  In the end we write all the Codewords
    res = fwrite(data, sizeof(uint8_t) * 3, 4096, LTFile);
  }
  if (messages == MESSAGES_ON) {
    printf (" Done.\n");
  }
	fclose(LTFile);
  return res;
}
