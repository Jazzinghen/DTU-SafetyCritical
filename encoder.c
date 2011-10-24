#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "headers/utils.h"
#include "headers/encoder.h"
#include "headers/test.h"

/*!\brief	This function calculate Golay CodeWord from 12 bits of data
 *		    stored in a GolayCW structure
 *
 * \param	Parity mode: GOLAY_24 or GOLAY_23
 * \param	Pointer to a codeword
 *
 */
void Encode(uint8_t parity_mode, GolayCW *CodeWord) {
	CodeWord->cw.check = GetSyndrome(CodeWord->cw.data)&0x7ff;
	/*
	 * Add parity bit if needed
	 */
	if (parity_mode == GOLAY_24){
		CodeWord->cw.parity = GetParity(CodeWord->CodeWord);
    }
}

/*!\brief	This function encode 12 bits of data stored in a GolayCW structure
 *		    by using lookup tables
 *
 * \param	Parity mode: GOLAY_24 or GOLAY_23
 * \param	Pointer to a codeword
 * \param   Pointer to a encoding lookup table
 *
 */
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

/*  This is the function used to compute the Lookup Table for the encoding.
 *
 *  The idea is pretty simple: we just compute the codeword associated to every single 12Bits combination
 *  and then store it using the data as index. When we want to encode a chunk of data we just have to look
 *  for the entire codeword in the Lookup Table.
 *
 *  This function checks also if there is already a Lookup Table File. If this is the case it loads
 *  the Lookup Table in the array. On the other hand, if the file does not exist, then the Lookup Table
 *  is computed and stored both in an external file and in the array.
 *
 *  Bear in mind that it doesn't check whether the file has a correct Lookup Table or not, so if there's
 *  any suspicion that the file is wrong/corrupted simply eliminate it and the function will generate a new
 *  one.
 */
size_t ComputeELT(uint8_t messages, uint8_t mode, GolayCW * LookupTable) {
  uint16_t fakeData = 0;
  uint16_t i = 0;
  uint16_t j = 0;
  FILE *LTFile;
  size_t res;

  union {
    uint8_t bytes[4];
    uint32_t cw;
  } tempData;

  GolayCW tempCW;

  uint8_t data [4096 * 3];

  memset(LookupTable, 0, sizeof(GolayCW) * 4096);

  LTFile = fopen(ELT_FILE_NAME, "rb");

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
    LTFile = fopen(ELT_FILE_NAME, "wb");
    for (fakeData = 0; fakeData < 4096; fakeData++) {
      tempData.cw = 0;
      tempCW.CodeWord = 0;
      j = fakeData * 3;
      tempCW.cw.data = fakeData;
      Encode(GOLAY_24, &tempCW);
      LookupTable[fakeData].CodeWord = tempCW.CodeWord;
      tempData.cw = tempCW.CodeWord;

      data[j] = tempData.bytes[0];
      data[j+1] = tempData.bytes[1];
      data[j+2] = tempData.bytes[2];

      if ((fakeData%256) == 0 && messages == MESSAGES_ON) {
        printf (".");
      }
    }
    res = fwrite(data, sizeof(uint8_t) * 3, 4096, LTFile);
  }
  if (messages == MESSAGES_ON) {
    printf (" Done.\n");
  }
	fclose(LTFile);
  return res;
}

/*!\brief	This is the function used to encode files
 *
 * \param	src:	path to the source file
 * \param	dst:	path to the destination file
 * \param   mode: 	GOLAY_24 or GOLAY_23
 *
 * \retval	zero if file encoding was successful.
 */
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
