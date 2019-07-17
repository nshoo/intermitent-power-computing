#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

struct destination {
    uint8_t type;
    unsigned int index;
};

struct intention {
    uint16_t value;
    struct destination dest;
    bool updatesFlag;
    uint8_t flag;
};

#pragma PERSISTENT ( temp )
struct intention temp = {0, {0, 0}};
#pragma PERSISTENT ( memory )
uint8_t memory[4096] = {[0x200]= 0x12, 0x32, 0x81, 0x0, 0x41, 0x0, 0x12, 0x18, 0x61, 0x1, 0x6f, 0x3, 0x8e, 0xf5, 0x8f, 0x10, 0xfe, 0x29, 0xfe, 0x65, 0x0, 0xee, 0x12, 0x18, 0x61, 0x0, 0x6f, 0x3, 0x8e, 0xf5, 0x8f, 0x10, 0xfe, 0x29, 0xfe, 0x65, 0x0, 0xee, 0x6f, 0x3, 0x8e, 0xf5, 0x8f, 0x10, 0xfe, 0x29, 0xfe, 0x65, 0x0, 0xee, 0x6e, 0x10, 0x60, 0x32, 0x61, 0x9b, 0x62, 0x0, 0x83, 0x0, 0x43, 0x0, 0x12, 0xb2, 0x83, 0x20, 0x84, 0x10, 0x65, 0x20, 0x84, 0x52, 0xfe, 0x29, 0xfe, 0x55, 0x6f, 0x3, 0x8e, 0xf4, 0x80, 0x40, 0x22, 0x2, 0x84, 0xf0, 0x85, 0x10, 0x66, 0x10, 0x85, 0x62, 0xfe, 0x29, 0xfe, 0x55, 0x6f, 0x3, 0x8e, 0xf4, 0x80, 0x50, 0x22, 0x2, 0x85, 0xf0, 0x84, 0x53, 0x83, 0x40, 0x82, 0x30, 0x83, 0x10, 0x84, 0x10, 0x65, 0x1, 0x84, 0x56, 0x83, 0x40, 0x81, 0x30, 0x83, 0x20, 0x43, 0x0, 0x12, 0x8c, 0x83, 0x10, 0x64, 0x80, 0x83, 0x41, 0x81, 0x30, 0x12, 0x98, 0x83, 0x10, 0x64, 0x80, 0x6f, 0xff, 0x84, 0xf3, 0x83, 0x42, 0x81, 0x30, 0x83, 0x0, 0x64, 0x1, 0x83, 0x45, 0x80, 0x30, 0x83, 0x0, 0xfe, 0x29, 0xfe, 0x55, 0x6f, 0x3, 0x8e, 0xf4, 0x80, 0x30, 0x22, 0xce, 0x83, 0xf0, 0x12, 0x3a, 0x83, 0x10, 0xfe, 0x29, 0xfe, 0x55, 0x6f, 0x3, 0x8e, 0xf4, 0x80, 0x30, 0x22, 0xce, 0x83, 0xf0, 0x6f, 0x3, 0x8e, 0xf5, 0x8f, 0x0, 0xfe, 0x29, 0xfe, 0x65, 0x0, 0xee, 0x0, 0xff, 0x6f, 0x3, 0x8e, 0xf5, 0x8f, 0x0, 0xfe, 0x29, 0xfe, 0x65, 0x0, 0xee};
#pragma PERSISTENT ( regs )
uint8_t regs[16] = {0};
#pragma PERSISTENT ( stack )
uint16_t stack[16] = {0};
#pragma PERSISTENT ( pc )
uint16_t pc = 0x200;
#pragma PERSISTENT ( pcCopy )
uint16_t pcCopy = 0;
#pragma PERSISTENT ( completion )
uint8_t completion = 0;
#pragma PERSISTENT ( I )
uint8_t I = 0;
#pragma PERSISTENT ( unimplimented )
unsigned int unimplimented = 0;
#pragma PERSISTENT ( output )
uint8_t output = 0;
#pragma PERSISTENT ( sp )
uint8_t sp = 0;
#pragma PERSISTENT ( spCopy )
uint8_t spCopy = 0;

void stageIntention(struct intention intent){
    temp = intent;
}

void updateFlag(){
    if(temp.updatesFlag) regs[0xF] = temp.flag;
}

void executeIntention(){
    // TYPES: reg, data, comp, ptr(special)
    switch(temp.dest.type){
    case 0: regs[temp.dest.index] = temp.value; break;
    case 1: memory[temp.dest.index] = temp.value; break;
    case 2: pc = temp.value; completion = 3; return;
    case 3: unimplimented = temp.value; break;
    case 4: output = temp.value; break;
    case 5: break; // pass
    case 6: I = temp.value; break;
    }
    completion = 2;
}

void delay(uint32_t loops){
    volatile uint32_t i;
    for(i=loops; i>0; i--);
}

void resetProgram(){
    memset(memory, 0, sizeof(memory)/sizeof(memory[0]));
    memset(regs, 0, sizeof(regs)/sizeof(regs[0]));
    pc = 0;
    pcCopy = 0;
    completion = 0;
    P1OUT = BIT0;
    for(;;){}
}

uint16_t valOfBin(uint16_t inst, unsigned int start, unsigned int end){
    uint16_t total = 0;
    int i;
    for(i = start; i < end; i++){
        total *= 2;
        total += ((0x8000 >> i) & inst) ? 1 : 0;
    }
    return total;
}

uint16_t getWord(uint16_t address){
    return (memory[address] * 0x100) + memory[address + 1];
}

void exit(){
    if(output == 172) P1OUT = BIT1;
    while(1);
}

// END OF OLD CODE

struct intention unimplimentedInstruction(uint16_t inst){ // *
    struct intention intent;
    intent.updatesFlag = false;
    intent.dest.type = 3;
    intent.dest.index = 0;
    intent.value = unimplimented + 1;
    return intent;
}

struct intention instRet(uint16_t inst){ // *
    struct intention intent;
    intent.updatesFlag = false;
    if(inst == 0){
        intent.dest.type = 5;
        return intent;
    }
    switch(valOfBin(inst, 8, 16)){
        case 0xE0:
            return unimplimentedInstruction(inst);
        case 0xEE:
            if(sp){
                if(sp == spCopy) sp--;
                intent.dest.type = 2;
                intent.dest.index = 0;
                intent.value = stack[sp] + 2;
            }
            else {
                exit();
            }
            break;
        case 0xFF:
            intent.dest.type = 4;
            intent.dest.index = 0;
            intent.value = regs[0];
            break; // OUTPUT
    }
    return intent;
}

struct intention instJump(uint16_t inst){ // *
    struct intention intent;
    intent.updatesFlag = false;
    intent.dest.type = 2;
    intent.dest.index = 0;
    intent.value = valOfBin(inst, 4, 16);
    return intent;
}

struct intention instCall(uint16_t inst){ // *
    struct intention intent;
    intent.updatesFlag = false;
    if(sp == spCopy) sp++;
    stack[sp - 1] = pc;
    intent.dest.type = 2;
    intent.dest.index = 0;
    intent.value = valOfBin(inst, 4, 16);
    return intent;
}

struct intention instSkipEqual(uint16_t inst){
    struct intention intent;
    intent.updatesFlag = false;
    intent.dest.type = 2;
    intent.dest.index = 0;
    intent.value = (regs[valOfBin(inst, 4, 8)] == valOfBin(inst, 8, 16)) ? pc + 4 : pc + 2;
    return intent;
}

struct intention instSkipNotEqual(uint16_t inst){
    struct intention intent;
    intent.updatesFlag = false;
    intent.dest.type = 2;
    intent.dest.index = 0;
    intent.value = (regs[valOfBin(inst, 4, 8)] != valOfBin(inst, 8, 16)) ? pc + 4 : pc + 2;
    return intent;
}

struct intention instSkipRegEqual(uint16_t inst){
    struct intention intent;
    intent.updatesFlag = false;
    intent.dest.type = 2;
    intent.dest.index = 0;
    intent.value = (regs[valOfBin(inst, 4, 8)] == regs[valOfBin(inst, 8, 16)]) ? pc + 4 : pc + 2;
    return intent;
}

struct intention instLoadImm(uint16_t inst){
    struct intention intent;
    intent.updatesFlag = false;
    intent.dest.type = 0;
    intent.dest.index = valOfBin(inst, 4, 8);
    intent.value = valOfBin(inst, 8, 16);
    return intent;
}

struct intention instAdd(uint16_t inst){
    struct intention intent;
    intent.updatesFlag = false;
    intent.dest.index = valOfBin(inst, 4, 8);
    intent.dest.type = 0;
    intent.value = regs[valOfBin(inst, 4, 8)] + valOfBin(inst, 8, 16);
    return intent;
}

struct intention instBitwise(uint16_t inst){
    struct intention intent;
    intent.updatesFlag = false;
    unsigned int x = valOfBin(inst, 4, 8);
    unsigned int y = valOfBin(inst, 8, 12);
    intent.dest.index = x;
    intent.dest.type = 0;
    uint16_t result;
    switch(valOfBin(inst, 12, 16)){
        case 0:
            intent.value = regs[y]; break;
        case 1:
            intent.value = regs[x] | regs[y]; break;
        case 2:
            intent.value = regs[x] & regs[y]; break;
        case 3:
            intent.value = regs[x] ^ regs[y]; break;
        case 4:
            result = regs[x] + regs[y];
            intent.updatesFlag = true;
            intent.flag = (result > 255) ? 1 : 0;
            intent.value = result & 0xFF;
            break;
        case 5:
            intent.value = regs[x] + ~(regs[y]) + 1; // ?
            intent.flag = (regs[x] > regs[y]) ? 1 : 0;
            intent.updatesFlag = true;
            break;
        case 6:
            intent.flag = (regs[x] & 0x01) ? 1 : 0;
            intent.updatesFlag = true;
            intent.value = regs[x] >> regs[y];
            break;
        case 7:
            intent.value = regs[y] + ~(regs[x]) + 1;
            intent.flag = (regs[y] > regs[x]) ? 1 : 0;
            intent.updatesFlag = true;
            break;
        case 0xE:
            intent.flag = (regs[x] & 0x80) ? 1 : 0;
            intent.updatesFlag = true;
            intent.value = regs[x] << regs[y];
            break;
    }
    return intent;
}

struct intention instSkipRegNotEqual(uint16_t inst){
    struct intention intent;
    intent.updatesFlag = false;
    intent.dest.type = 2;
    intent.dest.index = 0;
    intent.value = (regs[valOfBin(inst, 4, 8)] != regs[valOfBin(inst, 8, 16)]) ? pc + 4 : pc + 2;
    return intent;
}

struct intention instLoadI(uint16_t inst){
    struct intention intent;
    intent.updatesFlag = false;
    intent.dest.type = 6;
    intent.dest.index = 0;
    intent.value = valOfBin(inst, 4, 16);
    return intent;
}

struct intention instJumpRegOffset(uint16_t inst){
    struct intention intent;
    intent.updatesFlag = false;
    intent.dest.type = 2;
    intent.dest.index = 0;
    intent.value = valOfBin(inst, 4, 16) + regs[0];
    return intent;
}

void storeRegBDC(unsigned int index){
    uint8_t working = regs[index];
    unsigned int power = 100;
    int i;
    for(i = 0; i < 3; i++){
        memory[I+i] = 0;
        while(power >= working){
            working -= power;
            memory[I+i]++;
        }
        power /= 10;
    }
}

void storeRegs(unsigned int x){
    int i;
    for(i = 0; i <= x; i++){
        memory[I+i] = regs[i];
    }
}

void readRegs(unsigned int x){
    int i;
    for(i = 0; i <= x; i++){
        regs[i] = memory[I+i];
    }
}

struct intention instMisc(uint16_t inst){
    struct intention intent;
    intent.updatesFlag = false;
    intent.dest.index = 0;
    unsigned int x = valOfBin(inst, 4, 8);
    switch(valOfBin(inst, 8, 16)){
        case 0x07: return unimplimentedInstruction(inst);
        case 0x0A: return unimplimentedInstruction(inst);
        case 0x15: return unimplimentedInstruction(inst);
        case 0x18: return unimplimentedInstruction(inst);
        case 0x1E:
            intent.dest.type = 6;
            intent.value = I + regs[x];
            break;
        case 0x29: return unimplimentedInstruction(inst);
        case 0x33: intent.dest.type = 5; storeRegBDC(x); break;
        case 0x55: intent.dest.type = 5; storeRegs(x); break;
        case 0x65: intent.dest.type = 5; readRegs(x); break;
    }
    return intent;
}

struct intention (*instructions[])(uint16_t) = {
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

void continueProgram(){
    switch(completion){
    case 0: stageIntention(instructions[valOfBin(getWord(pc), 0, 4)](getWord(pc))); completion = 1; break;
    case 1: updateFlag(); executeIntention(); break; // sets own completion (2 def or 3)
    case 2: if(pc == pcCopy) pc += 2; completion = 3; break;
    case 3: pcCopy = pc; spCopy = sp; completion = 0;
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
    P1OUT = BIT0;
    //if(SYSRSTIV == SYSRSTIV_RSTNMI){resetProgram();}
    while(pc < sizeof(memory)/sizeof(*memory)){
        continueProgram();
    }
    return 0;
}
