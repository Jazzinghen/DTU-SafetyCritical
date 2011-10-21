#include "headers/utils.h"

uint32_t Encode(uint8_t parity_mode, GolayCW *CodeWord) {
	CodeWord->cw.check = GetSyndrome(CodeWord->cw.data)&0x7ff;
	if (parity_mode == GOLAY_24){
		CodeWord->cw.parity = GetParity(CodeWord->CodeWord);
	}
}
