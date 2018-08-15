#include <stdio.h>
#define measurementFlag() __asm__("#MEASUREFLAG")

int main(){
  measurementFlag();
  printf("Hello World!\n");
  measurementFlag();
  measurementFlag();
  printf("Testing testing!\n");
  printf("12 times 5 is %d!", 60);
  measurementFlag();
  return 0;
}
