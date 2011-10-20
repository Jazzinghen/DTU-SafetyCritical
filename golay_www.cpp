#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define POLY  0xAE3  /* or use the other polynomial, 0xC75 */

void PrintBinary(unsigned long CodeWord) {
     int i;
     for(i = 31; i >= 0; i--) {
           printf("%d", (CodeWord>>i)&0x01);
     }
     puts("");
}

unsigned long golay(unsigned long cw) 
/* This function calculates [23,12] Golay codewords. 
   The format of the returned longint is 
   [checkbits(11),data(12)]. */ 
{ 
  int i; 
  unsigned long c; 
  cw&=0xfffl;
  c=cw; /* save original codeword */ 
  for (i=1; i<=12; i++)  /* examine each data bit */ 
    { 
      if (cw & 1)        /* test data bit */ 
        cw^=POLY;        /* XOR polynomial */ 
      cw>>=1;            /* shift intermediate result */
    } 
  return((cw<<12)|c);    /* assemble codeword */ 
} 


int parity(unsigned long cw) 
/* This function checks the overall parity of codeword cw.
   If parity is even, 0 is returned, else 1. */ 
{ 
  unsigned char p; 

  /* XOR the bytes of the codeword */ 
  p=*(unsigned char*)&cw; 
  p^=*((unsigned char*)&cw+1); 
  p^=*((unsigned char*)&cw+2); 

  /* XOR the halves of the intermediate result */ 
  p=p ^ (p>>4); 
  p=p ^ (p>>2); 
  p=p ^ (p>>1); 

  /* return the parity result */ 
  return(p & 1); 
} 






unsigned long syndrome(unsigned long cw) 
/* This function calculates and returns the syndrome 
   of a [23,12] Golay codeword. */ 
{ 
  int i; 
  cw&=0x7fffffl; 
  for (i=1; i<=12; i++)  /* examine each data bit */ 
    { 
      if (cw & 1)        /* test data bit */ 
        cw^=POLY;        /* XOR polynomial */ 
      cw>>=1;            /* shift intermediate result */ 
    } 
  return(cw<<12);        /* value pairs with upper bits of cw */
} 




int weight(unsigned long cw) 
/* This function calculates the weight of 
   23 bit codeword cw. */ 
{ 
  int bits,k; 

  /* nibble weight table */ 
  const char wgt[16] = {0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4}; 

  bits=0; /* bit counter */ 
  k=0; 
  /* do all bits, six nibbles max */ 
  while ((k<6) && (cw)) 
    { 
      bits=bits+wgt[cw & 0xf]; 
      cw>>=4; 
      k++; 
    } 

  return(bits); 
} 




unsigned long rotate_left(unsigned long cw, int n) 
/* This function rotates 23 bit codeword cw left by n bits. */ 
{ 
  int i; 

  if (n != 0) 
    { 
      for (i=1; i<=n; i++) 
        { 
          if ((cw & 0x400000l) != 0) 
            cw=(cw << 1) | 1; 
          else 
            cw<<=1; 
        } 
    } 

  return(cw & 0x7fffffl); 
} 

unsigned long rotate_right(unsigned long cw, int n) 
/* This function rotates 23 bit codeword cw right by n bits. */ 
{ 
  int i; 

  if (n != 0) 
    { 
      for (i=1; i<=n; i++) 
        { 
          if ((cw & 1) != 0) 
            cw=(cw >> 1) | 0x400000l; 
          else 
            cw>>=1; 
        } 
    } 

  return(cw & 0x7fffffl); 
} 

unsigned long correct(unsigned long cw, int *errs) 
/* This function corrects Golay [23,12] codeword cw, returning the 
   corrected codeword. This function will produce the corrected codeword 
   for three or fewer errors. It will produce some other valid Golay 
   codeword for four or more errors, possibly not the intended 
   one. *errs is set to the number of bit errors corrected. */ 
{ 
  unsigned char 
    w;                /* current syndrome limit weight, 2 or 3 */ 
  unsigned long 
    mask;             /* mask for bit flipping */ 
  int 
    i,j;              /* index */ 
  unsigned long 
    s,                /* calculated syndrome */ 
    cwsaver;          /* saves initial value of cw */ 

  cwsaver=cw;         /* save */ 
  *errs=0; 
  w=3;                /* initial syndrome weight threshold */ 
  j=-1;               /* -1 = no trial bit flipping on first pass */ 
  mask=1; 
  while (j<23) /* flip each trial bit */ 
    { 
      if (j != -1) /* toggle a trial bit */ 
        { 
          if (j>0) /* restore last trial bit */ 
            { 
              cw=cwsaver ^ mask; 
              mask+=mask; /* point to next bit */ 
            } 
          cw=cwsaver ^ mask; /* flip next trial bit */ 
          w=2; /* lower the threshold while bit diddling */ 
        } 

      s=syndrome(cw); /* look for errors */ 
      if (s) /* errors exist */ 
        { 
          for (i=0; i<23; i++) /* check syndrome of each cyclic shift */ 
            { 
              if ((*errs=weight(s)) <= w) /* syndrome matches error pattern */
                { 
                  cw=cw ^ s;              /* remove errors */ 
                  cw=rotate_right(cw,i);  /* unrotate data */ 
                  return(s=cw); 
                } 
              else 
                { 
                  cw=rotate_left(cw,1);   /* rotate to next pattern */ 
                  s=syndrome(cw);         /* calc new syndrome */ 
                } 
            } 
          j++; /* toggle next trial bit */ 
        } 
      else 
        return(cw); /* return corrected codeword */ 
    } 

  return(cwsaver); /* return original if no corrections */ 
} /* correct */ 


int decode(int correct_mode, int *errs, unsigned long *cw) 
/* This function decodes codeword *cw in one of two modes. If correct_mode 
   is nonzero, error correction is attempted, with *errs set to the number of
   bits corrected, and returning 0 if no errors exist, or 1 if parity errors 
   exist. If correct_mode is zero, error detection is performed on *cw, 
   returning 0 if no errors exist, 1 if an overall parity error exists, and 
   2 if a codeword error exists. */ 
{ 
  unsigned long parity_bit; 

  if (correct_mode)               /* correct errors */ 
    { 
      parity_bit=*cw & 0x800000l; /* save parity bit */ 
      *cw&=~0x800000l;            /* remove parity bit for correction */

      *cw=correct(*cw, errs);     /* correct up to three bits */ 
      *cw|=parity_bit;            /* restore parity bit */ 

      /* check for 4 bit errors */ 
      if (parity(*cw))            /* odd parity is an error */ 
        return(1); 
      return(0); /* no errors */ 
    } 
  else /* detect errors only */ 
    { 
      *errs=0; 
      if (parity(*cw)) /* odd parity is an error */ 
        { 
          *errs=1; 
          return(1); 
        } 
      if (syndrome(*cw)) 
        { 
          *errs=1; 
          return(2); 
        } 
      else 
        return(0); /* no errors */ 
    } 
} /* decode */ 




int main(void)
/* This function tests the Golay routines for detection and correction
   of various patterns of error_limit bit errors. The error_mask cycles
   over all possible values, and error_limit selects the maximum number
   of induced errors. */
{
  unsigned long
    error_mask,        /* bitwise mask for inducing errors */
    trashed_codeword,  /* the codeword for trial correction */
    virgin_codeword;   /* the original codeword without errors */
  unsigned char        
    pass=1,            /* assume test passes */
    error_limit=3;     /* select number of induced bit errors here */
  int
    error_count;       /* receives number of errors corrected */
    PrintBinary(golay(0x555));
    
    
  virgin_codeword=golay(0x555); /* make a test codeword */
  if (parity(virgin_codeword))
    virgin_codeword^=0x800000l;
  for (error_mask=0; error_mask<0x800000l; error_mask++)
    {
      /* filter the mask for the selected number of bit errors */
      if (weight(error_mask) <= error_limit) /* you can make this faster! */
        {
          trashed_codeword=virgin_codeword ^ error_mask; /* induce bit errors */

          decode(1,&error_count,&trashed_codeword); /* try to correct bit errors */

          if (trashed_codeword ^ virgin_codeword)
            {
              printf("Unable to correct %d errors induced with error mask = 0x%lX\n",
                weight(error_mask),error_mask);
              pass=0;
            }

            if(0)
            {
              if (getchar() == 27) return 0; /* escape exits */

              /* other key prints status */
              printf("Current test count = %ld of %ld\n",error_mask,0x800000l);
            }
        }
    }
  printf("Golay test %s!\n",pass?"PASSED":"FAILED");
  getchar();
}
