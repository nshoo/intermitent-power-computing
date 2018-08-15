All that exists of this strange idea is an RPN calculator written in C.

An example calculation would look like:

12 5 + . # Outputs 17
(the dot just means print the top of the stack)

- Only for an Arduino so far
  - Would be easy to transfer to MSP but Serial is painful
- Not enough to qualify as a Forth yet
  - No floating point support (not strictly needed)
  - Needs ability to define new words
  - Needs looping & branching
  - Does however have basic stack operations (dup, swap, over)
- Could be made to include non-volatile operation pretty easily
  - Only data structure is a Stack
  - Only one data type
