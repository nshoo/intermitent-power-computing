A makeshift way to write data from the MSP430 => Arduino => PC
It's pretty fast and uses a protocol I made up (requires 2 wires connecting boards)
The reason for doing this from scratch is that serial on the MSP430 is confusing

Protocol
========
  - Character strings are converted to binary representations of ascii
  - Individual bits are sent by setting the DATA_WIRE to the appropriate value (HIGH for 1 LOW for 0) and then pulsing the TRIGGER_PIN once
  - To reduce errors a start byte is sent before every character (10101010)
  - On the other end the bits (after a start byte) are interpreted and displayed
  - Apparently able to operate as fast as both microcontrollers can operate (no delays)
  - Transmitted a paragraph in about a second

Challenges
==========
Because there are no pull down resistors (for simplicity) the voltage on the wires tends to float all over the place (especially after boot). Before the implementation of the start byte this manifested itself as gibberish. The start byte also helps keep the bits aligned, so only a single char can be messed up if there is an error (instead of the whole string). Despite the start byte, there are still some errors. Once I get communication going both ways instead of just one ways I can implement error checking and stop data corruption.

** NOTE: This is a little impractical, yet it works surprisingly well
