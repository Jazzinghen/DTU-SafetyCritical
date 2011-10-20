#include "headers/utils.h"
#include "headers/decoder.h"

uint8_t ErrorCheck (uint8_t parity_mode, GolayCW *codeWord){

  if (parity_mode == GOLAY_24){
      if (GetParity(codeWord->CodeWord) > 0){
          return(DECODE_PARITY_ERRORS);
        }
  }

  if (GetSyndrome(codeWord->CodeWord) > 0) {
        return(DECODE_SYNDROME_ERRORS);
  } else {
      return(DECODE_NO_ERRORS); /* no errors */
  }
} /* decode */

/* This function corrects Golay [23,12] codeword cw, returning the
   corrected codeword. This function will produce the corrected codeword
   for three or fewer errors. It will produce some other valid Golay
   codeword for four or more errors, possibly not the intended
   one. *errs is set to the number of bit errors corrected. */

uint8_t Correction (uint8_t parity_mode, GolayCW *codeWord)
{
  uint8_t limitWeight;                /* current syndrome limit weight, 2 or 3 */
  uint32_t mask;             /* mask for bit flipping */
  int16_t
    i,j;              /* index */
  uint32_t syndrome;                /* calculated syndrome */
  GolayCW tempCW;          /* saves initial value of cw */
  uint8_t errors = 0;
  uint32_t parity_bit;

  tempCW = *codeWord;         /* save */

  if (parity_mode == GOLAY_24){
    parity_bit = codeWord->cw.parity;
    codeWord->cw.parity = 0;
  }

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

      syndrome = (GetSyndrome(codeWord->CodeWord) << 12); /* look for errors */
      if (syndrome > 0) /* errors exist */
        {
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
          j++; /* toggle next trial bit */
        }
      else {
        if (parity_mode == GOLAY_24){
          codeWord->cw.parity = parity_bit;
          if (GetParity(codeWord->CodeWord) > 0){            /* odd parity is an error */
            return(DECODE_PARITY_ERRORS);
          }
        }
        return(DECODE_FIXED);
      }
    }

  *codeWord = tempCW;
  if (parity_mode == GOLAY_24){
    if (GetParity(codeWord->CodeWord) > 0){            /* odd parity is an error */
      return(DECODE_PARITY_ERRORS);
    }
  }
  return (DECODE_NO_ERRORS);
} /* correct */
