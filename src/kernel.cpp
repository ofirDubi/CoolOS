
#include <common/types.h>
#include <gdt.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/pci.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/vga.h>
#include <drivers/ata.h>
#include <gui/desktop.h>
#include <gui/window.h>
#include <memorymanagment.h>
#include <multitasking.h>
#include <common/coolio.h>
#include <syscalls.h>

#include <drivers/amd_am79c973.h>
#include <net/etherframe.h>
//uncomment for gui
//#define GRAPHICS_MODE

using namespace coolOS;
using namespace coolOS::common;
using namespace coolOS::drivers;
using namespace coolOS::hardwarecommunication;
using namespace coolOS::gui;
using namespace coolOS::net;

//default drivers 

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
        VideoMemory[80*y +x] =  (VideoMemory[80*y+x] & 0xF000) >> 4 
                                    | (VideoMemory[80*y+x] & 0x0F00) << 4 
                                    | (VideoMemory[80*y+x] & 0x00FF);
    }
    
    void OnMouseMove(int x_offset, int y_offset){
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

//multitasking test
void sysprintf(char * str){
    asm("int $0x80" : : "a" (4), "b" (str));
}
void taskA(){
   while(true){
        sysprintf("A");
    }
}

void taskB(){
    while(true){
       sysprintf("B"); 
    }
    
    

}
typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for(constructor* i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}


extern "C" void kernelMain(void * multiboot_structure, uint32_t magicnumber){ //maybe irelevent
    
    printf("Hello world\n");
    //create Global Descriptor Table
    GlobalDescriptorTable gdt;
    
    //the size of the ram
    uint32_t* memupper = (uint32_t*)(((size_t)multiboot_structure) + 8);
    size_t heap = 10*1024*1024; //10 MB
    
    MemoryManager memoryManager(heap, (*memupper)*1024 - heap - 10*1024);
    //dynamic memory allocation
    printf("heap: 0x");
    printfHex((heap >> 24) & 0xff);
    printfHex((heap >> 16) & 0xff);
    printfHex((heap >> 8) & 0xff);
    printfHex((heap) & 0xff);
    void * allocated = memoryManager.malloc(1024);
    
    printf("\nallocated: 0x");
    printfHex(((size_t)allocated >> 24) & 0xff);
    printfHex(((size_t)allocated >> 16) & 0xff);
    printfHex(((size_t)allocated >> 8) & 0xff);
    printfHex(((size_t)allocated) & 0xff);
    printf("\n");
    
    //multi-tasking
    TaskManager taskManager;
    /*
    Task task1(&gdt, taskA);
    Task task2(&gdt, taskB);
    
    taskManager.AddTask(&task1);
    taskManager.AddTask(&task2);
    */
    
    //set up PIC
    InterruptManager interrupts(0x20,&gdt, &taskManager);
    SyscallHandler syscalls(&interrupts, 0x80); //0x80 is software interrupt
    
    printf("Initializing Hardware, Stage 1\n");
#ifdef GRAPHICS_MODE
     Desktop desktop (320, 200, RGB_BLUE);
#endif
    DriverManager drvManager;
    
       
#ifdef GRAPHICS_MODE
        KeyboardDriver keyboard(&interrupts, &desktop);
#else
        PrintfKeyBoardEventHandler kbhandler;
        KeyboardDriver keyboard(&interrupts, &kbhandler);
#endif
        drvManager.AddDriver(&keyboard);
        
       
#ifdef GRAPHICS_MODE
        MouseDriver mouse(&interrupts, &desktop);
#else
        MouseToConsole mousehandler;
        MouseDriver mouse(&interrupts, &mousehandler);
#endif
        
        drvManager.AddDriver(&mouse);
        
        //set up PCI
        PeripheralComponentInterconnectController PCIController;
        PCIController.SelectDrivers(&drvManager, &interrupts);
        
        
        VideoGraphicsArray vga;
        
    printf("Initializing Hardware, Stage 2\n");
        //activate drivers
        drvManager.ActivateAll();
        
        
    //printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    printf("Initializing Hardware, Stage 3\n");
   
    //set vga
#ifdef GRAPHICS_MODE
    vga.SetMode(320, 200, 8);
    Window win1(&desktop, 10, 10, 20, 20, RGB_RED);
    desktop.AddChild(&win1);
    Window win2(&desktop, 40, 15, 30, 30, RGB_GREEN);
    desktop.AddChild(&win2);
#endif
    // set up hard drive
    //sends interrupt 14
    /*
    AdvancedTechnologyAttachment ata0m(0x1F0, true);
    printf("ATA Primary Master:\n");
    ata0m.Identify();
    AdvancedTechnologyAttachment ata0s(0x1F0, false);
    printf("\nATA Primary Slave:\n");
    ata0s.Identify();
    
    char * ataBuffer = "im such a cool operating system\n";
    ata0m.Write28(0, (uint8_t *)ataBuffer, 32);
    ata0m.Flush();

    ata0m.Read28(0, (uint8_t *)ataBuffer, 32);
    
    printf("accessing hard drive");
      //interrupt 15
    AdvancedTechnologyAttachment ata1m(0x170, true);
    AdvancedTechnologyAttachment ata1s(0x170, false);
    
    //check interrupts for third and fourth
    //if we have more - third 0x1E8
    //fourth: 0x168
    */
    
    amd_am79c973* eth0 = (amd_am79c973*)(drvManager.drivers[2]);
    EtherFrameProvider etherframe(eth0);
    etherframe.Send(0xFFFFFFFFFFFF, 0x0608, (uint8_t *)"FOO", 3);
    
    printf("activating interrupts");
   //start accepting interrupts
   interrupts.Activate();
   printf("interrupts activated");
    while(1){
#ifdef GRAPHICS_MODE
        desktop.Draw(&vga);
#endif
    }
}