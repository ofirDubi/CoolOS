/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   gdt_new.h
 * Author: ofir123dubi
 *
 * Created on June 4, 2018, 6:40 AM
 */

#ifndef __COMMON__GDT_NEW_H
#define __COMMON__GDT_NEW_H

#include <common/types.h>
#include <common/coolmemory.h>

namespace coolOS{
    
    //define an entry in a gdt table
    typedef struct gdt_entry_struct {
        common::uint16_t lim0_15;
	common::uint16_t base0_15;
	common::uint8_t base16_23;
	common::uint8_t acces;
	common::uint8_t lim16_19:4;
	common::uint8_t flag:4;
	common::uint8_t base24_31;
    }__attribute__ ((packed));
    
    typedef struct gdt_entry_struct gdt_entry;
    
    //register gdt
    typedef struct gdtr_struct {
	common::uint16_t limite;
	common::uint32_t base;
    } __attribute__ ((packed));
    
    typedef struct gdtr_struct gdtr;

    
    typedef struct tss_struct {
	common::uint32_t previous_task, __previous_task_unused;
	common::uint32_t esp0;
	common::uint32_t ss0, __ss0_unused;
	common::uint32_t esp1;
	common::uint32_t ss1, __ss1_unused;
	common::uint32_t esp2;
	common::uint32_t ss2, __ss2_unused;
	common::uint32_t cr3;
	common::uint32_t eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	common::uint32_t es, __es_unused;
	common::uint32_t cs, __cs_unused;
	common::uint32_t ss, __ss_unused;
	common::uint32_t ds, __ds_unused;
	common::uint32_t fs, __fs_unused;
	common::uint32_t gs, __gs_unused;
	common::uint32_t ldt_selector, __ldt_sel_unused;
	common::uint32_t debug_flag, io_map;
    } __attribute__ ((packed));

    typedef struct tss_struct tss;

    common::uint16_t  GDTGetCodeSegment();
    void init_gdt_entry(common::uint32_t, common::uint32_t, common::uint8_t, common::uint8_t,  gdt_entry *);
    void init_gdt(void);
}


#endif /* GDT_NEW_H */

