#include "headers/utils.h"
#include "headers/decoder.h"

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
  if (GetSyndrome(codeWord->CodeWord) > 0) {
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
      if (j != -1) /* toggle a trial bit */
        {
          if (j > 0) /* restore last trial bit */
            {
              codeWord->CodeWord = tempCW.CodeWord ^ mask;
              mask += mask; /* point to next bit */
            }
          codeWord->CodeWord = tempCW.CodeWord ^ mask; /* flip next trial bit */
          limitWeight = 2; /* lower the threshold while bit diddling */
        }

      //  Check for errors in the current CodeWord, and shift them to a proper position.
      syndrome = (GetSyndrome(codeWord->CodeWord) << 12);
      //  If there are errors
      if (syndrome > 0) {
        for (i=0; i<23; i++) {/* check syndrome of each cyclic shift */
            if ((errors = Weight(syndrome)) <= limitWeight) {
                codeWord->CodeWord = codeWord->CodeWord ^ syndrome;              /* remove errors */
                codeWord->CodeWord = RotR(codeWord->CodeWord, i);  /* unrotate data */
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
                codeWord->CodeWord = RotL(codeWord->CodeWord,1);   /* rotate to next pattern */
                syndrome = (GetSyndrome(codeWord->CodeWord) << 12);         /* calc new syndrome */
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
} /* correct */
