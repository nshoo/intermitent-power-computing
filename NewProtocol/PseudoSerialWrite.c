/*
Simple program to write arbitrary text over two wires to a reciver (Arduino)
Currently writing the first paragraph of wiki page on computers in ~1s
(protocol explaination in readme)
*/

#include <msp430.h>

#define TRIGGER_PIN BIT6
#define DATA_PIN BIT7
#define TOLERANCE 0

void delay(long loops){
    volatile long i;
    for(i = loops; i > 0; i--);
}

void sendStartByte(){
    P1OUT = 0;
    int i;
    for(i = 0; i < 8; i++){
        P1OUT ^= DATA_PIN;
        P1OUT ^= TRIGGER_PIN;
        delay(TOLERANCE);
        P1OUT ^= TRIGGER_PIN;
        delay(TOLERANCE);
    }
    P1OUT = 0;
}

void sendChar(char c){
    int i;
    int val = 256;
    for(i = 0; i < 8; i++){
        val /= 2;
        P1OUT = 0;
        delay(TOLERANCE);
        if(val <= c){
            P1OUT = DATA_PIN;
            c -= val;
        }
        delay(10);
        P1OUT |= TRIGGER_PIN;
        delay(TOLERANCE);
    }
    P1OUT = 0;
}

void sendString(const char str[]){
    unsigned int i;
    for(i = 0; str[i] != '\0'; i++){
        sendStartByte();
        sendChar(str[i]);
    }
}

int main(void)
{
	WDTCTL = WDTPW|WDTHOLD;
	P1DIR = TRIGGER_PIN + DATA_PIN + BIT0;
	PM5CTL0 &= ~LOCKLPM5;
	delay(200000);
	P1OUT = BIT0;
	delay(50000);
	sendString("A computer is a device that can be instructed to carry out sequences of arithmetic or logical operations automatically via computer programming\nModern computers have the ability to follow generalized sets of operations, called programs\nThese programs enable computers to perform an extremely wide range of tasks.");
	return 0;
}
