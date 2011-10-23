#ifndef TEST_H_INCLUDED
#define TEST_H_INCLUDED

#include <time.h>

#include "utils.h"

#define TEST_QUANTITY       10
#define TEST_TARGET         0xFFF
#define SEC_PER_NSEC        1000000000

#define LOOKUP_TEST         0
#define ONTHEFLY_TEST       1

#define LOAD_OR_REMOVE      0
#define ALWAYES_REMOVE      1



uint8_t TestRun (uint32_t data, uint8_t mode, GolayCW * encodeLookupTable, uint32_t * decodeLookUpTable);

int16_t LookUpTest (uint8_t mode, GolayCW * encodeLookupTable, uint32_t * decodeLookUpTable);

#ifdef __unix__
struct timespec ClockDifference (struct timespec begin, struct timespec end);
#endif

#endif // TEST_H_INCLUDED
