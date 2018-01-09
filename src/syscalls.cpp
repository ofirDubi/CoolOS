
/* 
 * File:   syscalls.cpp
 * Author: ofir123dubi
 *
 * Created on January 9, 2018, 2:05 AM
 */

#include <syscalls.h>
#include <common/coolio.h>
using namespace coolOS;
using namespace coolOS::common;
using namespace coolOS::hardwarecommunication;



SyscallHandler::SyscallHandler(InterruptManager* interruptManager, uint8_t InterruptNumber)
: InterruptHandler(interruptManager, InterruptNumber + interruptManager->getHardwareInterruptOffset())
{
    
}
    
SyscallHandler::~SyscallHandler(){
    
}

uint32_t SyscallHandler::HandleInterrupt(uint32_t esp){
    
    CPUState* cpu = (CPUState*)esp;
    //implement syscalls
    switch(cpu->eax){
        case 4:
            printf((char *)cpu->ebx);
            break;
        default:
            break;
    }
    
    
    return esp;
}
    