#include <stdio.h>
#include <stdlib.h>

#include "headers/utils.h"
#include "headers/encoder.h"
#include "headers/decoder.h"

uint8_t Test(uint32_t x, GolayCW * EncodeLookupTable, uint32_t * DecodeLookUpTable) {
	uint8_t i;
	uint32_t error_mask_arr[] = {0x00, 0x01,0x03,0x07, 0x0f};
	uint32_t error_mask;
	uint8_t error_status;

	GolayCW CodeWord1, CodeWord2;
	CodeWord1.CodeWord = x;
	CodeWord2.CodeWord = x;
	EncodeLT (GOLAY_24, &CodeWord1, EncodeLookupTable);
	CodeWord2.CodeWord = CodeWord1.CodeWord;

	for(i = 1; i <= 3; i++) {
		for (error_mask = error_mask_arr[i]; error_mask<0x800000; error_mask=NextBitPermutation(error_mask)) {
			CodeWord1.CodeWord = CodeWord2.CodeWord ^ error_mask;

      DecodeLT (GOLAY_24, &CodeWord1, DecodeLookUpTable);
			//error_status = Correction(GOLAY_23, &CodeWord1);
			if(CodeWord2.CodeWord ^ CodeWord1.CodeWord) {
        		printf ("Mask: %x Something went wrong. Status: %d\n", error_mask, error_status);
        		PrintBinary(CodeWord2.CodeWord ^ CodeWord1.CodeWord);
				return ALGORITHM_IS_NOT_CORRECT;
			}
		}
	}
	return ALGORITHM_IS_CORRECT;
}

int main(int argc, char** argv) {
#ifdef __unix__
	printf("aaaaaaaaaaaa\n");
#endif

	uint16_t i;
	GolayCW cw,cwlt;
	cw.CodeWord = 0;
	uint32_t tempSyndrome = 0;
#ifdef __unix__
  struct timespec startTime, endTime, timeElapsed;
#endif
	GolayCW encLookUp[4096];
	uint32_t decLookUp[2048];

  ComputeELT(GOLAY_24, encLookUp);
  ComputeDLT(decLookUp);
#ifdef __unix__
  printf("Beginning tests. Encoding data from 0x0 to 0xfff and then injecting all the possible errors.\n");

  clock_gettime(CLOCK_REALTIME, &startTime);
#endif
	for(i = 0x000; i<=0xfff; i++) {
		//Test(i, encLookUp, decLookUp);
	}
#ifdef __unix__
	clock_gettime(CLOCK_REALTIME, &endTime);

  timeElapsed = ClockDifference(startTime, endTime);

	printf("Tests completed. Time elapsed: ");
#endif
	getchar();
	/*for(i = 0x000; i<=0x0ff; i++) {
		cw.CodeWord = i;
		cwlt.CodeWord = i;

		Encode(GOLAY_24, &cw);
		EncodeLT(GOLAY_24, &cwlt, encLookUp);
		PrintBinary(cw.CodeWord^cwlt.CodeWord);
		//printf("%d\n", Correction(GOLAY_24, &cw));
	}
	//getchar();


	printf("Enc:%d\n", EncodeFile("aaa.txt", "bbb.txt", GOLAY_24));
	printf("Dec:%d\n", DecodeFile("bbb.txt", "ccc.txt", GOLAY_24));
	getchar();
	for(i = 0x000; i<=0x0ff; i++) {
		Test(i);
		*/
	/*for(i = 0x000; i<=0xfff; i++) {
		cw.CodeWord = i;
		cwlt.CodeWord = i;

		Encode(GOLAY_24, &cw);
		EncodeLT(GOLAY_24, &cwlt, encLookUp);

		if(cwlt.CodeWord ^ cw.CodeWord) {
			puts("err");
		}
	}
	printf("ok\n");*/

	cw.cw.data = 0x555;
	PrintBinary(cw.CodeWord);
	Encode(GOLAY_24, &cw);
	printf("The On-The-Fly Enconding:\n");
	PrintBinary(cw.CodeWord);
	printf("The LUT Enconding:\n");
	PrintBinary(encLookUp[cw.cw.data].CodeWord);

  printf("\nNow Testing Syndrome LUT! :D\n");

  cw.cw.parity=0;

  cwlt.CodeWord = cw.CodeWord;

	cw.cw.data = 0x555^0x1105;
	PrintBinary(cw.CodeWord);


	tempSyndrome = GetSyndrome(cw.CodeWord);
	printf("Ok, this is the computed difference: \n");
	PrintBinary(cwlt.CodeWord ^ cw.CodeWord);

	printf("While THIS is the one gotten from the LookUpTable:\n");
	//PrintBinary(tempSyndrome);
	//printf("The syndrome: %d\n", tempSyndrome);
	PrintBinary(decLookUp[tempSyndrome]);

  cwlt.CodeWord = cw.CodeWord;
	printf ("%d\n After Correction:\n", Correction(GOLAY_24, &cw));
	PrintBinary(cw.CodeWord);
	PrintBinary(cwlt.CodeWord ^ decLookUp[tempSyndrome]);


	//getchar();

	return (EXIT_SUCCESS);
}
