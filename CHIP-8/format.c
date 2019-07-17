#include <stdio.h>
#include <stdint.h>

uint8_t buffer[4096];

int main(){
	FILE * file = fopen("test.bin", "rb");
	unsigned long length;
	if(file){
		fseek(file, 0L, SEEK_END);
		length = ftell(file);
		rewind(file);
		fread(buffer, 4096, 1, file);
		for(unsigned long i = 0; i < length; i++){
			printf("0x%x, ", buffer[i]);
		}
	} else {
		printf("File open error!\n");
	}
}