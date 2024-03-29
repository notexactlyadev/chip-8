#pragma once
#include <array>
#include <string>
#include "defines.h"
#include "random_gen.h"

namespace chip8 {
	class CPU {
	private:
		u8 stack_pointer{0};
		u8 delayTimer{0};
		u8 soundTimer{0};
		u16 index{0};
		u16 program_counter{0};
		u16 opcode;
		std::array<u8, 16> V{};
		std::array<u8, 4096> memory{};
		std::array<u16, 16> stack{};

		size_t VIDEO_WIDTH = 64, VIDEO_HEIGHT = 32;
		random_gen rnd;
	public:
		std::array<u32, 64 * 32> video{};
		std::array<u8, 16> keys{};
		CPU();
		void LoadROM(const std::string&);
		void OP_00E0();
		void OP_00EE();
		void OP_1nnn();
		void OP_2nnn();
		void OP_3xkk();
		void OP_4xkk();
		void OP_5xy0();
		void OP_6xkk();
		void OP_7xkk();
		void OP_8xy0();
		void OP_8xy1();
		void OP_8xy2();
		void OP_8xy3();
		void OP_8xy4();
		void OP_8xy5();
		void OP_8xy6();
		void OP_8xy7();
		void OP_8xyE();
		void OP_9xy0();
		void OP_Annn();
		void OP_Bnnn();
		void OP_Cxkk();
		void OP_Dxyn();
		void OP_Ex9E();
		void OP_ExA1();
		void OP_Fx07();
		void OP_Fx0A();
		void OP_Fx15();
		void OP_Fx18();
		void OP_Fx1E();
		void OP_Fx29();
		void OP_Fx33();
		void OP_Fx55();
		void OP_Fx65();
		void OP_NOP();
		typedef void(CPU::* FN_PTR)();

		
		FN_PTR table[0x10]{  };
		FN_PTR table0[0xF]{  };
		FN_PTR table8[0xF]{  };
		FN_PTR tableE[0xF]{  };
		FN_PTR tableF[0x66]{  };

		void Table0();

		void Table8();

		void TableE();

		void TableF();

		void DoCycle();
	};
}
