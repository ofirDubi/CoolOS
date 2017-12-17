#define HIGH_BYTE 0xFF00
#define SCREEN_WIDTH 80

#include "types.h"
#include "gdt.h"
#include "interrupts.h"
#include "keyboard.h"

void printf(char *str){
    
    
    uint16_t * VideoMemory = (uint16_t *)0xb8000;
    
    static uint8_t x = 0, y=0;

    for(int i=0; str[i] != '\0'; i++){
        switch(str[i]){
                
            case '\n':
                y++;
                x =0;
                break;
            default:
                VideoMemory[SCREEN_WIDTH*y +x] = (VideoMemory[SCREEN_WIDTH*y +x] & HIGH_BYTE) | str[i];
                x++;
        }
        if(x >= SCREEN_WIDTH){
            y++;
            x =0;
        }
        if(y >=25){
            
            for(y =0; y<25; y++){
                for(x =0; x<SCREEN_WIDTH; x++){
                    VideoMemory[SCREEN_WIDTH*y +x] = (VideoMemory[SCREEN_WIDTH*y +x] & HIGH_BYTE) |' ';

                }
            }
            x=0;
            y=0;
        }
    }
}

extern "C" void kernelMain(void * multiboot_structure, uint32_t magicnumber){ //maybe irelevent
    
    printf("Hello Worldsdaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    
    GlobalDescriptorTable gdt;
    InterruptManager interrupts(&gdt);
    
    KeyboardDriver keyboard(&interrupts);
    
    interrupts.Activate();
    while(1);
}