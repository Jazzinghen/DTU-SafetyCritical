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

      errors = Correction(codeWord);     /* correct up to three bits */
      codeWord->cw.parity = parity_bit;            /* restore parity bit */

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

/* This function corrects Golay [23,12] codeword cw, returning the
   corrected codeword. This function will produce the corrected codeword
   for three or fewer errors. It will produce some other valid Golay
   codeword for four or more errors, possibly not the intended
   one. *errs is set to the number of bit errors corrected. */

uint8_t Correction (GolayCW *codeWord)
{
  uint8_t limitWeight;                /* current syndrome limit weight, 2 or 3 */
  uint32_t mask;             /* mask for bit flipping */
  int16_t
    i,j;              /* index */
  uint32_t syndrome;                /* calculated syndrome */
  GolayCW tempCW;          /* saves initial value of cw */

  uint8_t errors = 0;

  tempCW = *codeWord;         /* save */
  limitWeight = 3;                /* initial syndrome weight threshold */
  j = -1;               /* -1 = no trial bit flipping on first pass */
  mask = 1;
  while (j < 23) /* flip each trial bit */
    {
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

      syndrome = GetSyndrome(codeWord->CodeWord); /* look for errors */
      if (syndrome > 0) /* errors exist */
        {
          for (i=0; i<23; i++) {/* check syndrome of each cyclic shift */
              if ((errors = Weight(syndrome)) <= limitWeight) {
                  codeWord->CodeWord = codeWord->CodeWord ^ syndrome;              /* remove errors */
                  codeWord->CodeWord = RotR(codeWord->CodeWord, i);  /* unrotate data */
                  return(errors);
                }
              else
                {
                  codeWord->CodeWord = RotL(codeWord->CodeWord,1);   /* rotate to next pattern */
                  syndrome = GetSyndrome(codeWord->CodeWord);         /* calc new syndrome */
                }
            }
          j++; /* toggle next trial bit */
        }
      else
        return(errors); /* return corrected codeword */
    }

  *codeWord = tempCW;
  return(errors); /* return original if no corrections */
} /* correct */
