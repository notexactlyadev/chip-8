#pragma once
#include <cstdint>
#include <random>

namespace chip8 {
	class random_gen
	{
	private:
		std::random_device rddev;
		std::mt19937_64 random_engine;
		std::uniform_int_distribution<uint16_t> distribution;
	public:
		random_gen();
		uint8_t get();
	};
}

