#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

struct destination {
    uint8_t type;
    uint8_t index;
};

struct intention {
    int16_t value;
    struct destination dest;
};

#pragma PERSISTENT ( temp )
struct intention temp = {0, {0, 0}};
#pragma PERSISTENT ( data )
int8_t data[100] = {0};
//#pragma PERSISTENT ( test )
// int8_t  test[10] = {0};
#pragma PERSISTENT ( regs )
int8_t regs[8] = {0};
#pragma PERSISTENT ( ptr )
int16_t ptr = 0;
#pragma PERSISTENT ( ptrCopy )
int16_t ptrCopy = 0;
#pragma PERSISTENT ( compared )
int16_t compared = 0;
#pragma PERSISTENT ( completion )
uint8_t completion = 0;

uint16_t prog[] = {2053, 2304, 2624, 19010, 25088, 31499, 8194, 2817, 37633, 32269, 2816, 37633, 6433, 27137, 30980, 2927, 35842, 36100, 25605, 30999, 3072, 32280, 3073, 2052, 33024, 6657, 37122, 10241, 26753, 31513, 39936, 11105, 27392, 31505, 2053, 2305, 2816, 33280, 12865, 4962, 14626, 10241, 26753, 31526};

void stageIntention(struct intention intent){
    temp = intent;
}

void excecuteIntention(){
    // TYPES: reg, data, comp, ptr(special)
    switch(temp.dest.type){
    case 0: regs[temp.dest.index] = temp.value; break;
    case 1: data[temp.dest.index] = temp.value; break;
    case 2: compared = temp.value; break;
    case 3: ptr = temp.value; completion = 3; return;
    }
    completion = 2;
}

int valOfBin(uint16_t inst, bool sign, int start, int end) {
    int total = 0;
    int i;
    for(i = (sign) ? start + 1 : start; i < end; i++){
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
    return false;
}

struct intention instMov(uint16_t inst){
    struct intention intent;
    int isImmediate = isImmediate = valOfBin(inst, false, 4, 5);
    intent.dest.type = 0;
    intent.dest.index = valOfBin(inst, false, 5, 8);
    intent.value = (isImmediate) ? valOfBin(inst, true, 8, 16) : regs[valOfBin(inst, false, 8, 16)];
    return intent;
}

struct intention instArith(uint16_t inst){
    struct intention intent;
    intent.dest.type = 0;
    int isImmediate = valOfBin(inst, false, 4, 5);
    intent.dest.index = valOfBin(inst, false, 5, 8);
    int reg = regs[valOfBin(inst, false, 8, 11)];
    int term = (isImmediate) ? valOfBin(inst, true, 11, 16) : regs[valOfBin(inst, false, 11, 16)];
    switch(valOfBin(inst, false, 0, 4) - 1){
        case 0: intent.value = reg + term; break;
        case 1: intent.value = reg - term; break;
        case 2: intent.value = reg * term; break;
        case 3: intent.value = reg / term; break;
        case 4: intent.value = reg % term; break;
    }
    return intent;
}

struct intention instCmp(uint16_t inst){
    int isImmediate = valOfBin(inst, false, 4, 5);
    int reg = regs[valOfBin(inst, false, 5, 8)];
    int term = (isImmediate) ? valOfBin(inst, true, 8, 16) : regs[valOfBin(inst, false, 8, 16)];
    struct intention intent;
    intent.dest.type = 2;
    intent.dest.index = 0;
    intent.value = reg - term;
    return intent;
}

struct intention instJmp(uint16_t inst){
    int isImmediate = valOfBin(inst, false, 4, 5);
    bool truth = checkCondition(valOfBin(inst, false, 5, 8));
    int val = (isImmediate) ? valOfBin(inst, true, 8, 16) : regs[valOfBin(inst, false, 8, 16)];
    struct intention intent;
    intent.value = (truth) ? val - 1 : ptr + 1;
    intent.dest.type = 3;
    intent.dest.index = 0;
    return intent;
}

struct intention instRead(uint16_t inst){
    int isImmediate = valOfBin(inst, false, 4, 5);
    int reg = valOfBin(inst, false, 5, 8);
    int loc = (isImmediate) ? valOfBin(inst, false, 8, 16) : regs[valOfBin(inst, false, 8, 16)];
    struct intention intent;
    intent.value = data[loc];
    intent.dest.index = reg;
    intent.dest.type = 0;
    return intent;
}

struct intention instWrite(uint16_t inst){
    int isImmediate = valOfBin(inst, false, 4, 5);
    int reg = valOfBin(inst, false, 5, 8);
    int loc = (isImmediate) ? valOfBin(inst, false, 8, 16) : regs[valOfBin(inst, false, 8, 16)];
    struct intention intent;
    intent.value = regs[reg];
    intent.dest.index = loc;
    intent.dest.type = 1;
    return intent;
}

struct intention (*instructions[])(uint16_t) = {instMov, instArith, instArith, instArith, instArith, instArith, instCmp, instJmp, instRead, instWrite};

void continueProgram(){
    switch(completion){
    case 0: stageIntention(instructions[valOfBin(prog[ptr], false, 0, 4)](prog[ptr])); completion = 1; break;
    case 1: excecuteIntention(); break; // sets own completion (2 def or 3)
    case 2: if(ptr == ptrCopy) ptr++; completion = 3; break;
    case 3: ptrCopy = ptr; completion = 0;
    }
}

void delay(uint32_t loops){
    volatile uint32_t i;
    for(i=loops; i>0; i--);
}

void resetProgram(){
    memset(data, 0, sizeof(data)/sizeof(data[0]));
    memset(regs, 0, sizeof(regs)/sizeof(regs[0]));
    ptr = 0;
    ptrCopy = 0;
    compared = 0;
    completion = 0;
    P1OUT = BIT0;
    for(;;){}
}

void waitForButton(){
    while(true){
        if((P2IN & BIT7) != BIT7){
            break;
        } else if((P2IN & BIT3) != BIT3){
            resetProgram();
        }
    }
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;
    SYSCFG0 = FRWPPW | DFWP;
    //delay(2000);
    P1DIR |= 0x03;
    P2DIR &= ~(BIT7 | BIT3);
    P2REN |= BIT7 | BIT3;
    P2OUT = BIT7 | BIT3;
    P1OUT = 0;
    //if(SYSRSTIV == SYSRSTIV_RSTNMI){resetProgram();}
	P1OUT = BIT1;
    while(ptr < sizeof(prog)/sizeof(prog[0])){
	    continueProgram();
	    //delay(200);
	}
	int i = 0;
	volatile uint32_t c;
	P1OUT = BIT0;
	for(c=20000; c>0; c--);
	for(i = 0; i < regs[3]; i++){
	    for(c=20000; c>0; c--);
	    P1OUT = BIT1;
	    for(c=20000; c>0; c--);
	    P1OUT = 0;
	}
	P1OUT = 0;
	waitForButton();
	return 0;
}
