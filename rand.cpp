#include "random_c8.h"

random_c8::random_c8() {
	random_engine = std::mt19937_64(rddev());
}

uint8_t random_c8::get() {
	return distribution(random_engine);
}