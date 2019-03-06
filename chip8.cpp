#include "chip8.h"

class chip8 {
	void chip8::initialize() {
		I = 0;
		pc = 0x200;
		sp = 0;
		opcode = 0;
		memset(memory, 0, sizeof(memory));
		memset(V, 0, sizeof(V));
		memset(key, 0, sizeof(key));

		for (int i = 0; i < 80; ++i) {
			memory[i] = chip8_fontset[i];
		}

		sound_timer = 0;
		delay_timer = 0;
	}

	void chip8::emulateCycle() {
		opcode = memory[pc] << 8 | memory[pc + 1];
		decode(opcode);
		if (delay_timer) delay_timer--;
		if (sound_timer) sound_timer--;
		else printf("BEEP!\n");

	}

	void chip8::loadProgram(char* game) {
	}
	void chip8::decode(unsigned short opcode) {
		switch(opcode & 0xF000) {
			case 0x0000:
				switch (opcode & 0x00FF) {
					case 0x00E0:
						pc = stack[--sp];
					break;
					case 0x00EE:
						memset(gfx, 0, sizeof(gfx));
					break;
					default:
						// 0NNN
					break;
				} 
			break;
			case 0x1000:
				pc = opcode & 0x0FFF;
			break;
			case 0x2000:
				stack[sp++] = pc;
				pc = opcode & 0x0FFF;
			break;
			case 0x3000:
				if (V[(opcode >> 8) & 0x000F] == opcode & 0x00FF) {
					pc = pc + 4;
				}
			break;
			case 0x4000:
				if (V[(opcode >> 8) & 0x000F] != opcode & 0x00FF) {
					pc = pc + 4;
				}
			break;
			case 0x5000:
				if (V[(opcode >> 8) & 0x000F] == V[(opcode >> 4) & 0x000F]) {
					pc = pc + 4;
				}
			break;
			case 0x6000:
				V[(opcode >> 8) & 0x000F] = opcode & 0x00FF;
			break;
			case 0x7000:
				V[(opcode >> 8) & 0x000F] += opcode & 0x00FF;
			break;
			case 0x8000:
				switch (opcode & 0x000F) {
					case 0x0000:
						V[(opcode >> 8) & 0x000F] = V[(opcode >> 4) & 0x000F];
					break;
					case 0x0001:
						V[(opcode >> 8) & 0x000F] |= V[(opcode >> 4) & 0x000F];
					break;
					case 0x0002:
						V[(opcode >> 8) & 0x000F] &= V[(opcode >> 4) & 0x000F];
					break;
					case 0x0003:
						V[(opcode >> 8) & 0x000F] ^= V[(opcode >> 4) & 0x000F];
					break;
					case 0x0004:
						V[(opcode >> 8) & 0x000F] += V[(opcode >> 4) & 0x000F];	
						V[0xFF] = !!(V[(opcode >> 8) & 0x000F] % 0xFF);
						V[(opcode >> 8 & 0x000F] %= 0xFF;
					break;
					case 0x0005:
						V[0xF] = (V[(opcode >> 4) & 0x000F] > V[(opcode >> 4) & 0x000F])
						V[(opcode >> 8) & 0x000F] -= V[(opcode >> 4) & 0x000F];	
					break;
					case 0x0006:
						V[0xF] = V[(opcode >> 8) & 0x000F] & 1;
						V[(opcode >> 8) & 0x000F] >>= 1;
					break;
					case 0x0007:
						V[0xF] = (V[(opcode >> 4) & 0x000F] > V[(opcode >> 4) & 0x000F])
						V[(opcode >> 8) & 0x000F] = V[(opcode >> 4) & 0x000F] - V[(opcode >> 8) & 0x000F];
					break;
					case 0x000E:
						V[(opcode >> 8) & 0x000F] <<= 1;
					break;
				}
			break;
			case 0x9000:
			break;
			case 0xA000:
			break;
			case 0xB000:
			break;
			case 0xC000:
			break;
			case 0xD000:
			break;
			case 0xE000:
				switch (opcode & 0x000F) {
					case 0x000E:
					break;
					case 0x0001:
					break;
				}
			break;
			case 0xF000:
				switch (opcode & 0x00FF) {
					case 0x0007:
					break;
					case 0x000A:
					break;
					case 0x0015:
					break;
					case 0x0018:
					break;
					case 0x001E:
					break;
					case 0x0029:
					break;
					case 0x0033:
					break;
					case 0x0055:
					break;
					case 0x0065:
					break;
				}
			break;
			default:
				printf("Uknown opcode.\n");
			}
		}
	}
}
		
