/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include<common/coolio.h>

#define HIGH_BYTE 0xFF00
#define SCREEN_WIDTH 80

using namespace coolOS::common;


void coolOS::common::printf(char* str){
    
    
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

void coolOS::common::printfHex(uint8_t key){
    char * msg = "00";
    char * hex = "0123456789ABCDEF";
    msg[0] = hex[(key >> 4) & 0x0F];
    msg[1] = hex[key & 0x0f];
    printf(msg);
}