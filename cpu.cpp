#include <fstream>
#include "cpu.h"
#include "log.h"
#include "font.h"
#include "defines.h"

chip8::CPU::CPU() : program_counter(START_ADDRESS)  {
	log_subsys::Init();
	for (size_t i = 0; i < 0x10; ++i) {
		table[i] = &CPU::OP_NOP;
	}
	for(size_t i = 0; i < 0xF; ++i) {
		table0[i] = &CPU::OP_NOP;
	}
	for (size_t i = 0; i < 0xF; ++i) {
		tableE[i] = &CPU::OP_NOP;
	}
	for (size_t i = 0; i < 0x66; ++i) {
		tableF[i] = &CPU::OP_NOP;
	}
	
	table[0x0] = &CPU::Table0;
	table[0x1] = &CPU::OP_1nnn;
	table[0x2] = &CPU::OP_2nnn;
	table[0x3] = &CPU::OP_3xkk;
	table[0x4] = &CPU::OP_4xkk;
	table[0x5] = &CPU::OP_5xy0;
	table[0x6] = &CPU::OP_6xkk;
	table[0x7] = &CPU::OP_7xkk;
	table[0x8] = &CPU::Table8;
	table[0x9] = &CPU::OP_9xy0;
	table[0xA] = &CPU::OP_Annn;
	table[0xB] = &CPU::OP_Bnnn;
	table[0xC] = &CPU::OP_Cxkk;
	table[0xD] = &CPU::OP_Dxyn;
	table[0xE] = &CPU::TableE;
	table[0xF] = &CPU::TableF;

	table0[0x0] = &CPU::OP_00E0;
	table0[0xE] = &CPU::OP_00EE;

	table8[0x0] = &CPU::OP_8xy0;
	table8[0x1] = &CPU::OP_8xy1;
	table8[0x2] = &CPU::OP_8xy2;
	table8[0x3] = &CPU::OP_8xy3;
	table8[0x4] = &CPU::OP_8xy4;
	table8[0x5] = &CPU::OP_8xy5;
	table8[0x6] = &CPU::OP_8xy6;
	table8[0x7] = &CPU::OP_8xy7;
	table8[0xE] = &CPU::OP_8xyE;

	tableE[0x1] = &CPU::OP_ExA1;
	tableE[0xE] = &CPU::OP_Ex9E;

	tableF[0x07] = &CPU::OP_Fx07;
	tableF[0x0A] = &CPU::OP_Fx0A;
	tableF[0x15] = &CPU::OP_Fx15;
	tableF[0x18] = &CPU::OP_Fx18;
	tableF[0x1E] = &CPU::OP_Fx1E;
	tableF[0x29] = &CPU::OP_Fx29;
	tableF[0x33] = &CPU::OP_Fx33;
	tableF[0x55] = &CPU::OP_Fx55;
	tableF[0x65] = &CPU::OP_Fx65;
	
	C8_INFO("STARTING CPU AT ADDRESS {}...", reinterpret_cast<void*>(this));
	C8_INFO("PC: {}", program_counter);
	for (size_t i = 0; i < FONTSET_SZ; ++i) {
		memory.at(FONTSET_START + i) = fontset[i];
	}
}


void chip8::CPU::LoadROM(const std::string& filename) {
	const auto buffer = new char[8192];
	std::ifstream file(filename, std::ios::binary);
	file.rdbuf()->pubsetbuf(buffer, 8192);
	C8_INFO("Opening file {}", filename);
	if (file.is_open()) {
		std::string file_contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		for (size_t i = 0; i < file_contents.size(); ++i) {
			memory.at(START_ADDRESS + i) = file_contents.at(i);
		}
		file.close();
		C8_INFO("File loaded sucessfully!");
	} else {
		file.close();
		C8_ERROR("FAILED TO OPEN FILE {}, BAILING OUT!", filename);
		delete[] buffer;
		exit(0);
	}
	delete[] buffer;
}
