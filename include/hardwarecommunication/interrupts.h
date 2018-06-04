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

#ifndef __COMMON__HARDWARECOMMUNICATION__INTERRUPTS_H
#define __COMMON__HARDWARECOMMUNICATION__INTERRUPTS_H

#include <common/types.h>
#include <hardwarecommunication/port.h>
#include <gdt_new.h>
#include <multitasking.h>

namespace coolOS{
    namespace hardwarecommunication{
        
    class InterruptManager;

    class InterruptHandler{
    protected:

       coolOS::common::uint8_t interruptNumber;
       InterruptManager * interruptManager;

       InterruptHandler(InterruptManager * interruptManager, coolOS::common::uint8_t interruptNumber );
       ~InterruptHandler();

    public:
        virtual coolOS::common::uint32_t HandleInterrupt(coolOS::common::uint32_t esp);

    };

    class InterruptManager{
    friend class InterruptHandler; 
    protected:

        static InterruptManager * ActiveInterruptManager;
        InterruptHandler * handlers[256]; //a pointer for each interrupt
        TaskManager *taskManager; //pointer to the task manager
        
     
        

        struct GateDescriptor{
            coolOS::common::uint16_t handlerAddressLowBits;
            coolOS::common::uint16_t gdt_codeSegmentSelector;
            coolOS::common::uint8_t reserved;
            coolOS::common::uint8_t access;
            coolOS::common::uint16_t handlerAddresshighBits;

        }__attribute__((packed));

        static GateDescriptor interruptDescriptorTable[256]; //there are 256 interrupts

        struct interruptDescriptorTablePointer{
            coolOS::common::uint16_t size;
            coolOS::common::uint32_t base;
        }__attribute__((packed));
        
        coolOS::common::uint16_t hardwareInterruptOffset;
        static void SetInterruptDescriptorTableEntry(
            coolOS::common::uint8_t interrupt,
            coolOS::common::uint16_t gdt_codeSegmentSelectorOffset,
            void (*handler)(), 
            coolOS::common::uint8_t DescriptorPrivilegLevel,
            coolOS::common::uint8_t DescriptorType
        ); 

    

        static void InterruptIgnore();

        static void HandleInterruptRequest0x00(); //time interval interrupt
        static void HandleInterruptRequest0x01(); //keyboard interrupt
        static void HandleInterruptRequest0x02();
        static void HandleInterruptRequest0x03();
        static void HandleInterruptRequest0x04();
        static void HandleInterruptRequest0x05();
        static void HandleInterruptRequest0x06();
        static void HandleInterruptRequest0x07();
        static void HandleInterruptRequest0x08();
        static void HandleInterruptRequest0x09();
        static void HandleInterruptRequest0x0A();
        static void HandleInterruptRequest0x0B();
        static void HandleInterruptRequest0x0C(); //mouse interrupt
        static void HandleInterruptRequest0x0D();
        static void HandleInterruptRequest0x0E();
        static void HandleInterruptRequest0x0F();
        static void HandleInterruptRequest0x31();
        
        static void HandleInterruptRequest0x80(); //software interrupt - for syscallls

        static void HandleException0x00();
        static void HandleException0x01();
        static void HandleException0x02();
        static void HandleException0x03();
        static void HandleException0x04();
        static void HandleException0x05();
        static void HandleException0x06();
        static void HandleException0x07();
        static void HandleException0x08();
        static void HandleException0x09();
        static void HandleException0x0A();
        static void HandleException0x0B();
        static void HandleException0x0C();
        static void HandleException0x0D();
        static void HandleException0x0E();
        static void HandleException0x0F();
        static void HandleException0x10();
        static void HandleException0x11();
        static void HandleException0x12();
        static void HandleException0x13();
        
        
        static coolOS::common::uint32_t handleInterrupt(coolOS::common::uint8_t interrupt, coolOS::common::uint32_t esp);
        coolOS::common::uint32_t DoHandleInterrupt(coolOS::common::uint8_t interrupt, coolOS::common::uint32_t esp);
    
        
        Port<coolOS::common::uint8_t> picMasterCommand; //***slow port
        Port<coolOS::common::uint8_t> picMasterData; //***slow port
        Port<coolOS::common::uint8_t> picSlaveCommand; //***slow port
        Port<coolOS::common::uint8_t> picSlaveData; //***slow port

        
    public:

        InterruptManager(coolOS::common::uint16_t hardwareInterruptOffset, coolOS::TaskManager* taskManager);
        ~InterruptManager();
        coolOS::common::uint16_t getHardwareInterruptOffset();
        void Activate();
        void Deactivate();
     
    };


    }
}

#endif /* INTERRUPTS_H */

