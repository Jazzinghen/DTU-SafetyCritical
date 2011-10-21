#include "headers/utils.h"

void Encode(uint8_t parity_mode, GolayCW *CodeWord) {
	CodeWord->cw.check = GetSyndrome(CodeWord->cw.data)&0x7ff;
	if (parity_mode == GOLAY_24){
		CodeWord->cw.parity = GetParity(CodeWord->CodeWord);
  }

}

size_t ComputeELT(uint8_t mode, GolayCW * LookupTable) {
  uint16_t fakeData = 0;
  uint16_t i = 0;
  uint16_t j = 0;
  FILE *LTFile;
  size_t res;

  union {
    uint8_t bytes[4];
    uint32_t cw;
  } tempData;

  GolayCW tempCW;

  uint8_t data [4096 * 3];

  LTFile = fopen(ELT_FILE_NAME, "r");

  if (LTFile != NULL) {
    printf("Now reading Lookup Table: ");
    res = fread(data, sizeof(uint8_t) * 3, 4096, LTFile);
    for (i = 0; i < 4096; i++){
      j = i*3;
      tempData.bytes[0] = data[j];
      tempData.bytes[1] = data[j+1];
      tempData.bytes[2] = data[j+2];
      LookupTable[i].CodeWord = tempData.cw;
      if (mode == GOLAY_23) {
        LookupTable[i].cw.parity = 0;
      }
      if ((i%256) == 0) {
        printf (".");
      }
    }
    printf (" Done.\n");
  } else {
    printf("Now generating Lookup Table: ");
    LTFile = fopen(ELT_FILE_NAME, "w");
    for (fakeData = 0; fakeData < 4096; fakeData++) {
      tempData.cw = 0;
      tempCW.CodeWord = 0;
      j = fakeData * 3;
      tempCW.cw.data = fakeData;
      Encode(GOLAY_24, &tempCW);
      LookupTable[fakeData].CodeWord = tempCW.CodeWord;
      tempData.cw = tempCW.CodeWord;
      data[j] = tempData.bytes[0];
      data[j+1] = tempData.bytes[1];
      data[j+2] = tempData.bytes[2];

      if ((fakeData%256) == 0) {
        printf (".");
      }
    }
    res = fwrite(data, sizeof(uint8_t) * 3, 4096, LTFile);
    printf (" Done.\n");
  }

  return res;
}
