#include <msp430.h>

int nums[] = {34,  3, 40, 91, 25, 22,  4, 96, 84, 88, 62, 35,  6, 12, 97, 51, 27,  7, 81, 14, 13,  9, 39, 52, 92, 44, 67, 59, 42, 26, 78, 90, 76, 68, 28, 20, 65, 57, 21,  8, 72, 10, 70, 18, 31, 36, 46, 64, 93, 56, 17, 37, 99, 43, 61, 69, 45, 19, 63, 98, 16, 48, 50, 29, 83, 75, 15, 23, 94,  2, 24, 89, 71,100, 58,  5, 33, 30, 41, 86, 77, 55, 95, 66, 49, 47, 54, 60, 73, 79, 87, 74, 11, 80, 32, 82, 38, 53, 85,  1};
int correct[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100};

void sortNums(){
    int i;
    int count = 0;
    for(i = 0; i < (sizeof(nums) / sizeof(int)) - 1; i++){
        if(nums[i+1] < nums[i]){
            int temp = nums[i];
            nums[i] = nums[i+1];
            nums[i+1] = temp;
            count++;
        }
    }
    if(count != 0) sortNums();
}

int checkCorrect(){
    int i;
    for(i = 0; i < (sizeof(nums) / sizeof(int)) -1; i++){
        if(nums[i] != correct[i]) return 0;
    }
    return 1;
}

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
                                            // to activate previously configured port settings
    P1DIR |= 0x01;                          // Set P1.0 to output direction
    P1OUT = 0x00;

    sortNums();
    if(checkCorrect()) P1OUT = BIT0;

}
