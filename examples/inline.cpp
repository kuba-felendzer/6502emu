#include "6502.h"

int main() {
	Mem mem;
	CPU cpu;
	cpu.reset(mem);
	cpu.printreg();
	mem[0x1000] = CPU::INS_LDA_IM;
	mem[0x1001] = 0x42;
	cpu.exec(2, mem);
	cpu.printreg();
	return 0;
}
