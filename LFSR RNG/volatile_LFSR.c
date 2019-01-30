#include <msp430.h> 

unsigned char shift = '\xdb'; /* 11011011 */

/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;
	WDTCTL = WDTPW | WDTHOLD;
	PM5CTL0 &= ~LOCKLPM5;
	P1DIR = 0x03;
	P1DIR |= BIT5;
	P1OUT = BIT1;
	unsigned int xored;
	long i;
	for(i = 0; i < 100000; i++){
	    xored = ((shift >> 1) & 1U) ^ ((shift >> 4) & 1U); // second to last and fifth to last bit xor
	    shift >>= 1;
	    if(xored) shift |= 0x80;
	}
	if(shift == 151) P1OUT = BIT0;
	return 0;
}
