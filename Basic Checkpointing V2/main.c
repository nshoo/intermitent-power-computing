#include <msp430.h>
#include <stdlib.h>
#include "checkpoint.h"

uint32_t shift = '\x9b';

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
	P1DIR = BIT0 | BIT1 | BIT7 | BIT6;
	P1OUT = BIT7 | BIT1;
	fullRestore();
	P1OUT = 0;
	unsigned int xored;
	long i;
	for(i = 0; i < 5000; i++){
	    P1OUT = BIT1;
	    xored = ((shift >> 5) & 1U) ^ ((shift >> 4) & 1U); // second to last and fifth to last bit xor
	    shift >>= 1;
	    if(xored) shift |= 0x80000000;
	    SIMPLE_CHECKPOINT(0, 0);
	}
	if(shift == 2981315693){
	    P1OUT = BIT0;
	} else {
	    P1OUT = BIT0 | BIT1;
	}
	return 0;
}
