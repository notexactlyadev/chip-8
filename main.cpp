#include <chrono>
#include <iostream>
#include <string>
#include <memory>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include "sdl_platform.h"
#include "log.h"

#define VIDEO_SCALE 20

int main(const int argc, char** argv)
{
	if (argc != 3) {
		fprintf(stderr, "Usage: ./chip8 <Timing> <ROM Path\\ROMName>\n");
		fprintf(stderr, "Timing should be from 0 to 1000 miliseconds, any lower than 0 or higher than 1000 value will be ignored.");
		fprintf(stderr, "Example: ./chip8 10 ./TETRIS -- to launch tetris with a timing of 10ms\n");
		exit(EXIT_FAILURE);
	}
	std::cout << argv[0] << '\n';

	int cycleDelay = std::stof(argv[1]);
	if (cycleDelay < 0) {
		cycleDelay = 1;
	} else if (cycleDelay > 1000) {
		cycleDelay = 1000;
	}
	
	char const* romFilename = argv[2];

	auto platform = std::make_unique<chip8::SDL_Platform>("CHIP-8 Emulator", chip8::SCREEN_WIDTH * VIDEO_SCALE, chip8::SCREEN_HEIGHT * VIDEO_SCALE);
	auto chip8 = std::make_unique<chip8::CPU>();
	C8_INFO("CYCLE DELAY: {}", cycleDelay);
	chip8->LoadROM(romFilename);

	const int videoPitch = sizeof(chip8->video.at(0)) * chip8::SCREEN_WIDTH;

	auto lastCycleTime = std::chrono::high_resolution_clock::now();
	bool quit = false;

	while (!quit)
	{
		quit = platform->ProcessInput(chip8->keys);

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
