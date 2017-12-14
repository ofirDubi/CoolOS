#define HIGH_BYTE 0xFF00
#include "types.h"

void printf(char *str){
    
    uint16_t * VideoMemory = (uint16_t *)0xb8000;
    
    for(int i=0; str[i] != '\0'; i++){
        VideoMemory[i] = (VideoMemory[i] & HIGH_BYTE) | str[i];
        
    }
}

extern "C" void kernelMain(void * multiboot_structure, uint32_t magicnumber){ //maybe irelevent
    
    printf("Hello World");
    
    while(1);
}