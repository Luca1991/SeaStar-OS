
#include <stdint.h>



//! Set bit
inline uint8_t SetBit(uint8_t word, uint8_t mask) {
	return word | mask;
}

//! Clears bit
inline uint8_t ClrBit(uint8_t word, uint8_t mask) {
	return word & (~mask);
}

//! Assigns the bit defined in mask to a value
inline uint8_t AssignBit(uint8_t word, uint8_t mask, bool value) {
	return value ? SetBit(word, mask) : ClrBit(word, mask);
}

