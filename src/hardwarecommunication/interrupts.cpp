/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <hardwarecommunication/interrupts.h>
#include <common/coolio.h>

using namespace coolOS;
using namespace coolOS::common;
using namespace coolOS::hardwarecommunication;



InterruptHandler::InterruptHandler(InterruptManager* interruptManager, uint8_t interruptNumber){
    this->interruptNumber = interruptNumber;
    this->interruptManager = interruptManager;
    //insert this interrupt into it's managers handler pool
    interruptManager->handlers[interruptNumber] = this;
    //from now on, when an interrupt with interruptNumber will be raised, this handler's HandleInterrupt method will be called.
}
InterruptHandler::~InterruptHandler(){
    if(interruptManager->handlers[interruptNumber] == this){
       interruptManager->handlers[interruptNumber] = 0;
    }
   
}

uint32_t InterruptHandler::HandleInterrupt(uint32_t esp){ 
    
    return esp;
}












InterruptManager::GateDescriptor InterruptManager::interruptDescriptorTable[256];

//set the active manager to null
InterruptManager * InterruptManager::ActiveInterruptManager = 0;


void InterruptManager::SetInterruptDescriptorTableEntry(
        uint8_t interrupt,
        uint16_t codeSegmentSelectorOffset,
        void (*handler)(),  //the location of the handler function
        uint8_t DescriptorPrivilegLevel,
        uint8_t DescriptorType){
    
//do some bit fiddeling in order to fit the IDT descriptor structure    
    interruptDescriptorTable[interrupt].handlerAddressLowBits = ((uint32_t)handler) & 0xffff;
    interruptDescriptorTable[interrupt].handlerAddresshighBits = (((uint32_t)handler) >> 16) & 0xffff; 
    interruptDescriptorTable[interrupt].gdt_codeSegmentSelector = codeSegmentSelectorOffset;
    
    const uint8_t IDT_DESC_PRESENT = 0x80;
    interruptDescriptorTable[interrupt].access = IDT_DESC_PRESENT | ((DescriptorPrivilegLevel&3) << 5)| DescriptorType ;
    interruptDescriptorTable[interrupt].reserved = 0;

    
}
    

    
InterruptManager::InterruptManager(uint16_t hardwareInterruptOffset, TaskManager* taskManager)
    :   picMasterCommand(0x20), //port number for first pic's command port
        picMasterData(0x21), //port number for first pic's data port
        picSlaveCommand(0xA0), //port number for second pic's command port
        picSlaveData(0xA1) //port number for second pic's data port
{
    //initialize class variables
    this->taskManager = taskManager;
    this->hardwareInterruptOffset = hardwareInterruptOffset;
    uint32_t CodeSegment = GDTGetCodeSegment();
    
    
    const uint8_t IDT_INTERRUPT_GATE = 0xE; //define our interrupts with the Interrupt Gate
    
    //fill the IDT with ignored interrupts
    for(uint8_t i=255; i>0; --i){
        SetInterruptDescriptorTableEntry(i, CodeSegment, &InterruptIgnore, 0, IDT_INTERRUPT_GATE);
        handlers[i] = 0;
        
    }
    
    SetInterruptDescriptorTableEntry(0, CodeSegment, &InterruptIgnore, 0, IDT_INTERRUPT_GATE);
    handlers[0] = 0;
   
    //insert exception handlers
    SetInterruptDescriptorTableEntry(0x00, CodeSegment, &HandleException0x00, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x01, CodeSegment, &HandleException0x01, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x02, CodeSegment, &HandleException0x02, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x03, CodeSegment, &HandleException0x03, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x04, CodeSegment, &HandleException0x04, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x05, CodeSegment, &HandleException0x05, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x06, CodeSegment, &HandleException0x06, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x07, CodeSegment, &HandleException0x07, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x08, CodeSegment, &HandleException0x08, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x09, CodeSegment, &HandleException0x09, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x0A, CodeSegment, &HandleException0x0A, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x0B, CodeSegment, &HandleException0x0B, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x0C, CodeSegment, &HandleException0x0C, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x0D, CodeSegment, &HandleException0x0D, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x0E, CodeSegment, &HandleException0x0E, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x0F, CodeSegment, &HandleException0x0F, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x10, CodeSegment, &HandleException0x10, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x11, CodeSegment, &HandleException0x11, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x12, CodeSegment, &HandleException0x12, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x13, CodeSegment, &HandleException0x13, 0, IDT_INTERRUPT_GATE);

    //insert hardware interrupts handlers
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x00, CodeSegment, &HandleInterruptRequest0x00, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x01, CodeSegment, &HandleInterruptRequest0x01, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x02, CodeSegment, &HandleInterruptRequest0x02, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x03, CodeSegment, &HandleInterruptRequest0x03, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x04, CodeSegment, &HandleInterruptRequest0x04, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x05, CodeSegment, &HandleInterruptRequest0x05, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x06, CodeSegment, &HandleInterruptRequest0x06, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x07, CodeSegment, &HandleInterruptRequest0x07, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x08, CodeSegment, &HandleInterruptRequest0x08, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x09, CodeSegment, &HandleInterruptRequest0x09, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x0A, CodeSegment, &HandleInterruptRequest0x0A, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x0B, CodeSegment, &HandleInterruptRequest0x0B, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x0C, CodeSegment, &HandleInterruptRequest0x0C, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x0D, CodeSegment, &HandleInterruptRequest0x0D, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x0E, CodeSegment, &HandleInterruptRequest0x0E, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x0F, CodeSegment, &HandleInterruptRequest0x0F, 0, IDT_INTERRUPT_GATE);
    
    //insert software interrupt handler
    SetInterruptDescriptorTableEntry(0x80, CodeSegment, &HandleInterruptRequest0x80, 0, IDT_INTERRUPT_GATE);

    //initialize the PIC - 3 next data inputs are parameters for this command
    picMasterCommand.SlowWrite(0x11);
    picSlaveCommand.SlowWrite(0x11);
    
    picMasterData.SlowWrite(hardwareInterruptOffset); //add hardwareInterruptOffset to the interrupt you get
    picSlaveData.SlowWrite(hardwareInterruptOffset + 8); //add hardwareInterruptOffset+8 to the interrupt you get - every pic can use 8 interrupts
    
    picMasterData.SlowWrite(0x04); // tells the master that he is the master
    picSlaveData.SlowWrite(0x02); // tells the slave that he is the slave
    
    // 0x86 mode
    picMasterData.SlowWrite(0x01); 
    picSlaveData.SlowWrite(0x01); 
    
    //reset PIC mask
    picMasterData.SlowWrite(0x00); 
    picSlaveData.SlowWrite(0x00); 
    
    interruptDescriptorTablePointer idt;
    idt.size = 256 * sizeof(GateDescriptor) -1;
    idt.base = (uint32_t)interruptDescriptorTable;
    
    asm volatile("lidt %0" :: "m" (idt)); //load interrupt descriptor table
    
}

uint16_t InterruptManager::getHardwareInterruptOffset(){
    return this->hardwareInterruptOffset;
}

InterruptManager::~InterruptManager(){
    Deactivate();
}

//activate this interrupt manager
void InterruptManager::Activate(){
    if(ActiveInterruptManager != 0){
        ActiveInterruptManager->Deactivate();
    }
    ActiveInterruptManager = this;
    
    asm("sti"); //start interrupts 
}

//disable this interrupt manager
void InterruptManager::Deactivate(){
    if(ActiveInterruptManager == this){
    
    ActiveInterruptManager = 0;
    asm("cli"); //clear interrupts 
    
    }
}

uint32_t InterruptManager::handleInterrupt(uint8_t interrupt, uint32_t esp){
    if(ActiveInterruptManager != 0){
        return ActiveInterruptManager->DoHandleInterrupt(interrupt, esp);
    }
    return esp;
}

uint32_t InterruptManager::DoHandleInterrupt(uint8_t interrupt, uint32_t esp){
    static bool is_alive = true;
    /*if(interrupt != hardwareInterruptOffset){ //don't print for hardware clock interrupt
        printf("received interrupt 0x");
        printfHex(interrupt);
    }*/
    
    //check if an interrupt handler is defined
    if(handlers[interrupt] != 0){
        
        esp = handlers[interrupt]->HandleInterrupt(esp);
    }
    else if(interrupt != hardwareInterruptOffset){ //don't print for hardware clock interrupt
        if(interrupt == 0x0D){
            if(is_alive){
                printf("!!!\nSYSTEM IS DEAD X_X\n!!!");
                is_alive = false;
            }
        }else{
        printf("UNHANDLED INTERRUPT 0x");
        printfHex(interrupt-hardwareInterruptOffset);
        printf("\n");
        }
    }
    
    if(interrupt == hardwareInterruptOffset){ //timer interrupt
       // printf("************************ calling task *******************");
        esp = (uint32_t)taskManager->Schedule((CPUState*)esp); //TODO: fix datatype from uint32 to cpustate
    }
    
    //answer the interrupt, so new interrupts can come 
    if(hardwareInterruptOffset <= interrupt && interrupt < hardwareInterruptOffset +16 ){ //if it is a hardware interrupt
        
        picMasterCommand.SlowWrite(0x20);
        if(hardwareInterruptOffset +8 <= interrupt){ //if the interrupt came from slave pic
            picSlaveCommand.SlowWrite(0x20);
        }
    }
    
    
    
    return esp;
}