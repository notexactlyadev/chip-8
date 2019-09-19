#include "random_gen.h"


chip8::random_gen::random_gen() {
	random_engine = std::mt19937_64(rddev());
	distribution = std::uniform_int_distribution<uint16_t>(0, 255U);
}

uint8_t chip8::random_gen::get() {
	return distribution(random_engine);
}