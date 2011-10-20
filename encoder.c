#include "headers/utils.h"

uint32_t Encode(GolayCW *CodeWord) {
	CodeWord->cw.check = GetSyndrome(CodeWord->cw.data)&0x7ff;
}
