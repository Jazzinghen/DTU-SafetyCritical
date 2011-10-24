#ifndef TEST_H_INCLUDED
#define TEST_H_INCLUDED

#include <time.h>

#include "utils.h"

#define TEST_QUANTITY       10          //  Quantity of consecutive Bruteforcing tests
#define LOOKUP_QUANTITY     1           //  Quantity of consecutive Lookup Table Generation/Loading tests
#define TEST_TARGET         0xFFF       //  Target Data Chunk (From 0x0 to TEST_TARGET)
#define SEC_PER_NSEC        1000000000

#define LOOKUP_TEST         0
#define ONTHEFLY_TEST       1

#define LOAD_OR_REMOVE      0
#define ALWAYS_REMOVE       1

#define MESSAGES_OFF        0
#define MESSAGES_ON         1

#define TEST_SUCCESFUL      0
#define TEST_FAILED         1

/** \brief Function to run a bruteforcing test on a chunk of data
 *
 *  This function takes a chunck of 12 bits of data, computes its Codeword and then uses a bruteforcing
 *  technique to inject every possible error made of 1, 2 or 3 bits. For every iteration of the
 *  bruteforcing the function tries to correct the codeword and then checks if the corrected codeword
 *  is the same as the original.
 *
 *  The function can be used with both On-The-Fly Correction or LookUp Table Based Correction.
 *
 *  \param  data:               The data chunk to test
 *  \param  mode:               The mode to use when correcting the Codeword, either LOOKUP_TEST or ONTHEFLY_TEST
 *  \param  encodeLookupTable:  Pointer to the Encoding Lookup Table
 *  \param  decodeLookupTable:  Pointer to the Decodification Lookup Table
 *
 *  \return The result of the test. Either TEST_SUCCESSFUL or TEST_FAILED
 */
uint8_t TestRun (uint32_t data, uint8_t mode, GolayCW * encodeLookupTable, uint32_t * decodeLookUpTable);

/** \brief Function to run a Lookup Table Reading/Generation test.
 *
 *  This function is simply needed to test the loading/generation of the two Lookup Tables. Generation
 *  from scratch can be enforced.
 *
 *  \param  mode:               The mode to use when executing this test. Either LOAD_OR_REMOVE or ALWAYS_REMOVE
 *  \param  encodeLookupTable:  Pointer to the Encoding Lookup Table
 *  \param  decodeLookupTable:  Pointer to the Decodification Lookup Table
 *
 *  \return The result of the file operations inside the Test
 */
size_t LookUpTest (uint8_t mode, GolayCW * encodeLookupTable, uint32_t * decodeLookUpTable);

/*!\brief	This is the function used to inject errors into
 *        a file.
 *
 * \param	src:		path to the file.
 * \param	mode:		'power' of added noise to the file
 * 						0: about 0.5  bit error per 1 byte
 *						1: about 1    bit error per 1 byte
 *						2: about 1.4 bit error per 1 byte
 *						3: about 1.5  bit error per 1 byte
 * \retval				number of injected errors
 */
int32_t InjectErrorsFile (char *src, uint8_t mode);

#ifdef __unix__
/** \brief Function to compute the difference between to timespec structures.
 *
 *  The timespec structure is a time.h sctructure used to get high resolution times by using one of the system's
 *  timers, such as the System wide Real Time Clock. Since the structure has a variable for the seconds and a
 *  variable for the nanoseconds we need a specific function to get the difference between two high resolution
 *  times.
 *
 *  \param  begin:  Beginning time structure
 *  \param  end:    Ending time structure
 *
 *  \return The difference between the two times
 */
struct timespec ClockDifference (struct timespec begin, struct timespec end);
#endif

#endif // TEST_H_INCLUDED
