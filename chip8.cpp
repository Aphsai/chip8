#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cstring>

unsigned char chip8_fontset[80] =
{ 
    0xF0, 0x90, 0x90, 0x90, 0xF0, 
    0x20, 0x60, 0x20, 0x20, 0x70, 
    0xF0, 0x10, 0xF0, 0x80, 0xF0, 
    0xF0, 0x10, 0xF0, 0x10, 0xF0, 
    0x90, 0x90, 0xF0, 0x10, 0x10, 
    0xF0, 0x80, 0xF0, 0x10, 0xF0, 
    0xF0, 0x80, 0xF0, 0x90, 0xF0, 
    0xF0, 0x10, 0x20, 0x40, 0x40, 
    0xF0, 0x90, 0xF0, 0x90, 0xF0, 
    0xF0, 0x90, 0xF0, 0x10, 0xF0, 
    0xF0, 0x90, 0xF0, 0x90, 0x90, 
    0xE0, 0x90, 0xE0, 0x90, 0xE0, 
    0xF0, 0x80, 0x80, 0x80, 0xF0, 
    0xE0, 0x90, 0x90, 0x90, 0xE0, 
    0xF0, 0x80, 0xF0, 0x80, 0xF0, 
    0xF0, 0x80, 0xF0, 0x80, 0x80  
};

chip8::chip8() {}
chip8::~chip8() {}
void chip8::initialize() {
	I = 0;
	pc = 0x200;
	sp = 0;
	opcode = 0;

	memset(memory, 0, sizeof(memory));
	memset(V, 0, sizeof(V));
	memset(key, 0, sizeof(key));
	memset(gfx, 0, sizeof(gfx));

	for (int x = 0; x < 80; x++) {
		memory[x] = chip8_fontset[x];
	}

	sound_timer = 0;
	delay_timer = 0;

	drawFlag = true;
	srand (time(NULL));
}

void chip8::emulateCycle() {
	opcode = memory[pc] << 8 | memory[pc + 1];
	printf("%X\n", opcode);
	if (!decode(opcode)) return;
	if (delay_timer > 0) delay_timer--;
	if (sound_timer == 1) printf("BEEP.\n");
	if (sound_timer > 0) sound_timer--;

}

bool chip8::loadGame(const char* game) {
	initialize();
	printf("Loading... %s\n", game);
	FILE * pFile = fopen(game, "rb");
	if (pFile == NULL) {
		fputs ("File Error", stderr);
		return false;
	}

	fseek(pFile, 0, SEEK_END);
	long lSize = ftell(pFile);
	rewind(pFile);
	printf("Filesize: %d\n", (int)lSize);

	char *buffer = (char*)malloc(sizeof(char) *lSize);
	if (buffer == NULL) {
		fputs("Memory error", stderr);
		return false;
	}

	size_t result = fread (buffer, 1, lSize, pFile);
	if (result != lSize) {
		fputs("Reading error", stderr);
		return false;
	}
	if (4096 - 512 > lSize) {
		for (int x = 0; x < lSize; x++) {
			memory[x + 512] = buffer[x];
		}
	} else {
		printf("Error: ROM TOO BIG FOR MEMORY");
	}

	fclose(pFile);
	free(buffer);
	return true;
}
bool chip8::decode(unsigned short opcode) {
	switch(opcode & 0xF000) {
		case 0x0000:
			switch (opcode & 0x000F) {
				case 0x000E:
					pc = stack[--sp];
					pc += 2;
				break;
				case 0x0000:
					memset(gfx, 0, sizeof(gfx));
					drawFlag = true;
					pc += 2;
				break;
				default:
					printf("Unknown opcode: 0~ %X\n", opcode);
					return false;
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
			pc += 2 + 2 * (V[(opcode >> 8) & 0x000F] == (opcode & 0x00FF));
		break;
		case 0x4000:
			pc += 2 + 2 * (V[(opcode >> 8) & 0x000F] != (opcode & 0x00FF));
		break;
		case 0x5000:
			pc += 2 + 2 * (V[(opcode >> 8) & 0x000F] == V[(opcode >> 4) & 0x000F]);
		break;
		case 0x6000:
			V[(opcode >> 8) & 0x000F] = (opcode & 0x00FF);
			pc += 2;
		break;
		case 0x7000:
			V[(opcode >> 8) & 0x000F] += (opcode & 0x00FF);
			pc += 2;
		break;
		case 0x8000:
			switch (opcode & 0x000F) {
				case 0x0000:
					V[(opcode >> 8) & 0x000F] = V[(opcode >> 4) & 0x000F];
					pc += 2;
				break;
				case 0x0001:
					V[(opcode >> 8) & 0x000F] |= V[(opcode >> 4) & 0x000F];
					pc += 2;
				break;
				case 0x0002:
					V[(opcode >> 8) & 0x000F] &= V[(opcode >> 4) & 0x000F];
					pc += 2;
				break;
				case 0x0003:
					V[(opcode >> 8) & 0x000F] ^= V[(opcode >> 4) & 0x000F];
					pc += 2;
				break;
				case 0x0004: {
					V[0xF] = (0xFF - V[(opcode >> 8) & 0x000F] < V[(opcode >> 4) & 0x000F]);
					V[(opcode >> 8) & 0x000F] += V[(opcode >> 4) & 0x000F];	
					pc += 2;

				}
				break;
				case 0x0005: {
					V[0xF] = (V[(opcode >> 4) & 0x000F] < V[(opcode >> 8) & 0x000F]);
					V[(opcode >> 8) & 0x000F] -= V[(opcode >> 4) & 0x000F];	
					pc += 2;
				}
				break;
				case 0x0006: {
					V[0xF] = V[(opcode >> 8) & 0x000F] & 0x1;
					V[(opcode >> 8) & 0x000F] >>= 1;
					pc += 2;
				}
				break;
				case 0x0007: {
					V[0xF] = (V[(opcode >> 8) & 0x000F] < V[(opcode >> 4) & 0x000F]);
					V[(opcode >> 8) & 0x000F] = V[(opcode >> 4) & 0x000F] - V[(opcode >> 8) & 0x000F];
					pc += 2;
				}
				break;
				case 0x000E:
					V[0xF] = V[(opcode >> 8) & 0x000F] >> 7;
					V[(opcode >> 8) & 0x000F] <<= 1;
					pc += 2;
				break;
				default:
					printf("Uknown opcode 8~ %X\n", opcode);
					return false;

			}
		break;
		case 0x9000:
			pc += 2 + 2 * (V[(opcode >> 8) & 0x000F] != V[(opcode >> 4) & 0x000F]);
		break;
		case 0xA000:
			I = opcode & 0x0FFF;
			pc += 2;
		break;
		case 0xB000:
			pc = V[0x0] + (opcode & 0x0FFF);
		break;
		case 0xC000:
			V[(opcode >> 8) & 0x000F] = (opcode & 0x00FF) & (rand() % 0xFF);
			pc += 2;
		break;
		case 0xD000: {
			unsigned short x = V[(opcode >> 8) & 0x000F];
			unsigned short y = V[(opcode >> 4) & 0x000F];
			unsigned short N = opcode & 0x000F;
			V[0xF] = 0;
			for (int yLine = 0; yLine < N; yLine++) {
				unsigned short pixel = memory[I + yLine];
				for (int xLine = 0; xLine < 8; xLine++) {
					if (pixel & (0x80 >> xLine)) {
						if (gfx[x + xLine + (y + yLine) * 64]) {
							V[0xF] = 1;
						}
						gfx[x + xLine + (y + yLine) * 64] ^= 1;
					}
				}
			}
			drawFlag = true;
			pc += 2;
	    	}
		break;
		case 0xE000:
			switch (opcode & 0x00FF) {
				case 0x009E:
					pc += 2 + 2 * (key[V[(opcode >> 8) & 0x000F]]);
				break;
				case 0x00A1:
					pc += 2 + 2 * (!key[V[(opcode >> 8) & 0x000F]]);
				break;
				default:
					printf("Uknown opcode ~E %X\n", opcode);
			}
		break;
		case 0xF000:
			switch (opcode & 0x00FF) {
				case 0x0007:
					V[(opcode >> 8) & 0x000F] = delay_timer;
					pc += 2;
				break;
				case 0x000A: {
					bool keypress = false;
					for (int x = 0; x < 16; x++) {
						if (key[x] != 0) {
							keypress = true;
							V[(opcode >> 8) & 0x000F] = x;
						}
					}
					if (!keypress) return false;
					pc += 2; 
				}
				break;
				case 0x0015:
					delay_timer = V[(opcode >> 8) & 0x000F];
					pc += 2;
				break;
				case 0x0018:
					sound_timer = V[(opcode >> 8) & 0x000F];
					pc += 2;
				break;
				case 0x001E:
					V[0xF] = (I + V[(opcode >> 8) & 0x000F] > 0xFFF);
					I += V[(opcode >> 8) & 0x000F];
					pc += 2;
				break;
				case 0x0029:
					I = V[(opcode >> 8) & 0x000F] * 0x5;
					pc += 2;
				break;
				case 0x0033:
					memory[I] = V[(opcode >> 8) & 0x000F] / 100;
					memory[I + 1] = (V[(opcode >> 8) & 0x000F] / 10) % 10;
					memory[I + 2] = (V[(opcode >> 8) & 0x000F] % 100) % 10;
					pc += 2;
				break;
				case 0x0055:
					for (int x = 0; x < ((opcode >> 8) & 0x000F) + 1; x++) {
						memory[I + x] = V[x];
					}
					I += ((opcode >> 8) & 0x000F) + 1;
					pc += 2;
				break;
				case 0x0065:
					for (int x = 0; x < ((opcode >> 8) & 0x000F) + 1; x++) {
						V[x] = memory[I + x];
					}
					I += ((opcode >> 8) & 0x000F) + 1;
					pc += 2;
				break;
				default:
					printf("Uknown opcocde ~F %X\n", opcode);
			}
		break;
		default:
			printf("Uknown opcode. %X\n", opcode);
			return false;
	}
	return true;
}
	
void chip8::debugRender()
{
	for(int y = 0; y < 32; ++y) {
		for(int x = 0; x < 64; ++x) {
			if (!gfx[(y * 64) + x]) 
				printf("O");
			else 
				printf(" ");
		}
		printf("\n");
	}
	printf("\n");
}
