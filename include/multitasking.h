/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   multitasking.h
 * Author: ofir123dubi
 *
 * Created on December 30, 2017, 9:29 AM
 */

#ifndef __COOLOS__MULTITASKING_H
#define __COOLOS__MULTITASKING_H

#include <gdt.h>
#include <common/types.h>


namespace coolOS{
    
    
    struct CPUState{
        common::uint32_t eax;
        common::uint32_t ebx;
        common::uint32_t ecx;
        common::uint32_t edx;
        
        
        common::uint32_t esi;
        common::uint32_t edi;
        common::uint32_t ebp; //
        /*
        common::uint32_t gs; 
        common::uint32_t fs; 
        common::uint32_t es; 
        common::uint32_t ds; 
        */
        
        common::uint32_t error;
        
          
        common::uint32_t eip;
        common::uint32_t cs;
        common::uint32_t eflags;        
        common::uint32_t esp;
        common::uint32_t ss;

        
        
    }__attribute__((packed));
    
    class Task{
        friend class TaskManager;
        
    private:
        common::uint8_t stack[4096]; // 4KB
        CPUState* cpustate;
        
    public:
        Task(GlobalDescriptorTable *gdt, void entrypoint());
        ~Task();
        
    };
    
    class TaskManager{
    private:
        Task* tasks[256];
        int numTasks;
        int currentTask;
    public:
        TaskManager();
        ~TaskManager();
        
        bool AddTask(Task * task);
        CPUState* Schedule(CPUState* cpustate);
    };
    
}

#endif /* MULTITASKING_H */

