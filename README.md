# Intermediate Power Computing (Project Repository)

There really isn't too much here, as of now (I plan to fix that)
Nevertheless, the current contents are (each has a folder):
- _"Code Measuring"_ - simple python script for measuring how much code a block of C compiles down to
  - Uses assembly inlining in c to place ASM comments in C code
  - Comments from the C code pass down to the assembly, serving as markers or "flags"
  - Python script then prints out number of lines of ASM between flags
  - Explaination of how to use the script is in the folder, as well as a sample C program to test
  - Pretty rudimentary, and still doesn't account for jumps


- _"Forthish RPN Calc"_ - A non-volatile calculator? (Mostly unimplemented)
  - As a sort of demo/proof of concept
  - [RPN][1] Calculator that would save results across reboots
  - Inspired by learning about the [Forth][2] language
  - It would be interesting to make a forth which could compute through power loss, since its so simple to implement and the only data structure is a "stack" of integers
  - The intense factoring enforced by Forth could make Forth "words" (functions) the ideal "fundamental block" for power loss resistant programs
  - **Pretty theoretical/vaguely off topic**
  - Part of the reason for _"New Protocol"_

  [1]: https://en.wikipedia.org/wiki/Reverse_Polish_notation
  [2]: https://en.wikipedia.org/wiki/Forth_(programming_language)


- _"New Protocol"_ - A protocol for transmitting text between to baords
  - This isn't too important, just sort of hobby/side-project that happened to tie in (not too practical)
  - Basically, Serial on the TI-Launchpads is confusing, and non-standard
  - Wanted to be abled to communicate with MSP430 without deciphering serial
  - Created new "Pseudo Serial" protocol sending bits over two wires between an arduino (nice serial lib) and the MSP
  - Was able to transmit the first paragraph of the wikipedia page on "computers" in about a second, from the MSP430 => Arduino => My Computer (while retaining the MSP's fast boot)
