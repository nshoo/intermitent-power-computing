#include <msp430.h>
#include <stdlib.h>
#include "checkpoint.h"

unsigned char shift = '\x9b';

void pause(){
    volatile unsigned int i;
    for(i=20000; i>0; i--);
}
/**
 * main.c
 */
int main(void)
{
    SYSCFG0 = FRWPPW | DFWP;
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	PM5CTL0 &= ~LOCKLPM5;
	P1DIR = BIT0 | BIT1;
	P1OUT = 0;
	fullRestore();
	unsigned int xored;
	long i;
	for(i = 0; i < 5000; i++){
	    P1OUT = BIT1;
	    xored = ((shift >> 5) & 1U) ^ ((shift >> 4) & 1U); // second to last and fifth to last bit xor
	    shift >>= 1;
	    if(xored) shift |= 0x80;
	    SIMPLE_CHECKPOINT(NULL, 0);
	}
	if(shift == 172) P1OUT = BIT0;
	return 0;
}
