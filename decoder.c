#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "headers/utils.h"
#include "headers/decoder.h"
#include "headers/test.h"

/*!\brief	This is the function used to decode files
 *
 * \param	src:	path to the source file
 * \param	dst:	path to the destination file
 * \param   mode: 	GOLAY_24 or GOLAY_23
 *
 * \retval	zero if file decoding was successful.
 */
uint8_t DecodeFile (char *src, char *dst, uint8_t mode) {
	/* open source and destination files */	
	FILE *fp_s = fopen(src, "rb");
	FILE *fp_d = fopen(dst, "wb");

	/* Upon not successful open return 1 */
	if(!fp_s || !fp_d) {
		return 1;
	}

	GolayCW cw1, cw2;
	uint8_t src_data[6]={0};

	/* Everytime read from file 6 bytes of data to decode */
	while(fread(src_data, 1, 6, fp_s)) {
		/* 6 bytes of data = 8*6 bits = 48 bits = 2*24 bits = 2*CodeWord */
		cw1.CodeWord  = (src_data[0]);
		cw1.CodeWord |= (src_data[1])<<8;
		cw1.CodeWord |= (src_data[2])<<16;

		cw2.CodeWord  = (src_data[3]);
		cw2.CodeWord |= (src_data[4])<<8;
		cw2.CodeWord |= (src_data[5])<<16;

		/* Decode our two codewords */
		Correction(mode, &cw1);
		Correction(mode, &cw2);

		/* Save decoded data in a destination file */
		fputc((cw1.CodeWord>>4)&0xff, fp_d);
		fputc((cw1.CodeWord<<4)&0xf0 | (cw2.CodeWord>>8)&0x0f, fp_d);
		fputc((cw2.CodeWord)   &0xff, fp_d);

		memset(src_data, 0, sizeof(src_data));
	}

	/* Close files */
	fclose(fp_s);
	fclose(fp_d);

	return 0;
}

/*!\brief	This function decode Golay CodeWords by using lookup tables
 *
 * \param	Parity mode: GOLAY_24 or GOLAY_23
 * \param	Pointer to a codeword
 * \param   Pointer to a decoding lookup table
 *
 */
void DecodeLT (uint8_t mode, GolayCW *CodeWord, uint32_t *LookupTable) {
	CodeWord->CodeWord = CodeWord->CodeWord ^ LookupTable[GetSyndrome(CodeWord->CodeWord&0x7fffff)];
}

/*  This is the function used to check whether a Golay Codeword is correct or not.
 *  It can be used to check whether there have been errors and, by definition, can
 *  detect up to 6 bits of errors and any number of odd bits of error, in any pattern.
 *
 *  It has been designed to work with both 23 Bits and 24 Bits Golay CodeWords.
 */

uint8_t ErrorCheck (uint8_t parity_mode, GolayCW *codeWord){

  //  If we are working with 24 Bit Golay CodeWords, then we will have to do a parity check
  if (parity_mode == GOLAY_24){
      if (GetParity(codeWord->CodeWord) > 0){
          return(DECODE_PARITY_ERRORS); //  Parity Error found!
        }
  }

  //  We check for a reminder in the division of the Codeword by the Generator Polynomial
  if (GetSyndrome(codeWord->CodeWord&0x7fffff) > 0) {
      return(DECODE_SYNDROME_ERRORS); //  Means that there has been at least one error.
  } else {
      return(DECODE_NO_ERRORS);       //  No errors detected.
  }
}

/*  This function used to correct a Golay Codeword. If there are less than 4 wrong bits, in
 *  any pattern, then this function can detect and fix the error. If there are more errors, then
 *  the function will return a correct Golay Codeword but there is a great probability that
 *  it won't be the original one.
 *
 *  It has beed designed to work with both 23 and 24 Bits version of Golay CodeWords, we have
 *  just to ignore the Parity bit in the case of 23 bits.
 */

uint8_t Correction (uint8_t parity_mode, GolayCW *codeWord)
{
  uint32_t mask = 1;            //  Mask for bit flipping
  uint32_t syndrome;            //  Calculated Syndrome
  uint8_t limitWeight = 3;      //  The limit of errors that we will accept. It's either 2 or 3.
  uint8_t errors = 0;           //  Number of errors encountered

  GolayCW tempCW = *codeWord;   //  Variable to store the original CodeWord
  uint32_t parity_bit;          //  Variable to store the original Parity Bit (Used for 24Bit Golay CodeWords

  int16_t i;
  int16_t j = -1;               // Used to count trial bit flipping. We set to -1 since we didn't do a single one.


  //  If there are no errors just skip to the end, we don't have to do anything...
  if (ErrorCheck(parity_mode, codeWord) != DECODE_NO_ERRORS){

    //  If we are working wit 24Bits CodeWords
    if (parity_mode == GOLAY_24){
      //  We will have to store the Parity bit, since it can create problems.
      parity_bit = codeWord->cw.parity;
      //  Then we reset the parity bit for both of the CodeWords (The original one and the backup)
      codeWord->cw.parity = 0;
      tempCW.cw.parity = 0;
    }

    /*  We will have then to begin to test every single shift of the 23 possible (Since a Golay Codeword
     *  is made of [12 bits of data, 11 bits of checksum].
     *  We could relay on the strength of the algorithm, since, in theory, every single 23 bit CodeWord maps
     *  to exactly one correct Golay Codeword, but it's always better to put a limit to te computations, so
     *  we will do EXACTLY 23 shifts.
     */
    while (j < 23) {

      //  If we are toggling trial bits
      if (j != -1) {
        //  We first restore the Codeword as it was before the toggling (If any toggle was done)
        if (j > 0)
          {
            codeWord->CodeWord = tempCW.CodeWord ^ mask;
            //  Then we point to the next trial bit.
            mask += mask;
          }
        //  Flip the trial bit and lower the thresshold since we are modifying them, or we risk to fix 4 bits errors.
        codeWord->CodeWord = tempCW.CodeWord ^ mask;
        limitWeight = 2;
      }

      //  Check for errors in the current CodeWord, and shift them to a proper position.
      syndrome = (GetSyndrome(codeWord->CodeWord) << 12);
      //  If there are errors
      if (syndrome > 0) {
        //  We do at most 23 shifts to find a correct Golay word, since we know there is one
        for (i=0; i<23; i++) {
            if ((errors = Weight(syndrome)) <= limitWeight) {
                //  If the number of errors is less then or limit we remove them
                codeWord->CodeWord = codeWord->CodeWord ^ syndrome;
                //  And restore the position of the bits.
                codeWord->CodeWord = RotR(codeWord->CodeWord, i);
                if (parity_mode == GOLAY_24){
                  codeWord->cw.parity = parity_bit;
                  if (GetParity(codeWord->CodeWord) > 0){            /* odd parity is an error */
                    return(DECODE_PARITY_ERRORS);
                  }
                }
                return(DECODE_FIXED);
              }
            else
              {
                //  If that was not the correct one we rotate the Codeword and compute the new Syndrome
                codeWord->CodeWord = RotL(codeWord->CodeWord,1);
                syndrome = (GetSyndrome(codeWord->CodeWord) << 12);
              }
          }

        j++;
      } else {
        //  If there are no errors we just have to check if there are parity errors if we are working with 24Bits CodeWords
        if (parity_mode == GOLAY_24){
          codeWord->cw.parity = parity_bit;
          if (GetParity(codeWord->CodeWord) > 0){
            return(DECODE_PARITY_ERRORS);
          }
        }
        return(DECODE_FIXED);
      }
    }
  } else {
    //  If we are here it means that there were no errors in the Golay Codeword, so we restore it.
    *codeWord = tempCW;
    //  And, if we are working with 24Bits CodeWords then we will do a parity check.
    if (parity_mode == GOLAY_24){
      //  We will have to restore the parity bit.
      tempCW.cw.parity = parity_bit;
      if (GetParity(codeWord->CodeWord) > 0){
        return(DECODE_PARITY_ERRORS);
      }
    }
    //  No errors at all, so nothing to fix.
    return (DECODE_NO_ERRORS);
  }
}

/*  This is the function used to compute the Lookup Table for the decoding.
 *  The idea is simple: we generate the position of all the possible errors (up to 3 bits, however, since
 *  over that quantity it's not possible to do a proper correction) and then we store them in the
 *  Lookup Table using the syndrome generated by the errors as index. These are stored in an array of
 *  Unsigned 32-bit integers.
 *
 *  This function checks also if there is already a Lookup Table File. If this is the case it loads
 *  the Lookup Table in the array. On the other hand, if the file does not exist, then the Lookup Table
 *  is computed and stored both in an external file and in the array.
 *
 *  Bear in mind that it doesn't check whether the file has a correct Lookup Table or not, so if there's
 *  any suspicion that the file is wrong/corrupted simply eliminate it and the function will generate a new
 *  one.
 */

size_t ComputeDLT(uint8_t messages, uint32_t * LookupTable) {

  uint32_t error_mask_arr[] = {0x00, 0x01, 0x03, 0x07}; //  Initial values for the error masks (1, 2 or 3 bits errors)
	uint32_t error_mask;                                  //  Current Error Mask

  uint16_t i = 0;
  uint32_t j = 0;

  FILE *LTFile;                                         //  File Descriptor for the LT File.
  size_t res;                                           //  Simple variable to store the result of fwrite or fread.

  union {
    uint8_t bytes[4];
    uint32_t cw;
  } tempData;                                           //  Union used to store an entire error mask and then split it into bytes.

  uint8_t data [2048 * 4];                              //  Temporary array used for file operations.

  //  Memsetting the Lookup Table. We don't want any garbage data, right?
  memset(LookupTable, 0, sizeof(uint32_t) * 2048);

  //  Opening the file. Using "rb" instead of only "r" since Windows is a pain. Windows is still using the C89 conventions, so
  //  "r" means opening the file in text mode. This way when we try to write 0xa it interprets it as '\n' and thus preponing '\r'
  //  This generates a lot of problems, so we had to use the "b", that means byte reading/writing. This is ignored in POSIX systems
  LTFile = fopen(DLT_FILE_NAME, "rb");

  //  If we found it we can read its contents.
  if (LTFile != NULL) {
    if (messages == MESSAGES_ON) {
      printf("Now reading Decoding Lookup Table: ");
    }
    //  All the data of the file will be put in the temporary array.
    res = fread(data, sizeof(uint32_t), 2048, LTFile);
    //  Now we'll store all of the data in the Lookup Table, by using the union to concatenate the bytes.
    for (i = 0; i < 2048; i++){
      j = i*4;
      tempData.bytes[0] = data[j];
      tempData.bytes[1] = data[j+1];
      tempData.bytes[2] = data[j+2];
      tempData.bytes[3] = data[j+3];
      LookupTable[i] = tempData.cw;
      if ((i%128) == 0 && messages == MESSAGES_ON) {
        printf (".");
      }
    }

  } else {
    //  In here we will generate the data AND write it into the file.
    if (messages == MESSAGES_ON) {
      printf("Now generating Decoding Lookup Table: ");
    }
    LTFile = fopen(DLT_FILE_NAME, "wb");
    //  To avoid infinite loops (Read Below) we set the error pattern for syndrome 0 as 0. It means that,
    //  if the syndrome is zero then the codeword is correct.
    data[0] = 0;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    LookupTable[0] = 0;
    //  We generate all the possible errors, from 1 to 3 bits
    for(i = 1; i <= 3; i++) {
      //  Generate the next permutation.
      for (error_mask = error_mask_arr[i]; error_mask<0x800000; error_mask=NextBitPermutation(error_mask)) {
        //  We'll apply the mask to the union, so that we can write it easily in the file.
        tempData.cw = error_mask;
        //  And then compute the syndrom, which will act as index, and write the data in the temporary array.
        j = (GetSyndrome(error_mask) * 4);
        data[j] = tempData.bytes[0];
        data[j+1] = tempData.bytes[1];
        data[j+2] = tempData.bytes[2];
        data[j+3] = tempData.bytes[3];
        LookupTable[GetSyndrome(error_mask)] = error_mask;
      }
    }
    //  Eventually we will write the array back into the Lookup Table File.
    res = fwrite(data, sizeof(uint32_t), 2048, LTFile);
  }
  if (messages == MESSAGES_ON) {
    printf (" Done.\n");
  }

  fclose(LTFile);
  //  We return the result of our writing/reading operations. Should be always 2048.
  return res;
}
