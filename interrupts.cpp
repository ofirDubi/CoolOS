/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "interrupts.h"

void printf(char * str);


InterruptHandler::InterruptHandler(uint8_t interruptNumber, InterruptManager* interruptManager){
    this->interruptNumber = interruptNumber;
    this->interruptManager = interruptManager;
    interruptManager->handlers[interruptNumber] = this;

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

InterruptManager * InterruptManager::ActiveInterruptManager = 0;


void InterruptManager::SetInterruptDescriptorTableEntry(
        uint8_t interruptNumber,
        uint16_t codeSegmentSelectorOffset,
        void (*handler)(),  //the location of the handler function
        uint8_t DescriptorPrivilegLevel,
        uint8_t DescriptorType){
    
    const uint8_t IDT_DESC_PRESENT = 0x80;
    
    interruptDescriptorTable[interruptNumber].handlerAddressLowBits = ((uint32_t)handler) & 0xffff;
    interruptDescriptorTable[interruptNumber].handlerAddresshighBits = (((uint32_t)handler) >> 16) & 0xffff; 
    interruptDescriptorTable[interruptNumber].gdt_codeSegmentSelector = codeSegmentSelectorOffset;
    interruptDescriptorTable[interruptNumber].access = IDT_DESC_PRESENT | DescriptorType | ((DescriptorPrivilegLevel&3) << 5);
    interruptDescriptorTable[interruptNumber].reserved = 0;

    
}
    

    
InterruptManager::InterruptManager(GlobalDescriptorTable * gdt)
    :   picMasterCommand(0x20),
        picMasterData(0x21),
        picSlaveCommand(0xA0),
        picSlaveData(0xA1)
{
    
    uint16_t CodeSegment = gdt->getCodeSegmentSelector();
    const uint8_t IDT_INTERRUPT_GATE = 0xE; //interrupt gate, not trap gate or task gate
    
    for(uint16_t i=0; i<256; i++){
        
        handlers[i] = 0;
        SetInterruptDescriptorTableEntry(i, CodeSegment, &IgnoreInterruptRequest, 0, IDT_INTERRUPT_GATE);
        
    }
    
    SetInterruptDescriptorTableEntry(0x20, CodeSegment, &HandleInterruptRequest0x00, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x21, CodeSegment, &HandleInterruptRequest0x01, 0, IDT_INTERRUPT_GATE);
    
    
    picMasterCommand.SlowWrite(0x11);
    picSlaveCommand.SlowWrite(0x11);
    
    picMasterData.SlowWrite(0x20); //add 20 to the interrupt you get
    picSlaveData.SlowWrite(0x28); //add 28 to the interrupt you get - every pic can use 8 interrupts
    
    picMasterData.SlowWrite(0x04); // tells the master that he is the master
    picSlaveData.SlowWrite(0x02); // tells the slave that he is the slave
    
    picMasterData.SlowWrite(0x01); 
    picSlaveData.SlowWrite(0x01); 
    
    picMasterData.SlowWrite(0x00); 
    picSlaveData.SlowWrite(0x00); 
    interruptDescriptorTablePointer idt;
    idt.size = 256 * sizeof(GateDescriptor) -1;
    idt.base = (uint32_t)interruptDescriptorTable;
    
    asm volatile("lidt %0" :: "m" (idt)); //load interrupt descriptor table
    
}

InterruptManager::~InterruptManager(){
    
}

void InterruptManager::Activate(){
    if(ActiveInterruptManager != 0){
        ActiveInterruptManager->Deactivate();
    }
    ActiveInterruptManager = this;
    
    asm("sti"); //start interrupts 
}

void InterruptManager::Deactivate(){
    if(ActiveInterruptManager == this){

    
    ActiveInterruptManager = 0;
    asm("cli"); //clear interrupts 
    
    }
}

uint32_t InterruptManager::handleInterrupt(uint8_t interruptNumber, uint32_t esp){
    if(ActiveInterruptManager != 0){
        return ActiveInterruptManager->DoHandleInterrupt(interruptNumber, esp);
    }
    return esp;
}

uint32_t InterruptManager::DoHandleInterrupt(uint8_t interruptNumber, uint32_t esp){
    
    
    if(handlers[interruptNumber] != 0){
        esp = handlers[interruptNumber]->HandleInterrupt(esp);
    }
    else if(interruptNumber != 0x20){ //don't print for hardware clock interrupt
        char * msg = "UNHANDLED INTERRUPT 0x00";
        char * hex = "0123456789ABCDEF";
        msg[22] = hex[(interruptNumber >> 4) & 0x0F];
        msg[23] = hex[interruptNumber & 0x0f];
        
        printf(msg);
    }
    //answer the interrupt, so new interrupts can come 
    else if(0x20 <= interruptNumber && interruptNumber < 0x30 ){ //if it is a hardware interrupt
        picMasterCommand.SlowWrite(0x20);
        
        if(0x28 <= interruptNumber){ //if the interrupt came from slave pic
            picSlaveCommand.SlowWrite(0x20);
        }
    }
    return esp;
}