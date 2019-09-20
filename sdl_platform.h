#pragma once
#include  "cpu.h"
#include <SDL.h>

namespace chip8 {
	class SDL_Platform
	{
	public:

		SDL_Platform(char const*, int, int);

		~SDL_Platform();

		void Update(void const*, int) const;

		static bool ProcessInput(uint8_t*);

	private:
		SDL_Window* window{};
		SDL_Renderer* renderer{};
		SDL_Texture* texture{};
		
	};
}
