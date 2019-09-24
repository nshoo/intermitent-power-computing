#include <stdlib.h>
#include <stdint.h>
#include "checkpoint.h"

uint16_t registers[16] = {0};
uint16_t regBackup;
#pragma PERSISTENT ( checkpoints );
struct Checkpoint checkpoints[2] = {{0}, {0}};
#pragma PERSISTENT ( current );
struct Checkpoint * current = NULL;
#pragma DATA_SECTION(index, ".unprotected");
unsigned int index;
#pragma DATA_SECTION(restorePointer, ".unprotected");
uint8_t * restorePointer;

// do register checkpointing before they get polluted by checkpointing routine itself
__attribute__((always_inline)) inline void checkpointRegisters(){
    asm volatile (" mov.w r15, &regBackup"
            "\n    mov.w #registers, r15"
            "\n    mov.w r1, 2(r15)"
            "\n    mov.w r2, 4(r15)"
            "\n    mov.w r4, 6(r15)"
            "\n    mov.w r5, 8(r15)"
            "\n    mov.w r6, 10(r15)"
            "\n    mov.w r7, 12(r15)"
            "\n    mov.w r8, 14(r15)"
            "\n    mov.w r9, 16(r15)"
            "\n    mov.w r10, 18(r15)"
            "\n    mov.w r11, 20(r15)"
            "\n    mov.w r12, 22(r15)"
            "\n    mov.w r13, 24(r15)"
            "\n    mov.w r14, 26(r15)"
            "\n    mov.w &regBackup, 28(r15)");
}

// Saves stack to current checkpoint
void checkpointStack(struct Checkpoint * temp, uint8_t * sp){
    unsigned int i;
    for(i = 0; sp <= STACK_END; i++){
        temp->stack[i] = *sp;
        sp++;
    }
}

// Saves all global vars (initialized and uninitialized)
void checkpointGlobals(struct Checkpoint * temp){
    uint8_t * globalPtr = DATA_START;
    unsigned int i;
    for(i = 0; globalPtr != DATA_END; i++){
        temp->globals[i] = *globalPtr;
        globalPtr++;
    }
}

// Checkpoint portion of the heap specified
void checkpointHeap(struct Checkpoint * temp, uint8_t * start, unsigned int length){

}

// Takes program state (all in RAM) and stores it to one of two checkpoint slots (whichever not disabled)
// Recieves current sp as a parameter so as to not mess it up with its locals
void fullCheckpoint(uint8_t * stackPointer, uint8_t * start, unsigned int length){
    struct Checkpoint * temp = (current == &checkpoints[0]) ? &checkpoints[1] : &checkpoints[0];
    // Set checkpoint PC to the return address of this checkpoint function (always at top of stack)
    asm volatile (" mov.w @SP, &registers");
    /* Now it's ok to add to stack so the rest of the checkpointing occurs in function calls
     * in order to avoid the unnecessary storage overhead created by "inline" */
    checkpointStack(temp, stackPointer);
    checkpointGlobals(temp);
    checkpointHeap(temp, start, length);
    // Copy over the registers to the checkpoint now that they have been safely stored
    unsigned int i;
    for(i = 0; i < 16; i++) temp->regs[i] = registers[i];
    current = temp;
}

/* Restores all of the current checkpoint if one exists, this is difficult because the restore routine
 * has to run while modifying its own environment, global variables are used and allocated
 * to the NOINIT section, where they will not be restored, because of this */
void fullRestore(){
    // If no checkpoint has been created yet execution resumes normally from the beginning
    if(current == NULL) return;
    // Otherwise proceed with restore by preloading registers
    for(index = 0; index < 16; index++) registers[index] = current->regs[index];
    // Restore all global state
    restorePointer = DATA_START;
    for(index = 0; restorePointer <= DATA_END; index++){
        *restorePointer = current->globals[index];
        restorePointer++;
    }
    // Restore all of the stack
    restorePointer = (uint8_t *)(current->regs[1]);
    for(index = 0; restorePointer <= STACK_END; index++){
        *restorePointer = current->stack[index];
        restorePointer++;
    }
    //Restore registers last
    asm volatile (" mov.w #registers, r15"
            "\n    mov.w 2(r15), r1"
            "\n    NOP"
            "\n    mov.w 4(r15), r2"
            "\n    NOP"
            "\n    mov.w 6(r15), r4"
            "\n    mov.w 8(r15), r5"
            "\n    mov.w 10(r15), r6"
            "\n    mov.w 12(r15), r7"
            "\n    mov.w 14(r15), r8"
            "\n    mov.w 16(r15), r9"
            "\n    mov.w 18(r15), r10"
            "\n    mov.w 20(r15), r11"
            "\n    mov.w 22(r15), r12"
            "\n    mov.w 24(r15), r13"
            "\n    mov.w 26(r15), r14"
            "\n    mov.w 28(r15), r15"
            "\n    mov.w &registers, PC");
}
