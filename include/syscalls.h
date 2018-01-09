/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   syscalls.h
 * Author: ofir123dubi
 *
 * Created on January 9, 2018, 2:05 AM
 */

#ifndef __COOLOS__SYSCALLS_H
#define __COOLOS__SYSCALLS_H

#include<hardwarecommunication/interrupts.h>
#include <common/types.h>
#include <multitasking.h>

namespace coolOS{
    
    class SyscallHandler : public hardwarecommunication::InterruptHandler{
    public:
        SyscallHandler(hardwarecommunication::InterruptManager* interruptManager, common::uint8_t InterruptNumber);
        ~SyscallHandler();
        
        common::uint32_t HandleInterrupt(common::uint32_t esp);
    };
    
}

#endif /* SYSCALLS_H */

