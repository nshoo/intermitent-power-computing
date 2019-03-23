mov r0, 5 # bin vals
mov r1, 0 # count
mov r2, 64 # place vals
div r2, r2, 2 # decrease place vals
cmp r2, r0 # check whether place fits
jmp gt, 11 # jump if doesn't
sub r0, r0, r2 # take away place
mov r3, 1 # prepare to write a one
write r1, r3 # write one
jmp 13 # skip writing 0
mov r3, 0 # prep write 0
write r1, r3 # write 0
add r1, r1, 1 # increase count
cmp r2, 1 # comp place vals
jmp ne, 4 # END OF VALUE SETUP
mov r3, 10 # LFSR Loops
read r4, 2
read r5, 4
cmp r4, r5
jmp ne, 23
mov r4, 0
jmp 24
mov r4, 1
mov r0, 4 # Count of places
read r1, r0
add r2, r0, 1
write r2, r1
sub r0, r0, 1
cmp r0, -1
jmp gt, 25 # end of shift block
write 0, r4
sub r3, r3, 1
cmp r3, 0
jmp gt, 17 # END of LFSR
mov r0, 5
mov r1, 1
mov r3, 0
read r2, r0
mul r2, r2, r1
add r3, r3, r2
mul r1, r1, 2
sub r0, r0, 1
cmp r0, -1
jmp gt, 38