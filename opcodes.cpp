#include <iostream>
#include "opcodes.h"
#include "cpu.h"
#include "log.h"
#include <sstream>

#define Vx ((u8)((opcode & 0x0F00U) >> 8U))
#define Vy ((u8)((opcode & 0x00F0U) >> 4U))

const char* DEBUG_STR = "PC: {}\t Opcode: {}\t FN: {}";

#define CALL_FN(tableName, AND_CONSTANT) { (*this.*tableName[opcode & AND_CONSTANT])(); }

template <typename T, typename = std::enable_if<std::is_integral_v<T> > >
inline static std::string hex_transform(const T& number) {
	char buffer[24];
#ifdef _MSC_VER
	sprintf_s(buffer, "0x%X", number);
#else
	sprintf(buffer, "0x%X", number);
#endif
	return { buffer };
}

#define LOG_EXECUTION { C8_TRACE(DEBUG_STR, program_counter, hex_transform(opcode), __FUNCTION__); }

namespace chip8 {


void CPU::OP_NOP() { return; }

void CPU::Table0() {
	CALL_FN(table0, 0x000FU);
}

void CPU::Table8() {
	CALL_FN(table8, 0x000FU);
}

void CPU::TableE() {
	CALL_FN(tableE, 0x000FU);
}

void CPU::TableF() {
	const size_t fn = opcode & 0x00FFU;
	if (fn < 102) {
		(*this.*tableF[fn])();
	}
}

void CPU::DoCycle() {
	try {
		opcode = (memory.at(program_counter) << 8U) | memory.at(program_counter + 1);
	} catch (std::exception& ex) {
		C8_ERROR("EXCEPTION IN DoCycle(): {}, CONTINUING!", ex.what());
	}
	program_counter += 2;
	const size_t fn = (opcode & 0xF000U) >> 12U;
	(*this.*table[fn])();
	if (delayTimer > 0) {
		--delayTimer;
	}
	if (soundTimer > 0) {
		--soundTimer;
	}
}

void CPU::OP_00E0() {
	video.fill(0);
}

void CPU::OP_00EE() {
	LOG_EXECUTION;
	--stack_pointer;
	program_counter = stack.at(stack_pointer);
}

void CPU::OP_1nnn() {
	LOG_EXECUTION;
	program_counter = opcode & 0x0FFFU;
}

void CPU::OP_2nnn() {
	LOG_EXECUTION;
	stack.at(stack_pointer) = program_counter;
	++stack_pointer;
	program_counter = opcode & 0x0FFFU;
}

void CPU::OP_3xkk() {
	LOG_EXECUTION;
	if (V.at(Vx) == static_cast<u8>(opcode & 0x00FFU)) {
		program_counter += 2;
	}
}

void CPU::OP_4xkk() {
	LOG_EXECUTION;
	if(V.at(Vx) != static_cast<u8>(opcode & 0x00FFU)) {
		program_counter += 2;
	}
}

void CPU::OP_5xy0() {
	LOG_EXECUTION;
	if(V.at(static_cast<u8>((opcode & 0x0F00U) >> 8U)) == V.at(static_cast<u8>((opcode & 0x0F00U) >> 4U))) {
		program_counter += 2;
	}
}

void CPU::OP_6xkk() {
	LOG_EXECUTION;
	V.at(Vx) = static_cast<u8>(opcode & 0x00FFU);
}

void CPU::OP_7xkk() {
	LOG_EXECUTION;
	V.at(Vx) += static_cast<u8>(opcode & 0x00FFU);
}

void CPU::OP_8xy0() {
	LOG_EXECUTION;
	V.at(Vx) = V.at(Vy);
}


void CPU::OP_8xy1() {
	LOG_EXECUTION;
	V.at(Vx) |= V.at(Vy);
}

void CPU::OP_8xy2() {
	LOG_EXECUTION;
	V.at(Vx) &= V.at(Vy);
}

void CPU::OP_8xy3() {
	LOG_EXECUTION;
	V.at(Vx) ^= V.at(Vy);
}

void CPU::OP_8xy4() {
	LOG_EXECUTION;
	const u16 sum = V.at(Vx) + V.at(Vy);
	if (sum > 0xFFU) {
		V.at(0xF) = true;
	}
	else {
		V.at(0xF) = false;
	}

	V.at(Vx) = sum & 0xFFU;
}

void CPU::OP_8xy5() {
	LOG_EXECUTION;
	if (V.at(Vx) > V.at(Vy)) {
		V.at(0xF) = true;
	} else {
		V.at(0xF) = false;
	}
	V.at(Vx) -= V.at(Vy);
}

void CPU::OP_8xy6() {
	LOG_EXECUTION;
	V.at(0xF) = V.at(Vx) & true;
	V.at(Vx) >>= 1;
}

void CPU::OP_8xy7() {
	LOG_EXECUTION;
	if(V.at(Vy) > V.at(Vx)) {
		V.at(0xF) = true;
	} else {
		V.at(0xF) = false;
	}

	V.at(Vx) = V.at(Vy) - V.at(Vx);
}

void CPU::OP_8xyE() {
	LOG_EXECUTION;
	V.at(0xF) = (V.at(Vx) & 0x80U) >> 7U;
	V.at(Vx) <<= 1;
}

void CPU::OP_9xy0() {
	LOG_EXECUTION;
	if (V.at(Vx) != V.at(Vy)) {
		program_counter += 2;
	}
}

void CPU::OP_Annn() {
	LOG_EXECUTION;
	index = opcode & 0x0FFFU;
}

void CPU::OP_Bnnn() {
	LOG_EXECUTION;
	program_counter = V.at(0) + (opcode & 0x0FFFU);
}

void CPU::OP_Cxkk() {
	LOG_EXECUTION;
	V.at(Vx) = rnd.get() & (opcode & 0x00FFU);
}

void CPU::OP_Dxyn() {
	LOG_EXECUTION;
	const u8 height = opcode & 0x000FU;

	const u8 xPos = V.at(Vx) % VIDEO_WIDTH;
	const u8 yPos = V.at(Vy) % VIDEO_HEIGHT;

	V.at(0xF) = false;

	for (size_t row = 0; row < height; ++row) {
		const auto sprByte = memory.at(index + row);
		for(size_t col = 0; col < 8; ++col) {
			const u8 sprPixel = sprByte & (0x80U >> col);
			const size_t vidpos = (yPos + row) * VIDEO_WIDTH + (xPos + col);
			if (vidpos >= 64 * 32) {
				continue;
			}
			auto* scrPixelPTR = &video.at(vidpos);
			if(sprPixel) {
				if (*scrPixelPTR == 0xFFFFFFFF) {
					V.at(0xF) = true;
				}

				*scrPixelPTR ^= 0xFFFFFFFF;
			}
		}
	}
}

void CPU::OP_Ex9E() {
	LOG_EXECUTION;
	const auto key = V.at(Vx);
	if (keys.at(key)) {
		program_counter += 2;
	}
}

void CPU::OP_ExA1() {
	LOG_EXECUTION;
	const auto key = V.at(Vx);
	if (!keys.at(key)) {
		program_counter += 2;
	}
}

void CPU::OP_Fx07() {
	LOG_EXECUTION;
	V.at(Vx) = delayTimer;
}

void CPU::OP_Fx0A() {
	LOG_EXECUTION;
	if		(keys.at(0x0)) { V.at(Vx) = 0x0; }
	else if (keys.at(0x1)) { V.at(Vx) = 0x1; }
	else if (keys.at(0x2)) { V.at(Vx) = 0x2; }
	else if (keys.at(0x3)) { V.at(Vx) = 0x3; }
	else if (keys.at(0x4)) { V.at(Vx) = 0x4; }
	else if (keys.at(0x5)) { V.at(Vx) = 0x5; }
	else if (keys.at(0x6)) { V.at(Vx) = 0x6; }
	else if (keys.at(0x7)) { V.at(Vx) = 0x7; }
	else if (keys.at(0x8)) { V.at(Vx) = 0x8; }
	else if (keys.at(0x9)) { V.at(Vx) = 0x9; }
	else if (keys.at(0xA)) { V.at(Vx) = 0xA; }
	else if (keys.at(0xB)) { V.at(Vx) = 0xB; }
	else if (keys.at(0xC)) { V.at(Vx) = 0xC; }
	else if (keys.at(0xD)) { V.at(Vx) = 0xD; }
	else if (keys.at(0xE)) { V.at(Vx) = 0xE; }
	else if (keys.at(0xF)) { V.at(Vx) = 0xF; }
	else { program_counter -= 2;  }
}

void CPU::OP_Fx15() {
	LOG_EXECUTION;
	delayTimer = V.at(Vx);
}


void CPU::OP_Fx18() {
	LOG_EXECUTION;
	soundTimer = V.at(Vx);
}

void CPU::OP_Fx1E() {
	LOG_EXECUTION;
	index += V.at(Vx);
}

void CPU::OP_Fx29() {
	LOG_EXECUTION;
	index = FONTSET_START + (V.at(Vx) * 5);
}

void CPU::OP_Fx33() {
	LOG_EXECUTION;
	auto value = V.at(Vx);
	memory.at(2 + index) = value % 10;
	value /= 10;

	memory.at(1 + index) = value % 10;

	value /= 10;

	memory.at(index) = value % 10;
}

void CPU::OP_Fx55() {
	LOG_EXECUTION;
	for (u8 i = 0; i <= Vx; ++i) {
		memory.at(i + index) = V.at(i);
	}
}

void CPU::OP_Fx65() {
	LOG_EXECUTION;
	for (u8 i = 0; i <= Vx; ++i) {
		V.at(i) = memory.at(i + index);
	}
}

}
