#pragma once

#ifndef chip8
#define chip8

class chip8 {
	public:
		~chip8();

		bool drawFlag;

		void initialize();
		void loadGame(char* game);
		void setKeys();
	private:
		unsigned short opcode; // Current opcode
		unsigned char memory[4096]; //4K Memory
		unsigned char V[16]; // The 16 registers
		unsigned short I; // Index register
		unsigned short pc; // Program counter

		unsigned char gfx[64 * 32]; // Graphics display
		unsigned char delay_timer; // Delay timer
		unsigned char sound_timer; // Sound timer

		unsigned short stack[16]; // for jump calls
		unsigned short sp; // stack pointer

		unsigned char key[16]; // keypad
}
#endif
