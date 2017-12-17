/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   interrupts.h
 * Author: ofir123dubi
 *
 * Created on December 16, 2017, 7:32 AM
 */

#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H

#include "types.h"
#include "port.h"
#include "gdt.h"



class InterruptManager;

class InterruptHandler{
protected:
    
   uint8_t interruptNumber;
   InterruptManager * interruptManager;
    
   InterruptHandler(uint8_t interruptNumber, InterruptManager * interruptManager);
   ~InterruptHandler();
   
public:
    virtual uint32_t HandleInterrupt(uint32_t esp);
   
};

class InterruptManager{
friend class InterruptHandler; 
protected:
    
    static InterruptManager * ActiveInterruptManager;
    InterruptHandler * handlers[256]; //a pointer for each interrupt
    
    struct GateDescriptor{
        uint16_t handlerAddressLowBits;
        uint16_t gdt_codeSegmentSelector;
        uint8_t reserved;
        uint8_t access;
        uint16_t handlerAddresshighBits;
        
    }__attribute__((packed));
    
    static GateDescriptor interruptDescriptorTable[256]; //there are 256 interrupts
    
    struct interruptDescriptorTablePointer{
        uint16_t size;
        uint32_t base;
    }__attribute__((packed));
    
    static void SetInterruptDescriptorTableEntry(
        uint8_t interruptNumber,
        uint16_t gdt_codeSegmentSelectorOffset,
        void (*handler)(), 
        uint8_t DescriptorPrivilegLevel,
        uint8_t DescriptorType
    ); 
    
    Port<uint8_t> picMasterCommand; //***slow port
    Port<uint8_t> picMasterData; //***slow port
    Port<uint8_t> picSlaveCommand; //***slow port
    Port<uint8_t> picSlaveData; //***slow port
    
    
public:
    
    InterruptManager(GlobalDescriptorTable * gdt);
    ~InterruptManager();
    
    void Activate();
    void Deactivate();
   static uint32_t handleInterrupt(uint8_t interruptNumer, uint32_t esp);
   uint32_t DoHandleInterrupt(uint8_t interruptNumer, uint32_t esp);

   static void IgnoreInterruptRequest(); //interval interrupt
   static void HandleInterruptRequest0x00(); //interval interrupt
   static void HandleInterruptRequest0x01(); //keyboard interrupt

};


#endif /* INTERRUPTS_H */

