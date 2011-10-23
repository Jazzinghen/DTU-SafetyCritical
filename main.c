#include <stdio.h>
#include <stdlib.h>

#include "headers/utils.h"
#include "headers/encoder.h"
#include "headers/decoder.h"
#include "headers/test.h"


int main(int argc, char** argv) {

	uint16_t i;
	uint32_t test_run = 0;
  double totalAverage = 0;


	printf("Enc:%d\n", EncodeFile("aaa.txt", "bbb.txt", GOLAY_24));
	printf("Number of errors injected: %d\n", InjectErrorsFile("bbb.txt", 0));
	printf("Dec:%d\n", DecodeFile("bbb.txt", "ccc.txt", GOLAY_24));

#ifdef __unix__
  struct timespec startTime, endTime, timeElapsed;
  double average;
#endif

	GolayCW encLookUp[4096];
	uint32_t decLookUp[2048];

  printf("Begin LookupTables tests.\n");

  for (test_run = 0; test_run < LOOKUP_QUANTITY; test_run++) {
#ifdef __unix__
  clock_gettime(CLOCK_REALTIME, &startTime);
#endif

  LookUpTest(LOAD_OR_REMOVE, encLookUp, decLookUp);

#ifdef __unix__
	clock_gettime(CLOCK_REALTIME, &endTime);

  timeElapsed = ClockDifference(startTime, endTime);

  average += timeElapsed.tv_sec * SEC_PER_NSEC + timeElapsed.tv_nsec;

	//printf("Tests completed. Time elapsed:\n\t%d seconds\n\t%ld nanoseconds.\n", (int32_t)timeElapsed.tv_sec, timeElapsed.tv_nsec);
#endif
  }

  printf("Average Lookup Tables generation time:\n\t%f Nanoseconds.\n", average/LOOKUP_QUANTITY);

  average = 0;

  for (test_run = 0; test_run < TEST_QUANTITY; test_run++){

#ifdef __unix__
  printf("Beginning test %i. Encoding data from 0x0 to 0xfff and then injecting all the possible errors.\n", test_run+1);

  clock_gettime(CLOCK_REALTIME, &startTime);
#endif

	for(i = 0x000; i <= TEST_TARGET; i++) {
	  //printf("Testing codeword %X: ", i);
		TestRun(i, ONTHEFLY_TEST, encLookUp, decLookUp);
		//printf(" Done.\n");
	}

#ifdef __unix__
	clock_gettime(CLOCK_REALTIME, &endTime);

  timeElapsed = ClockDifference(startTime, endTime);

	printf("Tests completed. Time elapsed:\n\t%d seconds\n\t%ld nanoseconds.\n", (int32_t)timeElapsed.tv_sec, timeElapsed.tv_nsec);

	average = (((double)((timeElapsed.tv_sec*SEC_PER_NSEC) + timeElapsed.tv_nsec))/TEST_TARGET);
	totalAverage += average;
	printf("That means that this takes:\n%f nanoseconds per word.\n", (average/2048));
#endif
  }

  printf("Average time per word over %i runs:\n%f nanoseconds per word.\n", TEST_QUANTITY, totalAverage/(TEST_QUANTITY * 2048));

	return (EXIT_SUCCESS);
}
