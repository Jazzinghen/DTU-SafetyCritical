#include "headers/utils.h"

uint32_t Encode(uint32_t CodeWord) {
	return ((GetSyndrome(CodeWord)<<12)|CodeWord)
}
