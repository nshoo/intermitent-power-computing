#include <msp430.h>

#pragma PERSISTENT(shift)
unsigned char shift = '\xdb';
#pragma PERSISTENT(count)
int count = 0;
#pragma PERSISTENT(canary)

void reset(){
    shift = '\xdb';
    count = 0;
}

void performShift(){
    unsigned int xored;
    if(count < 100){
        xored = ((shift >> 1) & 1U) ^ ((shift >> 4) & 1U);
        if(xored) shift = (0x80 | (shift >> 1));
        else shift >>= 1;
        count++;
        __delay_cycles(100000);
        performShift();

    } else if(shift == 9){
        P1OUT = BIT1;
    }
}

void main(void) {
    SYSCFG0 = FRWPPW | DFWP;
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
                                            // to activate previously configured port settings
    P1DIR |= 0x03;                         // Set P1.0 to output direction
    P1OUT = 0;

    switch (SYSRSTIV) {
    case SYSRSTIV_RSTNMI: reset(); break;
    case SYSRSTIV_BOR: performShift(); break;
    }

}
