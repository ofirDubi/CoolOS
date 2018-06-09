/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   new_gdt.cpp
 * Author: ofir123dubi
 * 
 * Created on June 4, 2018, 6:49 AM
 */

#include <gdt_new.h>

using namespace coolOS;
using namespace coolOS::common;
gdt_entry gdt_table[0xff];
tss default_tss;

uint16_t  coolOS::GDTGetCodeSegment(){
    return   (uint16_t)(&gdt_table[1]) - (uint16_t)gdt_table;
}

void coolOS::init_gdt(void)
{
 gdtr_struct gdtwrap;

    default_tss.debug_flag = 0x00;
    default_tss.io_map = 0x00;
    default_tss.esp0 = 0x1FFF0;
    default_tss.ss0 = 0x18;
    
    /* initialize gdt segments */
    init_gdt_entry(0x0, 0x0, 0x0, 0x0, &gdt_table[0]);
    init_gdt_entry(0x0, 0xFFFFF, 0x9B, 0x0D, &gdt_table[1]);    /* kernel code segment */
    init_gdt_entry(0x0, 0xFFFFF, 0x93, 0x0D, &gdt_table[2]);    /* kernel data segment*/
    init_gdt_entry(0x0, 0x0, 0x97, 0x0D, &gdt_table[3]);        /* kernel stack segment*/

    init_gdt_entry(0x0, 0xFFFFF, 0xFF, 0x0D, &gdt_table[4]);    /* user code segment */
    init_gdt_entry(0x0, 0xFFFFF, 0xF3, 0x0D, &gdt_table[5]);    /* user data segment*/
    init_gdt_entry(0x0, 0x0, 0xF7, 0x0D, &gdt_table[6]);        /* user stack segment*/

    init_gdt_entry((uint32_t) & default_tss, 0x67, 0xE9, 0x00, &gdt_table[7]);    /*  a gdt entry for tss */

    /* initialize the gdtr structure */
    gdtwrap.limite = 0xff * 8;
    gdtwrap.base = 0x00000800;

    /* copy the gdtr to its memory area */
    memcpy((char *) gdtwrap.base, (char *) gdt_table, gdtwrap.limite);

    /* load the gdtr registry */
    asm("lgdt (%0)": :"p" ((uint8_t *)&gdtwrap) );

    /* initiliaz the segments */
    asm("   movw $0x10, %ax    \n \
            movw %ax, %ds    \n \
            movw %ax, %es    \n \
            movw %ax, %fs    \n \
            movw %ax, %gs    \n \
            ljmp $0x08, $next    \n \
            next:        \n");
}

void coolOS::init_gdt_entry(uint32_t base, uint32_t limite, uint8_t acces, uint8_t flag,  gdt_entry *entry)
{
    entry->lim0_15 = (limite & 0xffff);
    entry->base0_15 = (base & 0xffff);
    entry->base16_23 = (base & 0xff0000) >> 16;
    entry->acces = acces;
    entry->lim16_19 = (limite & 0xf0000) >> 16;
    entry->flag = (flag & 0xf);
    entry->base24_31 = (base & 0xff000000) >> 24;
    return;
}
