#include "headers/utils.h"

/* This function decodes codeword *cw in one of two modes. If correct_mode
   is nonzero, error correction is attempted, with *errs set to the number of
   bits corrected, and returning 0 if no errors exist, or 1 if parity errors
   exist. If correct_mode is zero, error detection is performed on *cw,
   returning 0 if no errors exist, 1 if an overall parity error exists, and
   2 if a codeword error exists. */

uint8_t decode(uint8_t correct_mode, uint8_t *errors, GolayCW *codeWord){

  uint32_t parity_bit;

  if (correct_mode == CORRECT_ERRORS)               /* correct errors */
    {
      parity_bit = codeWord->cw.parity; /* save parity bit */
      codeWord->cw.parity = 0;            /* remove parity bit for correction */

      codeWord->CodeWord = Correction(codeWord->CodeWord, errors);     /* correct up to three bits */
      codeWord->CodeWord = parity_bit;            /* restore parity bit */

      /* check for 4 bit errors */
      if (GetParity(codeWord->CodeWord) > 0)            /* odd parity is an error */
        return(DECODE_PARITY_ERRORS);
      return(DECODE_NO_ERRORS); /* no errors */
    }
  else /* detect errors only */
    {
      *errors = 0;
      if (GetParity(codeWord->CodeWord) > 0) /* odd parity is an error */
        {
          *errors = 1;
          return(DECODE_PARITY_ERRORS);
        }
      if (GetSyndrome(codeWord->CodeWord) > 0)
        {
          *errs=1;
          return(DECODE_SYNDROME_ERRORS);
        }
      else
        return(DECODE_NO_ERRORS); /* no errors */
    }
} /* decode */

void Correction(void) {

}
