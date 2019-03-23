#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

uint16_t prog[] = {2053, 2304, 2624, 19010, 25088, 31499, 8194, 2817, 37633, 32269, 2816, 37633, 6433, 27137, 30980, 2927, 35842, 36100, 25605, 30999, 3072, 32280, 3073, 2052, 33024, 6657, 37122, 10241, 26753, 31513, 39936, 11105, 27392, 31505, 2053, 2305, 2816, 33280, 12865, 4962, 14626, 10241, 26753, 31526};
long compared = 0;
long ptr = 0;
int8_t data[50];
int regs[8];

void delay(int milliseconds)
{
    long pause;
    clock_t now,then;

    pause = milliseconds*(CLOCKS_PER_SEC/1000);
    now = then = clock();
    while( (now-then) < pause )
        now = clock();
}

int valOfBin(uint16_t inst, bool sign, int start, int end) {
	int total = 0;
	for(int i = (sign) ? start + 1 : start; i < end; i++){
		total *= 2;
		total += ((0x8000 >> i) & inst) ? 1 : 0;
	}
	if(((0x8000 >> start) & inst) && sign) total *= -1;
	return total;
}

bool checkCondition(int cond){
	switch(cond){
		case 0: return compared == 0; break;
		case 1: return compared != 0; break;
		case 2: return compared < 0; break;
		case 3: return compared > 0; break;
		case 4: return (compared < 0) || (compared == 0); break;
		case 5: return (compared > 0) || (compared == 0); break;
		case 6: return true; break;
	}
}

void instMov(uint16_t inst){
	int isImmediate = valOfBin(inst, false, 4, 5);
	int dest = valOfBin(inst, false, 5, 8);
	int val = (isImmediate) ? valOfBin(inst, true, 8, 16) : regs[valOfBin(inst, false, 8, 16)];
	regs[dest] = val;
}

void instArith(uint16_t inst){
	int isImmediate = valOfBin(inst, false, 4, 5);
	int dest = valOfBin(inst, false, 5, 8);
	int reg = regs[valOfBin(inst, false, 8, 11)];
	int term = (isImmediate) ? valOfBin(inst, true, 11, 16) : regs[valOfBin(inst, false, 11, 16)];
	switch(valOfBin(inst, false, 0, 4) - 1){
		case 0: regs[dest] = reg + term; break;
		case 1: regs[dest] = reg - term; break;
		case 2: regs[dest] = reg * term; break;
		case 3: regs[dest] = reg / term; break;
		case 4: regs[dest] = reg % term; break;
	}
}

void instCmp(uint16_t inst){
	int isImmediate = valOfBin(inst, false, 4, 5);
	int reg = regs[valOfBin(inst, false, 5, 8)];
	int term = (isImmediate) ? valOfBin(inst, true, 8, 16) : regs[valOfBin(inst, false, 8, 16)];
	compared = reg - term;
}

void instJmp(uint16_t inst){
	int isImmediate = valOfBin(inst, false, 4, 5);
	bool truth = checkCondition(valOfBin(inst, false, 5, 8));
	int val = (isImmediate) ? valOfBin(inst, true, 8, 16) : regs[valOfBin(inst, false, 8, 16)];
	if(truth) ptr = val - 2;
}

void instRead(uint16_t inst){
	int isImmediate = valOfBin(inst, false, 4, 5);
	int reg = valOfBin(inst, false, 5, 8);
	int loc = (isImmediate) ? valOfBin(inst, false, 8, 16) : regs[valOfBin(inst, false, 8, 16)];
	regs[reg] = data[loc];
}

void instWrite(uint16_t inst){
	int isImmediate = valOfBin(inst, false, 4, 5);
	int reg = valOfBin(inst, false, 5, 8);
	int loc = (isImmediate) ? valOfBin(inst, false, 8, 16) : regs[valOfBin(inst, false, 8, 16)];
	data[loc] = regs[reg];
}

void (*instructions[])(uint16_t) = {instMov, instArith, instArith, instArith, instArith, instArith, instCmp, instJmp, instRead, instWrite};

int main(){
	while(ptr < (sizeof(prog)/sizeof(prog[0]))){
		instructions[valOfBin(prog[ptr], false, 0, 4)](prog[ptr]);
		ptr++;
	}
	for(int i = 0; i < 8; i++){
		printf("%d\n", regs[i]);
	}
	for(int i = 0; i < 6; i++){
		printf("%d", data[i]);
	}
	printf("\nCompare: %d\n", compared);
}