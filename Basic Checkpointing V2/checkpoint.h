#ifndef CHECKPOINT_H_
#define CHECKPOINT_H_
#include <stdint.h>

#define SIMPLE_CHECKPOINT(loc, len) checkpointRegisters(); fullCheckpoint(__get_SP_register(), loc, len);

// Linker generated symbol whose addresses are the values they represent (type doesn't matter)
extern char __STACK_END;
extern char SYSMEM_SIZE_SYM;
extern char DATA_START_SYM;
extern char DATA_END_SYM;

// Added _symval to all linker variables
#define STACK_END (uint8_t *)(&__STACK_END)
#define HEAP_SIZE _symval(&SYSMEM_SIZE_SYM)
#define DATA_START (uint8_t *)(&DATA_START_SYM)
/* This one is dangerous because if the data (initialized) segment is empty then it will be the same as
 * DATA_START making the whole data segment (.bss & .data) appear to be empty
 * One solution would be to take the max of the end of bss and data but this would add overhead
 * note that the .data segment is never empty if malloc is used in the program */
#define DATA_END (uint8_t *)(&DATA_END_SYM)

struct Checkpoint {
    uint16_t regs[16];
    uint8_t globals[500];
    uint8_t heap[160];
    uint8_t stack[160];
    uint8_t * heapStart;
    unsigned int heapLength;
};

inline void checkpointRegisters() __attribute__((always_inline));

void checkpointStack(struct Checkpoint * current, uint8_t * sp);

void checkpointGlobals(struct Checkpoint * current);

void checkpointHeap(struct Checkpoint * current, uint8_t * start, unsigned int length);

// main checkpointing routine (only one which the programmer should call)
void fullCheckpoint(uint8_t * stackPointer, uint8_t * start, unsigned int length);

// Restores current checkpoint if one exists, should be placed at beginning of main
void fullRestore();

#endif
