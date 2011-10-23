#include <unistd.h>

#include "headers/test.h"
#include "headers/decoder.h"
#include "headers/encoder.h"

uint8_t TestRun (uint32_t data, uint8_t mode, GolayCW * encodeLookupTable, uint32_t * decodeLookUpTable) {
	uint8_t i;
	uint32_t test_no = 0;
	uint32_t error_mask_arr[] = {0x00, 0x01,0x03,0x07, 0x0f};
	uint32_t error_mask;
	uint8_t error_status = 0;

	GolayCW CodeWord1, CodeWord2;
	CodeWord1.CodeWord = data;
	CodeWord2.CodeWord = data;

	if (mode == LOOKUP_TEST) {
    EncodeLT (GOLAY_24, &CodeWord1, encodeLookupTable);
	} else {
    Encode(GOLAY_24, &CodeWord1);
	}

	CodeWord2.CodeWord = CodeWord1.CodeWord;

	for(i = 1; i <= 3; i++) {
		for (error_mask = error_mask_arr[i]; error_mask<0x800000; error_mask=NextBitPermutation(error_mask)) {
			test_no++;

			CodeWord1.CodeWord = CodeWord2.CodeWord ^ error_mask;
      if (mode == LOOKUP_TEST) {
        DecodeLT (GOLAY_24, &CodeWord1, decodeLookUpTable);
      } else {
        Correction (GOLAY_24, &CodeWord1);
      }

			if(CodeWord2.CodeWord ^ CodeWord1.CodeWord) {
        		printf ("Mask: %x Something went wrong. Status: %d\n", error_mask, error_status);
        		PrintBinary(CodeWord2.CodeWord ^ CodeWord1.CodeWord);
				return 1;
			}
		}
	}
	return 0;
}

int16_t LookUpTest (uint8_t mode, GolayCW * encodeLookupTable, uint32_t * decodeLookUpTable){
  int16_t res;

  if (mode == ALWAYES_REMOVE) {
    res = unlink(DLT_FILE_NAME);
    res = unlink(ELT_FILE_NAME);
  }

  ComputeELT(MESSAGES_OFF, GOLAY_24, encodeLookupTable);
  ComputeDLT(MESSAGES_OFF, decodeLookUpTable);

  return res;
}

#ifdef __unix__
struct timespec ClockDifference (struct timespec begin, struct timespec end) {
  struct timespec difference;

	if ((end.tv_nsec - begin.tv_nsec) < 0) {
		difference.tv_sec = end.tv_sec - begin.tv_sec - 1;
		difference.tv_nsec = 1000000000 + end.tv_nsec - begin.tv_nsec;
	} else {
		difference.tv_sec = end.tv_sec - begin.tv_sec;
		difference.tv_nsec = end.tv_nsec - begin.tv_nsec;
	}

	return difference;
}
#endif
