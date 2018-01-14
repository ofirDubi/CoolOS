/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   tts.h
 * Author: ofir123dubi
 *
 * Created on January 14, 2018, 7:41 AM
 */

#ifndef __COOLOS__TSS_H
#define __COOLOS__TSS_H

#include<common/types.h>
// A struct describing a Task State Segment.
namespace coolOS{
    struct tss_entry_struct
    {
       common::uint32_t prev_tss;   // The previous TSS - if we used hardware task switching this would form a linked list.
       common::uint32_t esp0;       // The stack pointer to load when we change to kernel mode.
       common::uint32_t ss0;        // The stack segment to load when we change to kernel mode.
       common::uint32_t esp1;       // everything below here is unusued now.. 
       common::uint32_t ss1;
       common::uint32_t esp2;
       common::uint32_t ss2;
       common::uint32_t cr3;
       common::uint32_t eip;
       common::uint32_t eflags;
       common::uint32_t eax;
       common::uint32_t ecx;
       common::uint32_t edx;
       common::uint32_t ebx;
       common::uint32_t esp;
       common::uint32_t ebp;
       common::uint32_t esi;
       common::uint32_t edi;
       common::uint32_t es;         
       common::uint32_t cs;        
       common::uint32_t ss;        
       common::uint32_t ds;        
       common::uint32_t fs;       
       common::uint32_t gs;         
       common::uint32_t ldt;      
       common::uint16_t trap;
       common::uint16_t iomap_base;
    } __packed;

    typedef struct tss_entry_struct tss_entry_t;

}



#endif /* TSS_H */

