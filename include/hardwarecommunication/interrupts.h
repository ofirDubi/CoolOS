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
#include <gdt.h>

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

        static void SetInterruptDescriptorTableEntry(
            coolOS::common::uint8_t interruptNumber,
            coolOS::common::uint16_t gdt_codeSegmentSelectorOffset,
            void (*handler)(), 
            coolOS::common::uint8_t DescriptorPrivilegLevel,
            coolOS::common::uint8_t DescriptorType
        ); 

        Port<coolOS::common::uint8_t> picMasterCommand; //***slow port
        Port<coolOS::common::uint8_t> picMasterData; //***slow port
        Port<coolOS::common::uint8_t> picSlaveCommand; //***slow port
        Port<coolOS::common::uint8_t> picSlaveData; //***slow port


    public:

        InterruptManager(GlobalDescriptorTable * gdt);
        ~InterruptManager();

        void Activate();
        void Deactivate();
       static coolOS::common::uint32_t handleInterrupt(coolOS::common::uint8_t interruptNumer, coolOS::common::uint32_t esp);
       coolOS::common::uint32_t DoHandleInterrupt(coolOS::common::uint8_t interruptNumer, coolOS::common::uint32_t esp);

       static void IgnoreInterruptRequest(); //interval interrupt
       static void HandleInterruptRequest0x00(); //interval interrupt
       static void HandleInterruptRequest0x01(); //keyboard interrupt
       static void HandleInterruptRequest0x0C(); //keyboard interrupt

    };


    }
}

#endif /* INTERRUPTS_H */

