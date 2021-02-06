#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define MAX_MEM 65536
// MAX_MEM = 1024 * 64
#define instruction static constexpr Byte

// https://www.obelisk.me.uk/6502

using Byte = unsigned char;
using Word = unsigned short;
using u32  = unsigned int;

struct Mem {
	Byte Data[MAX_MEM];

	void init() {
		for(u32 i = 0; i < MAX_MEM; i++) {
			Data[i] = 0;
		}
	}

	void dump() {
		for(u32 i = 0; i < MAX_MEM; i += 16) {
			printf("%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", Data[i], Data[i + 1], Data[i + 2], Data[i + 3], Data[i + 4], Data[i + 5], Data[i + 6], Data[i + 7], Data[i + 8], Data[i + 9], Data[i + 10], Data[i + 11], Data[i + 12], Data[i + 13], Data[i + 14], Data[i + 15]);
		}
	}

	void dump(u32 in) {
		printf("%d", Data[in]);
	}

	// read a byte
	Byte operator[](u32 addr) const {
		assert(addr < MAX_MEM); // check if the address is in memory
		return Data[addr];
	}

	// write a byte
	Byte& operator[](u32 addr) {
		assert(addr < MAX_MEM); // check if the address is in memory
		return Data[addr];
	}
};

struct CPU {
	Word PC; // program counter
	Word SP; // stack pointer

	Byte A;  // accumulator register
	Byte X, Y;  // index registers X and Y

	Byte C : 1; // carry flag
	Byte Z : 1; // zero flag
	Byte I : 1; // interrupt disable
	Byte D : 1; // decimal mode
	Byte B : 1; // break command
	Byte V : 1; // overflow flag
	Byte N : 1; // negative flag
	
	void clearflags() {
		C = 0;
		Z = 0;
		I = 0;
		D = 0;
		B = 0;
		V = 0;
		N = 0;
	}

	void printflags() {
		printf("C: %d\nZ: %d\nI: %d\nD: %d\nB: %d\nV: %d\nN: %d\n", C, Z, I, D, B, V, N);
	}

	void printflagsc() {
		printf("CZIDBVN\n%d%d%d%d%d%d%d\n", C, Z, I, D, B, V, N);
	}

	void reset(Mem &memory) {
		// clear all the data and perform the boot process
		PC = 0x1000;
		SP = 0x1AA3;
		clearflags(); // clear all the flags
		A = X = Y = 0; // clear registers
		memory.init();
	}

	Byte fetchbyte(u32 &cycles, Mem memory) {
		Byte data = memory[PC];
		PC++;
		cycles--;
		return data;
	}

	Byte readbyte(u32 &cycles, Mem memory, Byte addr) {
		Byte jumpto = memory[addr];
		Byte data = memory[jumpto];
		cycles--;
		return data;
	}

	Byte readbyte(u32 &cycles, Mem memory) {
		Byte jumpto = memory[PC];
		Byte data = memory[jumpto];
		cycles--;
		return data;
	}

	Byte readnextbyte(u32 &cycles, Mem memory) {
		Byte data = memory[PC];
		cycles--;
		return data;
	}

	// register loading
	instruction LDA = 0xA9;
	instruction LDX = 0xA2;
	instruction LDY = 0xA0;
	// register modification
	instruction INX = 0xE8;
	instruction INY = 0xC8;
	// stack pointer operations
	instruction TSX = 0xBA;
	// flag operation
	instruction CLC = 0x18;
	instruction CLD = 0xD8;
	instruction CLI = 0x58;
	instruction CLV = 0xB8;

	void exec(u32 cycles, Mem &memory) {
		// execute <cycles> instructions in memory
		while(cycles > 0) { // while we have cycles left to do
			// the next instruction in memory is where PC tells us it is
			Byte instr = fetchbyte(cycles, memory);
			switch(instr) {
				case LDA: {
					Byte M = fetchbyte(cycles, memory);
					A = M;
					Z = (M == 0);
					N = (M & 0b10000000) > 0;
				} break;
				case LDX: {
					Byte M = fetchbyte(cycles, memory);
					X = M;
					Z = (M == 0);
					N = (M & 0b10000000) > 0;
				} break;
				case LDY: {
					Byte M = fetchbyte(cycles, memory);
					Y = M;
					Z = (M == 0);
					N = (M & 0b10000000) > 0;
				} break;
				case TSX: {
					Byte val = fetchbyte(cycles, memory);
					X = this -> SP;
					Z = (A == 0);
					N = (A & 0b10000000) > 0;
				} break;
				case CLC: {
					// printf("%d\n", C); testing code
					Byte val = fetchbyte(cycles, memory);
					C = 0;
					// printf("%d\n", C); testing code
				} break;
				case CLD: {
					// printf("%d\n", D); testing code
					Byte val = fetchbyte(cycles, memory);
					D = 0;
					// printf("%d\n", D); testing code
				} break;
				case CLI: {
					// printf("%d\n", I); testing code
					Byte val = fetchbyte(cycles, memory);
					I = 0;
					// printf("%d\n", I); testing code
				} break;
				case CLV: {
					// printf("%d\n", V); testing code
					Byte val = fetchbyte(cycles, memory);
					V = 0;
					// printf("%d\n", V); testing code
				} break;
				case INX: {
					X += 1;
					Byte fart = fetchbyte(cycles, memory);
					Z = (fart == 0);
					N = (fart & 0b10000000) > 0;
				} break;
				case INY: {
					X += 1;
					Byte fart = fetchbyte(cycles, memory);
					Z = (fart == 0);
					N = (fart & 0b10000000) > 0;
				} break;
				default: {
					printf("Instruction not handled: %d\n; Located at: %x\n", instr, cycles + 0x1000);
					cycles = 0;
				} break;
			}
		}
	}

	void printreg() {
		printf("Reg A: 0x%04X\nReg X: 0x%04X\nReg Y: 0x%04X\n", A, X, Y);
	}
};