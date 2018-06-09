
#include <common/types.h>
//#include <gdt.h>
#include <gdt_new.h>
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
#include <net/arp.h>
#include <net/ipv4.h>
#include <net/icmp.h>
//uncomment for gui
//#define GRAPHICS_MODE
//#define BOOK_MODE 
//#define NET
//#define WRITE_TO_DRIVE
using namespace coolOS;
using namespace coolOS::common;
using namespace coolOS::drivers;
using namespace coolOS::hardwarecommunication;
using namespace coolOS::gui;
using namespace coolOS::net;

//default drivers 

//default driver that prints key strokes
class PrintfKeyBoardEventHandler : public KeyBoardEventHandler{
public:
    void OnKeyDown(uint8_t c){
        char * foo = " ";
        foo[0] = c;
        printf(foo);
    }
};

//default mouse handler that shows the mouse cursor on the screen
class MouseToConsole : public MouseEventHandler{
    int8_t x, y;
public:
    MouseToConsole(){
        //the location of the screen in memory
        static uint16_t * VideoMemory = (uint16_t *)0xb8000; 
        //define the initial location of the mouse cursor as values 
        x=40;
        y=12;
        
        //draw the cursor on the screen by flipping the background color with the forground color
       //the cursor will be white
        VideoMemory[80*y +x] =  (VideoMemory[80*y+x] & 0xF000) >> 4 
                                    | (VideoMemory[80*y+x] & 0x0F00) << 4 
                                    | (VideoMemory[80*y+x] & 0x00FF);
    }
    
    void OnMouseMove(int x_offset, int y_offset){
        static uint16_t * VideoMemory = (uint16_t *)0xb8000; 
        
        //delete the cursor from it's previous location
        VideoMemory[80*y +x] =  ((VideoMemory[80*y+x] & 0xF000) >> 4) 
                                    | ((VideoMemory[80*y+x] & 0x0F00) << 4) 
                                    | ((VideoMemory[80*y+x] & 0x00FF));
        // change x
        x+= x_offset;
        
        //check x axis limits
        if( x<0 ){
            x=0;
        }
        if(x >=80){
            x = 79;
        }
        
        //change y
        y += y_offset;
        
        //check y axis limits
        if( y<0 ){
            y=0;
        }
        if(y >=25){
            y = 24;
        }
        
        //draw the cursor in it's new location
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
extern "C" constructor start_ctors; //receive start pointer from linker
extern "C" constructor end_ctors; //receive end pointer from linker
extern "C" void callConstructors()
{
    for(constructor* i = &start_ctors; i != &end_ctors; i++) //go through each address between the start pointer and the end pointer
        (*i)(); //initialize constructors
}



#ifdef BOOK_MODE
extern "C" void kernelMain(void * multiboot_structure, uint32_t magicnumber){ //receive the multiboot structure
    
    printf("Hello World :)\n"); // print :)
        //create Global Descriptor Table
    init_gdt();
    printf("done with gdt\n");
    
    TaskManager taskManager;
        //set up PIC
    InterruptManager interrupts(0x20, &taskManager);
    
    //create a driver manager
    DriverManager drvManager;
    
    //create an instance of the default event handler
    PrintfKeyBoardEventHandler kbhandler;
    
    //create a KeyboardDriver that uses our default keyboard handler 
    KeyboardDriver keyboard(&interrupts, &kbhandler);
    
    //the size of the memupper points to the start of our kernel code (including our stack)
    uint32_t* memupper = (uint32_t*)(((size_t)multiboot_structure) + 8);
    
    //the heap starts at 10 MB
    size_t heap = 10*1024*1024; //10 MB
    
    //start the heap at 10MB, a large enough margin
     MemoryManager memoryManager(heap, (*memupper)*1024 - heap - 10*1024);
     
    //print heap location 
    printf("heap: 0x");
    printfHex((heap >> 24) & 0xff);
    printfHex((heap >> 16) & 0xff);
    printfHex((heap >> 8) & 0xff);
    printfHex((heap) & 0xff);
    //dynamic memory allocation
    void * allocated_1 = memoryManager.malloc(1024);
    void * allocated = memoryManager.malloc(1024);
    //print allocated location
    printf("\nallocated: 0x");
    printfHex(((size_t)allocated >> 24) & 0xff);
    printfHex(((size_t)allocated >> 16) & 0xff);
    printfHex(((size_t)allocated >> 8) & 0xff);
    printfHex(((size_t)allocated) & 0xff);
    printf("\n");
    
    //free the allocated memory
    memoryManager.free(allocated);
 
    //add keyboard driver
    drvManager.AddDriver(&keyboard);
    
    //create a mouse driver and handler
    MouseToConsole mousehandler;
    MouseDriver mouse(&interrupts, &mousehandler);

    //insert the mouse driver into the driver manager
    drvManager.AddDriver(&mouse);
    
    //activate drivers
    drvManager.ActivateAll();

 
    
   
    
    
    interrupts.Activate();
    printf("activated interrupts");
    
    
    while(true){ //keep the operating system alive
        
    }
}



#endif


#ifndef BOOK_MODE


extern "C" void kernelMain(void * multiboot_structure, uint32_t magicnumber){ //receive the 
    
    printf("Hello world\n");
    //create Global Descriptor Table
   // GlobalDescriptorTable gdt;
    init_gdt();

    
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
    Task task1(taskA);
    Task task2( taskB);
    
    taskManager.AddTask(&task1);
    taskManager.AddTask(&task2);
    */
    
    //set up PIC
    InterruptManager interrupts(0x20, &taskManager);
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
#ifdef WRITE_TO_DRIVE
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
    
#endif
#ifndef NET
   
   //start accepting interrupts
   interrupts.Activate();
   printf("interrupts activated");
#endif

    
#ifdef NET 
    //setup ip
    uint8_t ip1 = 10, ip2 =0, ip3 = 2, ip4 = 15;    
    uint8_t gip1 = 10, gip2 =0, gip3 = 2, gip4 = 2;
    uint8_t subnet1 = 255, subnet2 =255, subnet3 = 255, subnet4 = 0;

    uint32_t ip_be = ((uint32_t)ip4 <<24)
            | ((uint32_t)ip3 <<16)  
            | ((uint32_t)ip2 <<8)  
            | (uint32_t)ip1 ;
    
    uint32_t gip_be = ((uint32_t)gip4 <<24)
            | ((uint32_t)gip3 <<16)  
            | ((uint32_t)gip2 <<8)  
            | (uint32_t)gip1 ;
    
    uint32_t subnet_be = ((uint32_t)subnet4 <<24)
            | ((uint32_t)subnet3 <<16)  
            | ((uint32_t)subnet2 <<8)  
            | (uint32_t)subnet1 ;
    
    amd_am79c973* eth0 = (amd_am79c973*)(drvManager.drivers[2]);  
    
   
    
    eth0->SetIPAddress(ip_be);
    
    EtherFrameProvider etherframe(eth0);
    
    AddressResolutionProtocol arp(&etherframe);
    
    
    
    
    InternetProtocolProvider ipv4(&etherframe, &arp, gip_be, subnet_be);

    //etherframe.Send(0xFFFFFFFFFFFF, 0x0608, (uint8_t *)"FOO", 3);
    
    InternetControlMessageProtocol icmp(&ipv4);
    
    printf("activating interrupts");
   //start accepting interrupts
   interrupts.Activate();
   printf("interrupts activated");
   
  printf("\n\n\n\n\n\n\n\n\n");
  
  
  ipv4.Send(gip_be, 0x014, (uint8_t*) "foobar", 6);
  printf("Broadcasting\n\n\n\n\n\n\n\n\n");
   arp.BroadcastMACAddress(gip_be);
  printf("sending icmp\n");
  icmp.RequestEchoReply(gip_be);
  
  /* 
   printf("ARP: resolving address\n");
   uint64_t ans = arp.Resolve(gip_be);
   
   uint8_t * buffer = (uint8_t *)&ans;
   for(int i=0; i<8;i++){
       printfHex(buffer[i]);
       printf(" ");
   }
   printf("\nfinished resolving\n");
  */
#endif
#ifdef GRAPHICS_MODE
    desktop.Draw(&vga); 
#endif 

    while(1){
#ifdef GRAPHICS_MODE
        //desktop.Draw(&vga);
#endif
    }
}
#endif