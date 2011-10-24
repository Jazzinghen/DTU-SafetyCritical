#ifndef ENCODER_H_INCLUDED
#define ENCODER_H_INCLUDED

#include <stdint.h>
#include "utils.h"

/*!\brief	This function calculate Golay CodeWord from 12 bits of data
 *		    stored in a GolayCW structure
 *
 * \param	Parity mode: GOLAY_24 or GOLAY_23
 * \param	Pointer to a codeword
 *
 */
void Encode (uint8_t parity_mode, GolayCW *CodeWord);

/*!\brief	This function encode 12 bits of data stored in a GolayCW structure
 *		    by using lookup tables
 *
 * \param	Parity mode: GOLAY_24 or GOLAY_23
 * \param	Pointer to a codeword
 * \param   Pointer to a encoding lookup table
 *
 */
void EncodeLT (uint8_t parity_mode, GolayCW *CodeWord, GolayCW * LookupTable);

/*!\brief	This is the function used to encode files
 *
 * \param	src:	path to the source file
 * \param	dst:	path to the destination file
 * \param   mode: 	GOLAY_24 or GOLAY_23
 *
 * \retval	zero if file encoding was successful.
 */
uint32_t EncodeFile(char *src, char *dst, uint8_t mode);

/**  \brief Function used to compute the Lookup Table for the encoding.
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
 *
 *  \param message:     Wheter we want some informations printed on screen or not. Either MESSAGES_ON or MESSAGES_OFF
 *  \param LookupTable: Pointer to the Encoding Lookup Table
 *
 *  \return The result of the file operations inside the function.
 */
size_t  ComputeELT(uint8_t message, GolayCW * LookupTable);

#endif // ENCODER_H_INCLUDED
