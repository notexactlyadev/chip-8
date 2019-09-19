#include <chrono>
#include <iostream>
#include <string>
#include <memory>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include "sdl_platform.h"

#define VIDEO_SCALE 20

int main(const int argc, char** argv)
{

	std::cout << argv[0] << '\n';

	const int cycleDelay = std::stoi(argv[1]);
	char const* romFilename = argv[2];

	auto platform = std::make_unique<chip8::SDL_Platform>("CHIP-8 Emulator", chip8::SCREEN_WIDTH * VIDEO_SCALE, chip8::SCREEN_HEIGHT * VIDEO_SCALE);
	auto chip8 = std::make_unique<chip8::CPU>();
	chip8->LoadROM(romFilename);

	const int videoPitch = sizeof(chip8->video.at(0)) * chip8::SCREEN_WIDTH;

	auto lastCycleTime = std::chrono::high_resolution_clock::now();
	bool quit = false;

	while (!quit)
	{
		quit = platform->ProcessInput(&chip8->keys.at(0));

		auto currentTime = std::chrono::high_resolution_clock::now();
		const float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

		if (dt > cycleDelay)
		{
			lastCycleTime = currentTime;

			chip8->DoCycle();

			platform->Update(&chip8->video.at(0), videoPitch);
		}
	}

	return 0;
}
