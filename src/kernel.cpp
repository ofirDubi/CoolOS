#define HIGH_BYTE 0xFF00
#define SCREEN_WIDTH 80

#include <common/types.h>
#include <gdt.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/pci.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/vga.h>

using namespace coolOS;
using namespace coolOS::common;
using namespace coolOS::drivers;
using namespace coolOS::hardwarecommunication;


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

void printfHex(uint8_t key){
    char * msg = "00";
    char * hex = "0123456789ABCDEF";
    msg[0] = hex[(key >> 4) & 0x0F];
    msg[1] = hex[key & 0x0f];
    printf(msg);
}


class PrintfKeyBoardEventHandler : public KeyBoardEventHandler{
public:
    void OnKeyDown(char c){
        char * foo = " ";
        foo[0] = c;
        printf(foo);
    }
};


class MouseToConsole : public MouseEventHandler{
    int8_t x, y;
public:
    MouseToConsole(){
    static uint16_t * VideoMemory = (uint16_t *)0xb8000; 

    x=40;
    y=12;
    //initiate the cursor to values x=40 and y=12
    VideoMemory[80*y +x] =  ((VideoMemory[80*y+x] & 0xF000) >> 4) 
                                | ((VideoMemory[80*y+x] & 0x0F00) << 4) 
                                | ((VideoMemory[80*y+x] & 0x00FF));
    }
    
    void OnMouseMove(int8_t x_offset, int8_t y_offset){
        static uint16_t * VideoMemory = (uint16_t *)0xb8000; 

        VideoMemory[80*y +x] =  ((VideoMemory[80*y+x] & 0xF000) >> 4) 
                                    | ((VideoMemory[80*y+x] & 0x0F00) << 4) 
                                    | ((VideoMemory[80*y+x] & 0x00FF));
            x+= x_offset;
            if( x<0 ){
                x=0;
            }
            if(x >=80){
                x = 79;
            }

            y += y_offset;
            if( y<0 ){
                y=0;
            }
            if(y >=25){
                y = 24;
            }
            //show the cursor - flip the background and the forgroud colors
            VideoMemory[80*y +x] =  ((VideoMemory[80*y+x] & 0xF000) >> 4) 
                                    | ((VideoMemory[80*y+x] & 0x0F00) << 4) 
                                    | ((VideoMemory[80*y+x] & 0x00FF));
    }

};
extern "C" void kernelMain(void * multiboot_structure, uint32_t magicnumber){ //maybe irelevent
    
    printf("Hello world\n");
    
    GlobalDescriptorTable gdt;
    InterruptManager interrupts(&gdt);
    
    printf("Initializing Hardware, Stage 1\n");

    
    DriverManager drvManager;
    
        PrintfKeyBoardEventHandler kbhandler;
        KeyboardDriver keyboard(&interrupts, &kbhandler);
        drvManager.AddDriver(&keyboard);
        
        MouseToConsole mousehandler;
        MouseDriver mouse(&interrupts, &mousehandler);
        drvManager.AddDriver(&mouse);
        
        PeripheralComponentInterconnectController PCIController;
        PCIController.SelectDrivers(&drvManager, &interrupts);
        
        VideoGraphicsArray vga;
        
    printf("Initializing Hardware, Stage 2\n");

        drvManager.ActivateAll();

    printf("Initializing Hardware, Stage 3\n");

    interrupts.Activate();
    
    //set vga
    vga.SetMode(320, 200, 8);
    
    //draw blue rectangle
    
    for(int32_t y = 0; y<200; y++){
        for(int32_t x = 0; x<320; x++){
            VideoGraphicsArray::RGB color = {0x00, 0x00, 0xA8};
            vga.PutPixel(x,y,color);
        }
    }
    
    while(1);
}