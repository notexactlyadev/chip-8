#pragma once

using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;

namespace chip8 {
	constexpr u32 FONTSET_SZ = 80;
	constexpr size_t FONTSET_START = 0x50;
	constexpr u32 START_ADDRESS = 0x200;

	constexpr int SCREEN_WIDTH = 64;
	constexpr int SCREEN_HEIGHT = 32;
}

