#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <stdint.h>
#include <stdio.h>

#define GneratorPoly                0xAE3 // Another alternative would be to use the codeword 0xC75

//modes
#define GOLAY_23                    0x00
#define GOLAY_24                    0x01
#define LT_ENABLED					        0x02

#define ELT_FILE_NAME				        ".ELT.lt"
#define DLT_FILE_NAME				        ".DLT.lt"

#define DECODE_NO_ERRORS            0
#define DECODE_PARITY_ERRORS        1
#define DECODE_FIXED                2
#define DECODE_SYNDROME_ERRORS      3

/** \brief Definition of a Golay Codeword.
 *
 *  A Golay Codeword is made of twelve bits of data, eleven bits of checksum
 *  and, if using 24-bits Golay Codes, a parity bit.
 *  This way we can use wither the entire Codeword or directly set every field
 *  of it.
 */
typedef union {
  uint32_t CodeWord;      /**< The entire codeword */
  struct {
    uint32_t data   : 12;
    uint32_t check  : 11;
    uint32_t parity  : 1;
  } cw;                   /**< The Structure used to read every single part of the Codeword */
} GolayCW;

/*!\brief	This is the function used to print
 * 			CodeWord in binary representation
 *
 * \param	CodeWord:	 value to print
 *
 */
void PrintBinary (uint32_t CodeWord);

/*!\brief	This is the function used to compute
 *			the lexicographically next bit permutation
 *
 * \param	word:	previous permutation of bits
 *
 * \retval			next permutation of bits
 * \note	FROM:	Bit Twiddling Hacks
 * 			URL:	http://www-graphics.stanford.edu/~seander/bithacks.html
 */
uint32_t NextBitPermutation(uint32_t word);

/*!\brief	This is the function used to count 1-bits in a word
 *			by "devide and conquer" strategy.
 *
 * \param	CodeWord: a word
 *
 * \retval			number of 1-bits
 * \note	FROM:	Hackers delight p. 95;
 */
int32_t Weight (uint32_t CodeWord);

/*!\brief	This is the function used to compute the parity of a word
 *			by "devide and conquer" strategy.
 *
 * \param	CodeWord: a word
 *
 * \retval			parity bit
 * \note	FROM:	Hackers delight p. 105;
 */
int32_t GetParity (uint32_t CodeWord);

uint32_t GetSyndrome (uint32_t CodeWord);

/*!\brief	This is the function used to rotate with carry
 *          23 bit long word by n bits to the left
 *
 * \param	CodeWord: a word to rotate
 * \param	i:		  number of rotation
 * \retval			  result of rotation
 */
uint32_t RotL (uint32_t CodeWord, int32_t i);

/*!\brief	This is the function used to rotate with carry
 *          23 bit long word by n bits to the right
 *
 * \param	CodeWord: a word to rotate
 * \param	i:		  number of rotation
 * \retval			  result of rotation
 */
uint32_t RotR (uint32_t CodeWord, int32_t i);

#endif // UTILS_H_INCLUDED
