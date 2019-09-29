### Simple Checkpoint

This is the code for a simple checkpoint of a device's state, designed for use with the MSP430 (FR2433). Calling the macro `SIMPLE_CHECKPOINT()` saves the registers, the stack, _all_ globals, and a portion of the heap. The macro requires that the user deignate an area on the heap to be checkpointed, by passing a pointer to its start, and its length in bytes as parameters. The checkpointing routine is then able to infer all other properties of the checkpoint through the use of linker variables. Unfortunately, the use of linker variables requires some setup.

## Setting Up Linker Variables
There are four linker variables needed for the checkpoint to function properly:
- `__STACK_END`
- `SYSMEM_SIZE_SYM`
- `DATA_START_SYM`
- `DATA_END_SYM`
