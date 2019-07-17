#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

uint16_t opcode;
uint8_t memory[4096];
uint8_t V[16];
uint16_t stack[16];
uint16_t I;
uint16_t pc;
uint16_t sp;

void printAllRegs(){
	for(int i = 0; i < 16; i++){
		printf("Reg: %d\n", V[i]);
	}
}

uint16_t valOfBin(uint16_t inst, unsigned int start, unsigned int end){
	uint16_t total = 0;
	for(int i = start; i < end; i++){
		total *= 2;
		total += ((0x8000 >> i) & inst) ? 1 : 0;
	}
	return total;
}

uint16_t getWordFromAddress(uint16_t address){
	return (memory[address] * 0x100) + memory[address + 1];
}

bool unimplimentedInstruction(uint16_t inst){
	return true;
}

bool instRet(uint16_t inst){
	switch(valOfBin(inst, 8, 16)){
		case 0xE0:
			printf("Unimplimented: CLS!\n"); break;
		case 0xEE:
			if(sp) pc = stack[--sp]; else {
				printf("stack underflow!\n"); printAllRegs(); exit(1);
			} break;
		case 0xFF:
			printAllRegs();
			printf("OUTPUT: %d\n", V[0]); break;
	}
	return true;
}

bool instJump(uint16_t inst){
	pc = valOfBin(inst, 4, 16);
	return false;
}

bool instCall(uint16_t inst){
	stack[sp++] = pc;
	pc = valOfBin(inst, 4, 16);
	return false;
}


bool instSkipEqual(uint16_t inst){
	if(V[valOfBin(inst, 4, 8)] == valOfBin(inst, 8, 16)) pc += 2;
	return true;
}

bool instSkipNotEqual(uint16_t inst){
	if(V[valOfBin(inst, 4, 8)] != valOfBin(inst, 8, 16)) pc += 2;
	return true;
}

bool instSkipRegEqual(uint16_t inst){
	if(V[valOfBin(inst, 4, 8)] == V[valOfBin(inst, 8, 16)]) pc += 2;
	return true;
}

bool instLoadImm(uint16_t inst){
	V[valOfBin(inst, 4, 8)] = valOfBin(inst, 8, 16);
	return true;
}

bool instAdd(uint16_t inst){
	V[valOfBin(inst, 4, 8)] += valOfBin(inst, 8, 16);
	return true;
}

bool instBitwise(uint16_t inst){
	unsigned int x = valOfBin(inst, 4, 8);
	unsigned int y = valOfBin(inst, 8, 12);
	uint16_t result;
	switch(valOfBin(inst, 12, 16)){
		case 0:
			V[x] = V[y]; break;
		case 1:
			V[x] |= V[y]; break;
		case 2:
			V[x] &= V[y]; break;
		case 3:
			V[x] ^= V[y]; break;
		case 4:
			result = V[x] + V[y];
			V[0xF] = (result > 255) ? 1 : 0;
			V[x] = result & 0xFF;
			break;
		case 5:
			result = (V[x] > V[y]) ? 1 : 0;
			V[x] += ~(V[y]) + 1;
			V[0xF] = result;
			break;
		case 6:
			V[0xF] = (V[x] & 0x01) ? 1 : 0;
			V[x] >>= V[y];
			break;
		case 7:
			result = (V[y] > V[x]) ? 1 : 0;
			V[x] = V[y] + ~(V[x]) + 1;
			V[0xF] = result;
			break;
		case 0xE:
			V[0xF] = (V[x] & 0x80) ? 1 : 0;
			V[x] <<= V[y];
			break;
	}
	return true;
}

bool instSkipRegNotEqual(uint16_t inst){
	if(V[valOfBin(inst, 4, 8)] != V[valOfBin(inst, 8, 16)]) pc += 2;
	return true;
}

bool instLoadI(uint16_t inst){
	I = valOfBin(inst, 4, 16);
	return true;
}

bool instJumpRegOffset(uint16_t inst){
	pc = valOfBin(inst, 4, 16) + V[0];
	return false;
}

void storeRegBDC(unsigned int index){
	printf("BCD!\n");
	uint8_t working = V[index];
	unsigned int power = 100;
	for(int i = 0; i < 3; i++){
		memory[I+i] = 0;
		while(power >= working){
			working -= power;
			memory[I+i]++;
		}
		power /= 10;
	}
}

void storeRegs(unsigned int x){
	for(int i = 0; i <= x; i++){
		memory[I+i] = V[i];
	}
}

void readRegs(unsigned int x){
	for(int i = 0; i <= x; i++){
		V[i] = memory[I+i];
	}
}

bool instMisc(uint16_t inst){
	unsigned int x = valOfBin(inst, 4, 8);
	switch(valOfBin(inst, 8, 16)){
		case 0x07: unimplimentedInstruction(inst); break;
		case 0x0A: unimplimentedInstruction(inst); break;
		case 0x15: unimplimentedInstruction(inst); break;
		case 0x18: unimplimentedInstruction(inst); break;
		case 0x1E: I += V[x]; break; 
		case 0x29: I = 5 * V[x]; break;
		case 0x33: storeRegBDC(x); break;
		case 0x55: storeRegs(x); break;
		case 0x65: readRegs(x); break;
	}
	return true;
}

bool (*instructions[])(uint16_t) = {
	instRet,
	instJump,
	instCall,
	instSkipEqual,
	instSkipNotEqual,
	instSkipRegEqual,
	instLoadImm,
	instAdd,
	instBitwise,
	instSkipRegNotEqual,
	instLoadI,
	instJumpRegOffset,
	unimplimentedInstruction,
	unimplimentedInstruction,
	unimplimentedInstruction,
	instMisc
};

void readFileInput(const char filename[]){
	FILE *file;
    file = fopen(filename, "rb");
    if(file) fread((memory + 0x200), 4096, 1, file); else
    printf("File open failure!");
}

int main(){
	readFileInput("prog.bin");
	pc = 0x200;
	unsigned int instLength = sizeof(instructions) / sizeof(*instructions);
	while(pc < sizeof(memory)){
		opcode = getWordFromAddress(pc);
		if(valOfBin(opcode, 0, 4) < instLength){
			if(instructions[valOfBin(opcode, 0, 4)](opcode)) pc += 2;
		} else {
			printf("Unimplimented instruction!\n");
			pc += 2;
		}
	}
	printAllRegs();
}